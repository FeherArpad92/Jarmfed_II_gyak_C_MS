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





