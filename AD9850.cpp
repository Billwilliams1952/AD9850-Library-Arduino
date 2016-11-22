/*
 * AD9850.cpp
 * 
 * Copyright 2016 Bill Williams <wlwilliams1952@gmail.com, github/BillWilliams1952>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

 // LOTS TODO: ----------- WORK IN PROGRESS ------------

#include "AD9850.h"

// Define AD9850 using serial clocking
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClock, uint8_t reset,
				 uint8_t powerDown, uint8_t dataPin ) {
	this->freqUpdate = freqUpdate;
	this->wordClock = wordClock;
	this->reset = reset;
	Init();
	data0 = powerDown;			// Connected to pin D2 on AD9850
	pinMode(data2,powerDown);
	digitalWrite(data2,LOW);	// Not in power down
	data7 = dataPin;			// data pion for serial transfers
	pinMode(data7,dataPin);
	digitalWrite(data7,LOW);
	phaseByte = 0x03;
	serialLoad = true;
}

// Define AD9850 using parallel clocking using non-sequential bits.
// This will require 8 separate digitalWrites for each pin. Very
// slow.
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClock, uint8_t reset,
		 uint8_t data7, uint8_t data6, uint8_t data5, uint8_t data4,
		 uint8_t data3, uint8_t data2, uint8_t data1, uint8_t data0 ) {
	this->freqUpdate = freqUpdate;
	this->wordClock = wordClock;
	this->reset = reset;
	Init();
	// this->powerDown = powerDown;
	this->data0 = data0;
	this->data1 = data1;
	this->data2 = data2;
	this->data3 = data3;
	this->data4 = data4;
	this->data5 = data5;
	this->data6 = data6;
	this->data7 = data7;
	phaseByte = 0x00;
	
	serialLoad = false;
	useDirectPort = false;
}

// Define AD9850 using a PORT - assumes bits in order D0 to D0,
// D1 to D1, ..., D7 to D7. Can use PORTX command to update all
// eight bits at once, then load them into the AD9850.
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClock, uint8_t reset,
		 char portName ) {
	this->freqUpdate = freqUpdate;
	this->wordClock = wordClock;
	this->reset = reset;
	Init();
	this->portName = portName;
	serialLoad = false;
	useDirectPort = true;
	phaseByte = 0x00;
}

/*
 * Private function to initialize common pins and variables
 */
void AD9850 :: Init ( void ) {
	pinMode(freqUpdate,OUTPUT);
	digitalWrite(freqUpdate,LOW);
	pinMode(wordClock,OUTPUT);
	digitalWrite(wordClock,LOW);
	pinMode(reset,OUTPUT);
	digitalWrite(reset,HIGH);		// not in reset mode
	phase = 0.0;
	frequency = 0.0;
	freqWord = 0x0000;
	powerDown = false;
}

void AD9850 :: ApplySignal ( float frequencyInHz, float phaseInDeg ) {
	CalculateFrequencyWord(frequencyInHz);
	CalculatePhaseByte(phaseInDeg);
	if ( serialLoad ) LoadSerial();
	else LoadParallel();
}

void AD9850 :: SetFrequency ( float frequencyInHz ) {
	// 32 bit frequency word
	CalculateFrequencyWord(frequencyInHz);
	// Now write it out, either by Serial or by Parallel
	if ( serialLoad ) LoadSerial();
	else LoadParallel();
}

void AD9850 :: IncrementFrequency ( float frequencyInHz ) {
	SetFrequency(frequency+frequencyInHz);
}

/*
 * Private function to calculate the frequency word given a frequency
 * in Hertz.
 */
void AD9850 :: CalculateFrequencyWord ( float frequencyInHz ) {
	if ( frequencyInHz > 62500000.0 ) frequencyInHz = 62500000.0;
	else if ( frequencyInHz < 0.0 ) frequencyInHz = 0.0;
	frequency = frequencyInHz;
	freqWord = (uint32_t)(frequencyInHz * BITS_PER_HZ);	
}

