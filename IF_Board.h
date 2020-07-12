#ifndef IF_Board_h
#define IF_Board_h

#include <arduino.h>
#include <Wire.h>

class IF_Board
{
  public:
  	IF_Board(void);
  	void begin(void);
  	void splash(void);
  	void sMeterInit(void);
  	void getSMeterReading(void);
  	void setAGCMode(uint8_t);
  	void setManIFGain(void);
  
  private:
  	void updateSMeterDsply(uint8_t);
};


#endif