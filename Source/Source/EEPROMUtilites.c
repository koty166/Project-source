#include "EEPROMUtilites.h"

void WriteEEPROM(unsigned int Addr, unsigned char Data)
{
	while(EECR & (1<<EEWE)){}
	EEAR = Addr;
	EEDR = Data;
	EECR |=(1<<EEMWE);
	EECR |=(1<<EEWE);
}
void WriteEEPROMInt(unsigned int Addr, unsigned int Data)
{
	WriteEEPROM(Addr,(unsigned char)Data);
	WriteEEPROM(Addr+1,(unsigned char)(Data>>8));
}

unsigned char ReadEEPROM(unsigned int Addr)
{
	while(EECR & (1<<EEWE)){}
	EEAR = Addr;
	EECR |=(1<<EERE);
	return EEDR;
}
unsigned int ReadEEPROMInt(unsigned int Addr)
{
	unsigned int Data;
	Data = ReadEEPROM(Addr+1);
	Data <<=8;
	Data += ReadEEPROM(Addr);
	return Data;
}
