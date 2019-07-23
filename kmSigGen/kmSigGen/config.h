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

// Enable one available localizations
#define KMSG_LOCALE "localeEn.h"
//#define KMSG_LOCALE "localePl.h"

// EEPROM definition of the ID/VER header
#define KMSG_MAGIC "KMSG100"
// last byte is '\0' so it's 8 bytes
#define KMSG_MAGIC_LENGTH 8
#define KMSG_MAX_PRESETS 5

// Definition of the TWI/I2C address (0xAD >> 1 ;-)
#define TWI_SLAVE_ADDRESS 0x56
#define TWI_BUFFER_LENGTH 8

// Port / pin definition for SPI (communication with AD9833)
#define SG_DDR DDRB
#define SG_PORT PORTB
#define SG_DD_SS	PB2
#define SG_DD_MOSI	PB3
#define SG_DD_MISO	PB4
#define SG_DD_SCK 	PB5

#define LCD_DDR DDRD
#define LCD_PORT PORTD
#define LCD_COLS 16
#define LCD_ROWS 2
#define STR_INTERNAL_BUFFERS_SIZE_OF LCD_COLS + 1

#define DEBUG_DDR DDRB
#define DEBUG_PORT PORTB
#define DEBUG_PIN PB1
#define DEBUG_STEPS 100

#define BUTTON_DDR DDRC
#define BUTTON_PORT_IN PINC
#define BUTTON_PIN PC1

#define RSE_DDR DDRC
#define RSE_PORT PORTC
#define RSE_PORT_IN PINC
#define RSE_PIN1 PC2
#define RSE_PIN2 PC3
#define RSE_TICKS_PER_SWITCH 4

// AD9833 master clock in Hz (25MHz default)
#define SG_MCLK 25000000ULL
// Multplier of the frequency provided to calcFreqReg and calcNearestFreq methods (*1000)
#define SG_FREQ_COEF 1000ULL

// Default frequency - 1MHz / SQUARE
#define DEFAULT_FREQUENCY 1000000000
#define DEFAULT_WAVE_TYPE UI_SIG_SQUARE
// Default preset1 - 985 248 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET1 985248000
#define DEFAULT_WAVE_TYPE_PRESET1 UI_SIG_SQUARE
// Default preset1 - 1 022 727 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET2 1022727000
#define DEFAULT_WAVE_TYPE_PRESET2 UI_SIG_SQUARE
// Default preset1 - 7 881 984 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET3 7881984000
#define DEFAULT_WAVE_TYPE_PRESET3 UI_SIG_SQUARE
// Default preset1 - 8181816 Hz / SQUARE
#define DEFAULT_FREQUENCY_PRESET4 8181816000
#define DEFAULT_WAVE_TYPE_PRESET4 UI_SIG_SQUARE

// screen saver enabled after 30 s on the main screen (comment line ot disable screen saver)
#define KMSG_SCREEN_SAVER_TIMEOUT 30000

// splash screen timeout 1.5s
#define KMSG_SPLASH_SCREEN_TIMEOUT 1500

// Misc/internal
// Disable using stdio (e.g. sprintf) and use alternative implementation
#define KMSG_NO_STDIO

// Disable using EEPROM memory routines (settings) and use predefined presets instead
//#define KMSG_NO_EEPROM

// Show approximate frequencies entered by user instead of real frequency from generator
#define KMSG_SHOW_APPROX_FREQ

// Disable TwoWire (I2C) routines so it's not possible to control module from external interface
//#define KMSG_NO_TWI

// Disable internal debug features
#define KMSG_NO_PIN_DEBUG

// In case of debug environment - disable some modules so application fits in FLASH space
// (use settings from above for release)
// NOTE: Do not change this part unless really necessary
#ifdef DEBUG
#define KMSG_NO_TWI
#define KMSG_NO_EEPROM
#endif

#endif /* CONFIG_H_ */
