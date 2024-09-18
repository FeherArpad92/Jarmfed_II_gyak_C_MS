/******************************************************************************
 * Created: 1. edu. week
 * Author : Árpád Fehér
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>

#define F_CPU 8000000UL
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "lcd.h"

/******************************************************************************
* Macros
******************************************************************************/
#define FALSE 0
#define TRUE 1

//UART 
#define BAUD9600 51 //Datasheet page 202.



/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint8_t task_10ms = FALSE, task_100ms = FALSE, task_500ms = FALSE;
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void timer_init(void);
void external_int_init(void);
void uart0_init(uint16_t baud);
void adc_init(void);


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
	TCCR0A = (1<<WGM01) | (0<<WGM00) | (1<<CS2) | (0<<CS1) | (1<<CS0);
	OCR0A = 77;
	TIMSK0 = (1<<OCIE0A);
}

/******************************************************************************
* Function:         void external_int_init(void)
* Description:      küls? megszakítás inicializálása
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
* Function:         void uart0_init(uint16_t baud)
* Description:      UART0 felkonfigurálása
* Input:
* Output:
* Notes:
******************************************************************************/
void uart0_init(uint16_t baud)
{
	/* Set baud rate */
	UBRR0H = (unsigned char) (baud>>8);
	UBRR0L = (unsigned char) baud;
	/* Set frame format: 8data, no parity & 1 stop bits */
	UCSR0C = (0<<UMSEL0) | (0<<UPM0) | (1<<USBS0) | (1<<UCSZ1) | (1<<UCSZ0);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
}

/******************************************************************************
* Function:         void adc_init(void)
* Description:      ADC konfigurálása
* Input:
* Output:
* Notes:
******************************************************************************/
void adc_init(void)
{
	ADMUX = 0;
	ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
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
	uart0_init(BAUD9600);
	lcd_init();
	sei();
	
	
    /* Replace with your application code */
    while (1) 
    {
		if(task_10ms)
		{
			PORTF ^=(1<<PF0);
			task_10ms=FALSE;
		}
		if(task_100ms)
		{
			PORTF ^=(1<<PF1);
			task_100ms=FALSE;
		}
		if(task_500ms)
		{
			//lcd_set_cursor_position(0);
			//lcd_write_string("valami");
			
			PORTF ^=(1<<PF2);
			task_500ms=FALSE;
		}
    }
}

/******************************************************************************
* Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect) //timer CTC interrupt
{
	timer_cnt++;
	if(timer_cnt % 1 == 0) task_10ms = TRUE;
	if(timer_cnt % 10 == 0) task_100ms = TRUE;
	if(timer_cnt % 50 == 0) task_500ms =TRUE;
}

ISR(INT0_vect) //extint 0 interrput
{
	PORTA = PORTA ^ 0x01;
}

ISR(USART0_RX_vect)
{
	char c = UDR0;
	lcd_write_char(c);
}