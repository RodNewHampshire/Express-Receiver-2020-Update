/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* Si570 ROUTINES
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
#include <Si570.h>
#include <Wire.h>

//#define SI570_DEBUG

//Index to arrays:
//0=160M, 1=80M, 2=40M, 3=30M, 4=20M, 5=17M, 6=15M, 7=12M, 8=10Ma, 9=10Mb, 10=10Mc

//VFO SI570 BAND DCO CENTER FREQUENCY CONSTANTS
const uint8_t Si_Reg_13[11] = {0xA1, 0xA1, 0xE1, 0xE1, 0xE1, 0x61, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1};
const uint8_t Si_Reg_14[11] = {0xC2, 0xC2, 0x42, 0x42, 0x43, 0xC2, 0x42, 0x42, 0x42, 0x42, 0x42};
const uint8_t Si_Reg_15[11] = {0xD9, 0xEC, 0xCD, 0xE8, 0x0E, 0xB6, 0xB5, 0xD1, 0xEA, 0xEE, 0xF2};
const uint8_t Si_Reg_16[11] = {0x5B, 0x01, 0x16, 0x94, 0x00, 0x6E, 0x1E, 0x34, 0x3A, 0x11, 0x9A};
const uint8_t Si_Reg_17[11] = {0x86, 0x89, 0xC8, 0x26, 0x6F, 0x36, 0x74, 0x81, 0xB7, 0x34, 0x73};
const uint8_t Si_Reg_18[11] = {0x50, 0xAD, 0x7C, 0xCF, 0x4A, 0xBC, 0xB5, 0xBB, 0x5E, 0x8D, 0xA4};

const uint32_t RFREQ_Center[11] = {0xD95B8650, 0xEC0189AD, 0xCD16C87C, 0xE89426CF, 0x0E006F4A, 0xB66E36BC, 0xB51E74B5,
                                0xD13481BB, 0xEA3AB75E, 0xEE11348D, 0xF29A73A4};

const uint32_t Band_Center[11] = {1900000, 3750000, 7150000, 10125000, 14175000, 18118000, 21225000, 24940000, 28250000,
                                        28757640, 29357640};

//TUNING OFFSET CONSTANTS FOR 1000-, 100-, AND 10-Hz
const uint32_t Inc_1000Hz[11] = {0x0002949B, 0x00025D8E, 0x00025D8E, 0x00025D8E, 0x00025D8E, 0x000201CE, 0x0001EF74, 0x0001EF74, 0x0001EF74, 0x0001EF74, 0x0001EF74};
const uint32_t Inc_100Hz[11] = {0x0000420F, 0x00003C8E, 0x00003C8E, 0x00003C8E, 0x00003C8E, 0x00003361, 0x0000318B, 0x0000318B, 0x0000318B, 0x0000318B, 0x0000318B};
const uint32_t Inc_10Hz[11] = {0x0000069B, 0x0000060E, 0x0000060E, 0x0000060E, 0x0000060E, 0x00000523, 0x000004F4, 0x000004F4, 0x000004F4, 0x000004F4, 0x000004F4};

//LO SI570 FREQUENCY CONSTANTS
const uint8_t Si570_LO[6] = {0x62, 0x43, 0x0A, 0xF4, 0x2A, 0xBE};       // 79.455000-MHz

uint8_t RFREQ[6];
char LO_CAL[13], VFO_CAL[13];

//                         160  80   40   30   20  17   15   12   10a  10b  10c
uint16_t Cal_Offset[11] = {120, 150, 269, 260, 240, 279, 150, 150, 150, 150, 150};

uint8_t Current_Band = 0xFF;

#define I2C_OSC_SEL 0x80                                                // atTiny85 I2C address for LO and VFO selection
#define LO_SELECT   0x00                                                // select LO on VFO, LO and BFO Board
#define VFO_SELECT  0xFF                                                // select VFO on VFO, LO and BFO Board
const uint8_t I2C_Si570 = 0x55;                                         // Si570 factory set I2C address

#define SI570_RECALL (1<<0) // Reg 135 Factory config recall bit


Si570::Si570(void)
{

}


void Si570::begin(void)
{
  
}


