/******************************************************************************
* Include files
******************************************************************************/
#include "peripherals.h"
#include <avr/io.h>
/******************************************************************************
* Macros
******************************************************************************/


/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/


/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/


/******************************************************************************
* Local Function Definitions
******************************************************************************/


/******************************************************************************
* Global Function Definitions
******************************************************************************/
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