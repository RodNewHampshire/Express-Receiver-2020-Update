/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* MAIN LCD FUNCTION ROUTINES
*
* Copyright 2017 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.9
* DECEMBER 27, 2017
*
******************************************************************************/

#include "Adafruit_LiquidCrystal.h"
#include <arduino.h>
#include <Main_LCD.h>


char Disp_VFOA[21] = {" VFO A:  ,   .   KHz"};
char Disp_VFOB[21] = {" VFO B:  ,   .   KHz"};
char Show_Memory[21] = {" MEM  :  ,   .   KHz"};
char Hide_Memory[21] = {"                    "};


Adafruit_LiquidCrystal _mainLCD(0);


Main_LCD::Main_LCD(void)
{

}


void Main_LCD::begin(void)
{
	_mainLCD.begin(20, 4);
}


void Main_LCD::clear(void)
{
	_mainLCD.clear();
}


void Main_LCD::splash(void)
{
 	_mainLCD.print(F("       AD5GH        "));
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("  EXPRESS RECEIVER  "));
  	_mainLCD.setCursor(0, 2);
  	_mainLCD.print(F("  HF UP CONVERTING  "));
  	_mainLCD.setCursor(0, 3);
  	_mainLCD.print(F(" SUPER HET RECEIVER "));
}


void Main_LCD::updateVFOADisplay(uint32_t *frequency, uint8_t *mode)
{
	String VFOA_FreqDisp;
    long VFO_A_Adjusted;

    if (*mode == 0) VFO_A_Adjusted = *frequency - 1500;
    else if (*mode == 1) VFO_A_Adjusted = *frequency + 1500;
    else VFO_A_Adjusted = *frequency;

    VFOA_FreqDisp = String(VFO_A_Adjusted, DEC);

    if (*frequency < 10000000)
    {
      char i;
      for (i = 6; i > 0; i--) VFOA_FreqDisp[i] = VFOA_FreqDisp[i - 1];
      VFOA_FreqDisp[0] = 0x20;
    }

    Disp_VFOA[7] = VFOA_FreqDisp[0];
    Disp_VFOA[8] = VFOA_FreqDisp[1];
    Disp_VFOA[10] = VFOA_FreqDisp[2];
    Disp_VFOA[11] = VFOA_FreqDisp[3];
    Disp_VFOA[12] = VFOA_FreqDisp[4];
    Disp_VFOA[14] = VFOA_FreqDisp[5];
    Disp_VFOA[15] = VFOA_FreqDisp[6];
    Disp_VFOA[16] = 0x30;                           // "1" Hertz digital is always zero
    _mainLCD.setCursor(0, 0);
    _mainLCD.print(Disp_VFOA);
}


void Main_LCD::updateVFOBDisplay(uint32_t *frequency, uint8_t *mode)
{
	String VFOB_FreqDisp;
    long VFO_B_Adjusted;

    if (*mode == 0) VFO_B_Adjusted = *frequency - 1500;
    else if (*mode == 1) VFO_B_Adjusted = *frequency + 1500;
    else VFO_B_Adjusted = *frequency;

    VFOB_FreqDisp = String(VFO_B_Adjusted, DEC);

    if (*frequency < 10000000)
    {
      char i;
      for (i = 6; i > 0; i--) VFOB_FreqDisp[i] = VFOB_FreqDisp[i - 1];
      VFOB_FreqDisp[0] = 0x20;
    }

    Disp_VFOB[7] = VFOB_FreqDisp[0];
    Disp_VFOB[8] = VFOB_FreqDisp[1];
    Disp_VFOB[10] = VFOB_FreqDisp[2];
    Disp_VFOB[11] = VFOB_FreqDisp[3];
    Disp_VFOB[12] = VFOB_FreqDisp[4];
    Disp_VFOB[14] = VFOB_FreqDisp[5];
    Disp_VFOB[15] = VFOB_FreqDisp[6];
    Disp_VFOB[16] = 0x30;                           // "1" Hertz digital is always zero
    _mainLCD.setCursor(0, 2);
    _mainLCD.print(Disp_VFOB);
}


void Main_LCD::showMemoryDisplay(uint32_t *frequency, uint8_t *mode, uint8_t *memNumber)
{
  	char MEM_NUM_Disp;
  	String MEMX_FreqDisp;
  	unsigned long MEM_Adjusted;

  	if (*mode == 0) MEM_Adjusted = *frequency - 1500;
  	else if (*mode == 1) MEM_Adjusted = *frequency + 1500;
  	else MEM_Adjusted = *frequency;

  	MEM_NUM_Disp = *memNumber + 0x30;               //convert char to ASCII
  	MEMX_FreqDisp = String(MEM_Adjusted, DEC);

  	if (MEM_Adjusted < 10000000)
  	{
    	char i;
    	for (i = 6; i > 0; i--) MEMX_FreqDisp[i] = MEMX_FreqDisp[i - 1];
    	MEMX_FreqDisp[0] = 0x20;
  	}

  	Show_Memory[5] = MEM_NUM_Disp;
  	Show_Memory[7] = MEMX_FreqDisp[0];
  	Show_Memory[8] = MEMX_FreqDisp[1];
  	Show_Memory[10] = MEMX_FreqDisp[2];
  	Show_Memory[11] = MEMX_FreqDisp[3];
  	Show_Memory[12] = MEMX_FreqDisp[4];
  	Show_Memory[14] = MEMX_FreqDisp[5];
  	Show_Memory[15] = MEMX_FreqDisp[6];
  	Show_Memory[16] = 0x30;                           // "1" Hertz digital is always zero
  	_mainLCD.setCursor(0, 3);
  	_mainLCD.print(Show_Memory);
}


void Main_LCD::hideMemoryDisplay(void)
{
  	_mainLCD.setCursor(0, 3);
  	_mainLCD.print(Hide_Memory);
}


void Main_LCD::indicateVFOA(void)
{
	if (Disp_VFOA[0] == '*') Disp_VFOA[0] = ' ';
    else Disp_VFOA[0] = '*';
    _mainLCD.setCursor(0, 0);
    _mainLCD.print(Disp_VFOA[0]);
}


void Main_LCD::indicateVFOB(void)
{
	if (Disp_VFOB[0] == '*') Disp_VFOB[0] = ' ';
    else Disp_VFOB[0] = '*';
    _mainLCD.setCursor(0, 2);
    _mainLCD.print(Disp_VFOB[0]);
}


void Main_LCD::deselectVFOA(void)
{
	Disp_VFOA[0] = ' ';
    _mainLCD.setCursor(0, 0);
    _mainLCD.print(Disp_VFOA[0]);
}


void Main_LCD::deselectVFOB(void)
{
	Disp_VFOB[0] = ' ';
    _mainLCD.setCursor(0, 2);
    _mainLCD.print(Disp_VFOB[0]);
}


void Main_LCD::eepromStart(void)
{
  	_mainLCD.clear();
  	_mainLCD.print(F("EEPROM"));
}


void Main_LCD:: eepromNoDataFound(void)                                        // updates Line 2 while leaving Line 1 unchaged
{
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("                    "));
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("No Data Found"));
  	delay(1000);
}


void Main_LCD:: eepromInitializing(void)                                        // updates Line 2 while leaving Line 1 unchaged
{
	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("                    "));
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("Initializing..."));
  	delay(1000);
}


void Main_LCD:: eepromReadingData(void)                                        // updates Line 2 while leaving Line 1 unchaged
{
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("                    "));
  	_mainLCD.setCursor(0, 1);
  	_mainLCD.print(F("Reading Data..."));
  	delay(1000);
}