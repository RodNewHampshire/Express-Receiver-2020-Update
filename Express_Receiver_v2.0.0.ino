
/****************************************************************************************************************************

  Express Receiver
  High Performance All Band Amateur Radio Receiver

  by Rod Gatehouse

  Distributed under the terms of the MIT License:
  http://www.opensource.org/licenses/mit-license

  VERSION 2.0.0
  May 26, 2020

  Added CAT control

*****************************************************************************************************************************/


/* RF Front End Insertion Loss as measured with N2PK Vector Network Analyzer
  160M:  0.7dB
  80M:   0.5dB
  40M:   1.0dB
  30M:   1.3dB
  20M:   1,2dB
  17M:   1.4dB
  15M:   1.4dB
  12M:   1.3dB
  10M:   1.8dB */

/* IFBW = 0     2100-Hz
   IFBW = 1     1800-Hz
   IFBW = 2     500-Hz
   IFBW = 3     250-Hz  */

/*IF Filter Insertion Loss as measured with N2PK Vector Network Analyzer
  2100-Hz:     5.25dB        0dB
  1800-Hz:     5.25dB        0db
  500-Hz:      7.5dB         -2.25dB
  250-Hz:      10.5dB        -5.25dB*/


#include <ft857d.h>
#include <Function.h>
#include <IF_Board.h>
#include <Keypad1.h>
#include <Local_Oscillator.h>
#include <Main_LCD.h>
#include <Right_LCD.h>
#include <Rotary.h>
#include <VFO.h>
#include <Xmt_Link.h>

uint16_t currentVersion  = 10101;                       // current software version X.YY.ZZ, i.e., 1.00.00

ft857d            catIntfc = ft857d();
Function          function;
IF_Board          ifBoard;
Keypad1           keypad1;
Local_Oscillator  localOscillator;
Main_LCD          mainLCD;
Right_LCD         rightLCD;
VFO               vfo;
Xmt_Link          xmtLink;

/********************Frequency, Band, & Mode Variables************************************************************************/

//Index to band arrays:
//0=160M, 1=80M, 2=40M, 3=30M, 4=20M, 5=17M, 6=15M, 7=12M, 8=10Ma, 9=10Mb, 10=10Mc

int16_t FreqUpdate = 0;
int16_t lastFreqUpdate = 0;
uint8_t VFO_Flag;                                       // 0 = VFO A, 1 = VFO B


/******************** for Memory Display *************************************************************************************/

#define MEM_HIDE_PERIOD 100000                          // time of memory display after last memory button press

unsigned long memoryDisplayHide = MEM_HIDE_PERIOD;
uint8_t MEM_NUM = 0;
uint8_t MEM_Disp_Flag = 0;


/******************** for loop ***********************************************************************************************/

#define  HEART_BEAT 2000                                // heart beat period
uint16_t heartBeat = 0;                                 // heart beat counter


/******************** for keypad *********************************************************************************************/

#define SLOW  0                                         // AGC settings
#define FAST  1
#define MAN   2

#define USB   0                                         // mode commands
#define LSB   1
#define  CW   2

// define initial settings for right LCD controls
uint8_t AGC = 0;                                        // AGC: SLOW
uint8_t IFBW = 0;                                       // IFBW: 2500 Hz
uint8_t RATE = 1;                                       // Tuning Rate: 100 Hz
int16_t TuningRate = 250;
uint8_t ATTN = 0;                                       // RF Attn: 0 dB
uint8_t MUTE = 0;                                       // Mute: OFF
uint8_t AMP = 0;                                        // RF Amp: OFF
uint8_t DH_FLAG = 0;                                    // Dial Hold: OFF


/******************** for Optical Encoders ***********************************************************************************/

#define TUNE_ENCDR_A 2                                  // main tuning encoder pin definitions
#define TUNE_ENCDR_B 3

uint8_t tuneOld = 0;

#define RF_ENCDR_A  63                                  // rf attn encoder pin definitions
#define RF_ENCDR_B  62

