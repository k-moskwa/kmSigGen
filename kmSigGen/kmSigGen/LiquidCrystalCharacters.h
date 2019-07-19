/*
 * LiquidCrystalCharacters.h
 *
 *  Created on: Jul 11, 2019
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

#ifndef LIQUIDCRYSTALCHARACTERS_H_
#define LIQUIDCRYSTALCHARACTERS_H_

static const uint8_t LCD_CURSOR_RIGHT_NO	= 0x01;
static const uint8_t LCD_CURSOR_RIGHT[] 	= { 16,24,28,30,28,24,16,32 } ;
static const uint8_t LCD_CURSOR_LEFT_NO		= 0x02;
static const uint8_t LCD_CURSOR_LEFT[] 		= { 1,3,7,15,7,3,1,32 };
static const uint8_t LCD_CURSOR_DOWN_NO		= 0x03;
static const uint8_t LCD_CURSOR_DOWN[] 		= { 32,32,32,32,32,31,14,4 };
static const uint8_t LCD_CHAR_CHECK_MARK_NO	= 0x04;
static const uint8_t LCD_CHAR_CHECK_MARK[] 	= { 1,1,2,2,20,20,8,8 };
static const uint8_t LCD_WAVE_SQUARE_NO		= 0x05;
static const uint8_t LCD_WAVE_SQUARE[]		= { 28,20,20,20,5,5,5,7 };
static const uint8_t LCD_WAVE_TRIANGLE_NO	= 0x06;
static const uint8_t LCD_WAVE_TRIANGLE[] 	= { 8,8,8,21,21,2,2,2 };
static const uint8_t LCD_WAVE_SINE_NO		= 0x07;
static const uint8_t LCD_WAVE_SINE[] 		= { 8,20,20,20,5,5,5,2 };
static const uint8_t LCD_WAVE_NONE_NO		= '-';

#endif /* LIQUIDCRYSTALCHARACTERS_H_ */
