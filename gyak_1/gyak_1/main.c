/******************************************************************************
 * Created: 
 * Author : ?rp?d Feh?r
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <avr/delay.h>

/******************************************************************************
* Macros
******************************************************************************/
#define PD0_ENA_DELAY 20
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
uint16_t ad_value =0;
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void timer_init(void);
void external_int_init(void);
void ad_init(void);
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
	
	DDRF = (1<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	PORTF = (1<<PF0)| (1<<PF1) | (1<<PF2) | (1<<PF3);
	
	DDRB = (0<<PB0);
	PORTB = (1<<PB0);
	
	DDRA = 0xff;
	PORTA = 0x00;
	
	DDRD = 0x00; //PD0 extint
	PORTD = 0x01; //PD0 extint
}

/*****************************************************************************
* Function:         void external_int_init(void)
* Description:      k?ls? megszak?t?sok inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void external_int_init(void)
{
	EICRA = (1<<ISC01);
	EIMSK = (1<<INT0);
}

/*****************************************************************************
* Function:         void ad_init(void)
* Description:      AD inicializ?l?sa
* Input:
* Output:
* Notes:
******************************************************************************/
void ad_init(void)
{
	ADMUX = 0;
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADIE);
	ADCSRB = 0;
}

/******************************************************************************
* Function:         void timer_init(void)
* Description:      Timer 0 inicializ?l?sa
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
	sei();
	
	//V?gtelen ciklus
	while(1)
	{
		
		if(timer_task_10ms)
		{
			PORTF ^= (1<<PF0);
			
			if((PINB & (1<<PB0)) == 0 && PB0_pushed == 0)
			{
				PORTA ^=0x01;
				PB0_pushed = 1;
			}
			if((PINB & (1<<PB0)) == (1<<PB0) && PB0_pushed == 1) PB0_pushed = 0;
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10;
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
ISR(TIMER0_COMP_vect)
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms=1;
	if((timer_cnt % 10) == 0) timer_task_100ms=1;
	if((timer_cnt % 50) == 0) timer_task_500ms=1;
	if((timer_cnt % 100) == 0) timer_task_1s=1;
}

ISR(INT0_vect)
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY)
	{
		
		PD0_re_enable_cnt=0;
	}
	PORTA ^=0x02;
}

ISR(ADC_vect)
{
	ad_value = ADC;
}