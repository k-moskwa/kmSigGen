/*
 * Settings.c
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

#ifndef _TESTS_ENV
#include <avr/eeprom.h>
#endif
#include <string.h>
#include <stdbool.h>

#include "config.h"

#include "Settings.h"
#include "UserInterface.h"
#include "SignalGeneratorAD9833.h"

#ifndef KMSG_NO_EEPROM
static uint64_t EEMEM _EEPROMsettingsPresets[KMSG_MAX_PRESETS];
static char EEMEM _EEPROMsettingsMagic[KMSG_MAGIC_LENGTH];
static uint64_t _settingsPresets[KMSG_MAX_PRESETS];

// Private functions
uint64_t combineWaveTypeAndFrequency(SgWaveType waveType, uint64_t frequency);
void splitWaveTypeAndFrequency(uint64_t presetValue, UIWaveType *waveType, uint64_t *frequency);

// Implementation
uint64_t combineWaveTypeAndFrequency(SgWaveType waveType, uint64_t frequency) {
	uint64_t result = (uint64_t)waveType;
	// wave type stored in MSB 8 bits out of 64bit value
	result <<= 56;
	result |= frequency;
	return result;
}

void splitWaveTypeAndFrequency(uint64_t presetValue, UIWaveType *waveType, uint64_t *frequency) {
	// wave type stored in MSB 8 bits out of 64bit value
	*waveType = (SgWaveType)((presetValue >> 56) & 0xFF);
	*frequency = presetValue & 0x00FFFFFFFFFFFFFF;
}

void settingsInit(void) {
	char settingsMagic[KMSG_MAGIC_LENGTH];
	eeprom_read_block(&settingsMagic, &_EEPROMsettingsMagic, KMSG_MAGIC_LENGTH);
	if (strncmp(settingsMagic, KMSG_MAGIC, KMSG_MAGIC_LENGTH) == 0) {
		// Settings Valid (have been written before) - read into memory
		eeprom_read_block((void*)&_settingsPresets,
				(const void*)&_EEPROMsettingsPresets,
				KMSG_MAX_PRESETS * sizeof(uint64_t));
	} else {
		// Write initial settings
		for (int i = 0; i < KMSG_MAX_PRESETS; i++) {
			_settingsPresets[i] = 0;
		}
		_settingsPresets[0] = combineWaveTypeAndFrequency(DEFAULT_WAVE_TYPE, DEFAULT_FREQUENCY);
		_settingsPresets[1] = combineWaveTypeAndFrequency(DEFAULT_WAVE_TYPE_PRESET1, DEFAULT_FREQUENCY_PRESET1);
		_settingsPresets[2] = combineWaveTypeAndFrequency(DEFAULT_WAVE_TYPE_PRESET2, DEFAULT_FREQUENCY_PRESET2);
		_settingsPresets[3] = combineWaveTypeAndFrequency(DEFAULT_WAVE_TYPE_PRESET3, DEFAULT_FREQUENCY_PRESET3);
		_settingsPresets[4] = combineWaveTypeAndFrequency(DEFAULT_WAVE_TYPE_PRESET4, DEFAULT_FREQUENCY_PRESET4);
		eeprom_write_block(KMSG_MAGIC,
							&_EEPROMsettingsMagic,
							KMSG_MAGIC_LENGTH);
		eeprom_write_block(&_settingsPresets,
							&_EEPROMsettingsPresets,
							KMSG_MAX_PRESETS * sizeof(uint64_t));
	}
}

void settingsSavePreset(uint8_t presetNumber, UIWaveType waveType, uint64_t frequency) {
	_settingsPresets[presetNumber] = combineWaveTypeAndFrequency(waveType, frequency);
	eeprom_write_block(	&_settingsPresets[presetNumber],
						&_EEPROMsettingsPresets[presetNumber],
						sizeof(uint64_t));
}

void settingsGetPreset(uint8_t presetNumber, UIWaveType *waveType, uint64_t *frequency) {
	splitWaveTypeAndFrequency(_settingsPresets[presetNumber], waveType, frequency);
}
#else
// routines for version without EEPROM access

void settingsInit(void) {
	// initialization is empty, cause no RAM and EEPROM is used
}

// return default preset
void settingsGetPreset(uint8_t presetNumber, UIWaveType *waveType, uint64_t *frequency) {
	switch(presetNumber) {
		case 1 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET1;
			*frequency = DEFAULT_FREQUENCY_PRESET1;
			break;
		}
		case 2 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET2;
			*frequency = DEFAULT_FREQUENCY_PRESET2;
			break;
		}
		case 3 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET3;
			*frequency = DEFAULT_FREQUENCY_PRESET3;
			break;
		}
		case 4 : {
			*waveType = DEFAULT_WAVE_TYPE_PRESET4;
			*frequency = DEFAULT_FREQUENCY_PRESET4;
			break;
		}
		default : {
			*waveType = DEFAULT_WAVE_TYPE;
			*frequency = DEFAULT_FREQUENCY;
			break;
		}
	}
}
#endif
