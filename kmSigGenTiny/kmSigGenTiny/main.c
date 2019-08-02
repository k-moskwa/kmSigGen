/*
 * main.c
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


#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "System.h"
#include "Buttons.h"
#include "Settings.h"
#include "SignalGeneratorAD9833.h"

int main(void) {
	// Set system clock frequency and source
	sysInit();
	// Initialize settings
	settingsInit();
	// Initialize reading from buttons (pull-up button pins)
	btnInit();
	// Get state of buttons and turn it into preset number
	uint8_t preset = btnGetState() + 1;
	// Wait until all buttons released so it's possible to transmit data to AD9833 over SPI
	btnWaitUntilReleased();
#ifdef NDEBUG
	// to make sure buttons were really released (software de-bouncing)
	_delay_ms(100); 
#endif
	// Initialize temp variables
	SgWaveType waveType = SG_SIG_NONE;
	uint32_t freqReg = 0;
	// Get frequency regulator for specific preset
	settingsGetPreset(preset, &waveType, &freqReg);
	// Initialize AD9833
	sgInit();
	// Set specific frequency from selected preset
	setGeneratorParameters(freqReg, waveType);
	// Set uC pins back as pull-up inputs
	btnInit();
	// no sei(); intentionally, the cpu just did what 
	// suppose to do, and is not necessary anymore
	sysPowerDown();

	return 0;
}