Rotary rotary = Rotary(RF_ENCDR_A, RF_ENCDR_B);
uint8_t rfDial = 0x30;

/******************** for Serial Ports ***************************************************************************************/


/******************** setup() ************************************************************************************************/

void setup()
{
  Serial.begin(9600);
  Serial.println("Serial port ready...");

  xmtLink.begin();
  mainLCD.begin();
  rightLCD.begin();

  function.eepromRead(&currentVersion);

  ifBoard.begin();
  keypad1.begin(4, 5, 6, 7, 8, 9, 10, 11);              // pins for 5 columns followed by 3 rows
  function.begin();

  mainLCD.splash();
  rightLCD.splash();
  ifBoard.splash();
  delay(500);

  vfo.eepromRead(&currentVersion);

  rightLCD.clear();
  rightLCD.init(AGC, IFBW, RATE, ATTN, MUTE, AMP, DH_FLAG);
  mainLCD.clear();

  vfo.initVFOB();
  vfo.initVFOA();
  VFO_Flag = 0;

  localOscillator.setFrequency();

  ifBoard.sMeterInit();

  pinMode(TUNE_ENCDR_A, INPUT);
  pinMode(TUNE_ENCDR_B, INPUT);

  if (digitalRead(TUNE_ENCDR_A)) bitSet(tuneOld, 4);
  if (digitalRead(TUNE_ENCDR_B)) bitSet(tuneOld, 5);

  attachInterrupt(digitalPinToInterrupt(TUNE_ENCDR_A), tuneEncdrISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TUNE_ENCDR_B), tuneEncdrISR, CHANGE);

  catIntfc.addCATPtt(catGoPtt);
  catIntfc.addCATAB(catGoToggleVFOs);
  catIntfc.addCATFSet(catSetFreq);
  catIntfc.addCATMSet(catSetMode);
  catIntfc.addCATGetFreq(catGetFreq);
  catIntfc.addCATGetMode(catGetMode);
  catIntfc.addCATSMeter(catGetSMeter);
  catIntfc.addCATTXStatus(catGetTXStatus);

  catIntfc.begin();
}


/******************** loop() *************************************************************************************************/

void loop ()
{
  xmtLink.available();
  catIntfc.check();

  if (FreqUpdate != 0)                                      // update VFO frequency if main tuning dial turned
  {
    if (!VFO_Flag) vfo.updateVFOA(&FreqUpdate);
    else vfo.updateVFOB(&FreqUpdate);
    FreqUpdate = 0;
  }

  else
  {
    if (++heartBeat == HEART_BEAT & FreqUpdate == 0)
    {
      heartBeat = 0;                                        // activities executed at heart beat cadence go here

      if (!VFO_Flag) mainLCD.indicateVFOA();                // flash "*" for active VFO
      else mainLCD.indicateVFOB();
      ifBoard.getSMeterReading();                           // update S Meter reading
    }


    if (FreqUpdate == 0)                                    // if a frequency update is not pending...
    {
      Keypad_Update();                                      // check keypad for button press
      if (rfEncdr() != 0) rightLCD.update(rfDial);          // check if RF Attn dial turned
    }


    if (MEM_Disp_Flag)                                      // timer for hiding memory display on Main LCD
    {
      if (memoryDisplayHide == 0)
      {
        mainLCD.hideMemoryDisplay();
        memoryDisplayHide = MEM_HIDE_PERIOD;
        MEM_Disp_Flag = 0;
      }
      else memoryDisplayHide--;
    }
  }
}


/******************** Interrupt Routines ************************************************************************************/

