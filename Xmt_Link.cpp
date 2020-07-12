/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* BFO ROUTINES
*
* Copyright 2018 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
*
* VERSION 1.1.0
* July 4, 2018
*
********************************************************************************/

#include <arduino.h>
#include <RelayBoard.h>
#include <VFO.h>
#include <Xmt_Link.h>
#include <Function.h>

VFO 		__VFO;
RelayBoard	__RelayBoard;
Function 	__Function;

char _respBD[5] = "BD0 ;";
char _cmdTX[5] = "MDTR;";
char _cmdRX[5] = "MDRX;";

char xmtCommand[20];
uint8_t xmtCounter = 0;
uint8_t xmtCharacter = 0;

char ampCommand[20];
uint8_t ampCounter = 0;
uint8_t ampCharacter = 0;

uint8_t vfoFlag;


Xmt_Link::Xmt_Link(void)
{

}


void Xmt_Link::begin(void)
{
	Serial2.begin(115200);							// transmitter serial link, pin 17 RX, pin 16 TX
	Serial3.begin(115200);							// amplifier serial link, pin 19 RX, pin 18 TX
}


uint8_t Xmt_Link::available(void)
{
	if(Serial2.available())
	{
		xmtCharacter = Serial2.read();

		if(xmtCharacter == ';') 
		{
			xmtCommand[xmtCounter] = xmtCharacter;
			executeXmtr();
			xmtCounter = 0;
		}

		else
		{
			xmtCommand[xmtCounter] = xmtCharacter;
			xmtCounter++;
			if(xmtCounter == 21) xmtCounter = 0;
		}
	}

	if(Serial3.available())
	{
		ampCharacter = Serial3.read();

		if(ampCharacter == ';') 
		{
			ampCommand[ampCounter] = ampCharacter;
			executeAmp();
			ampCounter = 0;
		}

		else
		{
			ampCommand[ampCounter] = ampCharacter;
			ampCounter++;
			if(ampCounter == 21) ampCounter = 0;
		}
	}
}


void Xmt_Link::executeXmtr(void)
{
	switch(xmtCommand[0])
	{
		case 'T':
			switch(xmtCommand[1])
			{
				case 'X':								//Transmit mode
					__Function.setMainAFGain(65);
					__RelayBoard.TX_MODE();				//Relay Board to transmit mode
					send_Amp(_cmdTX, 5);				//Amplifier to transmit mode
					break;
			}
			break;

		case 'R':
			switch(xmtCommand[1])
			{
				case 'X':								//Reveive mode
					send_Amp(_cmdRX, 5);				//Amplifier to receive mode
					__RelayBoard.RX_MODE();				//Relay Board to receive mode
					delay(50);
					__Function.setMainAFGain(0);
					break;
			}
			break;

		case 'B':
			switch(xmtCommand[1])
			{
				case 'D':								//transmitter request for frequency band
				updateXmtBand();						//send band update to both transmitter and amplifier
				break;
			}
			break;
	}
}


void Xmt_Link::executeAmp(void)
{
	switch(ampCommand[0])
	{
		case 'B':
			switch(ampCommand[1])
			{
				case 'D':								//amplifier request for frequency band
				updateXmtBand();						//send band update to both transmitter and amplifier
				break;
			}
			break;
	}
}


void Xmt_Link::updateXmtBand(void)						//send frequency band to transmitter and amplifier
{
	if(!vfoFlag) _respBD[3] = __VFO.getVFOA_Band();		//VFO A active
	else _respBD[3] = __VFO.getVFOB_Band();				//VFO B active
	send_Xmtr(_respBD, 5);
	send_Amp(_respBD, 5);
}


void Xmt_Link::send_Xmtr(uint8_t data[], uint8_t num)
{
	for (uint8_t i=0; i<num; i++) 
		{
			Serial2.write(data[i]);
		}
}


void Xmt_Link::send_Amp(uint8_t data[], uint8_t num)
{
	for (uint8_t i=0; i<num; i++) 
		{
			Serial3.write(data[i]);
		}
}


void Xmt_Link::vfoAB(uint8_t flag)
{
	vfoFlag = flag;
}

