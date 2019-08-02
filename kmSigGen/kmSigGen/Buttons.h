/** @file
 * @brief Functions for button in Rotary Shaft Encoder.
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

#include <stdint.h>
#include <stdbool.h>

/**
Initializes routines and variable responsible for button operations.
Following definitions to be set in config.h file @n
#define \b BUTTON_DDR  data direction register for button port (e.g DDRC) @n
#define \b BUTTON_PORT_IN button port input (e.g PINC) @n
#define \b BUTTON_PIN button pin (e.g PC1) @n
@param pin - pin to which button is connected
*/
void btnInit(uint8_t pin);

/**
To be periodically issued in the main loop.
*/
void btnLoop(void);

/**
Returns true in case button has been pressed since last btnReset function issued.
@return true in case button has been pressed since last btnReset function
*/
bool btnPressed(void);

/**
Resets the button state so application is ready for the next press.
*/
void btnReset(void);


#endif /* BUTTONS_H_ */
