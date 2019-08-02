/*
 * TWISlave.c
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
 * 
 */

#ifndef KMSG_NO_TWI
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include "config.h"
#include "TWISlave.h"

#define TWI_SDA_PIN PC4
#define TWI_SCL_PIN PC5

static uint32_t _twiRxBufferWords[TWI_BUFFER_LENGTH]; // round robin buffer
static volatile uint32_t _twiTxResponse = 0;
static volatile uint8_t _twiRxBufferWordsGetIndex = 0;
static volatile uint8_t _twiRxBufferWordsPutIndex = 0;
static volatile uint8_t _twiRxBufferWordsLength = 0;

static uint8_t _twiTxBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t _twiTxBufferIndex = 0;
static volatile uint8_t _twiTxBufferLength = 0;

static uint8_t _twiRxBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t _twiRxBufferIndex = 0;

// private functions
void twiTransmit(const uint8_t *data, uint8_t length);
void twiOnSlaveReceive(uint8_t *data, uint8_t length);
void twiOnSlaveTransmit(void);
void twiReleaseBus(void);
void twiTransmit(const uint8_t *data, uint8_t length);
void twiReply(bool ack);

// Implementation
bool twiIsDataInBuffer(void) {
	return (_twiRxBufferWordsLength > 0);
}

uint32_t twiGetDataFromBuffer(void) {
	uint32_t result = 0;
	if (_twiRxBufferWordsLength > 0) {
		result = _twiRxBufferWords[_twiRxBufferWordsGetIndex++];
		if (_twiRxBufferWordsGetIndex >= TWI_BUFFER_LENGTH) {
			_twiRxBufferWordsGetIndex = 0;
		}
		_twiRxBufferWordsLength--;
	}
	return result;
}

void twiPutDataResponse(uint32_t response) {
	_twiTxResponse = response;
}

void twiOnSlaveReceive(uint8_t *data, uint8_t length) {
	if (length % 4 == 0) { // accept only values 4 byte length
		uint8_t wordsReceived = length / 4;
		uint8_t bufferPos = 0;
		uint32_t word;
		for (int i = 0; i < wordsReceived; i++) {
			// MSB first
			word = data[bufferPos++];
			word <<= 8;
			word |= data[bufferPos++];
			word <<= 8;
			word |= data[bufferPos++];
			word <<= 8;
			// LSB last
			word |= data[bufferPos++];
			_twiRxBufferWords[_twiRxBufferWordsPutIndex++] = word;
			if (_twiRxBufferWordsPutIndex >= TWI_BUFFER_LENGTH) {
				_twiRxBufferWordsPutIndex = 0; // start from the beginning of round robin buffer
			}
			_twiRxBufferWordsLength++;
		}
	}
}

void twiOnSlaveTransmit(void) {
	uint8_t data[4];
	for (int i = 0; i < 4; i++) {
		data[i] = _twiTxResponse & 0xff;
		_twiTxResponse >>= 8;
	}
	twiTransmit(data, 4);
}

void twiInit(uint8_t address) {
	// load address into TWI address register; ignore global
	// TWI/I2C Slave Setup
	// set slave address to provided address, ignore general call
	// TWAR - TWI (Slave) Address Register
	TWAR = (address << 1) | 0x00;

	// set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
	// TWI-ENable , TWI Interrupt Enable
	// TWCR - TWI Control Register
	// TWEA - TWI Enable Ackowledge
	// TWEN - TWI Enable
	// TWINT - TWI Interrupt Flag
	// TWIE - TWI Interrupt Enable
	TWCR = _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWEN);

	_twiRxBufferWordsPutIndex = 0;
	_twiRxBufferWordsGetIndex = 0;
	_twiRxBufferWordsLength = 0;
}

void twiStop(void) {
	// send stop condition
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

	// wait for stop condition to be executed on bus
	// TWINT is not set after a stop condition!
	while (TWCR & _BV(TWSTO)) {
		continue;
	}
}

/*
 * Function twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
void twiReleaseBus(void)
{
  // release bus
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
}

/*
 * Function twiTransmit
 * Desc     fills slave tx buffer with data
 *          must be called in slave tx event callback
 * Input    data: pointer to byte array
 *          length: number of bytes in array
 */
