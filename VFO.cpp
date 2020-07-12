/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* VFO ROUTINES
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
#include <EEPROMex.h>
#include <Main_LCD.h>
#include <Right_LCD.h>
#include <Si570.h>
#include <VFO.h>
#include <Xmt_Link.h>


Main_LCD  _main_LCD;
Right_LCD _right_LCD;
Si570     _si570;
Xmt_Link  _Xmt_Link;

//Index to band arrays:
//0=160M, 1=80M, 2=40M, 3=30M, 4=20M, 5=17M, 6=15M, 7=12M, 8=10Ma, 9=10Mb, 10=10Mc

/* VFO STRUCTURE & VARIABLES */
typedef struct
{
  uint32_t Freq;
  uint8_t Band;
  uint8_t Mode;
} _FreqBandMode;

_FreqBandMode   VFO_A;
_FreqBandMode   VFO_B;
_FreqBandMode   Band_Settings_VFO_A[11];
_FreqBandMode   Band_Settings_VFO_B[11];
_FreqBandMode   Memory_Array[10];

uint8_t VFO_A_Current_Band;
uint8_t VFO_B_Current_Band;

uint32_t VFOA_Band_Limits[2], VFOB_Band_Limits[2];

uint16_t dummyFreqUpdateAccum = 0;


/* MEMORY STRUCTURE & VARIABLES */
typedef struct                                  // structure containing variables to be stored in EEPROM
{
  int Version;                                  // software version, if EEPROM version doesn't match software loaded, EEPROM values are initialized to default values; if software version matches, stored EEPROM values are used to initialize variables
  _FreqBandMode  EE_VFO_A;                      // VFO A
  _FreqBandMode  EE_VFO_B;                      // VFO B
  _FreqBandMode  EE_Band_Settings_VFO_A[11];    // Current settings for each band, VFO A
  _FreqBandMode  EE_Band_Settings_VFO_B[11];    // Current settings for each band, VFO B
  _FreqBandMode  EE_Memory_Array[10];           // ten memories

  uint8_t       EE_VFO_A_Current_Band;          // VFO A current band
  uint8_t       EE_VFO_B_Current_Band;          // VFO B current band

} eepromData;

eepromData savedData;

uint16_t eepromAddress = 0;                     // EEPROM start address


VFO::VFO(void)
{

}


void VFO::begin(void)
{
  
}


void VFO::initVFOA(void)
{
  Set_Band_Limits((long*)&VFOA_Band_Limits, VFO_A.Band);
  _right_LCD.update(Band_Cmnd + VFO_A.Band);
  _right_LCD.update(Mode_Cmnd + VFO_A.Mode);
  updateVFOA(&dummyFreqUpdateAccum);

  _Xmt_Link.updateXmtBand();
}


uint8_t VFO::getVFOA_Band(void)
{
    return(VFO_A.Band);
}


uint8_t VFO::getVFOB_Band(void)
{
    return(VFO_B.Band);
}

int32_t VFO::getVFOA_Dial_Freq(void)
{
    int32_t dialFreq;

    if(VFO_A.Mode == 0) dialFreq = VFO_A.Freq - 1500;
    else if (VFO_A.Mode == 1) dialFreq = VFO_A.Freq + 1500;
    else dialFreq = VFO_A.Freq;

    return(dialFreq);
}


int32_t VFO::getVFOB_Dial_Freq(void)
{
    int32_t dialFreq;

    if(VFO_B.Mode == 0) dialFreq = VFO_B.Freq - 1500;
    else if (VFO_B.Mode == 1) dialFreq = VFO_B.Freq + 1500;
    else dialFreq = VFO_B.Freq;

    return(dialFreq);
}


