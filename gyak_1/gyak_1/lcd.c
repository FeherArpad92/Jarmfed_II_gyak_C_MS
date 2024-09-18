/******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include "lcd.h"
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
* Description:      pulse on E port
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
* Description:      LCD inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_init(void)
{
	_delay_ms(50);
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//display_ON/OFF control
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_us(40);
	
	//display clear
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (1<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(2);
	
	//mode set
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	PORTC = (0<<LCD_RS) | (0<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (0<<LCD_D4);
	lcd_enable_pulse();
	_delay_ms(10);
	
}

/******************************************************************************
* Function:         void lcd_write_char(char c)
* Description:      karakter megjelenítése
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
* Function:        void lcd_set_cursor_position(uint8_t pos)
* Description:     beállítja a kurzor pozícióját
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
* Function:        void lcd_clear_display(void)
* Description:     kitörli a kijelz? tartalmát
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_clear_display(void)
{
	_delay_us(40);
	PORTC = (0x00 & 0xF0) | (0<<LCD_RS);
	lcd_enable_pulse();
	PORTC = ((0x01 & 0x0F)<<4) | (0<<LCD_RS);
	lcd_enable_pulse();
	_delay_ms(2);
}

/******************************************************************************
* Function:        void lcd_write_string(void)
* Description:     karaktersorozatot ír ki
* Input:
* Output:
* Notes:
******************************************************************************/
void lcd_write_string(char *c)
{
	char *p = c;
	while(*p != 0) lcd_write_char(*p++);
}