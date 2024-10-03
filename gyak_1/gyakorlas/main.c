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
#include "uart.h"
#include "peripherals.h"

/******************************************************************************
* Macros
******************************************************************************/
#define TRUE 1
#define FALSE 0



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
			//ADCSRA |= (1<<ADSC);
			//PORTA = ad_result;
			//PORTF ^= 0X02;
			//char write_string[50];
			//sprintf(write_string,"%4d",ad_result);
			//lcd_set_cursor_position(0);
			//lcd_write_string(write_string);
			//uart_write_string(write_string);
			//uart_write_string("\r\n");
			
			//1. feladat
			char write_string[50];
			int voltage = ((uint32_t)ad_result*5000) / 1023;
			sprintf(write_string,"%d.%d",voltage/1000, voltage % 1000);
			lcd_set_cursor_position(0);
			lcd_write_string(write_string);
			ADCSRA |= (1<<ADSC);
			
			
			
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