void Si570::VFO_Update(uint32_t *Freq, uint8_t *Band)
{ 
    int32_t calFreq;
    int32_t Freq_Offset;
    uint8_t NEG_OFFSET_FLAG = 0;
    int16_t thousands, hundreds, tens, n;
    uint64_t New_RFREQ = 0, Delta_RFREQ = 0;

    if(*Band != Current_Band)                                  // update DCO center frequency if band changed
    {
        Current_Band = *Band;
        Band_Change(*Band);
    }

    calFreq = *Freq + Cal_Offset[*Band];
                                                              // determine if new frequency is above or below center frequency
    if(calFreq >= Band_Center[*Band]) 
    {
        Freq_Offset = calFreq - Band_Center[*Band];
        NEG_OFFSET_FLAG = 0;
    }
    else if(calFreq < Band_Center[*Band])
    {
        Freq_Offset = Band_Center[*Band] - calFreq;
        NEG_OFFSET_FLAG = 1;
    }
                                                              // calculate required SI570 RFREQ offset
    thousands = 0;
    while(Freq_Offset >= 1000)
    {
        thousands++;
        Freq_Offset = Freq_Offset - 1000;
    }

    hundreds = 0;
    while(Freq_Offset >= 100)
    {
        hundreds++;
        Freq_Offset = Freq_Offset - 100;
    }

    tens = 0;
    while(Freq_Offset != 0)
    {
        tens++;
        Freq_Offset = Freq_Offset - 10;
    }

    Delta_RFREQ =   (Inc_1000Hz[*Band] * thousands) +
                    (Inc_100Hz[*Band] * hundreds) +
                    (Inc_10Hz[*Band] * tens);

    if(NEG_OFFSET_FLAG) New_RFREQ = RFREQ_Center[*Band] - Delta_RFREQ;
    else New_RFREQ = RFREQ_Center[*Band] + Delta_RFREQ;

    for(n=0; n<4; n++) RFREQ[n] = (uint8_t)(New_RFREQ >> 8*n);
                                                // explicitly truncating long New_RFREQ to char
                                                // RFREQ[0] = Reg 18
                                                // RFREQ[1] = Reg 17
                                                // RFREQ[2] = Reg 16
                                                // RFREQ[3] = Reg 15
    writeRegister(135, 0x20);
    for(n=3; n>0; n--) 
    {
        writeRegister(18-n, RFREQ[n]);
    }
    writeRegister(135, 0x00);
}


void Si570::Band_Change(uint8_t New_Band)       // set DCO center frequency for new band
{
    uint8_t Reg137, n;
    delay(10);
    Reg137 = readRegister((uint8_t) 137);
    writeRegister(137, Reg137 | 0x10);          // set DCO Freeze for reg 13 - 18 update
    writeRegister(13, Si_Reg_13[New_Band]);
    writeRegister(14, Si_Reg_14[New_Band]);
    RFREQ[3] = Si_Reg_15[New_Band];
    RFREQ[2] = Si_Reg_16[New_Band];
    RFREQ[1] = Si_Reg_17[New_Band];
    RFREQ[0] = Si_Reg_18[New_Band];
    for(n=3; n>0; n--) 
    {
        writeRegister(18-n, RFREQ[n]);
    }

    Reg137 = readRegister((uint8_t) 137);
    writeRegister(137, Reg137 & 0xEF);          // clear DCO Freeze
    writeRegister(135, 0x40);                   // set the NewFreq bit to update frequency
}



uint8_t Si570::readRegister(uint8_t byteAddress) 
{
    uint8_t data = 0x00;

    Wire.beginTransmission(I2C_Si570);
    Wire.write(byteAddress);
    Wire.endTransmission(0);
    Wire.requestFrom(I2C_Si570, (uint8_t) 1);
    if(Wire.available()) data = Wire.read();
    Wire.endTransmission();

#ifdef SI570_DEBUG
    Serial.print(F("Si570: READ: r"));
    Serial.print(byteAddress);
    Serial.print(": ");
    Serial.print("0x");
    Serial.print(data, HEX);
    Serial.println();
#endif

return data;
}


void Si570::writeRegister(uint8_t Si570_reg, uint8_t data)
{
    Wire.beginTransmission(I2C_Si570);
    Wire.write(Si570_reg);
    Wire.write(data);
    Wire.endTransmission(0);
    delay(10);                                  // this delay is needed to keep the Si570 from locking up periodically
}


void Si570::get10MHzRFREQ(void)
{
    uint8_t n;
    
    Wire.beginTransmission(I2C_OSC_SEL);        // set I2C bus for LO Si570
    Wire.write(byte (LO_SELECT));
    Wire.endTransmission();
    writeRegister(135, SI570_RECALL);
    for(n=13; n<19; n++) RFREQ[n-13] = readRegister(n);

    Serial.print("LO Cal.: ");
    Serial.print(RFREQ[0], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[1], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[2], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[3], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[4], HEX);
    Serial.print(' ');
    Serial.println(RFREQ[5], HEX);

    Wire.beginTransmission(I2C_OSC_SEL);        // revert I2C bus to VFO Si570
    Wire.write(byte (VFO_SELECT));
    Wire.endTransmission();
    writeRegister(135, SI570_RECALL);
    for(n=13; n<19; n++) RFREQ[n-13] = readRegister(n);

    Serial.print("VFO Cal.: ");
    Serial.print(RFREQ[0], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[1], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[2], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[3], HEX);
    Serial.print(' ');
    Serial.print(RFREQ[4], HEX);
    Serial.print(' ');
    Serial.println(RFREQ[5], HEX);
}
