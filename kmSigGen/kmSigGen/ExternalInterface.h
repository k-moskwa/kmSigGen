/** @file
 * @brief Functions for ESP-01 external interface.
 * ExternalInterface.h
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

#ifndef EXTERNALINTERFACE_H_
#define EXTERNALINTERFACE_H_

#include <stdbool.h>
#include <stdint.h>

/**
Returns true in case splash string has been changed from external module.
@result true in case splash string has been changed and needs to be updated
*/
bool extIsSplashStringChanged(void);

/**
Returns current splash string received from external module.
@result current splash string from external module
*/
const char *extGetSplashString(void);

/**
Returns true in WiFi address string has been changed from external module.
@result true in WiFi address string has been changed and needs to be updated
*/
bool extIsWifiAddressChanged(void);
/**
Returns current WiFi address received from external module.
@result current WiFi address from external module
*/
const char *extGetWifiAddress(void);

/**
To be periodically issued in the main loop.
The implementation takes care about delivering external commands to the 
right application modules or making it available via above functions
*/
void extLoop(void);

#endif /* EXTERNALINTERFACE_H_ */
