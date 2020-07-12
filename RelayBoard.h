#ifndef RelayBoard_h
#define RelayBoard_h

#include <arduino.h>
#include <Wire.h>


class RelayBoard
{
  public:
    RelayBoard(void);
    void begin(void); 
    void RF_BPF(uint8_t band);
    void RF_ATTN(uint8_t attn);
    void RF_AMP(uint8_t amp);
    void IF_FLTR(uint8_t ifbw);
    void TX_MODE(void);
    void RX_MODE(void);
  
  private:
};

#endif