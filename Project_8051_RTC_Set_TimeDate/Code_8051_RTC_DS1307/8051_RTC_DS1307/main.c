#include <REGX51.H>
#include <Thu_Vien_LCD.H>
#include <Real_Time_DS1307.H>

// Dinh nghia chan nut
sbit SET = P3^0;
sbit UP = P3^1;
sbit DOWN = P3^2;

// Bien thoi gian va trang thai
unsigned int Hour, Minute, Second, Mode, Day, Date, Month, Year;
unsigned char edit_mode = 0;         // 0: xem binh thuong, 1: chinh sua
unsigned char selected_field = 255;  // khoi dau = 255 de SET lan dau vao chinh Hour

// Delay don gian
void delay(long time)
{
	time = time * 25;
	while(time--);
}

// Xu ly nut nhan
void CheckButtons()
{
	if (SET == 0)
	{
		delay(200);
		while (SET == 0); // cho nha phim

		selected_field++;
		if (selected_field > 6)
		{
			selected_field = 255;  // quay lai che do xem thuong
			edit_mode = 0;

			// Ghi lai thoi gian va ngay thang vao DS1307
			DS1307_Write_Time(Hour, Minute, Second);
			DS1307_Write_Date(Day, Date, Month, Year);
		}
		else
		{
			edit_mode = 1;
		}
	}

	if (edit_mode)
	{
		if (UP == 0)
		{
			delay(200);
			while (UP == 0);
			switch (selected_field)
			{
				case 0: Hour = (Hour + 1) % 24; break;
				case 1: Minute = (Minute + 1) % 60; break;
				case 2: Second = (Second + 1) % 60; break;
				case 3: Day = (Day % 7) + 1; break;
				case 4: Date = (Date < 31) ? Date + 1 : 1; break;
				case 5: Month = (Month < 12) ? Month + 1 : 1; break;
				case 6: Year = (Year < 99) ? Year + 1 : 0; break;
			}
		}

		if (DOWN == 0)
		{
			delay(200);
			while (DOWN == 0);
			switch (selected_field)
			{
				case 0: Hour = (Hour > 0) ? Hour - 1 : 23; break;
				case 1: Minute = (Minute > 0) ? Minute - 1 : 59; break;
				case 2: Second = (Second > 0) ? Second - 1 : 59; break;
				case 3: Day = (Day > 1) ? Day - 1 : 7; break;
				case 4: Date = (Date > 1) ? Date - 1 : 31; break;
				case 5: Month = (Month > 1) ? Month - 1 : 12; break;
				case 6: Year = (Year > 0) ? Year - 1 : 99; break;
			}
		}
	}
}

void LCD_PutNumber2Digits(unsigned char val) {
    if (val < 10) LCD_PutChar('0');
    LCD_PutNumber(val);
}

// Hien thi thoi gian
void Display_Time()
{
	LCD_Gotoxy(4,0);

	if(selected_field == 0 && edit_mode) LCD_Puts("[");
	LCD_PutNumber2Digits(Hour);
	if(selected_field == 0 && edit_mode) LCD_Puts("]");
	LCD_Puts(":");

	if(selected_field == 1 && edit_mode) LCD_Puts("[");
	LCD_PutNumber2Digits(Minute);
	if(selected_field == 1 && edit_mode) LCD_Puts("]");
	LCD_Puts(":");

	if(selected_field == 2 && edit_mode) LCD_Puts("[");
	LCD_PutNumber2Digits(Second);
	if(Second < 100) LCD_PutChar(' ');  // xoa ky tu cu neu tung la 3 chu so
	if(selected_field == 2 && edit_mode) LCD_Puts("]");
	LCD_Puts("     "); 
}

// Hien thi ngay thang
void Display_Date()
{
	LCD_Gotoxy(2, 1);

	if (selected_field == 3 && edit_mode) LCD_Puts("[");
	if (Day == 1) LCD_Puts("CN ");
	else { LCD_Puts("T"); LCD_PutNumber(Day); LCD_Puts(" "); }
	if (selected_field == 3 && edit_mode) LCD_Puts("]");

	if (selected_field == 4 && edit_mode) LCD_Puts("[");
	if (Date < 10) LCD_PutNumber(0);
	LCD_PutNumber(Date);
	if (selected_field == 4 && edit_mode) LCD_Puts("]");
	LCD_Puts("-");

	if (selected_field == 5 && edit_mode) LCD_Puts("[");
	if (Month < 10) LCD_PutNumber(0);
	LCD_PutNumber(Month);
	if (selected_field == 5 && edit_mode) LCD_Puts("]");
	LCD_Puts("-");

	if (selected_field == 6 && edit_mode) LCD_PutChar('[');
	LCD_PutNumber2Digits(Year);  // chi 2 so cuoi
	LCD_PutChar(' '); // xoa ky tu rac
	if (selected_field == 6 && edit_mode) LCD_PutChar(']');
	else LCD_PutChar(' ');
	LCD_Puts("     "); 
}

void main()
{
	DS1307_Init();
	LCD_Init();

	while (1)
	{
		CheckButtons();

		if (!edit_mode)
		{
			DS1307_Read_Time(&Hour, &Minute, &Second);
			DS1307_Read_Date(&Day, &Date, &Month, &Year);
		}

		//LCD_Clear();
		Display_Time();
		Display_Date();
		//delay(500); // delay hien thi (~ nua giay)
	}
}
