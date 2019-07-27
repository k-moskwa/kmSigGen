/*
 * StringTools.c
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
#include "strings.h"
#include "StringTools.h"
#include "LiquidCrystalCharacters.h"

#define STR_PACK_BYTES 3
#define STR_PACK_BITS 7
#define STR_PACK_ADDR_MASK 0x07
#define STR_PACK_BITS_MASK 0x7F
#define STR_PACK_WORD_MASK 0x1FFFFF
#define STR_PACK_COUNT_LSB_BIT 21
#define STR_PACK_BUFFER_LENGTH 20

static char _strBuffer[STR_INTERNAL_BUFFERS_SIZE_OF] = "";

void strUnpackBuffer(uint32_t command, char *bufferResult) {
  // extract buffer position
  uint8_t bufferPos = ((command >> STR_PACK_COUNT_LSB_BIT) & STR_PACK_ADDR_MASK) * STR_PACK_BYTES;
  // terminate buffer with 0
  bufferResult[STR_PACK_BYTES + bufferPos] = '\0';
  //command &= STR_PACK_WORD_MASK; // only character part is needed
  for (int i = 1; i <= STR_PACK_BYTES; i++) {
    bufferResult[bufferPos + STR_PACK_BYTES - i] = command & STR_PACK_BITS_MASK;
    command >>= STR_PACK_BITS;
  }
}

bool strIsEmpty(const char *buffer) {
	return (buffer[0] == '\0');
}

#ifdef KMSG_NO_STDIO
void strAddToBuffer(char *buffer, const char *str, uint8_t *currentBufferPos) {
	uint8_t sourcePos = 0;
	char c = 0;
	do {
		c = str[sourcePos++];
		buffer[(*currentBufferPos)++] = c;
	} while (c != 0);
	(*currentBufferPos)--;
}
#endif

void strSignalFrequencyToStr(char *buffer, uint16_t freqHi, uint16_t freqLo) {
	#ifndef KMSG_NO_STDIO
	sprintf(buffer, "% 4d.%04d", freqHi, freqLo);
	#else
	// function is assuming buffer has space at least (digits * 2 + 2)
	static const uint8_t digits = 4;

	bool spacesInsteadOfZeros = false;
	for (uint8_t i = 0; i < digits; i++) {
		buffer[digits - i - 1] = (
		spacesInsteadOfZeros == false ? (freqHi % 10) + '0' : ' ');
		freqHi /= 10;
		if (freqHi == 0) {
			spacesInsteadOfZeros = true;
		}
	}
	for (uint8_t i = 0; i < digits; i++) {
		buffer[digits * 2 - i] = (freqLo % 10) + '0';
		freqLo /= 10;
	}
	buffer[digits] = '.';
	buffer[digits * 2 + 1] = '\0';
	#endif
}

const char *strSignalMultiplierToStr(FreqMultiplier multpilier) {
	switch (multpilier) {
		case STR_MULT_MEGA : {
			return STR_MHZ;
		}
		case STR_MULT_KILO : {
			return STR_KHZ;
		}
		default : {
			return STR_HZ;
		}
	}
}

char strWaveTypeToSingleChar(UIWaveType waveType) {
	switch (waveType) {
		case UI_SIG_SQUARE : {
			return LCD_WAVE_SQUARE_NO;
		}
		case UI_SIG_SINE : {
			return LCD_WAVE_SINE_NO;
		}
		case UI_SIG_TRIANGLE : {
			return LCD_WAVE_TRIANGLE_NO;
		}
		default : {
			return LCD_WAVE_NONE_NO;
		}
	}
}

void strSignalParametersToStr(char *buffer, UIWaveType waveType, uint16_t freqHi, uint16_t freqLo, FreqMultiplier multpilier) {
	char *sigTypeStr = STR_NONE;
	switch (waveType) {
		case UI_SIG_SQUARE: {
			sigTypeStr = STR_SQUARE;
			break;
		}
		case UI_SIG_SINE: {
			sigTypeStr = STR_SINE;
			break;
		}
		case UI_SIG_TRIANGLE: {
			sigTypeStr = STR_TRIANGLE;
			break;
		}
		case UI_SIG_NONE: {
			// Intentionally
			break;
		}
	}
	strSignalFrequencyToStr(_strBuffer, freqHi, freqLo);
	#ifndef KMSG_NO_STDIO
	sprintf(buffer, "%s%s%s%s", sigTypeStr, STR_FREQUENCY_SHORT, _strBuffer, strSignalMultiplierToStr(multpilier));
	#else
	uint8_t bufferPos = 0;
	strAddToBuffer(buffer, sigTypeStr, &bufferPos);
	strAddToBuffer(buffer, STR_FREQUENCY_SHORT, &bufferPos);
	strAddToBuffer(buffer, _strBuffer, &bufferPos);
	strAddToBuffer(buffer, strSignalMultiplierToStr(multpilier), &bufferPos);
	#endif
}