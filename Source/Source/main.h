
#ifndef MAIN_H_
#define MAIN_H_
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

#define  true 0x1
#define  false 0x0
#define T1S 1000
#define T0001S 1
#define T001S 10
#define T01S 100


#include "ADC.h"
#include "LCD.h"
#include "EEPROMUtilites.h"
#include "PWM.h"


#endif /* MAIN_H_ */