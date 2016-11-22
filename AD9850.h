/*
 * AD9850.h
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

#ifndef __AD9850_H__

#define __AD9850_H__

#include <Arduino.h>

#define Pow_2_32		4294967295UL		// 2^32 - 1
#define MHZ_125			125000000UL			// Module clock rate
#define BITS_PER_HZ		34.359738368		// Pow_2_32 / MHZ_125
#define RESOLUTION_HZ	0.02910383			// 1 / BITS_PER_HZ
#define	BITS_PER_DEG	0.088888889			// 32 / 360
#define RESOLUTION_DEG	11.25				// 1 / BITS_PER_DEG
#define POWER_DOWN		B00000100
#define SERIAL_MODE		B00000011

#define PULSE_HIGH(pin)		digitalWrite((pin),HIGH); digitalWrite((pin),LOW);

#define PULSE_LOW(pin)		digitalWrite((pin),LOW); digitalWrite((pin),HIGH);

class AD9850 {
	public:
		// Will start in parallel mode unless pin 2 is LOW and
		// pins 3 and 4 are HIGH at power on. The AD9850 module has
		// pullups on the two pins requiring 

		// Pin2 D2 - Power Down, Pin3 D1, Pin4 D0 (control)
		// Set to 0 1 1 for Serial
		// RESET  0 1 1, Pulse WordClock, Pulse FreqUpdate
		// Set to 0 0 0 for Parallel
		// RESET  0 0 0, Pulse WordkClk, Pulse FreqUpdate
		//    Now in Parallel Mode

		// Define AD9850 using Serial Clocking. But not SPI. Clock as
		// fast as possible.
		AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
				 uint8_t powerDown, uint8_t dataPin );

		// Define AD9850 using parallel clocking using non-sequential bits.
		// This will require 8 separate digitalWrites for each pin.
		AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
				 uint8_t data7, uint8_t data6, uint8_t data5, uint8_t data4,
				 uint8_t data3, uint8_t data2, uint8_t data1, uint8_t data0 );

		// Define AD9850 using a PORT - assumes bits in order D0 to D0,
		// D1 to D1, ..., D7 to D7. Can use PORTX command to update all
		// eight bits at once, then load them into the AD9850.
		AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
				 char portName );

		void ApplySignal ( float frequencyInHz, float phaseInDeg );

		void SetFrequency ( float frequencyInHz );

		void IncrementFrequency ( float frequencyInHz );

		void SetPhase ( float phaseInDeg );

		void IncrementPhase ( float phaseInDeg );

		void Reset ( void );

		void PowerDown ( bool enable );

		float GetFreqResolution ( void ) { return RESOLUTION_HZ; }

		float GetPhaseResolution ( void ) { return RESOLUTION_DEG; }

	private:

		void Init ( void );

		void CalculateFrequencyWord ( float frequencyInHz );

		void CalculatePhaseByte ( float phaseInDeg );

		void LoadSerial ( void );

		void LoadParallel ( void );

		uint32_t freqWord;	// latest frequency to load
		uint8_t	 phaseByte;	// latest phase value to load
		uint8_t	freqUpdate, /* On the rising edge of this clock, the
							 * DDS updates to the frequency (or phase)
							 * loaded in the data input register; it then
							 * resets the pointer to Word 0. */
				wordClock, reset, data7, data6, data5, data4,
				data3, data2, data1, data0;
		char	portName;
		float 	frequency, phase;	// latest programmed values
		bool	powerDown = false, serialLoad, useDirectPort;
		
};


#endif