void VFO::bandChangeVFOA(void)
{
    Band_Settings_VFO_A[VFO_A_Current_Band].Freq = VFO_A.Freq;        // save current band frequency & mode
    Band_Settings_VFO_A[VFO_A_Current_Band].Mode = VFO_A.Mode;

    VFO_A.Freq = Band_Settings_VFO_A[VFO_A.Band].Freq;                // load new band frequency & mode                
    VFO_A.Mode = Band_Settings_VFO_A[VFO_A.Band].Mode;

    VFO_A_Current_Band = VFO_A.Band;                                  // update current band

    Set_Band_Limits((long*)&VFOA_Band_Limits, VFO_A.Band);
    _right_LCD.update(Band_Cmnd + VFO_A.Band);
    _right_LCD.update(Mode_Cmnd + VFO_A.Mode);

    _Xmt_Link.updateXmtBand();

    eepromUpdate();

}


void VFO::updateVFOA(int16_t *_freqUpdate)
{
  VFO_A.Freq = VFO_A.Freq + *_freqUpdate;
  if (VFO_A.Freq < VFOA_Band_Limits[LOWER]) VFO_A.Freq = VFOA_Band_Limits[LOWER];
  if (VFO_A.Freq > VFOA_Band_Limits[UPPER]) VFO_A.Freq = VFOA_Band_Limits[UPPER];
  _si570.VFO_Update(&VFO_A.Freq, &VFO_A.Band);
  _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
}


void VFO::decrementBandVFOA(void)
{
  if (VFO_A.Band == 0)VFO_A.Band = 10;
  else VFO_A.Band--;
  bandChangeVFOA();
  updateVFOA(&dummyFreqUpdateAccum);
}


void VFO::incrementBandVFOA(void)
{
  if (VFO_A.Band == 10)VFO_A.Band = 0;
  else VFO_A.Band++;
  bandChangeVFOA();
  updateVFOA(&dummyFreqUpdateAccum);
}


uint8_t VFO::changeVFOAmode(void)
{
  uint8_t temp = VFO_A.Mode;
  if (temp == 2) temp = 0;
  else temp++;
  VFO_A.Mode = temp;
  _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
  return temp;
}


void VFO::initVFOB(void)
{
  Set_Band_Limits((long*)&VFOB_Band_Limits, VFO_B.Band);
  _right_LCD.update(Band_Cmnd + VFO_B.Band);
  _right_LCD.update(Mode_Cmnd + VFO_B.Mode);
  updateVFOB(&dummyFreqUpdateAccum);

  _Xmt_Link.updateXmtBand();
}


void VFO::bandChangeVFOB()
{
    Band_Settings_VFO_B[VFO_B_Current_Band].Freq = VFO_B.Freq;        // save current band frequency & mode
    Band_Settings_VFO_B[VFO_B_Current_Band].Mode = VFO_B.Mode;

    VFO_B.Freq = Band_Settings_VFO_B[VFO_B.Band].Freq;                // load new band frequency & mode
    VFO_B.Mode = Band_Settings_VFO_B[VFO_B.Band].Mode;

    VFO_B_Current_Band = VFO_B.Band;                                  // update current band

    Set_Band_Limits((long*)&VFOB_Band_Limits, VFO_B.Band);
    _right_LCD.update(Band_Cmnd + VFO_B.Band);
    _right_LCD.update(Mode_Cmnd + VFO_B.Mode);

    _Xmt_Link.updateXmtBand();

    eepromUpdate();
}


void VFO::updateVFOB(int16_t *_freqUpdate)
{
  VFO_B.Freq = VFO_B.Freq + *_freqUpdate;
  if (VFO_B.Freq < VFOB_Band_Limits[LOWER]) VFO_B.Freq = VFOB_Band_Limits[LOWER];
  if (VFO_B.Freq > VFOB_Band_Limits[UPPER]) VFO_B.Freq = VFOB_Band_Limits[UPPER];
  _si570.VFO_Update(&VFO_B.Freq, &VFO_B.Band);
  _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);
}


void VFO::decrementBandVFOB(void)
{
  if (VFO_B.Band == 0)VFO_B.Band = 10;
  else VFO_B.Band--;
  bandChangeVFOB();
  updateVFOB(&dummyFreqUpdateAccum);
}


