/*
 * EEPROMUtilites.h
 *
 * Created: 06.01.2021 20:28:48
 *  Author: Я
 */ 


#ifndef EEPROMUTILITES_H_
#define EEPROMUTILITES_H_

#include "avr/io.h"
void  WriteEEPROM(unsigned int, unsigned char);
unsigned char ReadEEPROM(unsigned int);
void WriteEEPROMInt(unsigned int Addr, unsigned int Data);
unsigned int ReadEEPROMInt(unsigned int Addr);



#endif /* EEPROMUTILITES_H_ */