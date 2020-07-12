#ifndef Right_LCD_h
#define Right_LCD_h

#include <arduino.h>

#define IFBW_Cmnd       0x10                		// right LCD related commands
#define ATTN_Cmnd       0x30
#define AMP_Cmnd        0x40
#define Band_Cmnd       0x50
#define Mode_Cmnd       0x60
#define AGC_Cmnd        0x70
#define MUTE_Cmnd       0x80
#define TXLK_Cmnd       0x90
#define RATE_Cmnd       0xA0


class Right_LCD
{
  public:
  	Right_LCD(void);
  	void begin(void);
  	void init(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  	void clear(void);
  	void splash(void);
  	void update(uint8_t);
  	
  private:
};

#endif