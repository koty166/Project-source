#include "PWM.h"

void InitPWM()
{
	ASSR = 0x00;
	TCCR2 = 0x18;
	TCNT0 = 0x00;
	
}
void StartPWM(unsigned int Frequency)
{
	Frequency*=2;
	unsigned int Del;
	char Bits;
	if(Frequency < 125)
	{
		Del = 1024;
		Bits = 0x7;
	}
	else if(Frequency < 245)
	{
		Del = 256;
		Bits = 0x6;
	}
	else if(Frequency < 488)
	{
		Del = 128;
		Bits = 0x5;
	}
	else if(Frequency < 978)
	{
		Del = 64;
		Bits = 0x4;
	}
	else if(Frequency < 2000)
	{
		Del = 32;
		Bits = 0x3;
	}
	
	OCR2 = (unsigned char)(8000000/Del/Frequency);
	TCCR2 |= Bits;
}
void StopPWM()
{
	TCCR2 &= 0xF8;
	OCR2 = 0;
}
