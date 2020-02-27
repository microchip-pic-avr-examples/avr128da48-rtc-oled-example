<img src="images/microchiptechnologyinc.png" height="60">

# AVR128DA48 RTC with OLED Click Example
This repository provides an Atmel Studio solution with a bare metal code example for an RTC implementation.
This example implements a desktop clock built using the AVR-DA's RTC and OLED Click.
The clock can be set to the correct time through the use of a button connected to PC7 (on-board SW0).
Long pressing SW0 will switch between seconds, minutes and hours to be changed and short pressing SW0 will increase the value of the selected time unit.

## Configurations
- Digital Input Pin:
	* PC7 pin as input pin with internal pull-up enabled
- RTC
	* RTC Clock
	* Input clock 32 kHz / 32
	* Overflow interrupt enabled
	* Period 0x3c4
- CPUINT:
	* Global interrupt enabled
- TCB0:
	* Input clock main clock (16MHz) / 2 (from prescaler)
	* Period: 0x1d4c
	* Overflow interrupt enabled
- OLEDC_Click:
	* SPI Master
	* CS pin: PA7, DC pin: PD0, EN pin: PD6, RST pin: PD7, RW pin: PD3
- SPI Master:
	* SPI Master Polled mode
	* MISO: PA5, MOSI: PA4, SCK: PA6
	* Input clock main clock (16MHz) / 4 (prescaler)


<img src="images/AVR128DA48_CNANO_instructions.PNG" height="250">

## Required Tools
- Atmel Studio 7.0.2397 or newer
- AVR-Dx 1.0.18 or newer Device Pack
- AVR128DA48 Curiosity Nano (DM164151)

## Compatibility
The source code is compatible with the following devices: AVR128DA28, AVR128DA32, AVR128DA48, and AVR128DA64.
