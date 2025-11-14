#include <REGX51.H>
#include <Thu_Vien_LCD.h>

#define LCD_RS  P2_0
#define LCD_RW  P2_1
#define LCD_EN  P2_2
#define LCD_D4  P2_3
#define LCD_D5  P2_4
#define LCD_D6  P2_5
#define LCD_D7  P2_6

void delay_us(unsigned int t){
        unsigned int i;
        for(i=0;i<t;i++);
}
void delay_ms(unsigned int t){
        unsigned int i,j;
        for(i=0;i<t;i++)
        for(j=0;j<125;j++);}
void LCD_Enable(void){
        LCD_EN =1;
        delay_us(3);
        LCD_EN =0;
        delay_us(50); 
}
void LCD_Send4Bit(unsigned char Data){
        LCD_D4=Data & 0x01;
        LCD_D5=(Data>>1)&1;
        LCD_D6=(Data>>2)&1;
        LCD_D7=(Data>>3)&1;
}
void LCD_SendCommand(unsigned char command){
        LCD_Send4Bit(command >>4);/* Send 4 bit high */
        LCD_Enable();
        LCD_Send4Bit(command); /* Send 4 bit low*/
        LCD_Enable();
}
void LCD_Clear(){// Xoa man hinh
        LCD_SendCommand(0x01); 
        delay_us(10);
}
void LCD_Init(){
        LCD_Send4Bit(0x00);	  //turn on LCD
        delay_ms(20);
        LCD_RS=0;
        LCD_RW=0;
        LCD_Send4Bit(0x03);	  //function setting
        LCD_Enable();
        delay_ms(5);
        LCD_Enable();
        delay_us(100);
        LCD_Enable();
        LCD_Send4Bit(0x02);	  //di chuyen con tro ve dau man hnh
        LCD_Enable();
        LCD_SendCommand( 0x28 ); //lua chon che do 4 bit
        LCD_SendCommand( 0x0c);  // bat hien thi va tat con tro di
        LCD_SendCommand( 0x06 );
        LCD_SendCommand(0x01);
}
void LCD_Gotoxy(unsigned char x, unsigned char y){
        unsigned char address;
        if(!y)address=(0x80+x);
        else address=(0xc0+x);
        delay_us(1000);
        LCD_SendCommand(address);
        delay_us(50);
}
void LCD_PutChar(unsigned char Data){
        LCD_RS=1;
        LCD_SendCommand(Data);
        LCD_RS=0 ;
}
void LCD_Puts(char *s){
        while (*s){
                LCD_PutChar(*s);
                s++;
        }
}
void LCD_PutNumber(int number)
{
	if(number <= 9)
	{
		LCD_PutChar(number + 48);
	}
	else if(number > 9 && number < 100)
	{
		LCD_PutChar(number/10 + 48);
		LCD_PutChar(number%10 + 48);
	}
	else if(number > 99 && number < 1000)
	{
		LCD_PutChar(number/100 + 48);
		LCD_PutChar(number/10%10 + 48);
		LCD_PutChar(number%10 + 48);
	}
	else if(number > 999 && number < 10000)
	{
		LCD_PutChar(number/1000 + 48);
		LCD_PutChar(number/100%10 + 48);
		LCD_PutChar(number/10%10 + 48);
		LCD_PutChar(number%10 + 48);
	}
	
}