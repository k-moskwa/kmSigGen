/*
 * RotaryEncoder.c
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

#ifndef _TESTS_ENV
#include <avr/io.h>
#endif

#include <stdbool.h>
#include "config.h"

#define RSE_GRAY_CODE_BIT1 0x01
#define RSE_GRAY_CODE_BIT2 0x02
#define RSE_GRAY_CODE_B00 0x00
#define RSE_GRAY_CODE_B01 0x01
#define RSE_GRAY_CODE_B10 0x02
#define RSE_GRAY_CODE_B11 0x03


uint8_t _pin1 = 0, _pin2 = 0;
uint8_t _rsePreviousCode = 0;
int8_t _rseChange = 0;

// Private functions
uint8_t rseGetGrayCode(void);

// Implementation
uint8_t rseGetGrayCode(void) {
	uint8_t result = 0;
#ifndef _TESTS_ENV
	if (!bit_is_clear(RSE_PORT_IN, _pin1)) {
		result |= RSE_GRAY_CODE_BIT2;
	}

	if (!bit_is_clear(RSE_PORT_IN, _pin2)) {
		result |= RSE_GRAY_CODE_BIT1;
	}
#endif
	return result;
}

void rseInit(uint8_t pin1, uint8_t pin2) {
	_pin1 = pin1;
	_pin2 = pin2;
	_rsePreviousCode = 0;
	_rseChange = 0;

	// pull-up disabled for selected pins cause implemented on HW level
	RSE_PORT &= ~(_BV(_pin1) | _BV(_pin2));

	// set pins as input
	RSE_DDR &= ~(_BV(_pin1) | _BV(_pin2));
}

void rseLoop(void) {
	uint8_t currentCode = rseGetGrayCode();
	if (_rsePreviousCode != currentCode) {
		if ((_rsePreviousCode == RSE_GRAY_CODE_B11 && currentCode == RSE_GRAY_CODE_B01)
				|| (_rsePreviousCode == RSE_GRAY_CODE_B00 && currentCode == RSE_GRAY_CODE_B10)) {
			_rseChange++;
		}
	}

	if (_rsePreviousCode != currentCode) {
		if ((_rsePreviousCode == RSE_GRAY_CODE_B10 && currentCode == RSE_GRAY_CODE_B00)
				|| (_rsePreviousCode == RSE_GRAY_CODE_B01 && currentCode == RSE_GRAY_CODE_B11)) {
			_rseChange--;
		}
	}

	_rsePreviousCode = currentCode;
}

int8_t rseGetLastChange(void) {
	return _rseChange;
}

int8_t rseGetLastChangeAndReset(void) {
	int8_t current = _rseChange / RSE_TICKS_PER_SWITCH;
	// accumulate changes until the value is higher or lower than RSE_TICKS_PER_SWITCH
	if (current != 0) {
		_rseChange = 0;
	}
	return current;
}
