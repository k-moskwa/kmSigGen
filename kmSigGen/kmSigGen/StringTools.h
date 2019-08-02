/** @file
 * @brief Generic string tools.
 * StringTools.h
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
 *  References:
 * https://android.googlesource.com/kernel/lk/+/qcom-dima-8x74-fixes/lib/libc/itoa.c
 */

#ifndef STRINGTOOLS_H_
#define STRINGTOOLS_H_

#include <stdint.h>
#include <stdbool.h>
#include "UserInterface.h"
#include "SignalGeneratorAD9833.h"

typedef enum {
	STR_MULT_NONE = 1,
	STR_MULT_KILO = 2,
	STR_MULT_MEGA = 3
} FreqMultiplier;

/**
Unpacks characters received via TWI command into provided buffer.
@param command A command received from TWI interface
@result bufferResult String buffer to receive complete string
*/
void strUnpackBuffer(uint32_t command, char *bufferResult);

/**
Returns true if provided string is empty.
@param buffer A string in buffer to be checked
@result true in case string in buffer is empty
*/
bool strIsEmpty(const char *buffer);

/**
Changes HI and LO frequency parts used in UserInterface into string that can be displayed.
@param freqHi frequency value on the left side of dot ('.')
@param freqLo frequency value on the right side of dot
@result buffer A result string will be placed there
*/
void strSignalFrequencyToStr(char *buffer, uint16_t freqHi, uint16_t freqLo);

/**
Returns string corresponding to the provided multiplier argument.
@param multiplier A value to be converted to string
@result string representing multiplier
*/
const char *strSignalMultiplierToStr(FreqMultiplier multpilier);

/**
Returns complete string describing current frequency as used in UserInterface functions.
@param freqHi frequency value on the left side of dot ('.')
@param freqLo frequency value on the right side of dot
@param multiplier A value to be converted to string
@result Complete string describing current frequency as used in UserInterface functions
*/
void strSignalParametersToStr(char *buffer, UIWaveType waveType, uint16_t freqHi, uint16_t freqLo, FreqMultiplier multpilier);

/**
Returns single character corresponding to the provided waveType argument.
@param waveType A Wave Type as used by UI
@result single character corresponding to the provided waveType argument
*/
char strWaveTypeToSingleChar(UIWaveType waveType);


/**
Converts provided UIWaveType to SGWaveType value.
@param uiWaveType input parameter as WaveType defined in UserInterface
@result WaveType defined in SignalGenerator
*/
SgWaveType ui2sgWaveType(UIWaveType uiWaveType);

#endif /* STRINGTOOLS_H_ */
