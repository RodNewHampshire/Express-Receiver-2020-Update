/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* LOCAL OSCILLATOR ROUTINES
*
* Copyright 2017 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.9
* DECEMBER 27, 2017
*
******************************************************************************/

#include <arduino.h>
#include <Local_Oscillator.h>
#include <Si570.h>
#include <Wire.h>

#define I2C_OSC_SEL	0x80												// atTiny85 I2C address for LO and VFO selection
#define LO_SELECT  	0x00												// select LO on VFO, LO and BFO Board
#define VFO_SELECT 	0xFF												// select VFO on VFO, LO and BFO Board

#define Reg_137 137
#define Reg_135 135

//LO SI570 FREQUENCY CONSTANTS
const uint8_t Si570_LO[6] = {0x62, 0x43, 0x0A, 0xF4, 0x2A, 0xBE};       // Si570 registers 13 - 18 for 79.455000-MHz


Si570 _Si570;


Local_Oscillator::Local_Oscillator(void)
{

}


void Local_Oscillator::begin(void)
{
    
}


void Local_Oscillator::setFrequency(void)          		// set LO frequency
{
    uint8_t reg137, n;

    Wire.beginTransmission(I2C_OSC_SEL);          		// set I2C bus for LO Si570
    Wire.write(byte (LO_SELECT));
    Wire.endTransmission();

    reg137 = _Si570.readRegister(Reg_137);
    _Si570.writeRegister(Reg_137, reg137 | 0x10); 		// set DCO Freeze for reg 13 - 18 update

    for(n=0; n<6; n++) 
    {
        _Si570.writeRegister(13+n, Si570_LO[n]); 		// update registers 13 - 18
    }

    reg137 = _Si570.readRegister(Reg_137);
    _Si570.writeRegister(Reg_137, reg137 & 0xEF);  		// clear DCO Freeze
    _Si570.writeRegister(Reg_135, 0x40);              	// set the NewFreq bit to update frequency

    Wire.beginTransmission(I2C_OSC_SEL);          		// revert I2C bus to VFO Si570
    Wire.write(byte (VFO_SELECT));
    Wire.endTransmission();
}

