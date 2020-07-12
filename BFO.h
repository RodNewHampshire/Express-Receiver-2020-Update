#ifndef BFO_h
#define BFO_h

#include <arduino.h>

class BFO
{
  public:
  	BFO(void);
  	void begin(void);
  	void mode(uint8_t);
  	
  private:
  	void usb(void);
  	void lsb(void);
  	void cw(void);
};

#endif