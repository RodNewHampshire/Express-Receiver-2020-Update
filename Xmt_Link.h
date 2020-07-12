#ifndef Xmt_Link_h
#define Xmt_Link_h


class Xmt_Link
{
  public:
  	Xmt_Link(void);
    void begin(void);
    uint8_t available(void);
    void updateXmtBand(void);
    void vfoAB(uint8_t flag);
  
  private:
    void send_Xmtr(uint8_t data[], uint8_t num);
    void send_Amp(uint8_t data[], uint8_t num);
    void executeXmtr(void);
    void executeAmp(void);
};


#endif