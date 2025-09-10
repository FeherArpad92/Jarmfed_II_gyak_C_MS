/******************************************************************************
 * Created: 
 * Author : Fehér Árpád
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

/******************************************************************************
* Macros
******************************************************************************/
#define PD0_ENA_DELAY 80
#define TRUE 1
#define FALSE 0

/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint8_t timer_task_10ms=0, timer_task_100ms=0, timer_task_500ms=0, timer_task_1s=0;
uint8_t PB0_pushed = 0, PD0_re_enable_cnt = 0;

/******************************************************************************
* External Variables
******************************************************************************/

/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void timer_init(void);
void external_int_init(void);


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
	
	
	DDRA = 0xff;
	PORTA = 0x00;
	
	DDRD = 0x00; //PD0 extint
	PORTD = 0x01; //PD0 extint
}

/******************************************************************************
* Function:         void timer_init(void)
* Description:      Timer 0 inicializalasa
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

/*****************************************************************************
* Function:         void external_int_init(void)
* Description:      külso megszakitasok inicializalasa
* Input:
* Output:
* Notes:
******************************************************************************/
void external_int_init(void)
{
	EICRA = (1<<ISC01);
	EIMSK = (1<<INT0);
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
	sei();
	
	//V?gtelen ciklus
	while(1)
	{
		
		if(timer_task_10ms)
		{
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == 0)
			{
				PORTA ^=0x01;
				PB0_pushed = 1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1) PB0_pushed = 0;
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10; //perg?smentes?t?s
			
			PORTF ^= (1<<PF0);
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
ISR(TIMER0_COMP_vect) //timer megszak?t?s
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms=1;
	if((timer_cnt % 10) == 0) timer_task_100ms=1;
	if((timer_cnt % 50) == 0) timer_task_500ms=1;
	if((timer_cnt % 100) == 0) timer_task_1s=1;
}

ISR(INT0_vect) //external interrupt
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY) //perg?smentes?t?s logika
	{
		PORTA ^=0x02;
		PD0_re_enable_cnt=0;
	}
	
}