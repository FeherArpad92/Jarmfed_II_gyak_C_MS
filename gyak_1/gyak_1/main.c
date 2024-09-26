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
#include "lcd.h"

/******************************************************************************
* Macros
******************************************************************************/
#define TRUE 1
#define FALSE 0

//UART
#define BAUD9600 51

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint8_t task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;
uint16_t ad_result;
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
void uart0_send_char(char data);
void uart_write_string(char *string);

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
	
	DDRF = (1<<PF3) | (1<<PF2) | (1<<PF1) | (0<<PF0);
	//PORTF = 0x0f;
	
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
* Function:         void uart0_init(uint16_t baud)
* Description:      UART 0 inicializálása
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
* Function:         void uart0_send_char(char data)
* Description:      karakter küldése uarton
* Input:
* Output:
* Notes:
******************************************************************************/
void uart0_send_char(char data)
{
	/* Wait for empty transmit buffer */
	while ( ! ( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

/******************************************************************************
* Function:         void uart_write_string(char *string)
* Description:      karaktersorozat kiírása uartra
* Input:
* Output:
* Notes:
******************************************************************************/
void uart_write_string(char *string)
{
	char *p = string;
	while(*p != 0) uart0_send_char(*p++);
}

/******************************************************************************
* Function:         void adc_init(void)
* Description:      AD konverter felkonfigurálása
* Input:
* Output:
* Notes:
******************************************************************************/
void adc_init(void)
{
	ADMUX = 0;
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
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
	uart0_init(BAUD9600);
	adc_init();
	sei();
    /* Replace with your application code */
    while (1)
    {
		if(task_10ms == TRUE)
		{
			//PORTF ^= 0x01;
			task_10ms = FALSE;
		}
		
		if(task_100ms == TRUE)
		{
			ADCSRA |= (1<<ADSC); 
			PORTA =ad_result;
			PORTF ^= 0X02;
			char write_string[50];
			sprintf(write_string,"%4d",ad_result);
			lcd_set_cursor_position(0);
			lcd_write_string(write_string);
			uart_write_string(write_string);
			uart_write_string("\r\n");
			task_100ms = FALSE;
		}
		
		if(task_500ms == TRUE)
		{
			//char szoveg[] = "szoveg a kijelzon";
			//lcd_write_string(szoveg);
			//lcd_set_cursor_position(0);
			//lcd_clear_display();
			//lcd_write_string("masik szoveg");
			
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

ISR(USART0_RX_vect)
{
	char c = UDR0;
	if(c == 0x7F)
		lcd_clear_display();
	else
		lcd_write_char(c);
}

ISR(ADC_vect)
{
	ad_result = ADC;
}
