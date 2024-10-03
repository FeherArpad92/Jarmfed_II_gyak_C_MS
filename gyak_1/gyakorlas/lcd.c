/******************************************************************************
* Include files
******************************************************************************/
#include "lcd.h"
#include <avr/io.h>

#define F_CPU 8000000UL
#include <avr/delay.h>
/******************************************************************************
* Macros
******************************************************************************/


/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/


/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void lcd_enable_pulse(void);

/******************************************************************************
* Local Function Definitions
******************************************************************************/

/******************************************************************************
* Function:         void lcd_enable_pulse(void)
* Description:      LCD E lábára impulzus generálása
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_enable_pulse(void)
{
	PORTC = PORTC | (1<<LCD_E);
	_delay_us(1);
	PORTC = PORTC & ~(1<<LCD_E);
}

/******************************************************************************
* Global Function Definitions
******************************************************************************/



/******************************************************************************
* Function:         void lcd_init(void)
* Description:      LCD kijelz? inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_init(void)
{
	_delay_ms(50);
	
	//function set 1
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//function set 2
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//function set 3
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//display on/off control
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//clear display
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(2);
	
	//entry mode set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(10);
}

/******************************************************************************
* Function:         void lcd_write_char(char c)
* Description:      karakter kiküldése az LCD kijelz?re
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_write_char(char c)
{
	_delay_us(40);
	PORTC = (c & 0xf0) | (1<<LCD_RS);
	lcd_enable_pulse();
	PORTC = ((c & 0x0f)<<4) | (1<<LCD_RS);
	lcd_enable_pulse();
}


/******************************************************************************
* Function:         void lcd_clear_display(void)
* Description:      törli a kijelz?t
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_clear_display(void)
{
	_delay_us(40);
	PORTC = 0x00 | (0<<LCD_RS);
	lcd_enable_pulse();
	PORTC = (0x01<<4) | (0<<LCD_RS);
	lcd_enable_pulse();
	_delay_ms(2);
}


/******************************************************************************
* Function:         void lcd_set_cursor_position(uint8_t pos)
* Description:      kurzor pozíció beállítása
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_set_cursor_position(uint8_t pos)
{
	if(pos < 0x67)
	{
		pos = pos | (1<<LCD_D7);
		
		_delay_us(40);
		PORTC = (pos & 0xf0) | (0<<LCD_RS);
		lcd_enable_pulse();
		PORTC = ((pos & 0x0f)<<4) | (0<<LCD_RS);
		lcd_enable_pulse();
		_delay_us(40);
	}
	
}

/******************************************************************************
* Function:         void lcd_write_string(char *string)
* Description:      karaktersorozat kiírása az LCD kijelz?re
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_write_string(char *string)
{
	char *p = string;
	while(*p != 0) lcd_write_char(*p++);
}




