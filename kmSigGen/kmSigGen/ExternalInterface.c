/*
 * ExternalInterface.c
 *
 *  Created on: Jul 19, 2019
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

#include "ExternalInterface.h"
#include "LiquidCrystal.h"
#include "StringTools.h"
#include "UserInterface.h"
#include "SignalGeneratorAD9833.h"
#include "UserInterface.h"
#include "TWISlave.h"

#define EXT_BIN_COMMAND_POS_L1 29
#define EXT_SEND_BUFFER_SELECTOR_BIT 25

char _extStrBuffer1[STR_INTERNAL_BUFFERS_SIZE_OF] = "";
char _extStrBuffer2[STR_INTERNAL_BUFFERS_SIZE_OF] = "";

volatile bool _extStrBuffer1Changed = false;
volatile bool _extStrBuffer2Changed = false;

// Private functions
void extCommand(uint32_t binaryCommand, uint32_t *binaryResponse, bool *responseAvailable);
UIWaveType decodeWaveType(uint8_t data);

bool extIsSplashStringChanged(void) {
	bool result = _extStrBuffer1Changed;
	_extStrBuffer1Changed = false;
	return result;
}

bool extIsWifiAddressChanged(void) {
	bool result = _extStrBuffer2Changed;
	_extStrBuffer2Changed = false;
	return result;
}

const char *extGetSplashString(void) {
	return _extStrBuffer1;
}

const char *extGetWifiAddress(void) {
	return _extStrBuffer2;
}

void extLoop(void) {
#ifndef KMSG_NO_TWI
	if (twiIsDataInBuffer() == true) {
		bool responseAvailable = false;
		uint32_t response = 0;
		extCommand(twiGetDataFromBuffer(), &response, &responseAvailable);
		if (responseAvailable == true) {
			twiPutDataResponse(response);
		}
	}
#endif
}

UIWaveType decodeWaveType(uint8_t data) {
	switch (data) {
		case 0x00 : {
			return UI_SIG_SINE;
			// no break
		}
		case 0x01 : {
			return UI_SIG_TRIANGLE;
			// no break
		}
		case 0x02 : {
			return UI_SIG_SQUARE;
			// no break
		}
		default : {
			return UI_SIG_NONE;
			// no break
		}
	}
	return UI_SIG_NONE;
}

uint32_t encodeWaveTypeAndFrequency(void) {

	uint32_t result = sgCalcFreqReg(usrGetCurrentFrequency());

	UIWaveType currentWaveType = usrGetWaveType();
	switch (currentWaveType) {
		case UI_SIG_SQUARE : {
			// wave type square 0b01
			currentWaveType |= (0x01L << EXT_BIN_COMMAND_POS_L1);
			break;
		}			
		case UI_SIG_SINE : {
			// wave type square 0b10
			currentWaveType |= (0x02L << EXT_BIN_COMMAND_POS_L1);
			break;
		}
		case UI_SIG_TRIANGLE : {
			// wave type square 0b11
			currentWaveType |= (0x03L << EXT_BIN_COMMAND_POS_L1);
			break;
		}
		default : {
			// wave type none 0b00
		}
	}

	return result;
}

/**
Execute command from external module. The implementation takes care about delivering
external commands to right application modules or making it available via above functions.
@param binaryCommand to be executed (internal implementation takes care about the particular bits interpretation)
@result binaryResponse binary response of the command that is send back to the external module in case vresponseAvailable is true
@result responseAvailable in case it's true, then binaryResponse value should be sent back to the external module
*/
void extCommand(uint32_t binaryCommand, uint32_t *binaryResponse, bool *responseAvailable) {
	uint8_t binCommandL1 = (binaryCommand >> EXT_BIN_COMMAND_POS_L1);
	switch (binCommandL1) {
		case 0x00 : {
			// move bit 5 to bit 1 and bit 1 to bit 0 to get value in range from 0 to 3
			uint8_t waveData = ((binaryCommand >> 4) | (binaryCommand >> 1)) & 0x03;
			UIWaveType waveType = decodeWaveType(waveData);
			usrSetWaveType(waveType);
			break;
		}
		case 0x01 : {
			uint32_t freqReg0 = binaryCommand & 0x1FFFFFFF;
			usrSetFrequency(sgCalcFreq(freqReg0));
			break;
		}
		case 0x05 : {
			*binaryResponse = encodeWaveTypeAndFrequency();
			*responseAvailable = true;
			break;
		}
		case 0x07 : {
			if (((binaryCommand >> EXT_SEND_BUFFER_SELECTOR_BIT) & 0x01) == 0x00) {
				strUnpackBuffer(binaryCommand, _extStrBuffer1);
				_extStrBuffer1Changed = true;
			} else {
				strUnpackBuffer(binaryCommand, _extStrBuffer2);
				_extStrBuffer2Changed = true;
			}
			break;
		}
	}
}
