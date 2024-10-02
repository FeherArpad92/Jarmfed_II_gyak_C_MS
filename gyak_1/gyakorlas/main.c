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
#include "uart.h"
#include "peripherals.h"

/******************************************************************************
* Macros
******************************************************************************/
#define FALSE 0
#define TRUE 1

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint8_t task_10ms = FALSE, task_100ms = FALSE, task_500ms = FALSE;
uint16_t ad_result;
/******************************************************************************
* External Variables
******************************************************************************/

/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);

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
	//LED-ek
	DDRA = 0xff;
	PORTA = 0xff;
	
	//LED-ek
	DDRF = (1<<PF1) | (1<<PF2);
	PORTF = (1<<PF1) | (1<<PF2);
	
	//kulso megszakitas
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
	
	//LCD
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
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
	adc_init();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		if(task_10ms)
		{
			
			task_10ms=FALSE;
		}
		if(task_100ms)
		{
			
			//példa az ad érték lcd-re írására és uart-on pc felé küldésére
			char string_for_write_ad[50];
			sprintf(string_for_write_ad,"%4d",ad_result);
			lcd_set_cursor_position(0);
			lcd_write_string(string_for_write_ad);
			uart_0_write_string(string_for_write_ad);
			uart_0_write_string("\r\n");
			ADCSRA |= (1<<ADSC); // új ad konverzió indítása
			
			
			
			PORTF ^=(1<<PF1); //LED villogtatása
			task_100ms=FALSE;
		}
		if(task_500ms)
		{
			
			PORTF ^=(1<<PF2);//LED villogtatása
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
	PORTA = PORTA ^ 0x01; //
}

ISR(USART0_RX_vect)
{
	char c = UDR0;
	lcd_write_char(c);
}

ISR(ADC_vect)
{
	ad_result = ADC;
}