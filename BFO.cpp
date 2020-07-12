/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* BFO ROUTINES
*
* Copyright 2017 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
*
* VERSION 1.0.9
* December 27, 2017
*
******************************************************************************/


//USB BFO Frequency: 8,998,500-Hz; Frequency display offset: -1,500-Hz

//LSB BFO Frequency: 9,001,500-Hz; Frequency display offset: +1,500-Hz

//CW  BFO Frequency: 9,000,000-Hz; Frequency display offset: 0-Hz

#include <BFO.h>
#include <si5351.h>

Si5351 si5351;

BFO::BFO(void)
{

}

void BFO::begin(void)
{
	si5351.init(SI5351_CRYSTAL_LOAD_10PF, 0, 0);
	si5351.drive_strength(0, SI5351_DRIVE_2MA);
	si5351.drive_strength(1, SI5351_DRIVE_2MA);
}

void BFO::mode(uint8_t _mode)
{
	switch(_mode)
	{
		case 0:
			usb();
			break;

		case 1:
			lsb();
			break;

		case 2:
			cw();
			break;
	}
}

void BFO::usb(void)					// 
{
	si5351.set_freq(899850000ULL, SI5351_CLK0);
  	si5351.set_freq(899850000ULL, SI5351_CLK1);
}

void BFO::lsb(void)
{
	si5351.set_freq(900150000ULL, SI5351_CLK0);
  	si5351.set_freq(900150000ULL, SI5351_CLK1);
}

void BFO::cw(void)
{
	si5351.set_freq(900010000ULL, SI5351_CLK0);
  	si5351.set_freq(900010000ULL, SI5351_CLK1);
}

