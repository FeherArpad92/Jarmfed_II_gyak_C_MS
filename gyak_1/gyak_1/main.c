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
#include <stdio.h>

#include "lcd.h"

/******************************************************************************
* Macros
******************************************************************************/
#define PD0_ENA_DELAY 20
#define TRUE 1
#define FALSE 0

//uart
#define BAUD9600 51 //51 - 9600 Baud rate //Datasheet 202. page

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
void uart_0_init(uint16_t baud);
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
	
	DDRF = (0<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	PORTF = (0<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	
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



/******************************************************************************
* Function:         void uart_0_init(void)
* Description:      UART 0 inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void uart_0_init(uint16_t baud)
{
	
	/* Set baud rate */
	UBRR0H = (unsigned char) (baud>>8);
	UBRR0L = (unsigned char) baud;
	
	UCSR0A = 0;
	/* Set frame format: 8data, no parity & 1 stop bits */
	UCSR0C = (0<<UMSEL0) | (0<<UPM0) | (0<<USBS0) | (1<<UCSZ1) | (1<<UCSZ0);
	/* Enable receiver and transmitter and receive interrupt */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
}

void uart_0_transmit(char data)
{
	/* Wait for empty transmit buffer */
	while ( ! ( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

/******************************************************************************
* Function:         void lcd_set_cursor_position(uint8_t pos)
* Description:      kurzor pozicio beallitasa
* Input:
* Output:
* Notes:
******************************************************************************/
void uart_0_write_string(char *string)
{
	char *p = string;
	while(*p != 0) uart_0_transmit(*p++);
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
	uart_0_init(BAUD9600); 
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
				
				
				
				PB0_pushed = 1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1) PB0_pushed = 0;
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10;
			
			PORTF ^= (1<<PF1);
			ADCSRA |= (1<<ADSC);
			timer_task_10ms = 0;
		}
		
		if(timer_task_100ms)
		{
			char string_for_write[50];
			sprintf(string_for_write, "%4d \r\n", ad_value);
			uart_0_write_string(string_for_write);
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
ISR(TIMER0_COMP_vect) //timer megszakítás
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms=1;
	if((timer_cnt % 10) == 0) timer_task_100ms=1;
	if((timer_cnt % 50) == 0) timer_task_500ms=1;
	if((timer_cnt % 100) == 0) timer_task_1s=1;
}

ISR(INT0_vect) //external interrupt
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY)
	{
		PD0_re_enable_cnt=0;
	}
	PORTA ^=0x02;
}

ISR(ADC_vect) //AD megszakítás
{
	ad_value = ADC;
}

ISR(USART0_RX_vect)
{
	char c = UDR0;
	if(c == 0x7F)
		lcd_clear_display();
	else
		lcd_write_char(c);
	
}