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

#define POWER_DOWN_BIT	B00000100			// Register info
// Note, on the AD9850 module, the pins D1 and D0 are pulled up. This
// allows the user to enter Serial mode by just having a line connected
// to the PowerDown pin (D2).
#define SERIAL_MODE		B00000011			// Register info
#define PARALLEL_MODE	B00000000			// register info

#define PULSE_HIGH(pin)		digitalWrite((pin),HIGH); delayMicroseconds(15); digitalWrite((pin),LOW);

#define PULSE_LOW(pin)		digitalWrite((pin),LOW); delayMicroseconds(15); digitalWrite((pin),HIGH);

/*
 * For maximum transfer speed, pins are predefined allowing us to
 * use PORTX writes. Note, the reset pin can still use digitalWrites
 */
#define DATA_PORT					PORTD	// only parallel load
#define DATA_DIRECTION_PORT			DDRD	// ditto
// These are all on PORTB
#define CONTROL_PIN_PORT			PORTB
#define DIRECT_FREQ_UPDATE_PIN		8		// both parallel and serial
#define	DIRECT_WORD_CLOCK_PIN		9		// both parallel and serial
#define	DIRECT_DATA_PIN				10		// Only for serial
											// MUST go to D7 on AD9850
#define	DIRECT_POWER_DOWN_PIN		11		// only for serial
											// MUST go to D2 on AD9850

class AD9850 {
	public:

		// Define AD9850 using Serial mode loading.
		AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
				 uint8_t powerDownPin, uint8_t dataPin );

		// Define AD9850 using predefined pins.
		// Uses Direct Port writes for maximum speed.
		// See AD9850.h files for pin definitions
		AD9850 ( bool useSerialLoad, uint8_t reset );

		// Define AD9850 using Parallel mode loading.
		// This will require 8 separate digitalWrites for each pin.
		AD9850 ( uint8_t freqUpdate, uint8_t wordClk, uint8_t reset,
				 uint8_t data7, uint8_t data6, uint8_t data5, uint8_t data4,
				 uint8_t data3, uint8_t data2, uint8_t data1, uint8_t data0 );

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
		uint8_t	 freqUpdate, /* On the rising edge of this clock, the
							 * DDS updates to the frequency (or phase)
							 * loaded in the data input register; it then
							 * resets the pointer to Word 0. */
				wordClock,	// data loading clock, active HIGH
				reset,		// active HIGH
				// parallel data lines
				data7, data6, data5, data4, data3, data2, data1, data0;
		float 	frequency, phase;	// latest programmed values
		bool	powerDown, serialLoad, useDirectPort;
		
};


#endif
