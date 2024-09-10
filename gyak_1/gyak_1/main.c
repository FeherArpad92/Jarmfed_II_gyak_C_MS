/*
 * gyak1_2.c
 *
 * Created: 9/5/2024 12:38:17 PM
 * Author : feherarpad
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL 
#include <avr/delay.h>

#include <inttypes.h>

void port_init(void);
void timer_init(void);
void external_int_init(void);

#define TRUE 1
#define FALSE 0

uint8_t task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;

void port_init(void)
{
	DDRA = 0xff;
	PORTA = 0xff;
	
	DDRF = 0x0f;
	PORTF = 0x0f;
	
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
}

void timer_init(void)
{
	TCCR0A = (0<<WGM00) | (1<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
	OCR0A = 77;
	TIMSK0 = (1<<OCIE0A);
}

void external_int_init(void)
{
	EICRA = (1<<ISC01) | (0<<ISC00);
	EIMSK = (1<<INT0);
}

int main(void)
{
	port_init();
	timer_init();
	external_int_init();
	sei();
    /* Replace with your application code */
    while (1)
    {
		if(task_10ms == TRUE)
		{
			PORTF ^= 0x01;
			task_10ms = FALSE;
		}
		
		if(task_100ms == TRUE)
		{
			PORTF ^= 0X02;
			task_100ms = FALSE;
		}
		
		if(task_500ms == TRUE)
		{
			PORTF ^= 0x04;
			task_500ms = FALSE;
		}
    }
}

ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) task_10ms = TRUE;
	if((timer_cnt % 10) == 0) task_100ms =TRUE;
	if((timer_cnt % 50) == 0) task_500ms = TRUE;
	
	
}

ISR(INT0_vect)
{
	PORTA = PORTA ^ 0x01;
}