void VFO::incrementBandVFOB(void)
{
  if (VFO_B.Band == 10)VFO_B.Band = 0;
  else VFO_B.Band++;
  bandChangeVFOB();
  updateVFOB(&dummyFreqUpdateAccum);
}


uint8_t VFO::changeVFOBmode(void)
{
  uint8_t temp = VFO_B.Mode;
  if (temp == 2) temp = 0;
  else temp++;
  VFO_B.Mode = temp;
  _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);
  return temp;
}


void VFO::VFOAtoVFOB(uint8_t *vfoFlag)
{
  Band_Settings_VFO_B[VFO_A.Band].Freq = VFO_A.Freq;                // save VFOA setting to corresponding VFOB band
  Band_Settings_VFO_B[VFO_A.Band].Mode = VFO_A.Mode;

  if (vfoFlag)                                                      // if VFOB is active...
  {
    if (VFO_A.Band != VFO_B.Band)                                   // if VFOA and VFO on different bands, change VFO B band
    {
    VFO_B.Band = VFO_A.Band;
    bandChangeVFOB();      
    }           
    
    else                                                            // if VFOA and VFOB already on same band...
    {
      VFO_B.Freq = VFO_A.Freq;                                      // copy frequency and mode data from VFOA to VFOB
      VFO_B.Mode = VFO_A.Mode;
    }

    initVFOB();                                                     // update VFO B
  }

  else                                                              // if VFOA is active...
  {
    Band_Settings_VFO_B[VFO_B.Band].Freq = VFO_B.Freq;              // save current VFOB settings
    Band_Settings_VFO_B[VFO_B.Band].Mode = VFO_B.Mode;
    VFO_B.Freq = VFO_A.Freq;                                        // load new VFOB settings from VFOA
    VFO_B.Mode = VFO_A.Mode;
    VFO_B.Band = VFO_A.Band;
    _main_LCD.updateVFOBDisplay(&VFO_B.Freq, &VFO_B.Mode);           // update the display
  }
  eepromUpdate();
}


void VFO::memoryToVFOA(uint8_t *memNumber, uint8_t *vfoFlag)
{
  if (VFO_A.Band != Memory_Array[*memNumber].Band)
  {
    Band_Settings_VFO_A[Memory_Array[*memNumber].Band].Freq = Memory_Array[*memNumber].Freq;        // save new band frequency & mode
    Band_Settings_VFO_A[Memory_Array[*memNumber].Band].Mode = Memory_Array[*memNumber].Mode;
    VFO_A.Band = Memory_Array[*memNumber].Band;
  
    Band_Settings_VFO_A[VFO_A_Current_Band].Freq = VFO_A.Freq;        // save current band frequency & mode
    Band_Settings_VFO_A[VFO_A_Current_Band].Mode = VFO_A.Mode;

    VFO_A.Freq = Band_Settings_VFO_A[VFO_A.Band].Freq;                // load new band frequency & mode                
    VFO_A.Mode = Band_Settings_VFO_A[VFO_A.Band].Mode;

    VFO_A_Current_Band = VFO_A.Band;                                  // update current band

    Set_Band_Limits((long*)&VFOA_Band_Limits, VFO_A.Band);
  }
  else
  {
    VFO_A.Freq = Memory_Array[*memNumber].Freq;
    VFO_A.Mode = Memory_Array[*memNumber].Mode;
  }

  if (!*vfoFlag) 
    {
      _right_LCD.update(Band_Cmnd + VFO_A.Band);
      _right_LCD.update(Mode_Cmnd + VFO_A.Mode);
      updateVFOA(&dummyFreqUpdateAccum);
    }
  else _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
  eepromUpdate();
}


