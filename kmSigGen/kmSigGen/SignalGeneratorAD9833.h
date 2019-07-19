/*
 * SignalGeneratorAD9833.h
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
 * https://www.analog.com/media/en/technical-documentation/data-sheets/ad9833.pdf
 * https://www.analog.com/media/en/technical-documentation/application-notes/AN-1070.pdf
 * https://github.com/Billwilliams1952/AD9833-Library-Arduino
 * https://github.com/tuomasnylund/function-gen
 * http://www.vwlowen.co.uk/arduino/AD9833-waveform-generator/AD9833-waveform-generator.htm
 * https://www.instructables.com/id/Signal-Generator-AD9833/
 * https://ww1.microchip.com/downloads/en/DeviceDoc/Microchip%208bit%20mcu%20AVR%20ATmega8A%20data%20sheet%2040001974A.pdf
 */

#ifndef SIGNALGENERATORAD9833_H_
#define SIGNALGENERATORAD9833_H_
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

// AD9833 maximum frequency
#define SG_MAX_FREQ SG_MCLK / 2

// Definition of wave types for Signal Generator
typedef enum {
	// Square Wave Type 5V p-p
	SG_SIG_SQUARE = 1,
	// Sine Wave Type 0.6V p-p
	SG_SIG_SINE = 2,
	// Triangle Wave Type 0.6V p-p
	SG_SIG_TRIANGLE = 3,
	// No output signal (signal level from last out value)
	SG_SIG_NONE = 0
} SgWaveType;

/**
Initialize and disable analog output of Signal Generator
Following definitions to be set in config.h file
#define SG_DDR data direction register for communication with Signal Generator (e.g. DDRB)
#define SG_PORT port for communication with Signal Generator (e.g. PORTB)
#define SG_DD_SS FSYNC pin (e.g PB2)
#define SG_DD_MOSI	SDAT pin (e.g PB3)
#define SG_DD_MISO	optional (e.g PB4)
#define SG_DD_SCK 	SCK pin (e.g. PB5)
#define SG_MCLK AD9833 master clock in Hz (e.g. 25000000ULL for 25MHz)
#define SG_FREQ_COEF multiplier of the frequency provided to calcFreqReg and calcNearestFreq methods (e.g 1000ULL for *1000)
*/
void sgInit(void);

/**
Calculate FreqReg from requested Frequency
@param freq requested frequency multiplied by 1000 (1 Hx = 1000L, 1KHz = 1000000L 1MHz = 1000000000
@result 28bit value of frequency regulator
*/
uint32_t sgCalcFreqReg(uint64_t freq);

/**
Calculate Frequency from FreqReg
@param freqReg  28bit value of the frequency regulator
@result frequency calculated using CG_CLOCK
*/
uint64_t sgCalcFreq(uint32_t freqReg);

/**
Calculate real nearest frequency from requested frequency
For 25MHz clock the frequency step for AD9833 is 0.093132257 Hz
@param freq requested frequency (multiplied by 1000)
@result real frequency calculated from nearest available frequency (multiplied by 1000)
*/
uint64_t sgCalcNearestFreq(uint64_t requestedFreq);

/**
Sets frequency and Wave Type of the Signal Generator to provided values
NOTE: frequency is set to nearest possible frequency available in 
Signal Generator that is based on AD9833 master clock (as defined in SG_MCLK)
@param requestedFreq requested frequency to be set in Signal Generator
@param waveType wave type
*/
void setGeneratorParameters(uint64_t requestedFreq, SgWaveType waveType);

#endif /* SIGNALGENERATORAD9833_H_ */
