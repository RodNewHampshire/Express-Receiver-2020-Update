#ifndef Main_LCD_h
#define Main_LCD_h


class Main_LCD
{
  public:
  	Main_LCD(void);
  	void begin(void);
  	void clear(void);
  	void splash(void);
  	void updateVFOADisplay(uint32_t *, uint8_t *);
  	void updateVFOBDisplay(uint32_t *, uint8_t *);
  	void showMemoryDisplay(uint32_t *, uint8_t *, uint8_t *);
  	void hideMemoryDisplay(void);
  	void indicateVFOA(void);
  	void indicateVFOB(void);
  	void deselectVFOA(void);
  	void deselectVFOB(void);
  	void eepromStart(void);
  	void eepromNoDataFound(void);
  	void eepromInitializing(void);
  	void eepromReadingData(void);
  	
  private:
};

#endif