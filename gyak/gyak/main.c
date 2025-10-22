/******************************************************************************
 * Created: 2025.09.10
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

#define PD0_ENA_DELAY 80 //ms

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint8_t timer_task_10ms=0, timer_task_100ms=0, timer_task_500ms=0, timer_task_1s=0;
uint8_t PD0_re_enable_cnt=0;
uint8_t PB0_pushed=0;
uint16_t ad_value = 0;
uint16_t time_0 = 0;
uint16_t time_1 = 0;

/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void write_voltage(uint16_t v);
void write_float(float tort_sz);
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

void write_voltage(uint16_t v)
{
	char string_for_write_ad[50];
	int voltage = ((uint32_t)v*5000)/1024;
	sprintf(string_for_write_ad, "%d.%03d",voltage/1000,voltage % 1000);
	
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write_ad);
}

void write_float(float tort_sz)
{
	char string_for_write[50];
	uint16_t egesz = tort_sz;
	uint32_t tort = (tort_sz-egesz)*1000000;
	
	sprintf(string_for_write, "%d.%06ld",egesz,tort);
	
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write);
	
	
}

void write_8bit(uint8_t num)
{
	lcd_set_cursor_position(0);
	for(int i=0;i<8;i++)
	{
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
	sprintf(string_for_write,"%X",num);
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
	external_interrupt_init();
	ad_init();
	lcd_init();
	
	uart_0_init(51);
	sei();
	/* Replace with your application code */
	while(1)
	{
		if(timer_task_10ms)
		{
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == 0)
			{
				PORTA ^=0x01;
				PB0_pushed=1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1) PB0_pushed = 0;
			
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10;
			//PORTF ^= (1<<PF0);
			timer_task_10ms=FALSE;
		}
		if(timer_task_100ms)
		{
			
			//write_voltage(ad_value);
			//write_float(M_PI);
			//write_8bit(0b01000111);
			//write_hexa_num(0x143F);
			
			ADCSRA |= (1<<ADSC);
			//PORTA = ad_value>>2;
			
			char string_for_write[50];
			sprintf(string_for_write, "%d %d %d", time_0,time_1,(time_1-time_0));
			lcd_set_cursor_position(0);
			lcd_write_string(string_for_write);
			
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
	if((timer_cnt % 10) == 0) timer_task_10ms = TRUE;
	if((timer_cnt % 100) == 0) timer_task_100ms = TRUE;
	if((timer_cnt % 500) == 0) timer_task_500ms = TRUE;
	if((timer_cnt % 1000) == 0) timer_task_1s = TRUE;
}

ISR(INT0_vect)
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY)
	{
		if(time_0 != 0 && time_1 == 0) time_1=timer_cnt;
		if(time_0 == 0) time_0 = timer_cnt;
		
		PORTA ^=0xff;
		PD0_re_enable_cnt=0;
	}
}

ISR(ADC_vect)
{
	ad_value = ADC;
}

ISR(USART0_RX_vect)
{
	char c= UDR0;
}
