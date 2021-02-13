 #include "LCD.h"
 void sendhalfbyte(unsigned char c)
{
	 PORTD|=0x80;
	 _delay_us(320);
	 PORTD  &= 0xF0; 
	 PORTD |= c;
	 PORTD&=0x7F;
	 _delay_us(320); 
}
 void sendbyte(unsigned char c, unsigned char IsData)
{	
	if (IsData != 0) PORTD |=0x40;
	else PORTD &=0xBF;
	
	unsigned char hc=(c>>4)&0xF,lc = c&0xF;
	sendhalfbyte(hc);
	sendhalfbyte(lc);
}
 void sendchar(unsigned char c)
 {
	 sendbyte(c,true);
 }
 void LCDInit()
 {	
	 _delay_ms(15); sendhalfbyte(0x2);
	 _delay_ms(4); sendbyte(0x28, false);
	 _delay_ms(1); sendbyte(0x6, false);
	 _delay_ms(1); sendbyte(0xC, false);
	 _delay_ms(1);}
 void SendStr(char Str[])
 {	
	 for(char i=0;Str[i]!='\0';i++)
	{
		sendchar(Str[i]);
	}
}
 void setpos(unsigned char x, unsigned y)
 {
char adress;

  adress=(0x40*y+x)|0b10000000;
  sendbyte(adress, 0); 
  _delay_us(50);
 }
 void LCDClear()
 {
	 sendbyte(0x1,false);
	 _delay_ms(2);
 }
