/*
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
 * https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__eeprom.html
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>
#include "SignalGeneratorAD9833.h"

/**
Initializes routines and variable responsible for button operations
Following definitions to be set in config.h file
#define DEFAULT_FREQUENCY default frequency of the signal generator after power up of the system in Hz multiplied by 1000 (e.g. 1000000000 for 1MHz)
#define DEFAULT_WAVE_TYPE UI_SIG_SQUARE default wave type of the signalr generator after power up of the system (e.g. UI_SIG_SQUARE)
#define DEFAULT_FREQUENCY_PRESET1 to DEFAULT_FREQUENCY_PRESET1 default preset 1 to 4 of signal generator frequency (e.g. 985248000)
#define DEFAULT_WAVE_TYPE_PRESET1 to DEFAULT_WAVE_TYPE_PRESET1 default wave type of preset 1 to 4 (e.g. UI_SIG_SQUARE)
*/

inline void settingsInit(void) {
	// initialization is empty, cause no RAM and EEPROM is used
}

/**
Returns preset of presetNumber to waveType and frequency result variables
@param presetNumber number of preset to be returned (0 to KMSG_MAX_PRESETS)
@result waveType UI wave type
@result frequency frequency in Hz multiplied by 1000
*/
inline void settingsGetPreset(uint8_t presetNumber, SgWaveType *waveType, uint32_t *freqReg) {
	switch(presetNumber) {
		case 1 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET1;
			*freqReg = FREQUENCY_REG_PRESET1;
			break;
		}
		case 2 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET2;
			*freqReg = FREQUENCY_REG_PRESET2;
			break;
		}
		case 3 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET3;
			*freqReg = FREQUENCY_REG_PRESET3;
			break;
		}
		case 4 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET4;
			*freqReg = FREQUENCY_REG_PRESET4;
			break;
		}
		case 5 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET5;
			*freqReg = FREQUENCY_REG_PRESET5;
			break;
		}
		case 6 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET6;
			*freqReg = FREQUENCY_REG_PRESET6;
			break;
		}
		case 7 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET7;
			*freqReg = FREQUENCY_REG_PRESET7;
			break;
		}
		case 8 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET8;
			*freqReg = FREQUENCY_REG_PRESET8;
			break;
		}
		default : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET1;
			*freqReg = FREQUENCY_REG_PRESET1;
			break;
		}
	}
}

#endif /* SETTINGS_H_ */
