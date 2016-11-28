# AD9850-Library-Arduino
Arduino library to control the AD9850 DDS module

**-------------CODE IS IN WORK------------**

**From Analog Devices data sheet:** (http://www.analog.com/media/en/technical-documentation/data-sheets/AD9850.pdf)
>"The AD9850 is a highly integrated device that uses advanced DDS technology coupled with an internal high speed, high performance D/A converter and comparator to form a complete, digitally programmable frequency synthesizer and clock generator function. When referenced to an accurate clock source, the AD9850 generates a spectrally pure, frequency/phase programmable, analog output sine wave. This sine wave can be used directly as a frequency source, or it can be converted to a square wave for agile-clock generator applications."

>"The AD9850’s innovative high speed DDS core provides a 32-bit frequency tuning word, which results in an output tuning resolution of 0.0291 Hz for a 125 MHz reference clock input. The AD9850’s circuit architecture allows the generation of output frequencies of up to one-half the reference clock frequency (or 62.5 MHz), and the output frequency can be digitally changed (asynchronously) at a rate of up to 23 million new frequencies per second. The device also provides five bits of digitally controlled phase modulation, which enables phase shifting of its output in increments of 180°, 90°, 45°, 22.5°, 11.25°, and any combination thereof. The AD9850 also contains a high speed comparator that can be configured to accept the (externally) filtered output of the DAC to generate a low jitter square wave output. This facilitates the device’s use as an agile clock generator function."

## Synopsis

**Sine Wave outputs at 20MHz**
![alt tag](https://cloud.githubusercontent.com/assets/3778024/20653563/021f9856-b4d5-11e6-96f8-370a5f83d253.png)

**Square Wave outputs. Duty cycle adjusted to 50%**
![alt tag](https://cloud.githubusercontent.com/assets/3778024/20653618/e1a235d8-b4d5-11e6-8dc4-dfd327e4fcbd.png)

## API

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

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.
