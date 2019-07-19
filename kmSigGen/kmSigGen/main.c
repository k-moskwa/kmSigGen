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


#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "config.h"
#include "LiquidCrystal.h"
#include "TWISlave.h"
#include "Debug.h"
#include "UserInterface.h"
#include "SignalGeneratorAD9833.h"
#include "ExternalInterface.h"
#include "Settings.h"

int main(void) {
	dbPullUpAllPorts();
	dbInit(DEBUG_STEPS);
	usrInit(DEFAULT_FREQUENCY, UI_SIG_SQUARE);
	sgInit();
	setGeneratorParameters(DEFAULT_FREQUENCY, SG_SIG_SQUARE);

#ifndef KMSG_NO_TWI
	sei(); // enable global interrupt for TWI/I2C
	twiInit(TWI_SLAVE_ADDRESS);
#endif

	settingsInit();

	// Main Loop
	while (true) {
		usrLoop();
		extLoop();
		dbStep();
		_delay_ms(1);
	}
}
