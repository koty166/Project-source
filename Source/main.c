#include "main.h"

unsigned int Tiks=0;
unsigned char Temp=20;//1 тик =  1 сек
unsigned int Frequency = 200;
float MassD,VD,ElapsedTime;// Массовый и объёмный расходы


void PortInit()
{
		DDRD=0xFF;
		PORTD=0;
		
		DDRB = 0x38;
		
		DDRC = 0x7;
		PORTC = 0x0;
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
	
void IsCalibrate()
{	
	char IsLeft = true,IsVisible = true;
	char* First = "Calibrate sens?";
	LCDClear();
	ADCInit();
	SendStr(First);
	while(1)
	{
		if(IsButtonPress(PINB & 0x1))
		{
			if (IsLeft == 0)
			{
				LCDClear();
				SendStr("Calibrating...");
				CalibrateSens();
			}
			_delay_ms(500);
			return;
		}
		else if(IsButtonPress(PINB & 0x2))
		{
			IsLeft = false;
		}
		else if (IsButtonPress(PINB & 0x4))
		{
			IsLeft = true;
		}
		setpos(0,1);
		if(IsLeft)
		{
			if (IsVisible)
			{
				SendStr("No");
				setpos(5,1);
				SendStr("Yes");
				IsVisible = false;
			}
			else
			{
				SendStr("  ");
				setpos(5,1);
				SendStr("Yes");
				IsVisible = true;
			}
		}
		else
		{
			SendStr("No");
			if (IsVisible)
			{
				setpos(5,1);
				SendStr("Yes");
				IsVisible = false;
			}
			else
			{
				setpos(5,1);
				SendStr("   ");
				IsVisible = true;
			}
		}
		_delay_ms(500);
	}
}

void UpWaterLevel()
{
	_delay_ms(500);
	char  IsFirst = 1;
	LCDClear();
	SendStr("Press OK then");
	setpos(0,1);
	SendStr("wat lv above h2");
	InitPWM();
	StartPWM(Frequency);
	PORTC |= 0x1;
	while(!IsButtonPress(PINB & 0x1))
	{	
		if (IsFirst == 1)
		{
			PORTC &= 0x248;
			PORTC |= 0x4;
			IsFirst = 0;
		}
		else if ( IsFirst == 0)
		{
			PORTC &= 0x248;
			PORTC |= 0x1;
			IsFirst = 1;
		}
		_delay_ms(T1S);
	}
	PORTC &= 0x248;
	StopPWM();
}

void CalibrateSens()
{   
	UpWaterLevel();
	unsigned int Summ=0,buf,Dark=0,WaterLight=0,Light=0;
	for(char i = 0;i<100;i++)
	{
		buf = ADCConvert(0x5);
		Summ+=buf;
		_delay_ms(T0001S*3);
	}
	Dark = Summ/100;
	PORTB |= 0x20;
	Summ = 0; 
	for(char i = 0;i<100;i++)
	{
		buf = ADCConvert(0x5);
		Summ+=buf;
		_delay_ms(T0001S*3);
	}
	Light = Summ/100;
	
	PORTB &=0xDF;
	PORTB |= 0x10;
	
	Summ = 0;	
	for(char i = 0;i<100;i++)
	{
		buf = ADCConvert(0x4);
		Summ += buf;
		_delay_ms(T0001S*3);
	}
	WaterLight = Summ/100;
	PORTB &=0xEF;
		
	WriteEEPROMInt(0,Dark);
	WriteEEPROMInt(2,Light);
	WriteEEPROMInt(4,WaterLight);	
	
	unsigned int adc_value = Dark;
	
	LCDClear();
	SendStr("Testing result:");
	setpos(0,1);
	sendchar(adc_value/1000+0x30);
	sendchar((adc_value%1000)/100+0x30);
	sendchar((adc_value%100)/10+0x30);
	sendchar(adc_value%10+0x30);
	sendchar(' ');
	adc_value = Light;
	sendchar(adc_value/1000+0x30);
	sendchar((adc_value%1000)/100+0x30);
	sendchar((adc_value%100)/10+0x30);
	sendchar(adc_value%10+0x30);
	sendchar(' ');
	adc_value = WaterLight;
	sendchar(adc_value/1000+0x30);
	sendchar((adc_value%1000)/100+0x30);
	sendchar((adc_value%100)/10+0x30);
	sendchar(adc_value%10+0x30);
	_delay_ms(5*1000);
}

char IsSame(int One,int Two,int Part)
{
	if(One >= Two - Part && One <= Two + Part)
	{
		return true;
	}
	else 
	{
		return false;
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
	const float V1 = 3.1415926535 * 0.1 * 0.05*0.05,V2 =3.1415926535 * 0.2 * 0.05*0.05,//h*pi*r*r; V2> V1
	h1 = 0.1, h2=0.2,R = 8.3144626181, Mr = 28.98,Temperature = Temp+273;
	
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
			if (Frequency > 32)
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
	unsigned int Dark=0,WaterLight=0,Light=0;
	
	Dark = ReadEEPROMInt(0);
	Light = ReadEEPROMInt(2);
	WaterLight = ReadEEPROMInt(4);
	
	TimerInit();
	char* Test1 = "Testing...", Test12 = "Stage 1 of 2",Test2 = "Testing...", Test22 = "Stage 2 of 2";
	char Stage = 1, IsFirst = 1;
	LCDClear();
	SendStr(Test1);
	setpos(0,1);
	SendStr(Test12);
	PORTC |= 0x5;
	
	InitPWM();
	StartPWM(Frequency);
	while(1)
	{
		if(Stage == 1 && IsSame(ADCConvert(0x5),WaterLight,50))
		{
			PORTC &= 0xF8;
			PORTC |= 0x2;
			StartTimer();///Разница команд от5* 10^-5 до 10^-3
			Stage = 2;
			LCDClear();
			SendStr(Test2);
			setpos(0,1);
			SendStr(Test22);
			
		}	
		else if(Stage == 2 && IsSame(ADCConvert(0x4),Light,50))
		{
			StopTimer();
			ElapsedTime = GetTime();
			PORTC &= 0xF8;
			StopPWM();
			CountResault();
			return;
		}
		/*if (Stage == 1)
		{
			if (IsFirst == 1)
			{
				PORTC &= 0x248;
				PORTC |= 0x4;
				IsFirst = 0;
			}
			else if ( IsFirst == 0)
			{
				PORTC &= 0x248;
				PORTC |= 0x1;
				IsFirst = 1;
			}
			_delay_ms(T1S);
		}*/
		_delay_ms(1000);
		
		if(Stage == 1)
		{
		LCDClear();
		unsigned int buf = ADCConvert(0x5);
		sendchar(buf/1000+0x30);
		sendchar((buf%1000)/100+0x30);
		sendchar((buf%100)/10+0x30);
		sendchar(buf%10+0x30);
		}
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
	char*  Result1 = "M:",Result2 = "V:";   
	sei();
	PortInit();                                                    
	LCDInit();
	/*InitPWM();
	StartPWM(35);
	TimerInit();
	_delay_ms(5000);
	SendStr("Timer is start");
	StartTimer();
	while(!IsButtonPress(PINB & 0x1)){
	}
	LCDClear();
	StopTimer();
	ElapsedTime = GetTime();
	WriteFloat(ElapsedTime,4);
	*/
	 while (1) 
    {
		IsCalibrate();
		ChooseFrequency();
		ChooseTemp();
		Testing();
		
		LCDClear();
		SendStr(Result1);
		WriteFloat(MassD,6);
		SendStr("[kg/s]");
		SendStr("Set Hz:");
		sendchar(Frequency/1000+0x30);
		sendchar((Frequency%1000)/100+0x30);
		sendchar((Frequency%100)/10+0x30);
		sendchar(Frequency%10+0x30);
		
		setpos(0,1);
		SendStr(Result2);
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
		
    }
}

