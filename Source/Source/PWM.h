


#ifndef PWM_H_
#define PWM_H_

#include <avr/interrupt.h>
void InitPWM();
void StartPWM(unsigned int Frequency);
void StopPWM();
unsigned int TryFrequency(unsigned int Frequency);
unsigned int Round(float Num);

#endif /* PWM_H_ */