/******************************************************************************
 * Created: 2025.09.11.
 * Author : Feher Arpad
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
#include "peripherals.h"
#include "lcd.h"
/******************************************************************************
* Macros
******************************************************************************/
#define TRUE 1
#define FALSE 0

#define PD0_ENA_DELAY 80

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint8_t timer_task_10ms = FALSE, timer_task_100ms = FALSE, timer_task_500ms = FALSE, timer_task_1s = FALSE;
uint8_t PD0_re_enable_cnt=0;
uint8_t PB0_pushed = FALSE;
uint16_t adc_result=0;

/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void timer_init(void);
void write_voltage(uint16_t ad_res);
void write_pi(float tort);
void write_8bit(uint8_t num);
void write_hexa_num(uint16_t num);

/******************************************************************************
* Local Function Definitions
******************************************************************************/

void port_init(void)
{
	DDRA = 0xff;
	DDRF = (0<<PF0) | (1<<PF1) | (1<<PF2) | (1<<PF3);
	
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
	
	DDRB = (0<<PB0);
	PORTB = (1<<PB0);
}


void write_voltage(uint16_t ad_res)
{
	char string_for_write_ad[50];
	int voltage = ((uint32_t)ad_res * 5000) /1024; // mV
	sprintf(string_for_write_ad, "%d.%03d V",voltage/1000, voltage % 1000);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write_ad);
}

void write_pi(float tort)
{
	char string_for_write[50];
	uint8_t int_part = tort;
	uint32_t float_part = (tort-int_part)*1000000;
	sprintf(string_for_write,"%d.%ld",int_part,float_part);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write);
}

void write_8bit(uint8_t num)
{
	lcd_set_cursor_position(0);
	uint8_t temp =num;
	for(uint8_t i=0;i<8;i++)
	{
		//lcd_set_cursor_position(7-i);
		//if((temp % 2)==0)
		//{
			//lcd_write_char('0');
		//}
		//else
		//{
			//lcd_write_char('1');
		//}
		//temp = temp/2;
		
		if(num & (1<<(7-i)))
		{
			lcd_write_char('1');
		}
		else
		{
			lcd_write_char('0');
		}
	}
}

void write_hexa_num(uint16_t num)
{
	char string_for_write[50];
	sprintf(string_for_write,"%x",num);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write);
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
	uart_0_init(51); //9600 baud
	sei();
	
	/* Replace with your application code */
	
	while(1)
	{
		if(timer_task_10ms)
		{
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == FALSE)
			{
				PORTA ^=0x01;
				PB0_pushed = TRUE;
			}
			
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == TRUE)
			{
				PB0_pushed=FALSE;
			}
			
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10;
			
			//PORTF ^= (1<<PF0);
			timer_task_10ms=FALSE;
		}
		
		if(timer_task_100ms)
		{
			
			
			ADCSRA |= (1<<ADSC);
			PORTA = adc_result>>2;
			
			//write_voltage(adc_result);
			//write_pi(M_PI);
			//write_8bit(124);
			write_hexa_num(0x1F78);
			
			
			PORTF ^= (1<<PF1);
			timer_task_100ms=FALSE;
		}
		
		if(timer_task_500ms)
		{
			PORTF ^= (1<<PF2);
			timer_task_500ms=FALSE;
		}
		if(timer_task_1s)
		{
			//lcd_clear_display();
			//lcd_set_cursor_position(0);
			//lcd_write_string("jhasdvfhdsa");
			PORTF ^= (1<<PF3);
			timer_task_1s=FALSE;
		}
	}
}


/******************************************************************************
* Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms = TRUE;
	if((timer_cnt % 10) == 0) timer_task_100ms = TRUE;
	if((timer_cnt % 50) == 0) timer_task_500ms = TRUE;
	if((timer_cnt % 100) == 0) timer_task_1s = TRUE;
}

ISR(INT0_vect)
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY)
	{
		PORTA ^=0xff;
		PD0_re_enable_cnt = 0;
	}
}

ISR(ADC_vect)
{
	adc_result = ADC;
}


ISR(USART0_RX_vect)
{
	char c = UDR0;
}


