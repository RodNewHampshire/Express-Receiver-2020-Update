/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* FUNCTION ROUTINES
*
* Copyright 2017 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.9
* December 27, 2017
*
******************************************************************************/

#include "Adafruit_LiquidCrystal.h"
#include <EEPROMex.h>
#include <Function.h>
#include <IF_Board.h>
#include <Keypad1.h>
#include <Si570.h>

#define AF_GAIN         0xA9            // DS1807 Potentiometer addresses
#define AUX_AF_GAIN     0xAA

#define I2C_DS1807      0x50 >> 1       // I2C Addresses
#define I2C_IF_Board    0x10 >> 1

#define DISABLE_ZEROCROSSING_DETECTION 0xBE

/*****************************************************************************/


Adafruit_LiquidCrystal functionLCD(2);
Keypad1     keyPress;
Si570       si_570;
IF_Board    _ifBoard;


/*   Line 1   */
const char Menu_Function[21]            = {'F','U','N','C','T','I','O','N',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
/*   Line 2   */
const char Menu_Si570_Cal[21]           = {'S','i','5','7','0',' ','C','a','l','i','b','.',' ','d','a','t','a',' ',' ',' ','\0'};
const char Menu_S_Meter_Cal[21]         = {'S','-','M','e','t','e','r',' ','C','a','l','i','b','r','a','t','i','o','n',' ','\0'};
const char Menu_Aux_Audio_Level[21]     = {'A','u','x',' ','A','u','d','i','o',' ','L','e','v','e','l',' ','S','e','t',' ','\0'};
const char Menu_Main_Audio_Level[21]    = {'M','a','i','n',' ','A','u','d','i','o',' ','L','e','v','e','l',' ','S','e','t','\0'};
const char Menu_Man_IF_Gain[21]         = {'M','a','n','u','a','l',' ','I','F',' ','G','a','i','n',' ','S','e','t',' ',' ','\0'};
const char Menu_Band_Cal_Offset[21]     = {'S','e','t',' ','B','a','n','d',' ','C','a','l',' ','O','f','f','s','e','t','s','\0'};


typedef struct                                  // structure containing variables to be stored in EEPROM
{
  uint16_t      Version;
  uint8_t       EE_Manual_IF_Gain;
  uint8_t       EE_Aux_Audio_Attn;
  uint8_t       EE_Main_Audio_Attn;

} eepromFunctionData;

eepromFunctionData functionData;

uint16_t _eepromAddress = 2048;                 // EEPROM Function data start address

uint8_t Aux_Audio_Attn;              
uint8_t Main_Audio_Attn;
uint8_t Manual_IF_Gain;


Function::Function(void)
{

}


void Function::begin(void)
{
    functionLCD.begin(20, 4);
    keyPress.begin(4, 5, 6, 7, 8, 9, 10, 11);   // pins for 5 columns followed by 3 rows
    setMainAFGain(Main_Audio_Attn);
    setAuxAFGain(Aux_Audio_Attn);
    disableZeroCrossingDetection();
}


void Function::select(void)
{
    
    uint8_t function;
    uint8_t done = 0;
    
    functionLCD.clear();
    functionLCD.setCursor(0, 0);
    functionLCD.print(Menu_Function);
    delay(500);                                 // because I like the effect of the delay
    function = 1;
    display(function);
    
    while(1)
    {
        delay(100);                             // keypad debounce
        switch(keyPress.scan())
        {
            case 11:                            // FUNC, exit function routines
                done = 1;
                break;

            case 12:                            // ENTER, execute function
                execute(function);
                functionLCD.clear();
                functionLCD.setCursor(0, 0);
                functionLCD.print(Menu_Function);
                display(function);
                break;

            case 13:                            // UP
                function++;
                if(function == 7) function = 1;
                display(function);
                break;

            case 14:                            // DOWN
                function--;
                if(function == 0) function = 6;
                display(function);
                break;
            
            default:                            // ignore other button presses
                break;
        }
        
        if(done) return;                        // needed to exit the while loop
    }
}


void Function::display(uint8_t func)
{
    functionLCD.setCursor(0, 1);
    switch(func)
    {
        case 0:
            break;
        
        case 1:                                 // S1570 Calibration Data Retrieval
            functionLCD.print(Menu_Si570_Cal);
            break;

        case 2:                                 //  Meter Calibration
            functionLCD.print(Menu_S_Meter_Cal);
            break;

        case 3:                                 // Aux audio level set
            functionLCD.print(Menu_Aux_Audio_Level);
            break;
            
        case 4:                                 // Aux audio level set
            functionLCD.print(Menu_Main_Audio_Level);
            break;
            
        case 5:                                 // Manual IF gain level set
            functionLCD.print(Menu_Man_IF_Gain);
            break;
        
        case 6:                                 // Enter Band frequency calibration offsets
            functionLCD.print(Menu_Band_Cal_Offset);
            break;
            
        default:                                // Ignore other values
            break;
    }
}


void Function::execute(uint8_t func)
{
    switch(func)
    {
        case 0:
            break;
        
        case 1:                                 // S1570 Calibration Data Retrieval
            si570CalData();
            break;

        case 2:                                 // S Meter Calibration
            sMeterCal();
            break;

        case 3:                                 // Aux audio level adjustment 
            auxAudioLevelSet();
            break;
            
        case 4:                                 // Main audio level adjustment 
            mainAudioLevelSet();
            break;
            
        case 5:                                     
            manIFGainSet();                     // Manual IF gain adjustment
            break;
        
        case 6:
                                                // Function_Band_Cal_Offset();
            break;
            
        default:                                // Ignore other button presses
            break;
    }
}


void Function::sMeterCal(void)
{
   /* int Sig_Strength;
    char done = 0;
    
    while(1)
    {
        Utilities_Long_Delay(DEBOUNCE);                 //Needed to debounce keypad within the while loop
        switch(Keypad_Scan())
        {
            case 11:                                    //FUNC, exit from routine
                done = 1;
                break;

            case 12:
                
                break;
                
            case 13:                                    //UP
                
                break;

            case 14:                                    //DOWN
                
                break;
            
            default:                                    //Ignore other button presses
                break;
        }
        Sig_Strength = Read_I2C_Byte(IF_Board);
        LCD_S_Meter_Calibration_Data_Display(Sig_Strength);
        
        if(done) return;                                //Needed to exit the while loop
    }*/
}


void Function::si570CalData(void)
{
    char done = 0;
    
    //si_570.get10MHzRFREQ();
    si570CalDataDisplay();                      // does not currently display the data.
    while(1)
    {
        delay(250);                             // needed to debounce keypad within the while loop
        switch(keyPress.scan())
        {
            case 11:                            // FUNC, exit function routines
                done = 1;
                return;
            
            default:                            // ignore other button presses
                break;
        }
        if(done) return;                        // needed to exit the while loop
    }
}


void Function::auxAudioLevelSet(void)
{ 
    uint8_t done = 0;
    
    auxAudioLevelDisplay(Aux_Audio_Attn);
    while(1)
    {
        delay(250);                             // needed to debounce keypad within the while loop
        switch(keyPress.scan())
        {
            case 11:                            // FUNC, exit function without changing value
                done = 1;
                return;                                    
            
            case 12:                            // ENTER, save new value and exit function
                done = 1;
                eepromUpdate();
                return;

            case 13:                            // UP
                if(Aux_Audio_Attn == 64);       // max value, do nothing
                else
                {
                    Aux_Audio_Attn++;
                    auxAudioLevelDisplay(Aux_Audio_Attn);
                    setAuxAFGain(Aux_Audio_Attn);
                }
                break;

            case 14:                            // DOWN
                if(Aux_Audio_Attn == 0);        // min value, do nothing
                else
                {
                    Aux_Audio_Attn--;
                    auxAudioLevelDisplay(Aux_Audio_Attn);
                    setAuxAFGain(Aux_Audio_Attn);
                }
                break;
            
            default:                            // ignore any other button presses
                break;
        }
        if(done) return;
    }
}


void Function::mainAudioLevelSet(void)
{ 
    uint8_t done = 0;
    
    mainAudioLevelDisplay(Main_Audio_Attn);
    while(1)
    {
        delay(250);                             // needed to debounce keypad within the while loop
        switch(keyPress.scan())
        {
            case 11:                            // FUNC, exit function without changing value
                done = 1;
                return;                                    
            
            case 12:                            // ENTER, save new value and exit function
                done = 1;
                eepromUpdate();
                return;

            case 13:                            // UP
                if(Main_Audio_Attn == 64);      // max value, do nothing
                else
                {
                    Main_Audio_Attn++;
                    mainAudioLevelDisplay(Main_Audio_Attn);
                    setMainAFGain(Main_Audio_Attn);
                }
                break;

            case 14:                            // DOWN
                if(Main_Audio_Attn == 0);       // min value, do nothing
                else
                {
                    Main_Audio_Attn--;
                    mainAudioLevelDisplay(Main_Audio_Attn);
                    setMainAFGain(Main_Audio_Attn);
                }
                break;
            
            default:                            // ignore any other button presses
                break;
        }
        if(done) return;
    }
}


void Function::manIFGainSet(void)
{
    char done = 0;
    manIFGainSetDisplay(Manual_IF_Gain);
    
    while(1)
    {   
        delay(250);                             // needed to debounce keypad within the while loop
        switch(keyPress.scan())
        {
            case 11:                            // FUNC, exit function without changing value
                done = 1;
                return;                                    
                
            case 12:                            // ENTER, save new value and exit function
                eepromUpdate();
                done = 1;
                return;

            case 13:                            // UP
                if(Manual_IF_Gain == 18);       // max value, do nothing
                else
                {
                    Manual_IF_Gain++;
                    manIFGainSetDisplay(Manual_IF_Gain);
                    _ifBoard.setManIFGain();
                }
                break;

            case 14:                            // DOWN
                if(Manual_IF_Gain == 0);        // min value, do nothing
                else
                {
                    Manual_IF_Gain--;
                    manIFGainSetDisplay(Manual_IF_Gain);
                    _ifBoard.setManIFGain();
                }
                break;
        
            default:
                break;
        }
        if(done) return;
    }
}


void Function::si570CalDataDisplay(void)
{ 
    char Disp_LO_Data[21] = {'L','O',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
    char Disp_VFO_Data[21] = {'V','F','O',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};

    /*Disp_LO_Data[6] = LO_CAL[11];
    Disp_LO_Data[7] = LO_CAL[10];
    Disp_LO_Data[9] = LO_CAL[9];
    Disp_LO_Data[10] = LO_CAL[8];
    Disp_LO_Data[12] = LO_CAL[7];
    Disp_LO_Data[13] = LO_CAL[6];
    Disp_LO_Data[14] = LO_CAL[5];
    Disp_LO_Data[15] = LO_CAL[4];
    Disp_LO_Data[16] = LO_CAL[3];
    Disp_LO_Data[17] = LO_CAL[2];
    Disp_LO_Data[18] = LO_CAL[1];
    Disp_LO_Data[19] = LO_CAL[0];*/
    functionLCD.setCursor(0, 2);
    functionLCD.print(Disp_LO_Data);
    
    /*Disp_VFO_Data[6] = VFO_CAL[11];
    Disp_VFO_Data[7] = VFO_CAL[10];
    Disp_VFO_Data[9] = VFO_CAL[9];
    Disp_VFO_Data[10] = VFO_CAL[8];
    Disp_VFO_Data[12] = VFO_CAL[7];
    Disp_VFO_Data[13] = VFO_CAL[6];
    Disp_VFO_Data[14] = VFO_CAL[5];
    Disp_VFO_Data[15] = VFO_CAL[4];
    Disp_VFO_Data[16] = VFO_CAL[3];
    Disp_VFO_Data[17] = VFO_CAL[2];
    Disp_VFO_Data[18] = VFO_CAL[1];
    Disp_VFO_Data[19] = VFO_CAL[0];*/
    functionLCD.setCursor(0, 3);
    functionLCD.print(Disp_VFO_Data);
}


void Function::auxAudioLevelDisplay(uint8_t level)
{ 
    uint8_t Level_Disp[2];
    char Disp_Aux_Audio_Lvl[21] = {'A','t','t','e','n','u','a','t','i','o','n',':',' ',' ',' ',' ','d','B',' ',' ','\0'};
    
    if (level < 10) 
    {
        Level_Disp[0] = 0x20;
        Level_Disp[1] = level + 0x30;
    }
    else
    {
        Level_Disp[0] = (level/10 + 0x30);
        Level_Disp[1] = ((level % 10) + 0x30);
    }
    
    Disp_Aux_Audio_Lvl[14] = Level_Disp[0];
    Disp_Aux_Audio_Lvl[15] = Level_Disp[1];
    functionLCD.setCursor(0, 3);
    functionLCD.print(Disp_Aux_Audio_Lvl);
}


void Function::mainAudioLevelDisplay(uint8_t level)
{
    uint8_t Level_Disp[2];
    char Disp_Main_Audio_Lvl[21] = {'A','t','t','e','n','u','a','t','i','o','n',':',' ',' ',' ',' ','d','B',' ',' ','\0'};
    
    if (level < 10) 
    {
        Level_Disp[0] = 0x20;
        Level_Disp[1] = level + 0x30;
    }
    else
    {
        Level_Disp[0] = (level/10 + 0x30);
        Level_Disp[1] = ((level % 10) + 0x30);
    }
    
    Disp_Main_Audio_Lvl[14] = Level_Disp[0];
    Disp_Main_Audio_Lvl[15] = Level_Disp[1];
    functionLCD.setCursor(0, 3);
    functionLCD.print(Disp_Main_Audio_Lvl);
}


void Function::manIFGainSetDisplay(uint8_t gainIndex)
{
    uint8_t Gain_Disp[2];
    uint8_t sign, gain;
    const char Man_IF_Gain_dB[19] = {45, 40, 35, 30, 25, 20, 15, 10, 5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45};
    char Disp_Man_IF_Gain[21] = {'M','a','n','.',' ','I','F',' ','G','a','i','n',':',' ',' ',' ',' ',' ','d','B','\0'};
    
    if(gainIndex < 9) sign = '-';                        //minus sign
    else sign = ' ';
    
    gain = Man_IF_Gain_dB[gainIndex];
    
    if (gain < 10) 
    {
        Gain_Disp[0] = 0x20;
        Gain_Disp[1] = gain + 0x30;
    }
    else
    {
        Gain_Disp[0] = (gain/10 + 0x30);
        Gain_Disp[1] = ((gain % 10) + 0x30);
    }
    
    Disp_Man_IF_Gain[15] = sign;
    Disp_Man_IF_Gain[16] = Gain_Disp[0];
    Disp_Man_IF_Gain[17] = Gain_Disp[1];
    functionLCD.setCursor(0, 3);
    functionLCD.print(Disp_Man_IF_Gain);
}

void Function::refreshManIFGainSetDisplay(void)
{
    _ifBoard.sMeterInit();
    manIFGainSetDisplay(Manual_IF_Gain);
}

void Function::sMeterCalDisplay(uint16_t Sig_Strength)
{ /*
    char S_Meter_Reading[4];
    char Disp_S_Meter_Cal[21] = {LCD_LINE_4,'S','-','M','e','t','e','r',' ','R','e','a','d','i','n','g',':',' ',' ',' ',' ','\0'};

    ultoa(S_Meter_Reading, Sig_Strength, 10);

    if(Sig_Strength < 0x0A)                         //"10" decimal
    {
        S_Meter_Reading[2] = S_Meter_Reading[0];
        S_Meter_Reading[1] = 0x20;
        S_Meter_Reading[0] = 0x20;
    }

    else if(Sig_Strength < 0x64)                    //"100" decimal
    {
        char i;
        for(i=2; i>0; i--) S_Meter_Reading[i] = S_Meter_Reading[i-1];
        S_Meter_Reading[0] = 0x20;
    }
    
    Disp_S_Meter_Cal[18] = S_Meter_Reading[0];
    Disp_S_Meter_Cal[19] = S_Meter_Reading[1];
    Disp_S_Meter_Cal[20] = S_Meter_Reading[2];
    LCD_Line_Update(&Disp_S_Meter_Cal); */
}


void Function::setMainAFGain(uint8_t level)
{
	Wire.beginTransmission(I2C_DS1807);
    Wire.write(AF_GAIN);
    Wire.write(level);
    Wire.endTransmission();
}


void Function::setAuxAFGain(uint8_t level)
{
	Wire.beginTransmission(I2C_DS1807);
    Wire.write(AUX_AF_GAIN);
    Wire.write(level);
    Wire.endTransmission();
}


void Function::disableZeroCrossingDetection(void)
{
    Wire.beginTransmission(I2C_DS1807);
    Wire.write(DISABLE_ZEROCROSSING_DETECTION);
    Wire.endTransmission();
}


void Function::eepromUpdate(void)                                         // WRITE VARIABLES TO EEPROM DATA ARRAY AND THEN UPDATE EEPROM
{
  functionData.EE_Aux_Audio_Attn = Aux_Audio_Attn;
  functionData.EE_Main_Audio_Attn = Main_Audio_Attn;
  functionData.EE_Manual_IF_Gain = Manual_IF_Gain;
  EEPROM.updateBlock(_eepromAddress, functionData);
}


uint8_t Function::getManualIFGain(void)
{
    return Manual_IF_Gain;
}


void Function::eepromRead(uint16_t *currentVersion)
{
    EEPROM.readBlock(_eepromAddress, functionData);             // READ EEPROM DATA INTO DATA ARRAY

    if (functionData.Version != *currentVersion)               // If new version of software or Arduino Mega not previously initialized, write default values to EEPROM
    {
        functionData.Version = *currentVersion;
        Aux_Audio_Attn = 0x00;              
        Main_Audio_Attn = 0x00;
        Manual_IF_Gain = 0x0E;
        eepromUpdate();                                             // write default values to EEPROM
    }
    Aux_Audio_Attn = functionData.EE_Aux_Audio_Attn;              
    Main_Audio_Attn = functionData.EE_Main_Audio_Attn;
    Manual_IF_Gain = functionData.EE_Manual_IF_Gain;
}