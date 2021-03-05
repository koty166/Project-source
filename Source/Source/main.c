#define OilHightMax 70
#define OilHightMin 50
#define LightLowMax 850
#define LightLowMax 650


#include "main.h"

const float R = 0.05, h1 = 0.1, h2 = 0.2;
unsigned int Tiks=0;//1 тик =  1 сек
unsigned char Temp=20;
unsigned int Frequency = 200;
float MassD,VD,ElapsedTime;// Массовый и объёмный расходы


void PortInit()
{
		DDRD=0xFF;
		
		DDRB = 0x38;
		PORTB = 0x30;
		
		DDRC = 0x7;
		PORTC = 0x7;
}

char IsButtonPress(char PinCondition)
{
	if(PinCondition)
	{
		_delay_ms(1);
		if(PinCondition)
		{
			return true;
		}
	}
}
	
void TimerInit()
{
	TCCR1B |=(1<<WGM12);
	TIMSK |= (1<<OCIE1A);
	OCR1AH = 0b01111010;
	OCR1AL = 0b00010010;
}

ISR(TIMER1_COMPA_vect)
{
	Tiks++;
	TCNT1H=0x00;
	TCNT1L=0x00;
}

float GetTime()
{
	unsigned int LTime = TCNT1H;
	LTime <<=8;
	LTime |= TCNT1L;
	return Tiks + LTime/31250.;
}

void StartTimer()
{
		TCCR1B |= (1<<CS12);
}

void StopTimer()
{
		TCCR1B &= 0xF8;
}

void CountResault()
{
	const long PA = 101325;
	const float V1 = 3.1415926535 * h1 * R*R,V2 =3.1415926535 * h2 * R*R,//h*pi*r*r; V2> V1
	R = 8.3144626181, Mr = 28.98,Temperature = Temp+273;
	
	VD = (V2 - V1) / ElapsedTime;
	float m1 = ((PA - 1000 * 10 * h1)*V1*Mr)/(R*Temperature),m2 = ((PA - 1000 * 10 * h2)*V2*Mr)/(R*Temperature);
	MassD = (m2-m1) / ElapsedTime;
}

void ChooseFrequency()
{
	LCDClear();
	SendStr("Choose Hz");
	_delay_ms(200);
	setpos(0,1);
	sendchar(Frequency/1000+0x30);
	sendchar((Frequency%1000)/100+0x30);
	sendchar((Frequency%100)/10+0x30);
	sendchar(Frequency%10+0x30);
	while(1)
	{
		if(IsButtonPress(PINB & 0x1))
		{
			_delay_ms(500);
			return;
		}
		else if(IsButtonPress(PINB & 0x2))
		{
			if (Frequency < 700)
			{
				Frequency++;
				setpos(0,1);
				sendchar(Frequency/1000+0x30);
				sendchar((Frequency%1000)/100+0x30);
				sendchar((Frequency%100)/10+0x30);
				sendchar(Frequency%10+0x30);

			}
		}
		else if (IsButtonPress(PINB & 0x4))
		{
			if (Frequency > 20)
			{
				Frequency--;
				setpos(0,1);
				sendchar(Frequency/1000+0x30);
				sendchar((Frequency%1000)/100+0x30);
				sendchar((Frequency%100)/10+0x30);
				sendchar(Frequency%10+0x30);				
			}
		}
		
		_delay_ms(100);
	}
	
}

void ChooseTemp()
{
	LCDClear();
	SendStr("Choose T");
	_delay_ms(200);
	setpos(0,1);
	sendchar(Temp/1000+0x30);
	sendchar((Temp/10)%10+0x30);
	sendchar(Temp%10+0x30);
	while(1)
	{
		if(IsButtonPress(PINB & 0x1))
		{
			_delay_ms(500);
			return;
		}
		else if(IsButtonPress(PINB & 0x2))
		{
			if (Temp < 100)
			{
				Temp++;
				setpos(0,1);
				sendchar(Temp/1000+0x30);
				sendchar((Temp/10)%10+0x30);
				sendchar(Temp%10+0x30);
				
			}
		}
		else if (IsButtonPress(PINB & 0x4))
		{
			if (Temp > 1)
			{
				Temp--;
				setpos(0,1);
				sendchar(Temp/1000+0x30);
				sendchar((Temp/10)%10+0x30);
				sendchar(Temp%10+0x30);
			}
		}
		
		_delay_ms(100);
	}
	
}

