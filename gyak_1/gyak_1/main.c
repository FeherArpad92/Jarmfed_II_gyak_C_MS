/******************************************************************************
 * Created: 
 * Author : Árpád Fehér
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <avr/delay.h>

/******************************************************************************
* Macros
******************************************************************************/
#define PD0_ENA_DELAY 20
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
uint16_t timer_cnt=0;
uint8_t timer_task_10ms=0, timer_task_100ms=0, timer_task_500ms=0, timer_task_1s=0;
uint8_t PB0_pushed = 0, PD0_re_enable_cnt = 0;
uint16_t ad_value =0;
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void timer_init(void);
void external_int_init(void);
void ad_init(void);
void lcd_enable_pulse(void);
void lcd_write_char(char c);
void lcd_set_cursor_position(uint8_t pos);
/******************************************************************************
* Local Function Definitions
******************************************************************************/

/*****************************************************************************
* Function:         void port_init(void)
* Description:      I/O portok inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void port_init(void)
{
	
	DDRF = (1<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	PORTF = (1<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	
	DDRB = (0<<PB0);
	PORTB = (1<<PB0);
	
	DDRA = 0xff;
	PORTA = 0x00;
	
	DDRD = 0x00; //PD0 extint
	PORTD = 0x01; //PD0 extint
	
	//LCD
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
}

/*****************************************************************************
* Function:         void external_int_init(void)
* Description:      k?ls? megszak?t?sok inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void external_int_init(void)
{
	EICRA = (1<<ISC01);
	EIMSK = (1<<INT0);
}

/*****************************************************************************
* Function:         void ad_init(void)
* Description:      AD inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void ad_init(void)
{
	ADMUX = 0;
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIE);
	ADCSRB = 0;
}

/******************************************************************************
* Function:         void timer_init(void)
* Description:      Timer 0 inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void timer_init(void)
{
	TCCR0A = (0<<WGM00) | (1<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
	OCR0A = 77;
	TIMSK0 = TIMSK0 | (1 << OCIE0A);
}

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

void lcd_enable_pulse(void)
{
	PORTC |= (1<<LCD_E);
	_delay_us(1);
	PORTC &= ~(1<<LCD_E);
}

void lcd_write_char(char c)
{
	_delay_us(40);
	PORTC = (c & 0xF0) | (1<<LCD_RS);
	lcd_enable_pulse();
	PORTC = ((c & 0x0F)<<4) | (1<<LCD_RS);
	lcd_enable_pulse();
}

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
* Function:         int main(void)
* Description:      main function
* Input:            
* Output:
* Notes:            
******************************************************************************/
int main(void)
{
	port_init();
	timer_init();
	external_int_init();
	ad_init();
	lcd_init();
	sei();
	
	//V?gtelen ciklus
	while(1)
	{
		
		if(timer_task_10ms)
		{
			PORTF ^= (1<<PF0);
			
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == 0)
			{
				PORTA ^=0x01;
				lcd_set_cursor_position(40);
				lcd_write_char('a');
				lcd_write_char('b');
				lcd_write_char('c');
				lcd_write_char('d');
				
				PB0_pushed = 1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1) PB0_pushed = 0;
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10;
			timer_task_10ms = 0;
		}
		
		if(timer_task_100ms)
		{
			PORTF ^= (1<<PF1);
			timer_task_100ms =0;
		}
		
		if(timer_task_500ms)
		{
			PORTF ^= (1<<PF2);
			timer_task_500ms=0;
		}
		
		if(timer_task_1s)
		{
			PORTF ^= (1<<PF3);
			timer_task_1s=0;
		}

	}
}
/******************************************************************************
* Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms=1;
	if((timer_cnt % 10) == 0) timer_task_100ms=1;
	if((timer_cnt % 50) == 0) timer_task_500ms=1;
	if((timer_cnt % 100) == 0) timer_task_1s=1;
}

ISR(INT0_vect)
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY)
	{
		
		PD0_re_enable_cnt=0;
	}
	PORTA ^=0x02;
}

ISR(ADC_vect)
{
	ad_value = ADC;
}