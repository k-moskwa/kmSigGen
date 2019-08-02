/*
 * SignalGeneratorAD9833.c
 *
 *  Created on: Jul 10, 2019
 *      Author: Krzysztof Moskwa
 *      License: GPL-3.0-or-later
 *
 *  Signal Generator based on AVR ATmega8, AD9833/AD9837 module and LCD display
 *  Copyright (C) 2019  Krzysztof Moskwa
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "config.h"

#ifndef _TESTS_ENV
#include <avr/io.h>
#include <util/delay.h>
#endif
#include "SignalGeneratorAD9833.h"

#define FREQ_LSB(X) (((X)      ) & 0x3FFF)
#define FREQ_MSB(X) (((X) >> 14) & 0x3FFF)
#define DELAY_US_FSYNC 1

// AD9833 control register
#define CTLR_R15		15
#define CTLR_R14		14
// Two write operations are required to load a complete word into either of the
// frequency registers. B28 = 1 allows a complete word to be loaded into a
// frequency register in two consecutive writes. The first write contains the
// 14 LSBs of the frequency word, and the next write contains the 14 MSBs.
// The first two bits of each 16-bit word define the frequency register to which the
// word is loaded, and should therefore be the same for both of the consecutive writes.
// The write to the frequency register occurs after both words have been loaded;
// therefore, the register never holds an intermediate value. When B28 = 0, the 28-bit
// frequency register operates as two 14-bit registers, one containing the 14 MSBs and
// the other containing the 14 LSBs. This means that the 14 MSBs of the frequency word
// can be altered independent of the 14 LSBs, and vice versa. To alter the 14 MSBs or
// the 14 LSBs, a single write is made to the appropriate frequency address. The control
// bit D12 (HLB) informs the AD9833 whether the bits to be altered are the 14 MSBs or 14 LSBs.
#define CTLR_B28		13
// This control bit allows the user to continuously load the MSBs or LSBs of a frequency
// register while ignoring the remaining 14 bits. This is useful if the complete 28-bit
// resolution is not required. HLB is used in conjunction with D13 (B28). This control bit
// indicates whether the 14 bits being loaded are being transferred to the 14 MSBs or 14 LSBs
// of the addressed frequency register. D13 (B28) must be set to 0 to be able to change the
// MSBs and LSBs of a frequency word separately. When D13 (B28) = 1, this control bit is
// ignored. HLB = 1 allows a write to the 14 MSBs of the addressed frequency register.
// HLB = 0 allows a write to the 14 LSBs of the addressed frequency register.
#define CTLR_HLB		12
#define CTLR_FSELECT	11
#define CTLR_PSELECT	10
#define CTRL_R9			9
#define CTRL_RESET		8
#define CTRL_SLEEP1		7
#define CTRL_SLEEP12	6
#define CTRL_OPBITEN	5
#define CTRL_R4			4
#define CTRL_DIV2		3
#define CTRL_R2			2
#define CTRL_MODE		1
#define CTRL_R0			0

#define REG_D15			15
#define REG_D14			14
#define REG_D13			13
#define REG_D12			12
#define REG_D11			11
#define REG_D10			10
#define REG_D9			9
#define REG_D8			8
#define REG_D7			7
#define REG_D6			6
#define REG_D5			5
#define REG_D4			4
#define REG_D3			3
#define REG_D2			2
#define REG_D1			1
#define REG_D0			0


#define WRITE_CTRL		_BV(B28)
#define WRITE_FREQ0		_BV(REG_D14)
#define WRITE_FREQ1		_BV(REG_D15)
#define WRITE_PHASE0	_BV(REG_D15) | _BV(REG_D14)
#define WRITE_PHASE1	_BV(REG_D15) | _BV(REG_D14) | _BV(REG_D13)

// WRITE A FULL 28-BIT WORD TO A FREQUENCY REGISTER
// (CONTROL REGISTER WRITE) B28 (D13) = 1
// WRITE TWO CONSECUTIVE 16-BIT WORDS
// MSB FIRST
// WRITE ANOTHER FULL 28-BIT WORD TO A FREQUENCY REGISTER
// To load data into the AD9833, FSYNC is held low after the first eight bits are transferred,
// and a second serial write operation is performed to the AD9833.
// Only after the second eight bits are transferred should FSYNC be taken high again.


// WRITE 14MSBs OR LSBs TO A FREQUENCY REGISTER?
// (CONTROL REGISTER WRITE) B28 (D13) = 0 HLB (D12) = 0/1
// WRITE A 16-BIT WORD

// WRITE TO PHASE REGISTER?
// (16-BIT WRITE) D15, D14 = 11 D13 = 0/1 (CHOOSE THE PHASE REGISTER) D12 = X D11 ... D0 = PHASE DATA

// RESET
// 0 No reset applied
// 1 Internal registers reset

// SLEEP1, SLEEP12
// 0 0 No power-down
// 0 1 DAC powered down
// 1 0 Internal clock disabled
// 1 1 Both the DAC powered down and the internal clock disabled
#define SLEEP_NONE		0
#define SLEEP_DAC		_BV(SLEEP12)
#define SLEEP_CLK		_BV(SLEEP1)
#define SLEEP_DAC_CLK	_BV(SLEEP1) | _BV(SLEEP12)

// OPBITEN, MODE, DIV2 Output Wave Definition (X means don't care)
// 0 0 X Sinusoid
// 0 1 X Triangle
// 1 0 0 DAC data MSB/2
// 1 0 1 DAC data MSB
// 1 1 X Reserved
#define OUT_SINE 		0
#define OUT_TRIANGLE	_BV(CTRL_MODE)
#define OUT_MSB_2		_BV(CTRL_OPBITEN)
#define OUT_MSB			_BV(CTRL_OPBITEN) | _BV(CTRL_DIV2)
#define OUT_SQUARE		OUT_MSB
#define OUT_RESERVER	_BV(CTRL_OPBITEN) | _BV(CTRL_MODE) | _BV(CTRL_DIV2)

#define DIV2	_BV(SPI2X)
#define DIV4	0
#define DIV8	_BV(SPI2X) | _BV(SPR0)
#define DIV16 	_BV(SPR0)
#define DIV32	_BV(SPI2X) | _BV(SPR1)
#define DIV64 	_BV(SPR1)
#define DIV128	_BV(SPR0) | _BV(SPR1)
#define DIV64_ALT 	_BV(SPR1) | _BV(SPR0) | _BV(SPR1)

static const uint64_t _calcFix = (((uint64_t)SG_MCLK * (uint64_t)SG_FREQ_COEF) >> AD9833_COEF_BIN) >> 1;
//#define CALC_FIX = ((SG_MCLK * SG_FREQ_COEF) >> AD9833_COEF_BIN) >> 1;

// private functions
void spiInit(void);
void spiWriteWord(uint16_t word);
void sgReset(void);
uint16_t sgGetCtrlWaveType(SgWaveType waveType);
void setFrequencyAndWaveType(uint32_t freqReg, SgWaveType waveType, bool fSelect);

// Implementation
inline void spiInit(void) {
	// soft SPI version
	// All SPI pins as output
	BUTTON_DDR |= _BV(SG_DD_SS) | _BV(SG_DD_MOSI) | _BV(SG_DD_SCK);
	// Initial state as high
	BUTTON_PORT |= _BV(SG_DD_SS) | _BV(SG_DD_MOSI) | _BV(SG_DD_SCK);
}

void spiWriteWord(uint16_t word) {
	// soft SPI version
	// SCK -> HIGH
	SG_PORT |= _BV(SG_DD_SCK);
	// FSYNC -> LOW
	SG_PORT &= ~_BV(SG_DD_SS);
	// Give AD9833 time to get ready to receive data
#ifdef NDEBUG
	_delay_us(DELAY_US_FSYNC);
#endif
	for (int i = 0; i < AD9833_WORD_SIZE; i++) {
		// Start from MSB
		if ((word & 0x8000) == 0) {
			SG_PORT &= ~_BV(SG_DD_MOSI);
		} else {
			SG_PORT |= _BV(SG_DD_MOSI);
		}
		word <<= 1;
		SG_PORT &= ~_BV(SG_DD_SCK);
#ifdef NDEBUG
		_delay_us(SG_SOFT_CLK_DELAY_US + 1);
#endif
		SG_PORT |= _BV(SG_DD_SCK);
#ifdef NDEBUG
		_delay_us(SG_SOFT_CLK_DELAY_US);
#endif
	}
	// DATA -> HIGH
	SG_PORT |= _BV(SG_DD_SS);
	// FSYNC -> HIGH
	SG_PORT |= _BV(SG_DD_SS);
}

inline void sgReset(void) {
	spiWriteWord(_BV(CTRL_RESET));
}

void sgInit(void) {
	spiInit();
	sgReset();
}

uint16_t sgGetCtrlWaveType(SgWaveType waveType) {
	switch (waveType) {
		case SG_SIG_SQUARE : {
			return OUT_SQUARE;
		}
		case SG_SIG_SINE : {
			return OUT_SINE;
		}
		case SG_SIG_TRIANGLE : {
			return OUT_TRIANGLE;
		}
		default : {
			return _BV(CTRL_RESET);
		}
	}
}


/*
 * @param fSelect - 0 for FREQ0 register, 1 for FREQ1 register
 */
inline void setFrequencyAndWaveType(uint32_t freqReg, SgWaveType waveType, bool fSelect) {
	uint16_t word = _BV(CTLR_B28);
	uint16_t regBits = WRITE_FREQ0;
	if (fSelect == true) {
		word |= _BV(CTLR_FSELECT); // not sure if it can be set while writing it
		regBits = WRITE_FREQ1;
	}
	word |= sgGetCtrlWaveType(waveType);
	spiWriteWord(word);
	word = (FREQ_LSB(freqReg)) | regBits;
	spiWriteWord(word);
	word = (FREQ_MSB(freqReg)) | regBits;
	spiWriteWord(word);
}

void setGeneratorParameters(uint32_t freqReg, SgWaveType waveType) {
	setFrequencyAndWaveType(freqReg, waveType, 0);
}
