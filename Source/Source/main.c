#define OilHightLight 80
#define OilLowLight 60
#define HightLight 720
#define LowLight 280


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

void DisplayCalibRes(int one,int two,int three)
{
	sendchar(one/1000+0x30);
	sendchar((one%1000)/100+0x30);
	sendchar((one%100)/10+0x30);
	sendchar(one%10+0x30);
	sendchar(' ');
	sendchar(two/1000+0x30);
	sendchar((two%1000)/100+0x30);
	sendchar((two%100)/10+0x30);
	sendchar(two%10+0x30);
	sendchar(' ');
	sendchar(three/1000+0x30);
	sendchar((three%1000)/100+0x30);
	sendchar((three%100)/10+0x30);
	sendchar(three%10+0x30);
}

void CalibrateSens()
{   
	PORTB |=0x30;
	unsigned int Summ=0, Summ1=0,buf,Dark1=0,WaterLight1=0,Light1=0,Dark=0,WaterLight=0,Light=0;
	char IsWaterLvl = false, IsWaterLvl1 = false;//пересёк ли первый уровень и  2 уровни
	for(char i = 0;i<100;i++)
	{
		Summ += ADCConvert(0x4);
		_delay_ms(2);
		Summ1 += ADCConvert(0x5);
	}
	Dark = Summ/100;
	Dark1 = Summ1/100;
	PORTB &= 0xCF;
	Summ = 0;
	Summ1 = 0;
	for(char i = 0;i<100;i++)
	{
		Summ += ADCConvert(0x4);
		_delay_ms(2);
		Summ1 += ADCConvert(0x5);
	}
	Light = Summ/100;
	Light1 = Summ1/100;
	
	LCDClear();
	DisplayCalibRes(Dark,Light,0);
	setpos(0,1);
	DisplayCalibRes(Dark1,Light1,0);
	
	
	PORTB &= 0xFC;
	
	while (1)
	{
		if((char)IsSame(ADCConvert(0x4),Light,(int)Light/5) == false && !IsWaterLvl)
		{
			IsWaterLvl = true;
			Summ=0;
			for(char i = 0;i<100;i++)
			{
				Summ += ADCConvert(0x4);
				_delay_ms(T001S*3);
			}
			WaterLight = Summ/100;
		}
		if((char)IsSame(ADCConvert(0x5),Light1,(int)Light1/5) == false && !IsWaterLvl1)
		{
			IsWaterLvl1 = true;
			Summ=0;
			for(char i = 0;i<100;i++)
			{
				Summ += ADCConvert(0x5);
				_delay_ms(T001S*3);
			}
			WaterLight1 = Summ/100;
			
			
			PORTC |= 0x3;
			break;
		}
		_delay_ms(10);
	}
		
	WriteEEPROMInt(0,Dark);
	WriteEEPROMInt(2,Light);
	WriteEEPROMInt(4,WaterLight);
	
	WriteEEPROMInt(6,Dark1);
	WriteEEPROMInt(8,Light1);
	WriteEEPROMInt(10,WaterLight1);
	
	
	LCDClear();
	DisplayCalibRes(Dark,Light,WaterLight);
	setpos(0,1);
	DisplayCalibRes(Dark1,Light1,WaterLight1);
	
	_delay_ms(5*T1S);
	
}

int IsSame(int One,int Two,int Part)
{
	if(One >= Two - Part && One <= Two + Part)
	{
		return 0x1;
	}
	else 
	{
		return 0x0;
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
			if (Frequency > 70)
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
	ADCInit();
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
		if(Stage == 1 && ADCConvert(0x5) >=50 && ADCConvert(0x5) <=70)
		{
			PORTC &= 0xF8;
			PORTC |= 0x2;
			StartTimer();
			Stage = 2;
			LCDClear();
			SendStr(Test2);
			setpos(0,1);
			SendStr(Test22);
			
		}	
		else if(Stage == 2 &&  ADCConvert(0x4) >=650 && ADCConvert(0x4) <=850)
		{
			StopTimer();
			ElapsedTime = GetTime();
			PORTC &= 0xF8;
			StopPWM();
			CountResault();
			return;
		}
		
		if(Stage == 1)
		{
		LCDClear();
		unsigned int buf = ADCConvert(0x5);
		sendchar(buf/1000+0x30);
		sendchar((buf%1000)/100+0x30);
		sendchar((buf%100)/10+0x30);
		sendchar(buf%10+0x30);
		}
		if(Stage == 2)
		{
			LCDClear();
			unsigned int buf = ADCConvert(0x4);
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
	sei();
	PortInit();                                                    
	LCDInit();
	 while (1) 
    {
		
		IsCalibrate();
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
		
    }
}

