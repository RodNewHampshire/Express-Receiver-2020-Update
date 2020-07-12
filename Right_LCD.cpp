/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* RIGHT LCD FUNCTION ROUTINES
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
#include <BFO.h>
#include <RelayBoard.h>
#include <Right_LCD.h>
#include <IF_Board.h>

BFO bfo;

Adafruit_LiquidCrystal _rightLCD(1);
RelayBoard relayBoard;
IF_Board if_Board;

char _rightLCDLine0[21] = {"IFBW:       MUTE:   "};
char _rightLCDLine1[21] = {"RATE:       FQLK:   "};
char _rightLCDLine2[21] = {"ATT AMP BND MOD AGC "};
char _rightLCDLine3[21] = {"                    "};

const char IFBW_1[4] = {"21  "};
const char IFBW_2[4] = {"1852"};
const char IFBW_3[4] = {"0005"};
const char IFBW_4[4] = {"0000"};

const char BAND_1[11] = {"1       111"};
const char BAND_2[11] = {"68432111000"};
const char BAND_3[11] = {"00000752abc"};

const char MODE_1[3] = {"UL "};
const char MODE_2[3] = {"SSC"};
const char MODE_3[3] = {"BBW"};

const char AGC_1[3] = {"SFM"};
const char AGC_2[3] = {"LSA"};
const char AGC_3[3] = {"WTN"};

const char RATE_1[4] = {"1   "};
const char RATE_2[4] = {"021 "};
const char RATE_3[4] = {"0501"};
const char RATE_4[4] = {"0000"};

const char ATTN_1[16] = {"0000011111222223"};
const char ATTN_2[16] = {"0246802468024680"};

const char AMP_1[2] = {"O "};
const char AMP_2[2] = {"FO"};
const char AMP_3[2] = {"FN"};

const char MUTE_1[2] = {"O "};
const char MUTE_2[2] = {"FO"};
const char MUTE_3[2] = {"FN"};

const char TXLK_1[2] = {"O "};
const char TXLK_2[2] = {"FO"};
const char TXLK_3[2] = {"FN"};

char IFBW_Dsply[4];
char BAND_Dsply[3];
char MODE_Dsply[3];
char AGC_Dsply[3];
char RATE_Dsply[4];
char ATTN_Dsply[2];
char AMP_Dsply[3];
char MUTE_Dsply[3];
char TXLK_Dsply[3];


Right_LCD::Right_LCD(void)
{

}


void Right_LCD::begin(void)
{
	_rightLCD.begin(20, 4);
	bfo.begin();
}


void Right_LCD::init(uint8_t _AGC, uint8_t _IFBW, uint8_t _RATE, uint8_t _ATTN, uint8_t _MUTE, uint8_t _AMP, uint8_t _DH_FLAG)
{

  	_rightLCD.setCursor(0, 0);
  	_rightLCD.print(_rightLCDLine0);

  	_rightLCD.setCursor(0, 1);
  	_rightLCD.print(_rightLCDLine1);

  	_rightLCD.setCursor(0, 2);
 	  _rightLCD.print(_rightLCDLine2);

  	_rightLCD.setCursor(0, 3);
  	_rightLCD.print(_rightLCDLine3);

  	update(IFBW_Cmnd + _IFBW);
  	update(MUTE_Cmnd + _MUTE);
  	update(RATE_Cmnd + _RATE);
  	update(TXLK_Cmnd + _DH_FLAG);
  	update(ATTN_Cmnd + _ATTN);
  	update(AMP_Cmnd + _AMP);
  	update(AGC_Cmnd + _AGC);
}


void Right_LCD::clear(void)
{
	_rightLCD.clear();
}


