#ifndef Si570_h
#define Si570_h




class Si570
{
  public:
  	Si570(void);
  	void begin(void);
  	void VFO_Update(uint32_t *Freq, uint8_t *Band);
  	void LO_Set(void);
  	void get10MHzRFREQ(void);
    uint8_t readRegister(uint8_t byteAddress);
    void writeRegister(uint8_t Si570_reg, uint8_t data);
  private:
  	void Band_Change(uint8_t New_Band);
};


#endif