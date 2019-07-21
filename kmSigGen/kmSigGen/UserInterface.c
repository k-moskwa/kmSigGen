/*
 * UserInterface.c
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

#ifndef USERINTERFACE_C_
#define USERINTERFACE_C_

#ifndef _TESTS_ENV
#include <avr/io.h>
#include <util/delay.h>
#endif
#include <stdlib.h>
#include <stdint.h>
#ifndef KMSG_NO_STDIO
#include <stdio.h>
#endif

#include "version.h"
#include "config.h"
#include "strings.h"
#include "Buttons.h"
#include "RotaryEncoder.h"
#include "LiquidCrystal.h"
#include "LiquidCrystalCharacters.h"
#include "UserInterface.h"
#include "SignalGeneratorAD9833.h"
#include "Settings.h"
#include "ExternalInterface.h"
#include "StringTools.h"

#define MENU_COORDS_SIZE_OF 4

typedef struct  {
	uint8_t x;
	uint8_t y;
} UInt8Pair;

typedef struct {
	uint16_t frequencyHi;
	uint16_t frequencyLo;
	FreqMultiplier multiplier;
	UIWaveType waveType;
} SignalGeneratorParams;

typedef enum {
	MENU_INIT = 0,
	MENU_POWER_SAVE_SHOW,
	MENU_POWER_SAVE,
	MENU_SPLASH,
	MENU_SPLASH_WAIT,
	MENU_MAIN_SHOW,
	MENU_MAIN,
	MENU_SELECT_AMP_FREQ_WAVE_SHOW,
	MENU_SELECT_AMP_FREQ_WAVE,
	MENU_EDIT_FREQ_SHOW,
	MENU_EDIT_FREQ_SELECT,
	MENU_EDIT_FREQ,
	MENU_EDIT_FREQ_APPLY,
	MENU_EDIT_FREQ_CANCEL,
	MENU_EDIT_WAVE_SHOW,
	MENU_EDIT_WAVE,
	MENU_LOAD_SAVE_SHOW,
	MENU_LOAD_SAVE_SELECT,
	MENU_PRESETS_SHOW,
	MENU_PRESETS_SELECT
} MenuStatess;

enum FreqSetupState {
	FS_BOF = 0, // defines first item in the menu array _freqSetupState
	FS_EOF = 11, // defines last item in the menu array _freqSetupState
	FS_CANCEL = 0,
	FS_WAVE_TYPE = 1,
	FS_AT = 2, //
	FS_1000 = 3,
	FS_100 = 4,
	FS_10 = 5,
	FS_1 = 6,
	FS_DOT = 7,
	FS_1_10 = 8,
	FS_1_100 = 9,
	FS_1_1000 = 10,
	FS_1_10000 = 11,
	FS_MULT = 12, // also position 13 & 14
	FS_APPLY = 15
};

typedef enum {
	PRESET_LOAD = 0,
	PRESET_SAVE = 1
} LoadSave;

static const int8_t _freqSetupStates[] = {
	FS_CANCEL,
	FS_WAVE_TYPE,
	FS_1000,
	FS_100,
	FS_10,
	FS_1,
	FS_1_10,
	FS_1_100,
	FS_1_1000,
	FS_1_10000,
	FS_MULT,
	FS_APPLY
};

bool _parametersChanged = false;

uint8_t _menuTicks = 0;
uint8_t _menuItems = 0;
UInt8Pair _menuCoords[MENU_COORDS_SIZE_OF];

uint8_t _menuState = MENU_INIT;
LoadSave _loadSave = PRESET_LOAD;

int8_t _freqSetupCurrentState = FS_CANCEL;

SignalGeneratorParams _signalGeneratorParams;
SignalGeneratorParams _signalGeneratorParamsOnEdit;
SignalGeneratorParams _signalGeneratorParamsTmp;

uint16_t _timeout = UINT16_MAX;
char _lcdStrBuffer[STR_INTERNAL_BUFFERS_SIZE_OF] = "";

// private functions
uint64_t usrSignalGeneratorParamsToFrequency(SignalGeneratorParams params);
void usrFrequencyToSignalGeneratorParams(uint64_t frequency, SignalGeneratorParams *params);
void usrSetTimeout(uint16_t timeout);
bool usrTimeoutLoop(void);
void usrInitDisplayCharacters(void);
int8_t usrRotValue(int8_t newValue, int8_t minValue, int8_t maxValue);
void usrMenuSignalParameters(void);
inline void usrNextState(uint8_t nextState);
void usrMenuShow(uint8_t itemNo, uint8_t currentItem, const char * menuItems[], const UInt8Pair menuCoords[]);
uint8_t usrGetMenuSelection();
void usrMenuShow2(uint8_t currentItem, const char * menuItems[]);
void usrMenuShow2WithText(uint8_t currentItem, const char *text, const char * menuItems[]);
void usrMenuShow4(uint8_t currentItem, const char * menuItems[]);
void usrMenuPowerSaveShow(void);
void usrMenuPowerSave(void);
void usrMenuSplash(void);
inline void usrMenuSplashWait(void);
inline void usrMenuMainShow(void);
inline void usrMenuMain(void);
void usrMenuSelectAmpFreqWaveShow(void);
void usrMenuSelectAmpFreqWave(void);
void usrMenuLoadSaveShow(void);
void usrMenuLoadSaveSelect(void);
void usrMenuPresetsShow(void);
void usrMenuPresetsSelect(void);
void usrMenuFreqShow(void);
void updateMainMenuIfOnScreen(void);
void usrMenuFreqSelect(void);
void usrMenuFreqEdit(void);
void usrMenuFreqApply(void);
void usrMenuFreqCancel(void);
void usrMenuWaveShow(void);
inline void usrMenuWave(void);
void usrMenuDispatcherLoop(void);

// Implementation
uint64_t usrSignalGeneratorParamsToFrequency(SignalGeneratorParams params) {
	uint64_t result = ((uint64_t)params.frequencyHi * 10000 + (uint64_t)params.frequencyLo);
	switch (params.multiplier) {
		case STR_MULT_MEGA: {
		result *= 1000ULL;
		//no break
		}
		case STR_MULT_KILO: {
		result *= 1000ULL;
		//no break
		}
		default: {
		// left empty intentionally
		}
	}
	result /= 10;
	return result;
}

void usrFrequencyToSignalGeneratorParams(uint64_t frequency, SignalGeneratorParams *params) {
	if (frequency >= 1000000000) {
		params->multiplier = STR_MULT_MEGA;
		frequency /= 100000;
	}
	else if (frequency >= 1000000) {
		params->multiplier = STR_MULT_KILO;
		frequency /= 100;
	}
	else {
		params->multiplier = STR_MULT_NONE;
		frequency *= 10;
	}
	params->frequencyHi = (uint16_t)(frequency / 10000ULL);
	params->frequencyLo = (uint16_t)(frequency % 10000ULL);
}

bool usrIsParametersChangedAndReset(void) {
	bool result = _parametersChanged;
	_parametersChanged = false;
	return result;
}

uint64_t usrGetCurrentFrequency(void) {
	return usrSignalGeneratorParamsToFrequency(_signalGeneratorParams);
}

UIWaveType usrGetWaveType(void) {
	return _signalGeneratorParams.waveType;
}

void usrSetTimeout(uint16_t timeout) {
	_timeout = timeout;
}

bool usrTimeoutLoop(void) {
	// UINT16_MAX means there is no timeout set anymore
	if (_timeout == UINT16_MAX) {
		return false;
	}
	if (_timeout != 0) {
		_timeout--;
	} else {
		_timeout = UINT16_MAX;
		return true;
	}
return false;
}

void usrInit(uint64_t frequency, UIWaveType waveType) {
	lcdInit(LCD_COLS, LCD_ROWS, LCD_5x8DOTS);
	lcdBegin(); // PORTD
	lcdBacklight();

	rseInit(RSE_PIN1, RSE_PIN2);
	btnInit(BUTTON_PIN);

	usrFrequencyToSignalGeneratorParams(frequency, &_signalGeneratorParams);
	_signalGeneratorParams.waveType = waveType;
}

void usrInitDisplayCharacters(void) {
	lcdCreateChar(LCD_CURSOR_RIGHT_NO, LCD_CURSOR_RIGHT);
	lcdCreateChar(LCD_CURSOR_LEFT_NO, LCD_CURSOR_LEFT);
	lcdCreateChar(LCD_CURSOR_DOWN_NO, LCD_CURSOR_DOWN);
	lcdCreateChar(LCD_CHAR_CHECK_MARK_NO, LCD_CHAR_CHECK_MARK);
	lcdCreateChar(LCD_WAVE_SQUARE_NO, LCD_WAVE_SQUARE);
	lcdCreateChar(LCD_WAVE_TRIANGLE_NO, LCD_WAVE_TRIANGLE);
	lcdCreateChar(LCD_WAVE_SINE_NO, LCD_WAVE_SINE);
}

int8_t usrRotValue(int8_t newValue, int8_t minValue, int8_t maxValue) {
	if (newValue < minValue) {
		return maxValue;
	}
	if (newValue > maxValue) {
		return minValue;
	}
	return newValue;
}

void usrMenuSignalParameters(void) {
	strSignalParametersToStr(_lcdStrBuffer,
			_signalGeneratorParams.waveType,
			_signalGeneratorParams.frequencyHi,
			_signalGeneratorParams.frequencyLo,
			_signalGeneratorParams.multiplier);
	const char * wiFiAddress = extGetWifiAddress();
	lcdSetCursor(0, 0);
	if (strIsEmpty(wiFiAddress) == true) {
		lcdPrint(STR_MENU_MAIN_SELECT);
	}
	else {
		lcdPrint(wiFiAddress);
		lcdFillSpacesToEndOfTheLine();
	}
	lcdSetCursor(0, 1);
	lcdPrint(_lcdStrBuffer);
}

void usrNextState(uint8_t nextState) {
	_menuState = nextState;
}

void usrMenuShow(uint8_t itemNo, uint8_t currentItem, const char * menuItems[], const UInt8Pair menuCoords[]) {
	_menuItems = 0;
	_menuTicks = 0;
	lcdClear();
	currentItem--; // convert from item number to loop number
	for (int i = 0; i < itemNo; i++)  {
		const char *menuItem = menuItems[i];
		const UInt8Pair menuCoord = menuCoords[i];
		_menuCoords[i] = menuCoords[i];
		if (i == currentItem) {
			lcdSetCursor(menuCoord.x, menuCoord.y);
			lcdWrite(STR_MENU_SELECTOR_CHAR);
		}
		lcdSetCursor(menuCoord.x + 1, menuCoord.y);
		lcdPrint(menuItem);
	}
	_menuItems = itemNo;
	_menuTicks = currentItem;
}

uint8_t usrGetMenuSelection() {
	const uint8_t previousMenuItem = _menuTicks;

	if (btnPressed() == true) {
		btnReset();
		return (previousMenuItem + 1);
	}
	const int8_t rseMaxValue = _menuItems;
	const int8_t rseCurrentValue = rseGetLastChangeAndReset();
	if (rseCurrentValue != 0) {

		int8_t currentMenuTicks = _menuTicks + rseCurrentValue;
		currentMenuTicks %= rseMaxValue;
		if (currentMenuTicks < 0 ) {
			currentMenuTicks = rseMaxValue - currentMenuTicks - 2;
		}
		uint8_t currentMenuItem = currentMenuTicks;
		_menuTicks = currentMenuTicks;
		if (currentMenuItem != previousMenuItem) {
			lcdSetCursor(_menuCoords[previousMenuItem].x, _menuCoords[previousMenuItem].y);
			lcdWrite(STR_MENU_EMPTY_CHAR);
			lcdSetCursor(_menuCoords[currentMenuItem].x, _menuCoords[currentMenuItem].y);
			lcdWrite(STR_MENU_SELECTOR_CHAR);
		}
	}
	return 0; // 0 means no menu selected
}

void usrMenuShow2(uint8_t currentItem, const char * menuItems[]) {
	static const UInt8Pair menuCoords[] = {
			{0x00, 0x00},
			{0x00, 0x01}
	};
	usrMenuShow(2, currentItem, menuItems, menuCoords);
}

void usrMenuShow2WithText(uint8_t currentItem, const char *text, const char * menuItems[]) {
	static const UInt8Pair menuCoords[] = {
			{0x07, 0x00},
			{0x0C, 0x00}
	};
	usrMenuShow(2, currentItem, menuItems, menuCoords);
	lcdSetCursor(0, 0);
	lcdPrint(text);
}

void usrMenuShow4(uint8_t currentItem, const char * menuItems[]) {
	static const UInt8Pair menuCoords[] = {
		{0x00, 0x00},
		{0x08, 0x00},
		{0x00, 0x01},
		{0x08, 0x01}
	};
	usrMenuShow(4, currentItem, menuItems, menuCoords);
}

void usrMenuPowerSaveShow(void) {
	lcdClear();
	lcdPrint(STR_PWR_SAVER1);
	lcdSetCursor(0, 1);
	lcdPrint(STR_PWR_SAVER2);
	lcdNoBacklight();
	usrNextState(MENU_POWER_SAVE);
}

void usrMenuPowerSave(void) {
	if (btnPressed() == true) {
		btnReset();
		usrNextState(MENU_MAIN_SHOW);
	}
}

void usrMenuSplash(void) {
	usrSetTimeout(KMSG_SPLASH_SCREEN_TIMEOUT);
	lcdClear();
	lcdPrint(APP_NAME " " APP_VERSION);
	usrNextState(MENU_SPLASH_WAIT);
	btnReset();
	usrInitDisplayCharacters();
	lcdSetCursor(0, 1);
	lcdPrint(extGetSplashString());
	lcdFillSpacesToEndOfTheLine();
}

void usrMenuSplashWait(void) {
	if (extIsSplashStringChanged() == true) {
		lcdSetCursor(0, 1);
		lcdPrint(extGetSplashString());
	}
	if (usrTimeoutLoop() == true || btnPressed() == true) {
		usrNextState(MENU_MAIN_SHOW);
		btnReset();
	}
}

void usrMenuMainShow(void) {
	lcdClear();
	usrMenuSignalParameters();
	usrNextState(MENU_MAIN);
#ifdef KMSG_SCREEN_SAVER_TIMEOUT
	usrSetTimeout(KMSG_SCREEN_SAVER_TIMEOUT); // power saver after 30 sec
#endif
	lcdBacklight();
}

void usrMenuMain(void) {
	if (usrTimeoutLoop() == true) {
		usrNextState(MENU_POWER_SAVE_SHOW);
	}
	if (btnPressed() == true) {
		btnReset();
		usrNextState(MENU_SELECT_AMP_FREQ_WAVE_SHOW);
	}
	if(extIsWifiAddressChanged() == true) {
		lcdSetCursor(0, 0);
		lcdPrint(extGetWifiAddress());
		lcdFillSpacesToEndOfTheLine();
	}
}

void usrMenuSelectAmpFreqWaveShow(void) {
	lcdClear();
	static const char * menuItems[] = {
			STR_MENU_RETURN,
			STR_MENU_FREQUENCY,
			STR_MENU_WAVE,
			STR_MENU_PRESETS
	};
	usrMenuShow4(1, menuItems);
	usrNextState(MENU_SELECT_AMP_FREQ_WAVE);
}

void usrMenuSelectAmpFreqWave(void) {
	uint8_t menuSelection = usrGetMenuSelection();
	switch (menuSelection) {
		// Return
		case 1 : {
			usrNextState(MENU_MAIN_SHOW);
			break;
		}
		// Frequency
		case 2 : {
			usrNextState(MENU_EDIT_FREQ_SHOW);
			break;
		}
		// Wave
		case 3 : {
			usrNextState(MENU_EDIT_WAVE_SHOW);
			break;
		}
		// Presets
		case 4 : {
			usrNextState(MENU_LOAD_SAVE_SHOW);
			break;
		}
		default : {
			// button not pressed
		}
	}
}

void usrMenuLoadSaveShow(void) {
	lcdClear();
	static const char * menuItems[] = {
			STR_MENU_LOAD,
			STR_MENU_SAVE,
	};
	usrMenuShow2(1, menuItems);
	usrNextState(MENU_LOAD_SAVE_SELECT);

}

void usrMenuLoadSaveSelect(void) {
	uint8_t menuSelection = usrGetMenuSelection();
	switch (menuSelection) {
		// Load
		case 1 : {
			_loadSave = PRESET_LOAD;
			usrNextState(MENU_PRESETS_SHOW);
			break;
		}
		// Save
		case 2 : {
			_loadSave = PRESET_SAVE;
			usrNextState(MENU_PRESETS_SHOW);
			break;
		}
		default : {
			// button not pressed
		}
	}
}

void usrMenuPresetsShow(void) {
	lcdClear();
	static const char * menuItems[] = {
			STR_MENU_PRESET1,
			STR_MENU_PRESET2,
			STR_MENU_PRESET3,
			STR_MENU_PRESET4
	};
	usrMenuShow4(1, menuItems);
	usrNextState(MENU_PRESETS_SELECT);
}

void usrMenuPresetsSelect(void) {
	uint8_t menuSelection = usrGetMenuSelection();
	if (menuSelection > 0) {
		switch (_loadSave) {
			case PRESET_LOAD : {
				UIWaveType waveType = UI_SIG_NONE;
				uint64_t frequency = 0;
				settingsGetPreset(menuSelection, &waveType, &frequency);
				usrFrequencyToSignalGeneratorParams(frequency, &_signalGeneratorParams);
				_signalGeneratorParams.waveType = waveType;
				_parametersChanged = true;
				break;
			}
			case PRESET_SAVE : {
#ifndef KMSG_NO_EEPROM
				uint64_t frequency = usrSignalGeneratorParamsToFrequency(_signalGeneratorParams);
				settingsSavePreset(menuSelection, _signalGeneratorParams.waveType, frequency);
				break;
#endif
			}
		}
	usrNextState(MENU_MAIN_SHOW);
	}
}

void usrMenuFreqShow(void) {
	lcdClear();
	_freqSetupCurrentState = FS_CANCEL;
	lcdSetCursor(_freqSetupCurrentState, 0);
	lcdWrite(LCD_CURSOR_DOWN_NO);
	strSignalFrequencyToStr(_lcdStrBuffer,
		_signalGeneratorParams.frequencyHi,
		_signalGeneratorParams.frequencyLo);
	lcdSetCursor(FS_CANCEL, 1);
	lcdWrite(LCD_CURSOR_LEFT_NO);
	lcdWrite(strWaveTypeToSingleChar(_signalGeneratorParams.waveType));
	lcdWrite(STR_FREQUENCY_SHORT_CHAR);
	lcdPrint(_lcdStrBuffer); // formatted signal frequency
	lcdPrint(strSignalMultiplierToStr(_signalGeneratorParams.multiplier));
	lcdSetCursor(FS_APPLY, 1);
	lcdWrite(LCD_CHAR_CHECK_MARK_NO);
	usrNextState(MENU_EDIT_FREQ_SELECT);
	_signalGeneratorParamsOnEdit = _signalGeneratorParams;
}

void updateMainMenuIfOnScreen(void) {
	if (_menuState == MENU_MAIN) {
		usrMenuMainShow();
	}
}

void usrSetFrequency(uint64_t frequency) {
	// recalculate frequency to on-screen structure
	usrFrequencyToSignalGeneratorParams(frequency, &_signalGeneratorParamsTmp);
	// copy wave type from current settings
	_signalGeneratorParamsTmp.waveType = _signalGeneratorParams.waveType;
	// copy into current value
	_signalGeneratorParams = _signalGeneratorParamsTmp;
	// mark parameters as changed so new value will be passed to signal generator
	_parametersChanged = true;
	updateMainMenuIfOnScreen();
}

void usrSetWaveType(UIWaveType waveType) {
	_signalGeneratorParams.waveType = waveType;
	_parametersChanged = true;
	updateMainMenuIfOnScreen();
}

void usrMenuFreqSelect(void) {
	if (btnPressed() == true) {
		btnReset();
		switch (_freqSetupStates[_freqSetupCurrentState]) {
			case FS_CANCEL : {
				static const char * menuItems[] = { STR_YES, STR_NO };
				usrMenuShow2WithText(2, STR_CANCEL, menuItems);
				usrNextState(MENU_EDIT_FREQ_CANCEL);
				break;
			}
			case FS_APPLY : {
				static const char * menuItems[] = { STR_YES, STR_NO };
				usrMenuShow2WithText(1, STR_APPLY, menuItems);
				uint64_t freq = sgCalcNearestFreq(usrSignalGeneratorParamsToFrequency(_signalGeneratorParamsOnEdit));
				usrFrequencyToSignalGeneratorParams(freq, &_signalGeneratorParamsTmp);
				strSignalFrequencyToStr(_lcdStrBuffer, _signalGeneratorParamsTmp.frequencyHi, _signalGeneratorParamsTmp.frequencyLo);
				lcdSetCursor(0, 1);
				lcdWrite('~');
				lcdPrint(_lcdStrBuffer);
				lcdPrint(strSignalMultiplierToStr(_signalGeneratorParamsTmp.multiplier));
				usrNextState(MENU_EDIT_FREQ_APPLY);
				break;
			}
			default : {
				lcdSetCursor(_freqSetupStates[_freqSetupCurrentState], 1);
				lcdBlink();
				usrNextState(MENU_EDIT_FREQ);
			}
		}
	}

	const int8_t rseCurrentValue = rseGetLastChangeAndReset();
	if (rseCurrentValue != 0) {
		lcdSetCursor(_freqSetupStates[_freqSetupCurrentState], 0);
		lcdWrite(STR_MENU_EMPTY_CHAR);
		_freqSetupCurrentState = usrRotValue(_freqSetupCurrentState + rseCurrentValue, FS_BOF, FS_EOF);
		lcdSetCursor(_freqSetupStates[_freqSetupCurrentState], 0);
		lcdWrite(STR_MENU_SELECTOR_ALT_CHAR);
	}
}

void usrMenuFreqEdit(void) {
	if (btnPressed() == true) {
		btnReset();
		lcdNoBlink();
		usrNextState(MENU_EDIT_FREQ_SELECT);
	}
	const int8_t rseCurrentValue = rseGetLastChangeAndReset();
	if (rseCurrentValue != 0) {
		lcdSetCursor(_freqSetupStates[_freqSetupCurrentState], 1);
		int16_t frequencyHi = _signalGeneratorParamsOnEdit.frequencyHi;
		int16_t frequencyLo = _signalGeneratorParamsOnEdit.frequencyLo;
		switch (_freqSetupStates[_freqSetupCurrentState]) {
			case FS_WAVE_TYPE : {
				_signalGeneratorParamsOnEdit.waveType = (UIWaveType)
						usrRotValue(_signalGeneratorParamsOnEdit.waveType + rseCurrentValue, UI_SIG_SQUARE, UI_SIG_NONE);
				lcdWrite(strWaveTypeToSingleChar(_signalGeneratorParamsOnEdit.waveType));
				break;
			}
			case FS_MULT : {
				_signalGeneratorParamsOnEdit.multiplier = (FreqMultiplier)
						usrRotValue(_signalGeneratorParamsOnEdit.multiplier + rseCurrentValue, STR_MULT_NONE, STR_MULT_MEGA);
				lcdPrint(strSignalMultiplierToStr(_signalGeneratorParamsOnEdit.multiplier));
				break;
			}
			case FS_1000 : {
				frequencyHi += rseCurrentValue * 1000;
				break;
			}
			case FS_100 : {
				frequencyHi += rseCurrentValue * 100;
				break;
			}
			case FS_10 : {
				frequencyHi += rseCurrentValue * 10;
				break;
			}
			case FS_1 : {
				frequencyHi += rseCurrentValue;
				break;
			}
			case FS_1_10 : {
				frequencyLo += rseCurrentValue * 1000;
				break;
			}
			case FS_1_100 : {
				frequencyLo += rseCurrentValue * 100;
				break;
			}
			case FS_1_1000 : {
				frequencyLo += rseCurrentValue * 10;
				break;
			}
			case FS_1_10000 : {
				frequencyLo += rseCurrentValue;
				break;
			}
		}
		if (frequencyLo < 0) {
			frequencyLo += 10000;
			frequencyHi--;
		}
		if (frequencyLo > 9999) {
			frequencyLo = 0;
			frequencyHi++;
		}
		if (frequencyHi < 0) {
			frequencyHi = 0;
		}
		if (frequencyHi > 999) {
			frequencyHi = 999;
		}
		if (_signalGeneratorParamsOnEdit.multiplier == STR_MULT_MEGA) {
			SignalGeneratorParams tmpGeneratorParams = _signalGeneratorParamsOnEdit;
			tmpGeneratorParams.frequencyHi = frequencyHi;
			tmpGeneratorParams.frequencyLo = frequencyLo;

			uint64_t freq = usrSignalGeneratorParamsToFrequency(tmpGeneratorParams);
			if (freq > (uint64_t)SG_MAX_FREQ * (uint64_t)1000ULL) {
				freq = (uint64_t)SG_MAX_FREQ * (uint64_t)1000ULL;
				usrFrequencyToSignalGeneratorParams(freq, &tmpGeneratorParams);
				frequencyHi = tmpGeneratorParams.frequencyHi;
				frequencyLo = tmpGeneratorParams.frequencyLo;
			}
		}
		if (_signalGeneratorParamsOnEdit.frequencyHi != (uint16_t)frequencyHi ||
			_signalGeneratorParamsOnEdit.frequencyLo != (uint16_t)frequencyLo) {
			strSignalFrequencyToStr(_lcdStrBuffer,
					frequencyHi,
					frequencyLo);
			lcdSetCursor(FS_1000, 1);
			lcdPrint(_lcdStrBuffer);
			_signalGeneratorParamsOnEdit.frequencyHi = frequencyHi;
			_signalGeneratorParamsOnEdit.frequencyLo = frequencyLo;
		}
	lcdSetCursor(_freqSetupStates[_freqSetupCurrentState], 1);
	}
}

void usrMenuFreqApply(void) {
	switch (usrGetMenuSelection()) {
		case 1 : {
#ifdef KMSG_SHOW_APPROX_FREQ
			_signalGeneratorParams = _signalGeneratorParamsOnEdit;
#else
			SignalGeneratorParams signalRecalcParameters = _signalGeneratorParamsOnEdit;
			uint64_t freq = sgCalcNearestFreq(usrSignalGeneratorParamsToFrequency(signalRecalcParameters));
			usrFrequencyToSignalGeneratorParams(freq, &signalRecalcParameters);
			_signalGeneratorParams = signalRecalcParameters;
#endif
			_parametersChanged = true;
			usrNextState(MENU_MAIN_SHOW);
			break;
		}
		case 2 : {
			usrNextState(MENU_EDIT_FREQ_SHOW);
			break;
		}
	}
}

void usrMenuFreqCancel(void) {
	uint8_t menuSelection = usrGetMenuSelection();

	switch (menuSelection) {
		// Yes selected
		case 1 : {
			usrNextState(MENU_MAIN_SHOW);
			break;
		}
		// No selected
		case 2 : {
			usrNextState(MENU_EDIT_FREQ_SHOW);
			break;
		}
	}
}

void usrMenuWaveShow(void) {
	lcdClear();
	btnReset();
	static const char * menuItems[] = {
			STR_MENU_WAVE_SQUARE,
			STR_MENU_WAVE_SINE,
			STR_MENU_WAVE_TRIANGLE,
			STR_MENU_WAVE_NONE
	};
	// implicit conversion from waveType to menu number
	usrMenuShow4(_signalGeneratorParams.waveType, menuItems);
	usrNextState(MENU_EDIT_WAVE);
}

void usrMenuWave(void) {
	const uint8_t menuSelection = usrGetMenuSelection();
	switch (menuSelection) {
		// Return
		case 1 : {
			_signalGeneratorParams.waveType = UI_SIG_SQUARE;
			break;
		}
		// Frequency
		case 2 : {
			_signalGeneratorParams.waveType = UI_SIG_SINE;
			break;
		}
		// Amplification
		case 3 : {
			_signalGeneratorParams.waveType = UI_SIG_TRIANGLE;
			break;
		}
		// Wave
		case 4 : {
			_signalGeneratorParams.waveType = UI_SIG_NONE;
			break;
		}
	}
	if (menuSelection != 0) {
		usrNextState(MENU_MAIN_SHOW);
		_parametersChanged = true;
	}
}

void usrMenuDispatcherLoop(void) {
	switch (_menuState) {
		case MENU_INIT: {
			usrNextState(MENU_SPLASH);
			break;
		}
		case MENU_POWER_SAVE_SHOW: {
			usrMenuPowerSaveShow();
			break;
		}
		case MENU_POWER_SAVE: {
			usrMenuPowerSave();
			break;
		}
		case MENU_SPLASH: {
			usrMenuSplash();
			break;
		}
		case MENU_SPLASH_WAIT: {
			usrMenuSplashWait();
			break;
		}
		case MENU_MAIN_SHOW: {
			usrMenuMainShow();
			break;
		}
		case MENU_MAIN: {
			usrMenuMain();
			break;
		}
		case MENU_SELECT_AMP_FREQ_WAVE_SHOW: {
			usrMenuSelectAmpFreqWaveShow();
			break;
		}
		case MENU_SELECT_AMP_FREQ_WAVE: {
			usrMenuSelectAmpFreqWave();
			break;
		}
		case MENU_LOAD_SAVE_SHOW: {
			usrMenuLoadSaveShow();
			break;
		}
		case MENU_LOAD_SAVE_SELECT: {
			usrMenuLoadSaveSelect();
			break;
		}
		case MENU_PRESETS_SHOW: {
			usrMenuPresetsShow();
			break;
		}
		case MENU_PRESETS_SELECT: {
			usrMenuPresetsSelect();
			break;
		}
		case MENU_EDIT_FREQ_SHOW: {
			usrMenuFreqShow();
			break;
		}
		case MENU_EDIT_FREQ_SELECT: {
			usrMenuFreqSelect();
			break;
		}
		case MENU_EDIT_FREQ: {
			usrMenuFreqEdit();
			break;
		}
		case MENU_EDIT_FREQ_APPLY: {
			usrMenuFreqApply();
			break;
		}
		case MENU_EDIT_FREQ_CANCEL: {
			usrMenuFreqCancel();
			break;
		}
		case MENU_EDIT_WAVE_SHOW: {
			usrMenuWaveShow();
			break;
		}
		case MENU_EDIT_WAVE: {
			usrMenuWave();
			break;
		}
	}
}

void usrLoop(void) {
	rseLoop();
	btnLoop();
	usrMenuDispatcherLoop();
	if (usrIsParametersChangedAndReset() == true) {
		SgWaveType waveType = SG_SIG_NONE;
		switch (usrGetWaveType()) {
			case UI_SIG_SINE : {
				waveType = SG_SIG_SINE;
				break;
			}
			case UI_SIG_SQUARE : {
				waveType = SG_SIG_SQUARE;
				break;
			}
			case UI_SIG_TRIANGLE : {
				waveType = SG_SIG_TRIANGLE;
				break;
			}
			default : {
				waveType = SG_SIG_NONE;
			}
		}
		setGeneratorParameters(usrGetCurrentFrequency(), waveType);
	}
}

#endif /* USERINTERFACE_C_ */