void Right_LCD::splash(void)
{
  _rightLCD.print(F("       AD5GH        "));
  _rightLCD.setCursor(0, 1);
  _rightLCD.print(F("  EXPRESS RECEIVER  "));
  _rightLCD.setCursor(0, 2);
  _rightLCD.print(F("  HF UP CONVERTING  "));
  _rightLCD.setCursor(0, 3);
  _rightLCD.print(F(" SUPER HET RECEIVER "));
}


void Right_LCD::update(uint8_t command)
{
	switch (command >> 4)
  {
    case 1:                                 //IF FILTER
      command = command & 0x0F;
      _rightLCDLine0[5] = IFBW_1[command];
      _rightLCDLine0[6] = IFBW_2[command];
      _rightLCDLine0[7] = IFBW_3[command];
      _rightLCDLine0[8] = IFBW_4[command];
      _rightLCD.setCursor(0, 0);
      _rightLCD.print(_rightLCDLine0);
      relayBoard.IF_FLTR(command);
      break;

    case 2:                                 //NOT USED

      break;

    case 3:                                 //RF ATTENUATOR
      command = command & 0x0F;
      _rightLCDLine3[1] = ATTN_1[command];
      _rightLCDLine3[2] = ATTN_2[command];
      _rightLCD.setCursor(0, 3);
      _rightLCD.print(_rightLCDLine3);
      relayBoard.RF_ATTN(command);
      break;

    case 4:                                 //RF PRE-AMP
      command = command & 0x0F;
      _rightLCDLine3[4] = AMP_1[command];
      _rightLCDLine3[5] = AMP_2[command];
      _rightLCDLine3[6] = AMP_3[command];
      _rightLCD.setCursor(0, 3);
      _rightLCD.print(_rightLCDLine3);
      relayBoard.RF_AMP(command);
      break;

    case 5:                                 //BAND
      command = command & 0x0F;
      _rightLCDLine3[8] = BAND_1[command];
      _rightLCDLine3[9] = BAND_2[command];
      _rightLCDLine3[10] = BAND_3[command];
      _rightLCD.setCursor(0, 3);
      _rightLCD.print(_rightLCDLine3);
      relayBoard.RF_BPF(command);
      break;

    case 6:                                 //MODE
      command = command & 0x0F;
      _rightLCDLine3[12] = MODE_1[command];
      _rightLCDLine3[13] = MODE_2[command];
      _rightLCDLine3[14] = MODE_3[command];
      _rightLCD.setCursor(0, 3);
      _rightLCD.print(_rightLCDLine3);
      bfo.mode(command);
      break;

    case 7:                                 //AGC
      if_Board.setAGCMode(command & 0x07);
      command = command & 0x0F;
      _rightLCDLine3[16] = AGC_1[command];
      _rightLCDLine3[17] = AGC_2[command];
      _rightLCDLine3[18] = AGC_3[command];
      _rightLCD.setCursor(0, 3);
      _rightLCD.print(_rightLCDLine3);
      break;

    case 8:                                 //MUTE
      command = command & 0x0F;
      _rightLCDLine0[17] = MUTE_1[command];
      _rightLCDLine0[18] = MUTE_2[command];
      _rightLCDLine0[19] = MUTE_3[command];
      _rightLCD.setCursor(0, 0);
      _rightLCD.print(_rightLCDLine0);
      break;

    case 9:                                 //TXLK
      command = command & 0x0F;
      _rightLCDLine1[17] = TXLK_1[command];
      _rightLCDLine1[18] = TXLK_2[command];
      _rightLCDLine1[19] = TXLK_3[command];
      _rightLCD.setCursor(0, 1);
      _rightLCD.print(_rightLCDLine1);
      break;

    case 10:                                //RATE
      command = command & 0x0F;
      _rightLCDLine1[5] = RATE_1[command];
      _rightLCDLine1[6] = RATE_2[command];
      _rightLCDLine1[7] = RATE_3[command];
      _rightLCDLine1[8] = RATE_4[command];
      _rightLCD.setCursor(0, 1);
      _rightLCD.print(_rightLCDLine1);
      break;
  }
}