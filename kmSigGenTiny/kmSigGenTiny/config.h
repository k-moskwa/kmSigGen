/*
 * config.h
 *
 *  Created on: Jul 09, 2019
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

#ifndef CONFIG_H_
#define CONFIG_H_

#define CCP_SIGNATURE 0xD8
#define TINY_MCLK_8MHZ 0

#define SG_DDR		DDRB
#define SG_PORT		PORTB
#define SG_DD_SS	PB0
#define SG_DD_SCK 	PB1
#define SG_DD_MOSI	PB2
#define SG_SOFT_CLK_DELAY_US 1

#define BUTTON_DDR DDRB
#define BUTTON_PORT PORTB
#define BUTTON_PORT_IN PINB
#define BUTTON_PIN_0 PB0
#define BUTTON_PIN_1 PB1
#define BUTTON_PIN_2 PB2

// AD9833 master clock in Hz (25MHz default)
#define SG_MCLK 25000000ULL
// Multplier of the frequency provided to calcFreqReg and calcNearestFreq methods (*1000)
#define SG_FREQ_COEF 1000ULL
// Bits in word sent via SPI to AD9833 chip
#define AD9833_WORD_SIZE 16

// Default preset1 - 985 248 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET1 985248000
#define DEFAULT_WAVE_TYPE_PRESET1 SG_SIG_SQUARE
// Default preset2 - 1 022 727 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET2 1022727000
#define DEFAULT_WAVE_TYPE_PRESET2 SG_SIG_SQUARE
// Default preset3 - 7 881 984 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET3 7881984000
#define DEFAULT_WAVE_TYPE_PRESET3 SG_SIG_SQUARE
// Default preset4 - 8 181 816 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET4 8181816000
#define DEFAULT_WAVE_TYPE_PRESET4 SG_SIG_SQUARE
// Default preset5 - 1 843 200 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET5 1843200000
#define DEFAULT_WAVE_TYPE_PRESET5 SG_SIG_SQUARE
// Default preset6 - 3 686 400 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET6 3686400000
#define DEFAULT_WAVE_TYPE_PRESET6 SG_SIG_SQUARE
// Default preset7 - 7 372 800 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET7 7372800000
#define DEFAULT_WAVE_TYPE_PRESET7 SG_SIG_SQUARE
// Default preset8 -11 059 200 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET8 11059200000
#define DEFAULT_WAVE_TYPE_PRESET8 SG_SIG_SQUARE
// Default frequency - 1MHz / SQUARE
#define DEFAULT_FREQUENCY DEFAULT_FREQUENCY_PRESET1
#define DEFAULT_WAVE_TYPE DEFAULT_WAVE_TYPE_PRESET1

#define AD9833_COEF_BIN 28
#define AD9833_COEF _BV(AD9833_COEF_BIN)

#define CALC_FIX ((((uint64_t)SG_MCLK * (uint64_t)SG_FREQ_COEF) >> AD9833_COEF_BIN) >> 1)

#define FREQUENCY_REG_PRESET1 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET1 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET2 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET2 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET3 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET3 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET4 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET4 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET5 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET5 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET6 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET6 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET7 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET7 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)
#define FREQUENCY_REG_PRESET8 (uint32_t)(((((uint64_t)DEFAULT_FREQUENCY_PRESET8 << AD9833_COEF_BIN) / (uint64_t)SG_MCLK) + CALC_FIX ) / SG_FREQ_COEF)

#endif /* CONFIG_H_ */
