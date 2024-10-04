/******************************************************************************
 * Created: 1. edu. week
 * Author : Árpád Fehér
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>

#define F_CPU 8000000UL
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <math.h>

#include "lcd.h"
#include "uart.h"
#include "peripherals.h"

/******************************************************************************
* Macros
******************************************************************************/
#define FALSE 0
#define TRUE 1

#define OSSZEADAS 1
#define KIVONAS 2
#define SZORZAS 3
#define OSZTAS 4

#define SZAM1 1
#define SZAM2 2

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint16_t timer_cnt=0;
uint16_t time_0 = 0;
uint16_t time_1 = 0;
uint8_t task_1ms = FALSE, task_10ms = FALSE, task_100ms = FALSE, task_500ms = FALSE;
uint16_t ad_result;

uint8_t szam_1;
uint8_t szam_2;
uint8_t muveleti_jel = OSSZEADAS;
uint8_t szam_valaszto = SZAM1;
/******************************************************************************
* External Variables
******************************************************************************/

/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void write_voltage(uint16_t ad_value);
void write_pi(void);
void write_8bit(uint8_t num);
void write_hexa_num(uint16_t num);
/******************************************************************************
* Local Function Definitions
******************************************************************************/
/******************************************************************************
* Function:         void port_init(void)
* Description:      I/O portok inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void port_init(void)
{
	//LED-ek
	DDRA = 0xff;
	PORTA = 0xff;
	
	//LED-ek
	DDRF = (1<<PF1) | (1<<PF2);
	PORTF = (1<<PF1) | (1<<PF2);
	
	//kulso megszakitas
	DDRD = (0<<PD0);
	PORTD = (1<<PD0);
	
	//LCD
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
}

/******************************************************************************
* Function:         void write_voltage(uint16_t ad_value)
* Description:      feszültség kiírása az LCD kijelz?re
* Input:
* Output:
* Notes:
******************************************************************************/
void write_voltage(uint16_t ad_value)
{
	//1. feladat
	int voltage = ((uint32_t)ad_value * 5000) / 1024; // feszültség millivoltban
	char string_for_write_ad[50];
	sprintf(string_for_write_ad, "%d.%d", voltage/1000, voltage % 1000);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write_ad);
}

/******************************************************************************
* Function:         void write_pi(void)
* Description:      PI kiírása 6 tizedesjegyig
* Input:
* Output:
* Notes:
******************************************************************************/
void write_pi(void)
{
	char string_for_write_ad[50];
	uint8_t int_part = (uint8_t)M_PI;
	uint32_t float_part = (M_PI-int_part) * 1000000;
	
	sprintf(string_for_write_ad,"%d.%ld",int_part,float_part);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write_ad);
}
/******************************************************************************
* Function:         void write_8bit(uint8_t num)
* Description:      kiírás bináris formátumban
* Input:
* Output:
* Notes:
******************************************************************************/
void write_8bit(uint8_t num)
{
	lcd_set_cursor_position(0);
	for(uint8_t i = 0; i<8; i++)
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

/******************************************************************************
* Function:         void write_hexa_num(uint16_t num)
* Description:      kiírás hexadecimális formátumban
* Input:
* Output:
* Notes:
******************************************************************************/
void write_hexa_num(uint16_t num)
{
	char string_for_write_ad[50];
	sprintf(string_for_write_ad,"%X",num);
	lcd_set_cursor_position(0);
	lcd_write_string(string_for_write_ad);
}

/******************************************************************************
* Function:         int main(void)
* Description:      main függvény
* Input:
* Output:
* Notes:
******************************************************************************/
int main(void)
{
	port_init();
	timer_init();
	external_int_init();
	uart0_init(BAUD9600);
	lcd_init();
	adc_init();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		if(task_1ms)
		{
			
			task_1ms=FALSE;
		}
		if(task_10ms)
		{
			
			task_10ms=FALSE;
		}
		if(task_100ms)
		{
			
			////példa az ad érték lcd-re írására és uart-on pc felé küldésére
			//char string_for_write_ad[50];
			//sprintf(string_for_write_ad,"%4d",ad_result);
			//lcd_set_cursor_position(0);
			//lcd_write_string(string_for_write_ad);
			//uart_0_write_string(string_for_write_ad);
			//uart_0_write_string("\r\n");
			//ADCSRA |= (1<<ADSC); // új ad konverzió indítása
			
			//1. feladat
			//write_voltage(ad_result);
			//ADCSRA |= (1<<ADSC); // új ad konverzió indítása
			
			//2. feladat
			//write_pi();
			
			//3. feladat
			//write_8bit(0b01001001);
			
			//4. feladat
			//write_hexa_num(0x32FF);
			
			//6. feladat
			//char string_for_write_ad[50];
			//sprintf(string_for_write_ad,"%d %d %d",time_1-time_0, time_0, time_1);
			//lcd_set_cursor_position(0);
			//lcd_write_string(string_for_write_ad);
			
			//8. feladat
			char string_for_write[50];
			switch(muveleti_jel)
			{
				case OSSZEADAS:
				{
					sprintf(string_for_write,"%3d+%3d=%3d",szam_1,szam_2,szam_1+szam_2);
					break;
				}
				case KIVONAS:
				{
					
					break;
				}
				case SZORZAS:
				{
					
					break;
				}
				case OSZTAS:
				{
					
					break;
				}
			}
			
			lcd_set_cursor_position(0);
			lcd_write_string(string_for_write);
			
			switch(szam_valaszto)
			{
				case SZAM1:
				{
					szam_1 = ad_result/4;
					break;
				}
				case SZAM2:
				{
					szam_2 = ad_result/4;
					break;
				}
			}
			
			ADCSRA |= (1<<ADSC); // új ad konverzió indítása
			
			
			
			PORTF ^=(1<<PF1); //LED villogtatása
			task_100ms=FALSE;
		}
		if(task_500ms)
		{
			
			PORTF ^=(1<<PF2);//LED villogtatása
			task_500ms=FALSE;
		}
    }
}

/******************************************************************************
* Interrupt Routines
******************************************************************************/
ISR(TIMER0_COMP_vect) //timer CTC interrupt
{
	timer_cnt++;
	task_1ms = TRUE;
	if(timer_cnt % 10 == 0) task_10ms = TRUE;
	if(timer_cnt % 100 == 0) task_100ms = TRUE;
	if(timer_cnt % 500 == 0) task_500ms =TRUE;
}

ISR(INT0_vect) //extint 0 interrput
{
	//6. feladat
	//if(time_0 != 0 && time_1 == 0) time_1 = timer_cnt;
	//if(time_0 == 0) time_0 = timer_cnt;
	szam_valaszto++;
	if(szam_valaszto>SZAM2) szam_valaszto = SZAM1;
	
	
	_delay_ms(20);
	
	PORTA = PORTA ^ 0x01; //
}

ISR(USART0_RX_vect)
{
	char c = UDR0;
	lcd_write_char(c);
}

ISR(ADC_vect)
{
	ad_result = ADC;
}