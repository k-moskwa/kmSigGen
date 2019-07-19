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
 *  References:
 * https://www.leniwiec.org/en/2014/04/28/rotary-shaft-encoder-how-to-connect-it-and-handle-it-with-avr-atmega8-16-32-168-328/
 *
 */

#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include <stdint.h>

/*
Initializes routines and variables responsible for rotary encoder operations
Following definitions to be set in config.h file
#define RSE_DDR  data direction register for rotary encoder port (e.g. DDRC)
#define RSE_PORT_IN rotary encoder port (e.g. PINC)
#define RSE_PIN1 rotary encoder pin 1 (e.g. PC2)
#define RSE_PIN2 rotary encoder pin 1 (e.g. PC3)
@param pin1 - first pin to which rotary shaft encoder is connected
@param pin2 - second pin to which rotary shaft encoder is connected
*/
void rseInit(uint8_t pin1, uint8_t pin2);

/*
 * To be periodically issued in the main loop;
 */
void rseLoop(void);

/*
 * Returns the last change since value has been reset
 */
int8_t getLastChange(void);

/*
 * Returns the last change since value has been reset, and the resets value
 */
int8_t rseGetLastChangeAndReset(void);

#endif /* ROTARYENCODER_H_ */
