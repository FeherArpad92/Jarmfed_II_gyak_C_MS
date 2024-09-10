#include <avr/io.h>

#define F_CPU 8000000UL
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define FALSE 0
#define TRUE 1

uint16_t timer_cnt=0;
uint8_t task_10ms = FALSE, task_100ms = FALSE, task_500ms = FALSE;

void port_init(void);
void timer_init(void);

void port_init(void)
{
	DDRA = 0xff;
	PORTA = 0xff;
	
	DDRF = 0x0f;
	PORTF = 0x0f;
}


void timer_init(void)
{
	TCCR0A = (1<<WGM01) | (0<<WGM00) | (1<<CS2) | (0<<CS1) | (1<<CS0);
	OCR0A = 77;
	TIMSK0 = (1<<OCIE0A);
}


int main(void)
{
	port_init();
	timer_init();
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
			PORTF ^=(1<<PF2);
			task_500ms=FALSE;
		}
    }
}

ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if(timer_cnt % 1 == 0) task_10ms = TRUE;
	if(timer_cnt % 10 == 0) task_100ms = TRUE; 
	if(timer_cnt % 50 == 0) task_500ms =TRUE;
}