void Testing()
{
	ADCInit();
	
	TimerInit();
	char* Test1 = "Testing...", Test12 = "Stage 1 of 2",Test2 = "Testing...", Test22 = "Stage 2 of 2";
	char Stage = 1, IsFirst = 1;
	LCDClear();
	SendStr(Test1);
	setpos(0,1);
	SendStr(Test12);
	PORTC = 0xFA;
	
	InitPWM();
	StartPWM(Frequency);
	while(1)
	{
		if(Stage == 1 && ADCConvert(0x4) >=50 && ADCConvert(0x5) <=70)
		{
			PORTC = 0xFD;
			StartTimer();
			Stage = 2;
			LCDClear();
			SendStr(Test2);
			setpos(0,1);
			SendStr(Test22);
			
		}	
		else if(Stage == 2 &&  ADCConvert(0x5) >=650 && ADCConvert(0x4) <=850)
		{
			StopTimer();
			ElapsedTime = GetTime();
			PORTC |= 0x7;
			StopPWM();
			CountResault();
			return;
		}
		
		if(Stage == 1)
		{
		LCDClear();
		unsigned int buf = ADCConvert(0x4);
		sendchar(buf/1000+0x30);
		sendchar((buf%1000)/100+0x30);
		sendchar((buf%100)/10+0x30);
		sendchar(buf%10+0x30);
						setpos(0,1);
						SendStr(Test12);
		}
		if(Stage == 2)
		{
			LCDClear();
			unsigned int buf = ADCConvert(0x5);
			sendchar(buf/1000+0x30);
			sendchar((buf%1000)/100+0x30);
			sendchar((buf%100)/10+0x30);
			sendchar(buf%10+0x30);
							setpos(0,1);
							SendStr(Test22);
		}
		_delay_ms(500);
	}
	
}

void WriteFloat(float D,char NumAfterDot)
{
	const char Len = 8;
	for (char i =0 ;i<NumAfterDot;i++)
	{
		D*=10;
	}
	unsigned long Data = D;
	char DataS[Len];
	
	for (char i=Len-1;i>0;i--)
	{	
		DataS[i] = Data%10;
		Data/=10;
	}
	for (char i=1;i<Len;i++)
	{
		if (i == Len-NumAfterDot)
		{
			sendchar('.');
		}
		sendchar(DataS[i]+0x30);
	}
	
}

int main(void)
{	 
	sei();
	PortInit();                                                    
	LCDInit();

	while (1) 
    {
		ChooseFrequency();
		ChooseTemp();
		Testing();
		
		LCDClear();
		SendStr("M:");
		WriteFloat(MassD,6);
		SendStr("[kg/s]");
		SendStr("Set Hz:");
		sendchar(Frequency/1000+0x30);
		sendchar((Frequency%1000)/100+0x30);
		sendchar((Frequency%100)/10+0x30);
		sendchar(Frequency%10+0x30);
		
		setpos(0,1);
		SendStr("V:");
		WriteFloat(VD,6);
		SendStr("[m3/s]");
		SendStr("El t:");
		
		WriteFloat(ElapsedTime,4);
		SendStr("[c]");
		while(!IsButtonPress(PINB & 0x1))
		{
			_delay_ms(2000);
			for (char i=0;i<16;i++)
			{
				sendbyte(0x18,false);
				_delay_us(50);
			}
			_delay_ms(2000);
			sendbyte(0x2,false);
		}
		_delay_ms(500);
		
		ElapsedTime = 0;
		Tiks = 0;
		
    }
}

