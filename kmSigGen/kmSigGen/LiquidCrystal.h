/*
 * LiquidCrystal.h
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
 * 
 *      4 bit connection over single AVR port to the 6102 16x2 display
 *      PIN7  = D7
 *      PIN6  = D6
 *      PIN5  = D5
 *      PIN4  = D4
 *      PIN3  = A (optional - back light pin to be connected via resistor)
 *      PIN2  = En
 *      PIN1  = Rw
 *      PIN0  = Rs
 *      D0, D1, D2, D3, K, Vss - connected to GND
 *      Vdd - to +5V
 *      Vee - to 10k var resistor
 *
 *  References:
 * http://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
 * https://www.openhacks.com/uploadsproductos/eone-1602a1.pdf
 * https://github.com/arduino-libraries/LiquidCrystal
 * https://github.com/agnunez/ESP8266-I2C-LCD1602
 * https://github.com/sleemanj/LiquidCrystal_I2C_DFR
 */

#include <stdbool.h>
#include <stdint.h>


#ifndef LIQUIDCRYSTAL_H_
#define LIQUIDCRYSTAL_H_

#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/**
Initializes routines and variable responsible for LCD1602 liquid crystal display
Following definitions to be set in config.h file
#define LCD_DDR data direction register for LCD port (e.g. DDRD)
#define LCD_PORT LCD port (e.g. PORTD)
recommended definitions to provide as function inputs
#define LCD_COLS 16
#define LCD_ROWS 2
@param lcd_cols	Number of columns your LCD display has.
@param lcd_rows	Number of rows your LCD display has.
@param charSize	The size in dots that the display has, use LCD_5x10DOTS or LCD_5x8DOTS.
*/
void lcdInit(uint8_t lcdCols, uint8_t lcdRows, uint8_t charSize);

/**
Set the LCD display in the correct begin state, must be called before anything else is done.
Uses port and DDR defined in config.h file (e.g.
#define PORT_LCD PORTD
#define DDR_LCD DDRD
*/
void lcdBegin(void);

/**
Remove all the characters currently shown. Next print/write operation will start
from the first position on LCD display.
*/
void lcdClear();

/**
 Next print/write operation will will start from the first position on the LCD display.
*/
void lcdHome();

/**
Do not show any characters on the LCD display. Backlight state will remain unchanged.
Also all characters written on the display will return, when the display in enabled again.
*/
void lcdNoDisplay();

/**
Show the characters on the LCD display, this is the normal behaviour. This method should
only be used after noDisplay() has been used.
*/
void lcdDisplay();

/**
Do not blink the cursor indicator.
*/
void lcdNoBlink();

/**
Start blinking the cursor indicator.
*/
void lcdBlink();

/**
Do not show a cursor indicator.
*/
void lcdNoCursor();

/**
Show a cursor indicator, cursor can blink on not blink. Use the
methods blink() and noBlink() for changing cursor blink.
*/
void lcdCursor();

/**
Scroll content of the display to the left
*/
void lcdScrollDisplayLeft();

/**
Scroll content of the display to the right
*/
void lcdScrollDisplayRight();

/**
Set mode of the display to left to right (default)
*/
void lcdLeftToRight();

/**
Set mode of the display to right to left
*/
void lcdRightToLeft();

/**
Disable back light of the LCD screen
*/
void lcdNoBacklight();

/**
Enable back light of the LCD screen
*/
void lcdBacklight();

/**
Enable auto-scroll
*/
void lcdAutoscroll();

/**
Disable auto-scroll
*/
void lcdNoAutoscroll();

/**
Define custom character at specific location (from 0x00 to 0x07)
With the shape defined in charMap array
@param location - location of the character
@param charMap - array of bytes which defines specific character
*/
void lcdCreateChar(uint8_t location, const uint8_t charMap[]);

/**
Set position of the cursor on place defined by col and row parameters
@param col - column (X) position of the cursor
@param row - row (Y) position of the cursor
*/
void lcdSetCursor(uint8_t col, uint8_t row);

/**
Write single character at the current cursor location
@param value - code of the character to be printed
*/
void lcdWrite(uint8_t value);

/**
Send raw LCD command
@param value - command value to be send to LCD
*/
void lcdCommand(uint8_t value);

/**
Enable blink of the cursor
*/
void lcdBlinkOn();

/**
Disable blink of the cursor
*/
void lcdBlinkOff();

/**
Enable showing cursor
*/
void lcdCursorOn();

/**
Disable showing cursor
*/
void lcdCursorOff();

/**
Auxiliary method filling with spaces from the
current cursor position to the end of the screen
*/
void lcdFillSpacesToEndOfTheLine(void);

/**
Print string from the character array
@param str - string to be printed
*/
void lcdPrint(const char *str);

#endif /* LIQUIDCRYSTAL_H_ */
