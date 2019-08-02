/** @file
 * @brief Functions for slave TWI/I2C operations.
 * TWISlave.h
 * 
 *  Created on: Jul 10, 2019
 *      Author: Krzysztof Moskwa
 * 	    License: GPL-3.0-or-later
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
 *  -# https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/Wire
 *  -# https://www.microchip.com/webdoc/AVRLibcReferenceManual/ch20s33s01.html
 *  -# https://ww1.microchip.com/downloads/en/DeviceDoc/Microchip%208bit%20mcu%20AVR%20ATmega8A%20data%20sheet%2040001974A.pdf
 */

#ifndef TWISLAVE_H_
#define TWISLAVE_H_

#ifndef KMSG_NO_TWI
#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

volatile uint8_t buffer_address;
volatile uint8_t rxbuffer[0xFF];

/** 
This function needs to be called only once to set up the TWI to respond to the address passed into the function.
@param address TWI address of the slave device
*/
void twiInit(uint8_t address);

/**
This function disables the TWI peripheral completely and therefore isolates the device from the bus.
*/
void twiStop(void);

/**
Returns true in case new data is available in the buffer.
@result true in case data has been received and is available from the buffer 
*/
bool twiIsDataInBuffer(void);

/**
Returns next data from the buffer. First it should be checked if data is available using twiIsDatainBuffer function.
@result data from buffer
*/
uint32_t twiGetDataFromBuffer(void);

/**
Sets response to the command so it will be returned to master.
@param response response word to be sent to master device
*/
void twiPutDataResponse(uint32_t response);

// Interrupt vector
ISR(TWI_vect);

#endif

#endif /* TWISLAVE_H_ */
