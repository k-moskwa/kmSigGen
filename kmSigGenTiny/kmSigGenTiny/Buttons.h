/*
 * RotaryEncoder.h
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

#ifndef BUTTONS_H_
#define BUTTONS_H_

/**
Initializes port for reading buttons on the PB0-2 of uC. Function for ATTiny10
No inline since function used twice
*/
void btnInit(void){
	// All pins as input
	BUTTON_DDR &= ~(_BV(BUTTON_PIN_0) | _BV(BUTTON_PIN_1) | _BV(BUTTON_PIN_2));
	// Pull Up button pins
	BUTTON_PORT |= _BV(BUTTON_PIN_0) | _BV(BUTTON_PIN_1) | _BV(BUTTON_PIN_2);
}

/**
Returns current state of buttons in form of binary code on lowest 3 bits. Function for ATTiny10
@result returns state of buttons in range from 0 to 7 depends on which button was pressed (0 for no buttons pressed)
*/
inline uint8_t btnGetState(void){
	uint8_t result = PINB;
	result &= _BV(BUTTON_PIN_0) | _BV(BUTTON_PIN_1) | _BV(BUTTON_PIN_2);
	result ^= _BV(BUTTON_PIN_0) | _BV(BUTTON_PIN_1) | _BV(BUTTON_PIN_2);
	return result;
}

/**
Waits until all buttons are released, only then exits. Function for ATTiny10
*/
inline void btnWaitUntilReleased(void){
	while (btnGetState() != 0) {
		continue;
	}
}

#endif /* BUTTONS_H_ */
