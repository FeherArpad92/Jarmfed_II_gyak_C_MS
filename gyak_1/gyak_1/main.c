/******************************************************************************
 * Created: 
 * Author : Árpád Fehér
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

#include "lcd.h"
#include "uart.h"
#include "can.h"

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
uint16_t ad_value =0;
uint8_t can_rx_data[8];
uint8_t can_tx_data[8];
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);


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
	
	DDRB = (0<<PB0);
	PORTB = (1<<PB0);
	
	DDRA = 0xff;
	PORTA = 0x00;
	
	DDRD = 0x00; //PD0 extint
	PORTD = 0x01; //PD0 extint
	
	//LCD
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
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
	uart_0_init(BAUD9600);
	can_init();
	CAN_ReceiveEnableMob(0, 0x00000001, FALSE, 8);	// enable next reception 
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
			
			if(PD0_re_enable_cnt<PD0_ENA_DELAY) PD0_re_enable_cnt += 10; //pergésmentesítés
			
			PORTF ^= (1<<PF1);
			ADCSRA |= (1<<ADSC);
			timer_task_10ms = 0;
		}
		
		if(timer_task_100ms)
		{
			char string_for_write[50];
			int voltage = ((uint32_t)ad_value * 5000) / 1024; //feszültség millivoltban
			sprintf(string_for_write, "%d.%03d V", voltage / 1000, voltage % 1000);
			uart_0_write_string(string_for_write);
			uart_0_write_string("\r\n");
			lcd_set_cursor_position(0);
			lcd_write_string(string_for_write);
			
			uint8_t can_tx_data[8];
			can_tx_data[0]=ad_value;
			can_tx_data[1]=ad_value>>8;
			//CAN_SendMob(1,0x1FFFFFFF,TRUE,2,can_tx_data);
			CAN_SendMob(2,0x000007FF,FALSE,2,can_tx_data);
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
ISR(TIMER0_COMP_vect) //timer megszakítás
{
	timer_cnt++;
	if((timer_cnt % 1) == 0) timer_task_10ms=1;
	if((timer_cnt % 10) == 0) timer_task_100ms=1;
	if((timer_cnt % 50) == 0) timer_task_500ms=1;
	if((timer_cnt % 100) == 0) timer_task_1s=1;
}

ISR(INT0_vect) //external interrupt
{
	if(PD0_re_enable_cnt == PD0_ENA_DELAY) //pergésmentesítés logika
	{
		PORTA ^=0x02;
		PD0_re_enable_cnt=0;
	}
	
}

ISR(ADC_vect) //AD megszakítás
{
	ad_value = ADC;
}

ISR(USART0_RX_vect) //UART adat fogadás megszakítás
{
	char c = UDR0;
	if(c == 0x7F)
		lcd_clear_display();
	else
		lcd_write_char(c);
}

ISR(CANIT_vect) //CAN megszakítás
{
	uint8_t i, dlc = 0;
	PORTA = PORTA ^ (1<<PA7);

	CANPAGE = 0;	// select MOb0, reset FIFO index

	if ( (CANSTMOB & (1<<RXOK)) != FALSE)	// Receive Complete
	{
		
		dlc = CANCDMOB & 0x0F;
		
		for (i=0; i<dlc; i++) can_rx_data[i] = CANMSG;
		
		CANSTMOB &= ~(1<<RXOK);	// clear RXOK flag
		CAN_ReceiveEnableMob(0, 0x00000001, FALSE, 8);	// enable next reception 
	}
}