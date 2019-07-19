/*
 * Debug.c
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

#include <avr/io.h>

#include "config.h"

#include "Debug.h"

uint8_t _dbSteps;
#ifndef NO_PIN_DEBUG
uint8_t _dbStepCurrent;
#endif

void dbPullUpAllPorts(void) {
	// PULL UP all ports
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
}

void dbInit(uint8_t steps) {
	_dbSteps = steps;
#ifndef KMSG_NO_PIN_DEBUG
	_dbStepCurrent = steps;
	DDRB |= _BV(DEBUG_PIN);
#endif
}

void dbStep(void) {
#ifndef KMSG_NO_PIN_DEBUG
	if (_dbStepCurrent == 0) {
		_dbStepCurrent = _dbSteps;
		DEBUG_PORT ^= _BV(DEBUG_PIN);
	} else {
		_dbStepCurrent--;
	}
#endif
}
