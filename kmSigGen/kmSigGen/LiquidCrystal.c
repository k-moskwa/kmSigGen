/*
 * LiquidCrystal.c
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

#include "LiquidCrystal.h"

#ifndef _TESTS_ENV
#include <avr/io.h>
#include <util/delay.h>
#endif

#include "config.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00

// flags for back light control
#ifndef KMSG_ATB
#define LCD_BACKLIGHT   _BV(PIN3)
#else
#define LCD_BACKLIGHT   _BV(PIN7)
#endif
#define LCD_NOBACKLIGHT 0x00


#define LCD_PIN_EN PIN2
#define LCD_PIN_RW PIN1
#define LCD_PIN_RS PIN0

#define En _BV(LCD_PIN_EN)  // Enable byte value
#define Rw _BV(LCD_PIN_RW)  // Read/Write byte value
#define Rs _BV(LCD_PIN_RS)  // Register select byte value

static uint8_t _displayFunction = 0;
static uint8_t _displayControl = 0;
static uint8_t _displayMode = 0;
static uint8_t _cols = 0;
static uint8_t _rows = 0;
static uint8_t _charSize = 0;
static uint8_t _backlightVal = 0;
static uint8_t _spaceToEndOfTheLine = 0;


// Private functions
void lcdSend(uint8_t value, uint8_t mode);
void lcdWrite4bits(uint8_t value);
void lcdPortWriteAndEnable(uint8_t data);
void lcdPortWrite(uint8_t data);

// Implementation
/**
 * Low level function. Sends 8 bit instruction or data to the LCD.
 * It splits the value into two 4 bit nibs and sends it via lcdWrite4Bits function
 * @param value - 8 bit instruction or data
 * @param mode it's either 0 (command) or Rs (1, instruction)
 * defining which mode is byte sent to the LCD
 */
void lcdSend(uint8_t value, uint8_t mode);

/**
 * Low level function. Sends 4 bit value to the LCD
 * @param value - 4 bit value to be sent to LCD
 */
void lcdWrite4bits(uint8_t value);

/**
 * Low level function sending specific data to the LCD. It sends the data value
 * first enabling EN bit of the LCD, then the same data is sent with EN bit disabled
 * @param data - data value to be sent to the LCD port with lcdPortWrite method
 */
void lcdPortWriteAndEnable(uint8_t data);

/**
 * Lowest level function writing provided data to the LCD port
 * It combines the value with the current backlight state before sending it
 * @param data - data value to be sent to the LCD port cobined with current backlight state
 */
void lcdPortWrite(uint8_t data);

void lcdInit(uint8_t lcdCols, uint8_t lcdRows, uint8_t charSize) {
	_cols = lcdCols;
	_rows = lcdRows;
	_charSize = charSize;
	_backlightVal = LCD_BACKLIGHT;
	// init rest of variables;
	_displayFunction = 0;
	_displayControl = 0;
	_displayMode = 0;
}

void lcdBegin(void) {
	// Use all pins of port (from config.h) and reset them to 0
	LCD_DDR = 0xFF;
	LCD_PORT = 0x00;
	_displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1) {
		_displayFunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charSize != 0) && (_rows == 1)) {
		_displayFunction |= LCD_5x10DOTS;
	}
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to data sheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. MCU can turn on way buffer 4.5V so we'll wait 50
	_delay_ms(50);
	// Now we pull both RS and R/W low to begin commands
	_backlightVal = LCD_NOBACKLIGHT;
	lcdPortWrite(0);	// reset expander and turn back light off
	_delay_us(1000);

	//put the LCD into 4 bit mode
	// this is according to the Hitachi HD44780 data sheet
	// figure 24, pg 46
	// we start in 8bit mode, try to set 4 bit mode
	lcdWrite4bits(0x03 << 4);
	_delay_us(4200); // wait min 4.1ms
	// second try
	lcdWrite4bits(0x03 << 4);
	_delay_us(100);
	// third go!
	lcdWrite4bits(0x03 << 4);
	_delay_us(100);
	// finally, set to 4-bit interface
	lcdWrite4bits(0x02 << 4);
	// set # lines, font size, etc.
	lcdCommand(LCD_FUNCTIONSET | _displayFunction);
	// turn the display on with no cursor or blinking default
	_displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcdDisplay();
	// clear it off
	lcdClear();
	// Initialize to default text direction (for roman languages)
	_displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcdCommand(LCD_ENTRYMODESET | _displayMode);
	lcdHome();
}

