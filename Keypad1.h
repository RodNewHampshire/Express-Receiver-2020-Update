#ifndef Keypad1_h
#define Keypad1_h

#include "Arduino.h"

class Keypad1
{
  public:
    Keypad1(void);
    void begin(char Col_1, char Col_2, char Col_3, char Col_4, char Col_5, char Row_1, char Row_2, char Row_3);
    uint8_t scan(void);
  private:
  	char _Col_1;
  	char _Col_2;
  	char _Col_3;
  	char _Col_4;
  	char _Col_5;
  	char _Row_1;
  	char _Row_2;
  	char _Row_3;
};


#endif