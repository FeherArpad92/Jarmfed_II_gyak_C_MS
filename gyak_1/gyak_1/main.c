/******************************************************************************
 * Created: 1. edu. week
 * Author : Árpád Fehér
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL 
#include <avr/delay.h>

#include <inttypes.h>

/******************************************************************************
* Macros
******************************************************************************/
#define TRUE 1
#define FALSE 0

//LCD
#define LCD_D7 7
#define LCD_D6 6
#define LCD_D5 5
#define LCD_D4 4
#define LCD_E 3
#define LCD_RS 2

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint8_t task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void timer_init(void);
void external_int_init(void);
void lcd_enable_pulse(void);
void lcd_init(void);
void lcd_write_char(char c);

/******************************************************************************
* Local Function Definitions
******************************************************************************/
/******************************************************************************
* Function:         void port_init(void)
* Description:      I/O portok inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void port_init(void)
{
	DDRA = 0xff;
	PORTA = 0xff;
	
	DDRF = 0x0f;
	PORTF = 0x0f;
	
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
	
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
}
/******************************************************************************
* Function:         void timer_init(void)
* Description:      timer inicializálása
* Input:
* Output:
* Notes:			10 ms, CTC
******************************************************************************/
void timer_init(void)
{
	TCCR0A = (0<<WGM00) | (1<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
	OCR0A = 77;
	TIMSK0 = (1<<OCIE0A);
}
/******************************************************************************
* Function:         void external_int_init(void)
* Description:      küls? megszakítás beállítása
* Input:
* Output:
* Notes:
******************************************************************************/
void external_int_init(void)
{
	EICRA = (1<<ISC01) | (0<<ISC00);
	EIMSK = (1<<INT0);
}

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
* Function:         int main(void)
* Description:      main függvény
* Input:
* Output:
* Notes:
******************************************************************************/
int main(void)
{
	port_init();
	timer_init();
	external_int_init();
	lcd_init();
	sei();
    /* Replace with your application code */
    while (1)
    {
		if(task_10ms == TRUE)
		{
			PORTF ^= 0x01;
			task_10ms = FALSE;
		}
		
		if(task_100ms == TRUE)
		{
			PORTF ^= 0X02;
			task_100ms = FALSE;
		}
		
		if(task_500ms == TRUE)
		{
			char szoveg[] = "szoveg a kijelzon";
			uint8_t i=0;
			while(szoveg[i] != 0)
			{
				lcd_write_char(szoveg[i]);
				i++;
			}
			
			PORTF ^= 0x04;
			task_500ms = FALSE;
		}
    }
}

/******************************************************************************
* Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect) //timer CTC interrupt
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) task_10ms = TRUE;
	if((timer_cnt % 10) == 0) task_100ms =TRUE;
	if((timer_cnt % 50) == 0) task_500ms = TRUE;
}

ISR(INT0_vect) //ext 0 interrupt
{
	
	PORTA = PORTA ^ 0x01;
}

