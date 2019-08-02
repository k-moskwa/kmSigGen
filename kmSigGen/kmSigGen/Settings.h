/** @file
 * @brief Functions allowing to load and store presets in EEPROM.
 * Settings.h
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
 * References:
 * -# https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__eeprom.html
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include "UserInterface.h"

/**
Initializes routines and variable responsible for button operations.
Following definitions to be set in config.h file @n
#define \b KMSG_MAGIC string that defines module and version allowing to preserve and update settings between versions (e.g. "KMSG100")@n
#define \b KMSG_MAGIC_LENGTH number of bytes in magic string defined in KMSG_MAGIC (e.g. 8 in above example last byte is '\0' so it's 8 bytes)@n
#define \b KMSG_MAX_PRESETS number of presets that can be stored in EEPROM (e.g. 5)@n
#define \b DEFAULT_FREQUENCY default frequency of the signal generator after power up of the system in Hz multiplied by 1000 (e.g. 1000000000 for 1MHz)@n
#define \b DEFAULT_WAVE_TYPE UI_SIG_SQUARE default wave type of the signalr generator after power up of the system (e.g. UI_SIG_SQUARE)@n
#define \b DEFAULT_FREQUENCY_PRESET1 to DEFAULT_FREQUENCY_PRESET1 default preset 1 to 4 of signal generator frequency (e.g. 985248000)@n
#define \b DEFAULT_WAVE_TYPE_PRESET1 to DEFAULT_WAVE_TYPE_PRESET1 default wave type of preset 1 to 4 (e.g. UI_SIG_SQUARE)@n
NOTE: To preserve EEPROM settings make sure EESAVE fuse bit is correctly defined (EESAVE = 0)@n
In this version once EEPROM is erased or has different magic value than defined in KMSG_MAGIC then values are overwritten with default presets 1 to 4
*/
void settingsInit(void);

/**
Returns preset of presetNumber to waveType and frequency result variables.
@param presetNumber number of preset to be returned (0 to KMSG_MAX_PRESETS)
@result waveType UI wave type
@result frequency frequency in Hz multiplied by 1000
NOTE: in case KMSG_NO_EEPROM is defined then values are not taken from EEPROM but just from predefined default presets
*/
void settingsGetPreset(uint8_t presetNumber, UIWaveType *waveType, uint64_t *frequency);

#ifndef KMSG_NO_EEPROM
/**
Stores preset of presetNumbre to EEPROM.
@param presetNumber number of preset to be saved (0 to KMSG_MAX_PRESETS)
@param waveType UI wave type
@param frequency frequency in Hz multiplied by 1000
*/
void settingsSavePreset(uint8_t presetNumber, UIWaveType waveType, uint64_t frequency);
#endif

#endif /* SETTINGS_H_ */
