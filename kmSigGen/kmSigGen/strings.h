/** @file
 * @brief Contains string definitions for application.
 * strings.h
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

#include "config.h"

#ifndef STRINGS_H_
#define STRINGS_H_

#include KMSG_LOCALE

#define STR_HZ  " Hz"
#define STR_KHZ "kHz"
#define STR_MHZ "MHz"

#define STR_FREQUENCY_SHORT "@"
#define STR_FREQUENCY_SHORT_CHAR '@'

#define STR_NONE_SHORT "-"
#define STR_SQUARE_SHORT "\5"
#define STR_SINE_SHORT "\7"
#define STR_TRIANGLE_SHORT "\6"

#define STR_MENU_SELECTOR_CHAR LCD_CURSOR_RIGHT_NO
#define STR_MENU_SELECTOR_ALT_CHAR LCD_CURSOR_DOWN_NO
#define STR_MENU_EMPTY_CHAR 0x20
#define STR_MENU_EMPTY ""

#endif /* STRINGS_H_ */
