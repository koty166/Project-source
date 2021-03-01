#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>
#include <avr/delay.h>
#define  true 1
#define  false 0

#define T1S 1000
#define T0001S 1
#define T001S 10
#define T01S 100

void sendhalfbyte(unsigned char Data);
void sendbyte(unsigned char data, unsigned char IsData);
void sendchar(unsigned char);
void LCDInit(void);
void SendStr(char Str[]);
void setpos(unsigned char x, unsigned y);
void LCDClear(void);

#endif /* LCD_H_ */