void tuneEncdrISR (void)
{
  uint8_t tuneNew = 0;
  uint8_t tuneDiff;

  if (DH_FLAG) return;
  if (FreqUpdate != 0) return;

  if (digitalRead(TUNE_ENCDR_A)) bitSet(tuneNew, 4);
  if (digitalRead(TUNE_ENCDR_B)) bitSet(tuneNew, 5);

  tuneDiff = tuneNew ^ tuneOld;

  switch (tuneDiff)
  {
    case 0x20:
      if (tuneNew == 0x00 | tuneNew == 0x30) FreqUpdate += TuningRate;
      else FreqUpdate -= TuningRate;
      break;

    case 0x10:
      if (tuneNew == 0x00 | tuneNew == 0x30) FreqUpdate -= TuningRate;
      else FreqUpdate += TuningRate;
      break;
  }
  tuneOld = tuneNew;

  if (lastFreqUpdate != FreqUpdate)
  {
    lastFreqUpdate = FreqUpdate;
    FreqUpdate = 0;
  }
}


/******************** RF Attn Encoder Routine *******************************************************************************/

uint8_t rfEncdr (void)
{
  unsigned char result = rotary.process();

  if (result == DIR_CW)
  {
    if (rfDial == 0x2F) rfDial = 0x40;                // change from rf attn mode to rf preamp mode
    else if (rfDial == 0x40) rfDial = 0x41;           // turn on rf preamp
    else if (rfDial == 0x41);                         // rf preamp already on, do nothing
    else rfDial--;                                    // otherwise, decrease rf attn
    return rfDial;
  }

  else if (result == DIR_CCW)
  {
    if (rfDial == 0x41) rfDial = 0x40;                // turn off rf preamp
    else if (rfDial == 0x40) rfDial = 0x30;           // change from rf preamp mode to rf attn mode
    else if (rfDial == 0x3F);                         // already at 30dB attn (max), do nothing
    else rfDial++;                                    // otherwise, increase rf attn
    return rfDial;
  }
  return 0;
}


/******************** Keypad Routines ***************************************************************************************/