void VFO::VFOAtoMemory(uint8_t *memNumber)
{
  Memory_Array[*memNumber].Freq = VFO_A.Freq;
  Memory_Array[*memNumber].Band = VFO_A.Band;
  Memory_Array[*memNumber].Mode = VFO_A.Mode;
  _main_LCD.showMemoryDisplay(&Memory_Array[*memNumber].Freq, &Memory_Array[*memNumber].Mode, memNumber);
  eepromUpdate();
}


void VFO::displayMemory(uint8_t *memNumber)
{
  _main_LCD.showMemoryDisplay(&Memory_Array[*memNumber].Freq, &Memory_Array[*memNumber].Mode, memNumber);
}


/********************BANDS EDGE LIMITS****************************************************************************************/

#define Bottom_160M   1800000
#define Top_160M      2000000

#define Bottom_80M    3500000
#define Top_80M       4000000

#define Bottom_40M    7000000
#define Top_40M       7300000

#define Bottom_30M    10100000
#define Top_30M       10150000

#define Bottom_20M    14000000
#define Top_20M       14350000

#define Bottom_17M    18068000
#define Top_17M       18168000

#define Bottom_15M    21000000
#define Top_15M       21450000

#define Bottom_12M    24890000
#define Top_12M       24990000

#define Bottom_10aM   28000000
#define Top_10aM      28500000

#define Bottom_10bM   28500000
#define Top_10bM      29000000

#define Bottom_10cM   29000000
#define Top_10cM      29700000


void VFO::Set_Band_Limits(long *Band_Limit_Array, char Band_Number)
{
  switch (Band_Number)
  {
    case 0:
      *(Band_Limit_Array) = Bottom_160M;
      *(Band_Limit_Array + 1) = Top_160M;
      break;

    case 1:
      *(Band_Limit_Array) = Bottom_80M;
      *(Band_Limit_Array + 1) = Top_80M;
      break;

    case 2:
      *(Band_Limit_Array) = Bottom_40M;
      *(Band_Limit_Array + 1) = Top_40M;
      break;

    case 3:
      *(Band_Limit_Array) = Bottom_30M;
      *(Band_Limit_Array + 1) = Top_30M;
      break;

    case 4:
      *(Band_Limit_Array) = Bottom_20M;
      *(Band_Limit_Array + 1) = Top_20M;
      break;

    case 5:
      *(Band_Limit_Array) = Bottom_17M;
      *(Band_Limit_Array + 1) = Top_17M;
      break;

    case 6:
      *(Band_Limit_Array) = Bottom_15M;
      *(Band_Limit_Array + 1) = Top_15M;
      break;

    case 7:
      *(Band_Limit_Array) = Bottom_12M;
      *(Band_Limit_Array + 1) = Top_12M;
      break;

    case 8:
      *(Band_Limit_Array) = Bottom_10aM;
      *(Band_Limit_Array + 1) = Top_10aM;
      break;

    case 9:
      *(Band_Limit_Array) = Bottom_10bM;
      *(Band_Limit_Array + 1) = Top_10bM;
      break;

    case 10:
      *(Band_Limit_Array) = Bottom_10cM;
      *(Band_Limit_Array + 1) = Top_10cM;
      break;
  }
}


uint8_t VFO::freqToBandNumber(int32_t freq)
{
  if(freq >= Bottom_160M && freq <= Top_160M) return (0);
  else if(freq >= Bottom_80M && freq <= Top_80M) return (1);
  else if(freq >= Bottom_40M && freq <= Top_40M) return (2);
  else if(freq >= Bottom_30M && freq <= Top_30M) return (3);
  else if(freq >= Bottom_20M && freq <= Top_20M) return (4);
  else if(freq >= Bottom_17M && freq <= Top_17M) return (5);
  else if(freq >= Bottom_15M && freq <= Top_15M) return (6);
  else if(freq >= Bottom_12M && freq <= Top_12M) return (7);
  else if(freq >= Bottom_10aM && freq <= Top_10aM) return (8);
  else if(freq >= Bottom_10bM && freq <= Top_10bM) return (9);
  else return (10);
}


