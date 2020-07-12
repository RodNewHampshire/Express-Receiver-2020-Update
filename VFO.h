#ifndef VFO_h
#define VFO_h

#define UPPER         1
#define LOWER         0

class VFO
{
  public:
  	VFO(void);
  	void begin(void);						
    uint8_t getVFOA_Band(void);
    uint8_t getVFOB_Band(void);	
    int32_t getVFOA_Dial_Freq(void);
    int32_t getVFOB_Dial_Freq(void);
    void initVFOA(void);
    void bandChangeVFOA(void);
    void updateVFOA(int16_t *freqUpdate);
    void decrementBandVFOA(void);
    void incrementBandVFOA(void);
    uint8_t changeVFOAmode(void);
    void initVFOB(void);
    void bandChangeVFOB(void);
    void updateVFOB(int16_t *_freqUpdate);
    void decrementBandVFOB(void);
    void incrementBandVFOB(void);
    uint8_t changeVFOBmode(void);
    void VFOAtoVFOB(uint8_t *vfoFlag);
    void memoryToVFOA(uint8_t *memNumber, uint8_t *vfoFlag);
    void VFOAtoMemory(uint8_t *memNumber);
    void displayMemory(uint8_t *memNumber);
    void eepromUpdate(void);
    void eepromRead(uint16_t *currentVersion);
    uint8_t freqToBandNumber(int32_t freq);
    void catVFO_A_FreqUpdate(int32_t freq);
  
  private:
  	void Set_Band_Limits(long *Band_Limit_Array, char Band_Number);
};


#endif