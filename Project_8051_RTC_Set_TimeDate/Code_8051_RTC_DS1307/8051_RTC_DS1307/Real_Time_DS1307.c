#include <REGX51.H>
#include <Real_Time_DS1307.H>

#define SCL P1_0
#define SDA P1_1

void Delay() {}

bit Soft_I2c_Get_Ack();
void Soft_I2c_Ack();
void Soft_I2c_Nak();

void I2C_Init()
{
 	SCL=1;
	SDA=1;
}
void I2C_Start()
{
	SCL = 1;	Delay();
	SDA = 0;	Delay();
	SCL = 0;
}
bit I2C_Get_Ack()
{
	bit result;
  SDA = 1;	Delay();
  SCL = 1;	Delay();
	result = SDA;
  SCL = 0;
	return result;
}
bit I2C_Write(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
	  SDA = (bit)(dat&0x80);	
	  SCL = 1;	Delay();
		SCL = 0;
		dat<<=1;
	}
	return(I2C_Get_Ack());
}
void I2C_Ack()
{
	SDA = 0;	Delay();
	SCL = 1;	Delay();
  SCL = 0;
}
void I2C_Nak()
{
  SDA = 1;	Delay();
  SCL = 1;	Delay();
  SCL = 0;
}
unsigned char I2C_Read(bit ack)
{
  unsigned char i, dat=0;
	for(i=0;i<8;i++)
	{
	  SDA = 1;	Delay();
	  SCL = 1;	Delay();
	  dat <<= 1;
		if(SDA)
		{
		 	dat |= 0x01;
		}
		SCL = 0;
  }
	if(ack)
	{
	 	I2C_Ack();
	}
	else
	{
	 	I2C_Nak();
	}
  return dat;
}
void I2C_Stop()
{ 	
	SDA = 0;	Delay();
	SCL = 1;	Delay();
	SDA = 1;
}
void DS1307_Write(unsigned char add, unsigned char dat)
{
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(add); 
	I2C_Write(dat); 
	I2C_Stop();
}
unsigned char DS1307_Read(unsigned char add)
{
	unsigned char dat;
	I2C_Start();
	I2C_Write(0xD0); 
	I2C_Write(add);
	I2C_Start(); 
	I2C_Write(0xD1); 
	dat = I2C_Read(0);
	I2C_Stop();
	return dat;
}
void DS1307_Init()
{
 	unsigned char tmp;
	I2C_Init();
	
	tmp = DS1307_Read(0x00);
	tmp &= 0x7F;
	DS1307_Write(0x00,tmp);		
}
bit DS1307_Read_Time(unsigned int * hour, unsigned int * minute, unsigned int * second)
{
	unsigned int h_tmp, m_tmp, s_tmp, *mode;
	bit am_pm;
	I2C_Start();
	I2C_Write(0xD0); 
	I2C_Write(0x00);
	I2C_Start(); 
	I2C_Write(0xD1); 
	s_tmp = I2C_Read(1);
	m_tmp = I2C_Read(1);
	h_tmp = I2C_Read(0);
	I2C_Stop();

	s_tmp &= 0x7F;
	*second = (s_tmp>>4)*10+(s_tmp&0x0F);
	m_tmp &= 0x7F;
	*minute = (m_tmp>>4)*10+(m_tmp&0x0F);

	if(h_tmp & 0x40) 		// Mode 12h
	{
		*mode = 12;
		if(h_tmp & 0x20)
		{
		 	am_pm = 1;		// PM
		}
		else
		{
		 	am_pm = 0;
		}
		h_tmp &= 0x1F;
	 	*hour = (h_tmp>>4)*10+(h_tmp&0x0F);
	}
	else
	{
		*mode = 24;
		h_tmp &= 0x3F;
	 	*hour = (h_tmp>>4)*10+(h_tmp&0x0F);
		if(*hour<12)
		{
		 	am_pm = 0;		// AM
		}
		else
		{
		 	am_pm = 1;
		}
	}	
	return am_pm;
}
void DS1307_Write_Time(unsigned int hour, unsigned int minute, unsigned int second)
{
	second = ((second/10)<<4)|(second%10);
	minute = ((minute/10)<<4)|(minute%10);
	hour   = ((hour  /10)<<4)|(hour  %10);

	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x00); 
	I2C_Write(second); 
	I2C_Write(minute);
	I2C_Write(hour);
	I2C_Stop();
}

void DS1307_Read_Date(unsigned int * day, unsigned int * date, 
											unsigned int * month, unsigned int * year)
{
	I2C_Start();
	I2C_Write(0xD0); 
	I2C_Write(0x03);
	I2C_Start(); 
	I2C_Write(0xD1);
	*day  = I2C_Read(1);
	*date = I2C_Read(1);
	*month= I2C_Read(1);
	*year = I2C_Read(0);
	I2C_Stop();
	*day &= 0x07;
	*date &= 0x3F;
	*date = (*date>>4)*10 + (*date & 0x0F);
	*month &= 0x1F;
	*month = (*month>>4)*10 + (*month & 0x0F);
	*year = (*year>>4)*10 + (*year & 0x0F);
}
void DS1307_Write_Date(unsigned char day, unsigned char date, 
											 unsigned char month, unsigned char year)
{
	date 	= ((date/10)<<4)  | (date%10);
	month 	= ((month/10)<<4) | (month%10);
	year	= ((year/10)<<4)  | (year%10);
	if(day<=1 || day>= 8) day = 1; 
	else day = day;
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x03); 
	I2C_Write(day); 
	I2C_Write(date);
	I2C_Write(month);
	I2C_Write(year);
	I2C_Stop();
}
void DS1307_Write_Bytes(unsigned char add, unsigned char * buff,	unsigned char len)
{
	unsigned char i=0;

	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(add);
	for(i=0;i<len;i++)
	{ 	 
		I2C_Write(buff[i]); 
	} 
	I2C_Stop();
}
void DS137_Read_Bytes(unsigned char add,unsigned char * buff,
	unsigned char len)
{
 	unsigned char i;

	I2C_Start();
	I2C_Write(0xD0); 
	I2C_Write(add);
	I2C_Start();
	I2C_Write(0xD1);
	for(i=0;i<len-1;i++)
	{ 
		buff[i]  = I2C_Read(1);
	}
	buff[i]  = I2C_Read(0);
	I2C_Stop();
}