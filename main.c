#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define F_CPU 8000000UL

#define TRUE  1u
#define FALSE 0u
#define PD0_ENA_DELAY 80

volatile uint8_t timer_task_10ms  = FALSE;
volatile uint8_t timer_task_100ms = FALSE;
volatile uint8_t timer_task_500ms = FALSE;
volatile uint8_t timer_task_1s    = FALSE;

uint8_t timer_cnt = 0u;
uint8_t PB0_pushed = 0;
uint8_t PD0_re_enable_cnt = 0;
uint16_t adc_result = 0;

void port_init(void);
void timer_init(void);
void external_int_init(void);
void ad_init(void);

void port_init(void)
{
	DDRF = (1 << PF0) |
	(1 << PF1) |
	(1 << PF2) |
	(1 << PF3);
	//bemenet, illetve felhúzó ellenállás
	DDRB = (0<<PB0);
	PORTB = (1<<PB0);
	
	//Kimenet beállítása
	DDRA = 0xff;
	
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
}

void timer_init(void)
{
	TCCR0A = (1 << WGM01) |
	(1 << CS02)  |
	(1 << CS00);

	OCR0A = 77;
	TIMSK0 = (1 << OCIE0A);
}

void external_int_init(void){
	EICRA = (1<<ISC01)|(0<<ISC00);
	EIMSK = (1<<INT0);
}

void ad_init(void){
	ADMUX = 0;
	ADCSRA = (1<<ADEN) | (1<<ADIE) | (1<<ADPS2)| (1<<ADPS1)| (1<<ADPS0);
}

int main(void)
{
	port_init();
	timer_init();
	//Ez mindig a sei() elõtt legyen!!!!
	external_int_init();
	ad_init();
	sei(); //Globálisan engedélyezi az interruptokat

	while(1)
	{
		if(timer_task_10ms)
		{
			timer_task_10ms = FALSE;
			PORTF ^= (1 << PF0);
			
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == 0){
				//gomb lenyomása
				PORTA ^= 0x01;
				PB0_pushed = 1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1){
				//gomb felengedése
				PB0_pushed=0;
			}
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY)
				PD0_re_enable_cnt +=10;
		}

		if(timer_task_100ms)
		{
			timer_task_100ms = FALSE;
			PORTF ^= (1 << PF1);
			//elindítjuk az AD átalakítást
			ADCSRA |= (1<<ADSC);
			PORTA = adc_result >>2;
		}

		if(timer_task_500ms)
		{
			timer_task_500ms = FALSE;
			PORTF ^= (1 << PF2);
		}

		if(timer_task_1s)
		{
			timer_task_1s = FALSE;
			PORTF ^= (1 << PF3);
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	timer_task_10ms = TRUE;
	timer_cnt++;

	if((timer_cnt % 10u) == 0u)
	timer_task_100ms = TRUE;

	if((timer_cnt % 50u) == 0u)
	timer_task_500ms = TRUE;

	if(timer_cnt >= 100u)
	{
		timer_task_1s = TRUE;
		timer_cnt = 0u;
	}
}

ISR(INT0_vect){
	if(PD0_re_enable_cnt>= PD0_ENA_DELAY){
		PORTA ^= 0xff;
		PD0_re_enable_cnt = 0;
	}
}

ISR(ADC_vect){
	adc_result = ADC;
}