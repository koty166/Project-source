#include "ADC.h"

void ADCInit()
{
	ADCSRA |= (1<<ADEN) |(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADMUX |= (1<<REFS1)|(1<<REFS0);
}
unsigned int ADCConvert(char Port)
{
	ADMUX &= 0XF8;
	ADMUX |= Port;
	ADCSRA |=(1<<ADSC);
	while(ADCSRA & (1<< ADSC))
	return ADC;
}
