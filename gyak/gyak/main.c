#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define F_CPU 8000000UL

#define TRUE  1u
#define FALSE 0u

volatile uint8_t timer_task_10ms  = FALSE;
volatile uint8_t timer_task_100ms = FALSE;
volatile uint8_t timer_task_500ms = FALSE;
volatile uint8_t timer_task_1s    = FALSE;

uint8_t timer_cnt = 0u;

void port_init(void);
void timer_init(void);

void port_init(void)
{
    DDRF = (1 << PF0) |
           (1 << PF1) |
           (1 << PF2) |
           (1 << PF3);
}

void timer_init(void)
{
    TCCR0A = (1 << WGM01) |
             (1 << CS02)  |
             (1 << CS00);

    OCR0A = 77;
    TIMSK0 = (1 << OCIE0A);
}

int main(void)
{
    port_init();
    timer_init();
    sei();

    while(1)
    {
        if(timer_task_10ms)
        {
            timer_task_10ms = FALSE;
            PORTF ^= (1 << PF0);
        }

        if(timer_task_100ms)
        {
            timer_task_100ms = FALSE;
            PORTF ^= (1 << PF1);
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