void twiTransmit(const uint8_t *data, uint8_t length) {
	uint8_t i;
	// set length and copy data into tx buffer
	_twiTxBufferLength = length;
	for (i = 0; i < length; ++i) {
		_twiTxBuffer[i] = data[i];
	}
}

/*
 * Function twiReply
 * Desc     sends byte or readys receive line
 * Input    ack: byte indicating to ack or to nack
 * Output   none
 */
void twiReply(bool ack) {
	// transmit master read ready signal, with or without ack
	if (ack == true) {
		// clear TWI interrupt flag, prepare to receive next byte and acknowledge
		TWCR |= _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
	} else {
		// clear TWI interrupt flag, prepare to receive next byte and dont acknowledge
		TWCR |= _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
}

ISR(TWI_vect) {

	// react on TWI status and handle different cases
	// TWSR - TWI Statu Register
	uint8_t status = TWSR & 0xFC; // mask-out the prescaler bits (TWPSn[1:0])
	// temporary stores the received data
	//uint8_t data = 0;

	switch (status) {
	// own address has been acknowledged (own SLA+R received, acknoledge sent)
		case TW_SR_SLA_ACK:   			// addressed, returned ack
		case TW_SR_GCALL_ACK:			// addressed generally, returned ack
		case TW_SR_ARB_LOST_SLA_ACK:	// lost arbitration, returned ack
		case TW_SR_ARB_LOST_GCALL_ACK: {	// lost arbitration, returned ack
			// indicate that rx buffer can be overwritten and ack
			_twiRxBufferIndex = 0;
			twiReply(true);
			break;
		}
		case TW_SR_DATA_ACK: 			// data received, returned ack
		case TW_SR_GCALL_DATA_ACK: {	// data received generally, returned ack
			// if there is still room in the rx buffer
			if (_twiRxBufferIndex < TWI_BUFFER_LENGTH) {
				// put byte in buffer and ack
				_twiRxBuffer[_twiRxBufferIndex++] = TWDR;
				twiReply(true);
			} else {
				// otherwise nack
				twiReply(false);
			}
			break;
		}
		case TW_SR_STOP: {				// stop or repeated start condition received		{
			// put a null char after data if there's room
			if (_twiRxBufferIndex < TWI_BUFFER_LENGTH) {
				_twiRxBuffer[_twiRxBufferIndex] = '\0';
			}
			twiStop();
			// routine to process received data
			twiOnSlaveReceive(_twiRxBuffer, _twiRxBufferIndex);
			// since we submit rx buffer to main routine, we can reset it
			_twiRxBufferIndex = 0;
			// ack future responses and leave slave receiver state
			twiReleaseBus();
			break;
		}
	    case TW_SR_DATA_NACK:       	// data received, returned nack
	    case TW_SR_GCALL_DATA_NACK: {	// data received generally, returned nack
			// nack back at master
			twiReply(false);
			break;
		}
	    // Slave Transmitter
		case TW_ST_SLA_ACK:          	// addressed, returned ack
		case TW_ST_ARB_LOST_SLA_ACK: {	// arbitration lost, returned ack
			// ready the tx buffer index for iteration
			_twiTxBufferIndex = 0;
			// set tx buffer length to be zero, to verify if user changes it
			_twiTxBufferLength = 0;
			// request for txBuffer to be filled and length to be set
			// note: user must call twiTransmit(bytes, length) to do this
			twiOnSlaveTransmit();
			// if they didn't change buffer & length, initialize it
			if (0 == _twiTxBufferLength) {
				_twiTxBufferLength = 1;
				_twiTxBuffer[0] = 0x00;
			}
			// transmit first byte from buffer, fall
		// no break
		}
	    case TW_ST_DATA_ACK: { 		// byte sent, ack returned
			// copy data to output register
			TWDR = _twiTxBuffer[_twiTxBufferIndex++];
			// if there is more to send, ack, otherwise nack
			if (_twiTxBufferIndex < _twiTxBufferLength) {
				twiReply(true);
			} else {
				twiReply(false);
			}
			break;
	    }
	    case TW_ST_DATA_NACK: 		// received nack, we are done
	    case TW_ST_LAST_DATA: {		// received ack, but we are done already!
			// ack future responses
			twiReply(true);
			break;
	    }
		case TW_NO_INFO: {			// no state information
			break;
		}
		case TW_BUS_ERROR: {		// bus error, illegal stop/start
			twiStop();
			break;
		}
	}
}

#endif