char Keypad_Update(void)
{
  char buttonPress, temp;

  buttonPress = keypad1.scan();
  if (buttonPress == 0) return 0;

  switch (buttonPress)
  {
    case 1:                                     // VFO A/B
      if (DH_FLAG) break;
      if (VFO_Flag)
      {
        VFO_Flag = 0;
        xmtLink.vfoAB(VFO_Flag);
        vfo.initVFOA();
        mainLCD.deselectVFOB();
      }
      else
      {
        VFO_Flag = 1;
        xmtLink.vfoAB(VFO_Flag);
        vfo.initVFOB();
        mainLCD.deselectVFOA();
      }
      break;

    case 2:                                     // VFO A -> B

      vfo.VFOAtoVFOB(&VFO_Flag);
      break;

    case 3:                                     // MEMORY SELECT
      if (!MEM_Disp_Flag)
      {
        MEM_Disp_Flag = 1;
        vfo.displayMemory(&MEM_NUM);
        break;
      }
      else
      {
        memoryDisplayHide = MEM_HIDE_PERIOD;
        MEM_NUM++;
        if (MEM_NUM == 10)MEM_NUM = 0;
        vfo.displayMemory(&MEM_NUM);
        break;
      }

    case 4:                                     // VFO A -> MEMORY
      if (!MEM_Disp_Flag) break;
      else
      {
        memoryDisplayHide = MEM_HIDE_PERIOD;
        vfo.VFOAtoMemory(&MEM_NUM);
      }
      break;

    case 5:                                     // MEMORY -> VFO A
      if (!MEM_Disp_Flag) break;
      else
      {
        memoryDisplayHide = MEM_HIDE_PERIOD;
        vfo.memoryToVFOA(&MEM_NUM, &VFO_Flag);
      }
      break;

    case 6:                                     // IF BANDWIDTH
      if (IFBW == 3) IFBW = 0;
      else IFBW++;
      rightLCD.update(IFBW_Cmnd + IFBW);
      break;

    case 7:                                     // AGC
      if (AGC == MAN) AGC = SLOW;
      else AGC++;
      rightLCD.update(AGC_Cmnd + AGC);
      break;

    case 8:                                     // MODE
      if (!VFO_Flag) rightLCD.update(Mode_Cmnd + vfo.changeVFOAmode());             // VFO A active
      else rightLCD.update(Mode_Cmnd + vfo.changeVFOBmode());                       // VFO B active
      break;

    case 9:                                     // RATE
      if (TuningRate == 250)
      {
        TuningRate = 1000;
        RATE = 0;
      }
      else if (TuningRate == 1000)
      {
        TuningRate = 100;
        RATE = 2;
      }
      else if (TuningRate == 100)
      {
        TuningRate = 10;
        RATE = 3;
      }
      else if (TuningRate == 10)
      {
        TuningRate = 250;
        RATE = 1;
      }
      rightLCD.update(RATE_Cmnd + RATE);
      break;

    case 10:                                    // MUTE
      if (MUTE == 0) MUTE = 1;
      else MUTE = 0;
      rightLCD.update(MUTE_Cmnd + MUTE);
      if (MUTE) function.setMainAFGain(65);
      else function.setMainAFGain(0);
      break;

    case 11:                                    // FUNCTION
      function.select();
      if (AGC == MAN) function.refreshManIFGainSetDisplay();      // when done, refresh manual IF gain display
      else ifBoard.sMeterInit();                                  // else when done, refresh s meter display
      break;

    case 12:                                    // ENTER
      vfo.eepromUpdate();                       // saves current frequency, band, mode settings to EEPROM when in main keypad loop
      break;

    case 13:                                    // UP
      if (DH_FLAG) break;
      FreqUpdate = 0;
      if (!VFO_Flag) vfo.incrementBandVFOA();   // band change up when in main keypad loop
      else vfo.incrementBandVFOB();
      break;

    case 14:                                    // DOWN
      if (DH_FLAG) break;
      FreqUpdate = 0;
      if (!VFO_Flag) vfo.decrementBandVFOA();   // band change down when in main keypad loop
      else vfo.decrementBandVFOB();
      break;

    case 15:                                    // DIAL HOLD - Doubling up with TX LOCK for now
      if (DH_FLAG == 0) DH_FLAG = 1;
      else if (DH_FLAG == 1)                    // release dial hold
      {
        DH_FLAG = 0;
        FreqUpdate = 0;                         // clear any frequency updates that may have occured during dial hold
      }
      rightLCD.update(TXLK_Cmnd + DH_FLAG);
      break;
  }
  delay(250);                                   // debounce delay
  return 1;
}


/******************** CAT Routines ******************************************************************************************/


void catGoPtt(bool pttf)                     // PTT Toggle
{
  bool ptt = pttf;

  if(ptt) rightLCD.update(AGC_Cmnd + MAN);
  else rightLCD.update(AGC_Cmnd + SLOW);

}


void catGoToggleVFOs()                       // VFO A - B Swap
{

  // Function code here

}


void catSetFreq(long freq)                   // Set Frequency
{
  vfo.catVFO_A_FreqUpdate(freq);
}


void catSetMode(byte m)                      // Set Mode
{
    
  // Function code here

}


long catGetFreq()                            // Get Frequency
{
  return vfo.getVFOA_Dial_Freq();
}


byte catGetMode()                            // Get Mode
{
  byte mode = 0x00;

  // Function code here

  return mode;
}


byte catGetSMeter()                          // Get S Meter
{
  byte sMeter;                               // Byte 4 LSB represent S Meter reading

  //Function code here

  return sMeter;
}


byte catGetTXStatus()                        // Get TX Status
{
  byte txStatus = 0x00;
    /*
     * Return a byte:
     * 0b abcdefgh
     *  a = 0 = PTT off
     *  a = 1 = PTT on
     *  b = 0 = HI SWR off
     *  b = 1 = HI SWR on
     *  c = 0 = split on
     *  c = 1 = split off
     *  d = dummy data
     *  efgh = PO meter data
     */

  // Function code here

  return txStatus;
}
/******************** END OF PROGRAM ****************************************************************************************/