void lcdClear() {
	lcdCommand(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	_spaceToEndOfTheLine = _cols;
	_delay_ms(2);  // this command takes a long time!

#ifdef _TESTS_ENV
	for (int i = 0; i < LCD_COLS * LCD_ROWS; i++) {
		_lcdTestBuff[i] = 0;
	}
#endif
}

void lcdHome() {
	lcdCommand(LCD_RETURNHOME);  // set cursor position to zero
	_spaceToEndOfTheLine = _cols;
	_delay_ms(2);  // this command takes a long time!
}

void lcdNoDisplay() {
	_displayControl &= ~LCD_DISPLAYON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdDisplay() {
	_displayControl |= LCD_DISPLAYON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdNoBlink() {
	_displayControl &= ~LCD_BLINKON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdBlink() {
	_displayControl |= LCD_BLINKON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdNoCursor() {
	_displayControl &= ~LCD_CURSORON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdCursor() {
	_displayControl |= LCD_CURSORON;
	lcdCommand(LCD_DISPLAYCONTROL | _displayControl);
}

void lcdScrollDisplayLeft() {
	lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcdScrollDisplayRight() {
	lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void lcdLeftToRight() {
	_displayMode |= LCD_ENTRYLEFT;
	lcdCommand(LCD_ENTRYMODESET | _displayMode);
}

void lcdRightToLeft() {
	_displayMode &= ~LCD_ENTRYLEFT;
	lcdCommand(LCD_ENTRYMODESET | _displayMode);
}

void lcdNoBacklight() {
	_backlightVal = LCD_NOBACKLIGHT;
	lcdPortWrite(0);
}

void lcdBacklight() {
	_backlightVal = LCD_BACKLIGHT;
	lcdPortWrite(0);
}

void lcdAutoscroll() {
	_displayMode |= LCD_ENTRYSHIFTINCREMENT;
	lcdCommand(LCD_ENTRYMODESET | _displayMode);
}

void lcdNoAutoscroll() {
	_displayMode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcdCommand(LCD_ENTRYMODESET | _displayMode);
}

void lcdCreateChar(uint8_t location , const uint8_t charMap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	lcdCommand(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i < 8; i++) {
		lcdWrite(charMap[i]);
	}
}

void lcdSetCursor(uint8_t col, uint8_t row) {
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows) {
		row = _rows-1;    // we count rows starting w/0
	}
	lcdCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
	_spaceToEndOfTheLine = _cols - col;
#ifdef _TESTS_ENV
	_lcdCurrentPos = col + row * _cols; 
#endif
}

void lcdBlinkOn() {
	lcdBlink();
}

void lcdBlinkOff() {
	lcdNoBlink();
}

void lcdCursorOn() {
	lcdCursor();
}

void lcdCursorOff() {
	lcdNoCursor();
}

void lcdPrintstr(const char *str) {
	lcdPrint(str);
}

void lcdFillSpacesToEndOfTheLine(void) {
	uint8_t loops = _spaceToEndOfTheLine;
	for (int i = 0; i < loops; i++) {
		lcdWrite(' ');
	}
}

void lcdPrint(const char *str) {
	uint8_t i = 0;
	while (str[i] != '\0') {
		lcdWrite(str[i++]);
	}
}

// mid level commands, for sending data/cmds
void lcdCommand(uint8_t value) {
#ifndef _TESTS_ENV
	lcdSend(value, 0);
#endif
}

void lcdWrite(uint8_t value) {
#ifndef _TESTS_ENV
	lcdSend(value, Rs);
	_spaceToEndOfTheLine--;
#else
	_lcdTestBuff[_lcdCurrentPos++] = value;
#endif
}

// low level data pushing commands
void lcdSend(uint8_t value, uint8_t mode) {
	uint8_t highNib =  value       & 0xf0;
	uint8_t lowNib  = (value << 4) & 0xf0;
	lcdWrite4bits((highNib) | mode);
	lcdWrite4bits((lowNib) | mode);
}

void lcdWrite4bits(uint8_t value) {
	lcdPortWriteAndEnable(value);
}

void lcdPortWriteAndEnable(uint8_t data) {
	lcdPortWrite(data | En);	// En high
	_delay_us(70);		// enable pulse must be >450ns (doesn't work if this value is less than 70)

	lcdPortWrite(data & ~En);	// En low
	_delay_us(1);		// commands need > 37us to settle
}

#ifndef KMSG_ATB
void lcdPortWrite(uint8_t data) {
	LCD_PORT = data | _backlightVal;
}
#else
void lcdPortWrite(uint8_t data) {
        uint8_t newData = (data & 0xF0) >> 1;
        newData |= data &0x0F;
        LCD_PORT = newData | _backlightVal;
}
#endif
