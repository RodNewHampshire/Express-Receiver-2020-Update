/******************************************************************************
*
* EXPRESS RECEIVER by AD5GH
* (http://www.expressreceiver.com)
* ARDUINO MEGA DISPLAY & CONTROL BOARD SOFTWARE
* KEYPAD ROUTINES
*
* Copyright 2017 Rod Gatehouse AD5GH
* Distributed under the terms of the MIT License:
* http://www.opensource.org/licenses/mit-license
*
* VERSION 1.0.9
* December 27, 2017
*
******************************************************************************/
#include <arduino.h>
#include <Keypad1.h>


Keypad1::Keypad1(void)
{

}

void Keypad1::begin(char Col_1, char Col_2, char Col_3, char Col_4, char Col_5, char Row_1, char Row_2, char Row_3)
{
  pinMode(Col_1, OUTPUT);
  pinMode(Col_2, OUTPUT);
  pinMode(Col_3, OUTPUT);
  pinMode(Col_4, OUTPUT);
  pinMode(Col_5, OUTPUT);

  digitalWrite(Col_1, LOW);
  digitalWrite(Col_2, LOW);
  digitalWrite(Col_3, LOW);
  digitalWrite(Col_4, LOW);
  digitalWrite(Col_5, LOW);

  pinMode(Row_1, INPUT_PULLUP);
  pinMode(Row_2, INPUT_PULLUP);
  pinMode(Row_3, INPUT_PULLUP);

  _Col_1 = Col_1;
  _Col_2 = Col_2;
  _Col_3 = Col_3;
  _Col_4 = Col_4;
  _Col_5 = Col_5;
  _Row_1 = Row_1;
  _Row_2 = Row_2;
  _Row_3 = Row_3;
}

uint8_t Keypad1::scan(void)
{

  /*Keyboard Key Mapping

    FUNC       ENTER   UP      DOWN    TX LOCK
    11         12      13      14      15

    A/B        A->B    MEM     A->MEM  MEM->A
    1          2       3       4       5

    IF FLTR    AGC     MODE    RATE    MUTE
    6          7       8       9       10
  */

  uint8_t b = 0;

  if (!digitalRead(_Row_1))
  {
    digitalWrite(_Col_1, HIGH);
    if (digitalRead(_Row_1))
    {
      digitalWrite(_Col_1, LOW);
      return b = 10;
    }
    digitalWrite(_Col_1, LOW);

    digitalWrite(_Col_2, HIGH);
    if (digitalRead(_Row_1))
    {
      digitalWrite(_Col_2, LOW);
      return b = 9;
    }
    digitalWrite(_Col_2, LOW);

    digitalWrite(_Col_3, HIGH);
    if (digitalRead(_Row_1))
    {
      digitalWrite(_Col_3, LOW);
      return b = 8;
    }
    digitalWrite(_Col_3, LOW);

    digitalWrite(_Col_4, HIGH);
    if (digitalRead(_Row_1))
    {
      digitalWrite(_Col_4, LOW);
      return b = 7  ;
    }
    digitalWrite(_Col_4, LOW);

    digitalWrite(_Col_5, HIGH);
    if (digitalRead(_Row_1))
    {
      digitalWrite(_Col_5, LOW);
      return b = 6  ;
    }
    digitalWrite(_Col_5, LOW);
  }

  else if (!digitalRead(_Row_2))
  {
    digitalWrite(_Col_1, HIGH);
    if (digitalRead(_Row_2))
    {
      digitalWrite(_Col_1, LOW);
      return b = 15;
    }
    digitalWrite(_Col_1, LOW);

    digitalWrite(_Col_2, HIGH);
    if (digitalRead(_Row_2))
    {
      digitalWrite(_Col_2, LOW);
      return b = 14;
    }
    digitalWrite(_Col_2, LOW);

    digitalWrite(_Col_3, HIGH);
    if (digitalRead(_Row_2))
    {
      digitalWrite(_Col_3, LOW);
      return b = 13;
    }
    digitalWrite(_Col_3, LOW);

    digitalWrite(_Col_4, HIGH);
    if (digitalRead(_Row_2))
    {
      digitalWrite(_Col_4, LOW);
      return b = 12;
    }
    digitalWrite(_Col_4, LOW);

    digitalWrite(_Col_5, HIGH);
    if (digitalRead(_Row_2))
    {
      digitalWrite(_Col_5, LOW);
      return b = 11;
    }
    digitalWrite(_Col_5, LOW);
  }

  else if (!digitalRead(_Row_3))
  {
    digitalWrite(_Col_1, HIGH);
    if (digitalRead(_Row_3))
    {
      digitalWrite(_Col_1, LOW);
      return b = 5;
    }
    digitalWrite(_Col_1, LOW);

    digitalWrite(_Col_2, HIGH);
    if (digitalRead(_Row_3))
    {
      digitalWrite(_Col_2, LOW);
      return b = 4;
    }
    digitalWrite(_Col_2, LOW);

    digitalWrite(_Col_3, HIGH);
    if (digitalRead(_Row_3))
    {
      digitalWrite(_Col_3, LOW);
      return b = 3;
    }
    digitalWrite(_Col_3, LOW);

    digitalWrite(_Col_4, HIGH);
    if (digitalRead(_Row_3))
    {
      digitalWrite(_Col_4, LOW);
      return b = 2  ;
    }
    digitalWrite(_Col_4, LOW);

    digitalWrite(_Col_5, HIGH);
    if (digitalRead(_Row_3))
    {
      digitalWrite(_Col_5, LOW);
      return b = 1  ;
    }
    digitalWrite(_Col_5, LOW);
  }
  return b;
}

