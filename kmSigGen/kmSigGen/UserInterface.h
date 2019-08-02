/** @file
 * @brief User Interface routines for the application.
 * UserInterface.h
 *
 *  Created on: Jul 12, 2019
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

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

/**
Definition of wave types for User Interface.
*/
typedef enum {
	/// Square Wave Type 5V p-p
	UI_SIG_SQUARE = 1,
	/// Sine Wave Type 0.6V p-p
	UI_SIG_SINE = 2,
	/// Triangle Wave Type 0.6V p-p
	UI_SIG_TRIANGLE = 3,
	/// No signal output
	UI_SIG_NONE = 4
} UIWaveType;

/**
Initialize and set startup frequency in User Interface.
@param frequency Desired frequency in Hz multiplied by 1000
@param waveType Wave Type
*/
void usrInit(uint64_t frequency, UIWaveType waveType);

/**
To be periodically issued in the main loop.
*/
void usrLoop(void);

/**
Returns true in case frequency parameter changed either by user interaction, 
loading preset or changing it from external interface.
@result true in case frequency value changed in user interface, false - otherwise
*/
bool usrIsParametersChangedAndReset(void);

/**
Returns current frequency as set in User Interface.
@result Current Frequency in User Interface
*/
uint64_t usrGetCurrentFrequency(void);

/**
Sets frequency in User Interface and updates Signal Generator accordingly.
@param Frequency Desired frequency to be set
*/
void usrSetFrequency(uint64_t frequency);

/**
Returns current Wave Type as set in User Interface.
@result Current Wave Type in User Interface
*/
UIWaveType usrGetWaveType(void);

/**
Sets Wave Type in User Interface and updates Signal Generator accordingly.
@param waveType Wave Type to be set
*/
void usrSetWaveType(UIWaveType waveType);

#endif /* USERINTERFACE_H_ */