void VFO::catVFO_A_FreqUpdate(int32_t freq)
{
  VFO_A.Band = freqToBandNumber(freq);
  bandChangeVFOA();
  
  if(VFO_A.Mode == 0) VFO_A.Freq = freq + 1500;
  else if (VFO_A.Mode == 1) VFO_A.Freq = freq - 1500;
  else VFO_A.Freq = freq;
  
  if (VFO_A.Freq < VFOA_Band_Limits[LOWER]) VFO_A.Freq = VFOA_Band_Limits[LOWER];
  if (VFO_A.Freq > VFOA_Band_Limits[UPPER]) VFO_A.Freq = VFOA_Band_Limits[UPPER];
  
  _si570.VFO_Update(&VFO_A.Freq, &VFO_A.Band);
  _main_LCD.updateVFOADisplay(&VFO_A.Freq, &VFO_A.Mode);
}


void VFO::eepromUpdate(void)                                         // WRITE VARIABLES TO EEPROM DATA ARRAY AND THEN UPDATE EEPROM
{
  for (int n = 0; n < 11; n++)
  {
    savedData.EE_Band_Settings_VFO_A[n].Freq = Band_Settings_VFO_A[n].Freq;
    savedData.EE_Band_Settings_VFO_A[n].Band = Band_Settings_VFO_A[n].Band;
    savedData.EE_Band_Settings_VFO_A[n].Mode = Band_Settings_VFO_A[n].Mode;

    savedData.EE_Band_Settings_VFO_B[n].Freq = Band_Settings_VFO_B[n].Freq;
    savedData.EE_Band_Settings_VFO_B[n].Band = Band_Settings_VFO_B[n].Band;
    savedData.EE_Band_Settings_VFO_B[n].Mode = Band_Settings_VFO_B[n].Mode;
  }

  for (int n = 0; n < 10; n++)
  {
    savedData.EE_Memory_Array[n].Freq = Memory_Array[n].Freq;
    savedData.EE_Memory_Array[n].Band = Memory_Array[n].Band;
    savedData.EE_Memory_Array[n].Mode = Memory_Array[n].Mode;
  }

  savedData.EE_VFO_A.Freq = VFO_A.Freq;
  savedData.EE_VFO_A.Band = VFO_A.Band;
  savedData.EE_VFO_A.Mode = VFO_A.Mode;

  savedData.EE_VFO_A_Current_Band = VFO_A_Current_Band;

  savedData.EE_VFO_B.Freq = VFO_B.Freq;
  savedData.EE_VFO_B.Band = VFO_B.Band;
  savedData.EE_VFO_B.Mode = VFO_B.Mode;

  savedData.EE_VFO_B_Current_Band = VFO_B_Current_Band;

  EEPROM.updateBlock(eepromAddress, savedData);
}


