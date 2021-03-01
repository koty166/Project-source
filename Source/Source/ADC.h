#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/delay.h>
void ADCInit(void);
unsigned int ADCConvert(char Port);

#endif /* ADC_H_ */