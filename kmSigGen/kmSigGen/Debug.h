/*
 * Debug.h
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
 * https://atnel2.blogspot.com/2014/04/puapki-programowe-debuger-na-jednej.html
 */

#ifndef DEBUG_H_
#define DEBUG_H_

/**
Initializes all ports to pull-up state
NOTE: This method should be issued first in the main routine
*/
void dbPullUpAllPorts(void);

/**
Initializes the debug routines with specific number of dbStep calls to switch DB_PIN between states
Following definitions to be set in config.h file
#define DEBUG_DDR data direction register for debug led (e.g. DDRB)
#define DEBUG_PORT debug led port (e.g. PORTB)
#define DEBUG_PIN debug led pin (e.g. PB1)
to disable debug feature completely - define NO_PIN_DEBUG env variable
@param number of steps to switch debug pin from high to low and from low to high state
*/
void dbInit(uint8_t steps);

/**
Issues next debug step.
*/
void dbStep(void);

/**
Changes output state of the Debug Led pin to High state
*/
void dbLedHigh(void);

/**
Changes output state of the Debug Led pin to Low state
*/
void dbLedLow(void);


#endif /* DEBUG_H_ */
