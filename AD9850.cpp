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
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClk uint8_t reset,
				 uint8_t powerDown, uint8_t dataPin ) {
	this->freqUpdate = freqUpdate;
	this->wordClk = wordClk;
	this->reset = reset;
	Init();
	this->powerDown = powerDown;
	data0 = dataPin;
	serialLoad = true;
}

// Define AD9850 using parallel clocking using non-sequential bits.
// This will require 8 separate digitalWrites for each pin. Very
// slow.
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
		 uint8_t data7, uint8_t data6, uint8_t data5, uint8_t data4,
		 uint8_t data3, uint8_t data2, uint8_t data1, uint8_t data0 ) {
	this->freqUpdate = freqUpdate;
	this->wordClk = wordClk;
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
	
	serialLoad = false;
	useDirectPort = false;
}

//2147483648

// Define AD9850 using a PORT - assumes bits in order D0 to D0,
// D1 to D1, ..., D7 to D7. Can use PORTX command to update all
// eight bits at once, then load them into the AD9850.
AD9850 :: AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
		 char portName ) {
	this->freqUpdate = freqUpdate;
	this->wordClk = wordClk;
	this->reset = reset;
	Init();
	this->portName = portName;
	serialLoad = false;
	useDirectPort = true;
}

AD9850 :: Init ( void ) {
	pinMode(freqUpdate,OUTPUT);
	digitalWrite(freqUpdate,LOW);
	pinMode(wordClk,OUTPUT);
	digitalWrite(wordClk,LOW);
	pinMode(reset,OUTPUT);
	digitalWrite(reset,LOW);

	phase = 0.0;
	frequency = 0.0;
	phaseByte = 0x00;
	freqWord = 0x0000;
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

void AD9850 :: CalculateFrequencyWord ( float frequencyInHz ) {
	if ( frequencyInHz > 62500000.0 ) frequencyInHz = 62500000.0;
	else if ( frequencyInHz < 0.0 ) frequencyInHz = 0.0;
	frequency = frequencInHz;
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

void AD9850 :: CalculatePhaseByte ( float phaseInDeg ) {
	phaseInDeg = fmod(phaseInDeg,360);
	if ( phaseInDeg < 0 ) phaseInDeg += 360;
	phase = phaseInDeg;
	// TODO: Calculate phase byte
	phaseByte = 0x00;
}

/*
 * Issue a Master Reset. This reset all internal registers except the
 * input register.
 */
void AD9850 :: Reset ( void ) {
	
}

void AD9850 :: PowerDown ( bool enable ) {
	// TODO: How to do this?
}

/*
 * Setup for a serial load. 40 bits of data.
 */
void AD9850 :: LoadSerial ( void ) {
	
}

/*
 * Parallel load the 40 bits (5 bytes) of frequency, phase, and control
 * data. Check if this is a direct port write.
 */
void AD9850 :: LoadParallel ( void ) {

}

