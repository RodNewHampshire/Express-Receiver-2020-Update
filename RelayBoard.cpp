/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* RELAY BOARD ROUTINES
*
* Copyright 2016 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.9
* DECEMBER 27, 2017
*
******************************************************************************/

/* PCF8575C I2C I/O EXPANDER
 *
 *                                              BITS
 *                      7       6       5       4       3       2       1       0
 *
 * I2C Slave Address    L       H       L       L       A2      A1      A0      R/W
 *
 * POx I/O Data Bus     P07     P06     PO5     PO4     PO3     PO2     PO1     PO0
 *
 * p1X I/O Data Bus     P17     P16     P15     P14     P13     P12     P11     P10
 *
 * 7-Bit 12C address plus R/W bit
 */


/********************RELAY BOARD 1 PORT ADDRESSING***************/

/*
 *  Relay Board 1
 *
 * Band Pass Filter select
 *  P0_0    15M                 0x0001
 *  PO_1    30M                 0x0002
 *  PO_2    160M                0x0004
 *  PO_3    17M                 0x0008
 *  PO_4    20M                 0x0010
 *  PO_5    80M                 0x0020
 *  PO_6    12M                 0x0040
 *  PO_7    10M                 0x0080
 *  P1_7    40M                 0x8000
 *
 * RF Attenuator select
 *  P1_3    2dB                 0x0800
 *  P1_4    4dB                 0x1000
 *  P1_5    8dB                 0x2000
 *  P1_6    16dB                0x4000
 *
 * RF Preamp select
 *  P1_2    ON                  0x0400
 *
 *  P1_1    Spare               0x0200
 *  P1_0    Spare               0x0100
 *
*/


/********************RELAY BOARD 2 PORT ADDRESSING***************/

/*
 *  Relay Board 2
 *
 * IF Filter select, IF Filter Board #1
 *  P0_0    1800-Hz             0x0001
 *  PO_1    2100-Hz             0x0002
 *  PO_2    Not used            0x0004
 *
 * IF Filter select, IF Filter Board #2
 *  PO_3    250-Hz              0x0008
 *  PO_4    500-Hz              0x0010
 *  PO_5    Not used            0x0020
 *
 *  PO_6    Spare               0x0040
 *  PO_7    Spare               0x0080
 *  P1_7    Spare               0x8000
 *
 *  P1_3    Spare               0x0800
 *  P1_4    Spare               0x1000
 *  P1_5    Spare               0x2000
 *  P1_6    Spare               0x4000
 *
 *  P1_2    Spare               0x0400
 *
 *  P1_1    Spare               0x0200
 *  P1_0    Spare               0x0100
 *
*/

#include <RelayBoard.h>

const uint16_t  BPF[11] = {0x0004, 0x0020, 0x8000, 0x0002, 0x0010, 0x0008, 0x0001, 0x0040, 0x0080, 0x0080, 0x0080};
const uint16_t ATTN[16] = {0x0000, 0x0800, 0x1000, 0x1800, 0x2000, 0x2800, 0x3000, 0x3800, 0x4000, 0x4800, 0x5000, 0x5800, 0x6000, 0x6800, 0x7000, 0x7800};
const uint16_t   AMP[2] = {0x0000, 0x0400};

const uint16_t IF_BW[4] = {0x0002, 0x0001, 0x0010, 0x0008};


uint8_t savedBand, savedAttn = 0, savedAmp = 0, savedIFBW = 0;

/******************** I2C Addresses ********************/

uint8_t Relay_Driver_1 = 0x4E >> 1;
uint8_t Relay_Driver_2 = 0x4C >> 1;



RelayBoard::RelayBoard(void)
{

}


void RelayBoard::begin(void)
{
  
}


void RelayBoard::RF_BPF(uint8_t band)
{
    uint16_t temp;

    savedBand = band;
    temp = BPF[band] | ATTN[savedAttn] | AMP[savedAmp];
    Wire.beginTransmission(Relay_Driver_1);
    Wire.write(temp & 0x00FF);
    Wire.write((temp & 0xFF00) >> 8);
    Wire.endTransmission();
}


void RelayBoard::RF_ATTN(uint8_t attn)
{
    uint16_t temp;

    if(attn == savedAttn)return;
    savedAttn = attn;
    temp = BPF[savedBand] | ATTN[attn] | AMP[savedAmp];
    Wire.beginTransmission(Relay_Driver_1);
    Wire.write(temp & 0x00FF);
    Wire.write((temp & 0xFF00) >> 8);
    Wire.endTransmission();
}


void RelayBoard::RF_AMP(uint8_t amp)
{
    uint16_t temp;

    if(amp == savedAmp)return;
    savedAmp = amp;
    temp = BPF[savedBand] | ATTN[savedAttn] | AMP[amp];
    Wire.beginTransmission(Relay_Driver_1);
    Wire.write(temp & 0x00FF);
    Wire.write((temp & 0xFF00) >> 8);
    Wire.endTransmission();
}


void RelayBoard::IF_FLTR(uint8_t ifbw)
{
    uint16_t temp;

    if(ifbw == savedIFBW)return;
    savedIFBW = ifbw;
    temp = IF_BW[savedIFBW];
    Wire.beginTransmission(Relay_Driver_2);
    Wire.write(temp & 0x00FF);
    Wire.write((temp & 0xFF00) >> 8);
    Wire.endTransmission();
}


void RelayBoard::TX_MODE(void)                          //disconnect BPFs, set RF Attn to 30dB, RF Amp off
{
    Wire.beginTransmission(Relay_Driver_2);
    Wire.write(0x0000 & 0x00FF);
    Wire.write((0x0000 & 0xFF00) >> 8);
    Wire.endTransmission();

    Wire.beginTransmission(Relay_Driver_1);             
    Wire.write(0x7800 & 0x00FF);
    Wire.write((0x7800 & 0xFF00) >> 8);
    Wire.endTransmission();
}


void RelayBoard::RX_MODE(void)                          //restore BPF, RF Attn, and RF Amp to saved settings
{
    uint16_t restore = BPF[savedBand] + ATTN[savedAttn] + AMP[savedAmp];
    Wire.beginTransmission(Relay_Driver_1);             
    Wire.write(restore & 0x00FF);
    Wire.write((restore & 0xFF00) >> 8);
    Wire.endTransmission();

    Wire.beginTransmission(Relay_Driver_2);
    Wire.write(IF_BW[savedIFBW] & 0x00FF);
    Wire.write((IF_BW[savedIFBW] & 0xFF00) >> 8);
    Wire.endTransmission();
}