void AD9850 :: SetPhase ( float phaseInDeg ) {
	CalculatePhaseByte(phaseInDeg);
	if ( serialLoad ) LoadSerial();
	else LoadParallel();
}

void AD9850 :: IncrementPhase ( float phaseInDeg ) {
	SetPhase(phase+phaseInDeg);
}

/*
 * Private function to calculate the phase byte (5 bits) given a phase
 * in degrees.
 */
void AD9850 :: CalculatePhaseByte ( float phaseInDeg ) {
	phaseInDeg = fmod(phaseInDeg,360);
	if ( phaseInDeg < 0 ) phaseInDeg += 360;
	phase = phaseInDeg;
	// TODO: Calculate phase byte
	// Lower 3 bits are PowerDown and 2 bits for the Control. Where '11'
	// is a Serial load and '00' is a parallel load.
	phaseByte = ((uint8_t)(phaseInDeg * BITS_PER_DEG)) << 3;
	// Now have phase info in bits D7 - D3
	//Serial.print("phaseByte: "); Serial.println(phaseByte,HEX);
}

/*
 * From Analog Devices Product Description sheet:
 * This is the master reset function; when set high, it clears all
 * registers (except the input register), and the DAC output goes to
 * cosine 0 after additional clock cycles.
 */
void AD9850 :: Reset ( void ) {
	PULSE_LOW(reset);
}

/*
 * Enable / Disable the Powerdown function
 */
void AD9850 :: PowerDown ( bool enable ) {
	powerDown = enable;
	if ( serialLoad ) LoadSerial();
	else LoadParallel();
}

/*
 * Private function to setup for a serial load. 40 bits of data.
 */
void AD9850 :: LoadSerial ( void ) {
	// freqWord and phaseByte
	// Load control byte with XXXXX011 (D2 D1 D0) pulse it in
	// Now setup for continuous clocking of data on data0 which should
	// be connected to D7 on the AD9850.
	// There are already pullup resistors
	digitalWrite(data2,powerDown);	// data2 connected to D2 on AD9850
	PULSE_HIGH(wordClock);
	PULSE_HIGH(freqUpdate);
	// Now serial is enabled - start clocking using wordClock. Frequency
	// first - bit0 to bit 31, then phase/control bit 0 to bit 7.
	uint32_t freq = freqWord;
	for ( uint8_t i = 0; i < 32; i++ ) {
		digitalWrite(data7,(uint8_t)freq&0x0001);
		PULSE_HIGH(wordClock);
		freq >>= 1;
	}
	uint8_t phase = phaseByte;
	if ( powerDown ) phase |= POWER_DOWN;
	else phase &= ~POWER_DOWN;
	phase |= SERIAL_MODE;
	for ( uint8_t i = 0; i < 8; i++ ) {
		digitalWrite(data7,phase&0x0001);
		PULSE_HIGH(wordClock);
		phase >>= 1;
	}
}

/*
 * Private function to perform a Parallel load the 40 bits (5 bytes) of
 * frequency, phase, and control data. Check if this is a direct port
 * write, and if so, use the PORTX direct write function.
 */
void AD9850 :: LoadParallel ( void ) {
	// freqWord and phaseByte
	// Load control byte with 5 bits of phase in D7-D3 then 000 in
	// D2-D0 (PPPPP000) pulse it in as first word. Then pulse in
	// the four frequency bytes. ?? Check this ??
	digitalWrite(data0,0);			// Setup for parallel transfer
	digitalWrite(data1,0);			// ditto
	digitalWrite(data2,powerDown);	// data2 powerDown bit
	uint8_t phase = phaseByte;
	if ( powerDown ) phase |= POWER_DOWN;
	else phase &= ~POWER_DOWN;
	phase &= ~SERIAL_MODE;
}

