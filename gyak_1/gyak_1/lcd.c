/******************************************************************************
* Include files
******************************************************************************/
#include "lcd.h"

#include <avr/io.h>
#include <inttypes.h>
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
* Description:      impulzus generálása az LCD E lábán az utasítás végrehajtásához
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_enable_pulse(void)
{
	PORTC |= (1<<LCD_E);
	_delay_us(1);
	PORTC &= ~(1<<LCD_E);
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
	
	//Function set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//Function set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//Function set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//Display ON/OFF control
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//Display clear
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(2);
	
	//Entry mode set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(10);
}



/******************************************************************************
* Function:         void lcd_write_char(char c)
* Description:      karakter kiirasa az LCD kijelzore
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_write_char(char c)
{
	_delay_us(40);
	PORTC = (c & 0xF0) | (1<<LCD_RS);
	lcd_enable_pulse();
	PORTC = ((c & 0x0F)<<4) | (1<<LCD_RS);
	lcd_enable_pulse();
}

/******************************************************************************
* Function:         void lcd_set_cursor_position(uint8_t pos)
* Description:      kurzor pozicio beallitasa
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_set_cursor_position(uint8_t pos)
{
	_delay_us(40);
	if (pos < 67)
	{
		pos = ((1<<LCD_D7) | pos);
		PORTC = (pos & 0xF0) | (0<<LCD_RS);
		lcd_enable_pulse();
		PORTC = ((pos & 0x0F)<<4) | (0<<LCD_RS);
		lcd_enable_pulse();
	}
}

/******************************************************************************
* Function:         void lcd_set_cursor_position(uint8_t pos)
* Description:      kurzor pozicio beallitasa
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
* Description:      kurzor pozicio beallitasa
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_write_string(char *string)
{
	char *p = string;
	while(*p != 0) lcd_write_char(*p++);
}