void VFO::eepromRead(uint16_t *currentVersion)
{
  _main_LCD.eepromStart();
  EEPROM.readBlock(eepromAddress, savedData);             // READ EEPROM DATA INTO DATA ARRAY

  if (savedData.Version != *currentVersion)               // If new version of software or Arduino Mega not previously initialized, write default values to EEPROM
  {
    _main_LCD.eepromNoDataFound();
    _main_LCD.eepromInitializing();
    savedData.Version = *currentVersion;

    Memory_Array[0].Freq = 1836600 + 1500;                // initialize memories to WSPR frequencies
    Memory_Array[0].Band = 0x00;
    Memory_Array[0].Mode = 0x00;

    Memory_Array[1].Freq = 3592600 + 1500;
    Memory_Array[1].Band = 0x01;
    Memory_Array[1].Mode = 0x00;

    Memory_Array[2].Freq = 7038600 + 1500;
    Memory_Array[2].Band = 0x02;
    Memory_Array[2].Mode = 0x00;

    Memory_Array[3].Freq = 10138700 + 1500;
    Memory_Array[3].Band = 0x03;
    Memory_Array[3].Mode = 0x00;

    Memory_Array[4].Freq = 14095600 + 1500;
    Memory_Array[4].Band = 0x04;
    Memory_Array[4].Mode = 0x00;

    Memory_Array[5].Freq = 18104600 + 1500;
    Memory_Array[5].Band = 0x05;
    Memory_Array[5].Mode = 0x00;

    Memory_Array[6].Freq = 21094600 + 1500;
    Memory_Array[6].Band = 0x06;
    Memory_Array[6].Mode = 0x00;

    Memory_Array[7].Freq = 24924600 + 1500;
    Memory_Array[7].Band = 0x07;
    Memory_Array[7].Mode = 0x00;

    Memory_Array[8].Freq = 28124000 + 1500;
    Memory_Array[8].Band = 0x08;
    Memory_Array[8].Mode = 0x00;

    Memory_Array[9].Freq = 28500000 + 1500;
    Memory_Array[9].Band = 0x09;
    Memory_Array[9].Mode = 0x00;

    VFO_A.Freq = 14250000 + 1500;
    VFO_A.Band = 0x04;
    VFO_A.Mode = 0x00;

    VFO_A_Current_Band = 0x04;

    VFO_B.Freq = 10138700 + 1500;
    VFO_B.Band = 0x03;
    VFO_B.Mode = 0x00;

    VFO_B_Current_Band = 0x03;

    Band_Settings_VFO_A[0].Freq = 1836600 + 1500;               // initialize band frequencies to WSPR frequencies
    Band_Settings_VFO_A[0].Band = 0x00;
    Band_Settings_VFO_A[0].Mode = 0x00;

    Band_Settings_VFO_A[1].Freq = 3592600 + 1500;
    Band_Settings_VFO_A[1].Band = 0x01;
    Band_Settings_VFO_A[1].Mode = 0x00;

    Band_Settings_VFO_A[2].Freq = 7038600 + 1500;
    Band_Settings_VFO_A[2].Band = 0x02;
    Band_Settings_VFO_A[2].Mode = 0x00;

    Band_Settings_VFO_A[3].Freq = 10138700 + 1500;
    Band_Settings_VFO_A[3].Band = 0x03;
    Band_Settings_VFO_A[3].Mode = 0x00;

    Band_Settings_VFO_A[4].Freq = 14095600 + 1500;
    Band_Settings_VFO_A[4].Band = 0x04;
    Band_Settings_VFO_A[4].Mode = 0x00;

    Band_Settings_VFO_A[5].Freq = 18104600 + 1500;
    Band_Settings_VFO_A[5].Band = 0x05;
    Band_Settings_VFO_A[5].Mode = 0x00;

    Band_Settings_VFO_A[6].Freq = 21094600 + 1500;
    Band_Settings_VFO_A[6].Band = 0x06;
    Band_Settings_VFO_A[6].Mode = 0x00;

    Band_Settings_VFO_A[7].Freq = 24924600 + 1500;
    Band_Settings_VFO_A[7].Band = 0x07;
    Band_Settings_VFO_A[7].Mode = 0x00;

    Band_Settings_VFO_A[8].Freq = 28124000 + 1500;
    Band_Settings_VFO_A[8].Band = 0x08;
    Band_Settings_VFO_A[8].Mode = 0x00;

    Band_Settings_VFO_A[9].Freq = 28500000 + 1500;
    Band_Settings_VFO_A[9].Band = 0x09;
    Band_Settings_VFO_A[9].Mode = 0x00;

    Band_Settings_VFO_A[10].Freq = 29000000 + 1500;
    Band_Settings_VFO_A[10].Band = 0x0A;
    Band_Settings_VFO_A[10].Mode = 0x00;


    Band_Settings_VFO_B[0].Freq = 1836600 + 1500;               // initialize memories to WSPR frequencies
    Band_Settings_VFO_B[0].Band = 0x00;
    Band_Settings_VFO_B[0].Mode = 0x00;

    Band_Settings_VFO_B[1].Freq = 3592600 + 1500;
    Band_Settings_VFO_B[1].Band = 0x01;
    Band_Settings_VFO_B[1].Mode = 0x00;

    Band_Settings_VFO_B[2].Freq = 7038600 + 1500;
    Band_Settings_VFO_B[2].Band = 0x02;
    Band_Settings_VFO_B[2].Mode = 0x00;

    Band_Settings_VFO_B[3].Freq = 10138700 + 1500;
    Band_Settings_VFO_B[3].Band = 0x03;
    Band_Settings_VFO_B[3].Mode = 0x00;

    Band_Settings_VFO_B[4].Freq = 14095600 + 1500;
    Band_Settings_VFO_B[4].Band = 0x04;
    Band_Settings_VFO_B[4].Mode = 0x00;

    Band_Settings_VFO_B[5].Freq = 18104600 + 1500;
    Band_Settings_VFO_B[5].Band = 0x05;
    Band_Settings_VFO_B[5].Mode = 0x00;

    Band_Settings_VFO_B[6].Freq = 21094600 + 1500;
    Band_Settings_VFO_B[6].Band = 0x06;
    Band_Settings_VFO_B[6].Mode = 0x00;

    Band_Settings_VFO_B[7].Freq = 24924600 + 1500;
    Band_Settings_VFO_B[7].Band = 0x07;
    Band_Settings_VFO_B[7].Mode = 0x00;

    Band_Settings_VFO_B[8].Freq = 28124000 + 1500;
    Band_Settings_VFO_B[8].Band = 0x08;
    Band_Settings_VFO_B[8].Mode = 0x00;

    Band_Settings_VFO_B[9].Freq = 28500000 + 1500;
    Band_Settings_VFO_B[9].Band = 0x09;
    Band_Settings_VFO_B[9].Mode = 0x00;

    Band_Settings_VFO_B[10].Freq = 29000000 + 1500;
    Band_Settings_VFO_B[10].Band = 0x0A;
    Band_Settings_VFO_B[10].Mode = 0x00;

    eepromUpdate();                                             // write default values to EEPROM
  }

  _main_LCD.eepromReadingData();                     // UPDATE VARIABLES FROM EEPROM DATA ARRAY

  for (int n = 0; n < 11; n++)
  {
    Band_Settings_VFO_A[n].Freq = savedData.EE_Band_Settings_VFO_A[n].Freq;
    Band_Settings_VFO_A[n].Band = savedData.EE_Band_Settings_VFO_A[n].Band;
    Band_Settings_VFO_A[n].Mode = savedData.EE_Band_Settings_VFO_A[n].Mode;

    Band_Settings_VFO_B[n].Freq = savedData.EE_Band_Settings_VFO_B[n].Freq;
    Band_Settings_VFO_B[n].Band = savedData.EE_Band_Settings_VFO_B[n].Band;
    Band_Settings_VFO_B[n].Mode = savedData.EE_Band_Settings_VFO_B[n].Mode;
  }

  for (int n = 0; n < 10; n++)
  {
    Memory_Array[n].Freq = savedData.EE_Memory_Array[n].Freq;
    Memory_Array[n].Band = savedData.EE_Memory_Array[n].Band;
    Memory_Array[n].Mode = savedData.EE_Memory_Array[n].Mode;
  }

  VFO_A.Freq = savedData.EE_VFO_A.Freq;
  VFO_A.Band = savedData.EE_VFO_A.Band;
  VFO_A.Mode = savedData.EE_VFO_A.Mode;

  VFO_A_Current_Band = savedData.EE_VFO_A_Current_Band;

  VFO_B.Freq = savedData.EE_VFO_B.Freq;
  VFO_B.Band = savedData.EE_VFO_B.Band;
  VFO_B.Mode = savedData.EE_VFO_B.Mode;

  VFO_B_Current_Band = savedData.EE_VFO_B_Current_Band;
}
