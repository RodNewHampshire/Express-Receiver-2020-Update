#ifndef Function_h
#define Function_h

#include <arduino.h>
#include <Wire.h>

class Function
{
  public:
  	Function(void);
  	void begin(void);
  	void select(void);
  	void setMainAFGain(uint8_t);
  	void setAuxAFGain(uint8_t);
    uint8_t getManualIFGain(void);
    void manIFGainSetDisplay(uint8_t);
    void refreshManIFGainSetDisplay(void);
    void manIFGainSet(void);
    void eepromUpdate(void);
    void eepromRead(uint16_t *currentVersion);
  	
  private:
  	void display(uint8_t);
  	void execute(uint8_t);
  	void sMeterCal(void);
  	void si570CalData(void);
  	void auxAudioLevelSet(void);
  	void mainAudioLevelSet(void);
  	void si570CalDataDisplay(void);
  	void auxAudioLevelDisplay(uint8_t);
  	void mainAudioLevelDisplay(uint8_t);
  	void sMeterCalDisplay(uint16_t);
    void disableZeroCrossingDetection(void);
};


#endif