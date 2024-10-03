/******************************************************************************
 * Created: 1. edu. week
 * Author : Árpád Fehér
******************************************************************************/
 /******************************************************************************
* Include files
******************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#define F_CPU 8000000UL 
#include <avr/delay.h>

#include <inttypes.h>
#include "lcd.h"
#include "uart.h"
#include "peripherals.h"

/******************************************************************************
* Macros
******************************************************************************/
#define TRUE 1
#define FALSE 0

#define OSSZEADAS 1
#define KIVONAS 2
#define SZORZAS 3
#define OSZTAS 4

#define SZAM1 0
#define SZAM2 1

/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Global Variables
******************************************************************************/
uint8_t task_1ms=FALSE, task_10ms =FALSE, task_100ms=FALSE, task_500ms=FALSE;
uint16_t timer_cnt=0;
uint16_t ad_result;
uint8_t pd0_pushed = FALSE;
uint16_t time_1=0;
uint16_t time_2=0;
uint8_t szam1=0, szam2=0;
uint8_t muveleti_jel=OSSZEADAS;
uint8_t szam_valaszto = SZAM1;
/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
* Local Function Declarations
******************************************************************************/
void port_init(void);
void print_voltage(uint16_t ad_r);



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
	DDRA = 0xff;
	PORTA = 0xff;
	
	DDRF = (1<<PF3) | (1<<PF2) | (1<<PF1) | (0<<PF0);
	//PORTF = 0x0f;
	
	DDRD = (0<<PD0) | (0<<PD1);
	PORTD = (1<<PD0) | (1<<PD1);
	
	DDRC = (1<<LCD_E) | (1<<LCD_RS) | (1<<LCD_D7) | (1<<LCD_D6) | (1<<LCD_D5) | (1<<LCD_D4);
	PORTC = (0<<LCD_E) | (0<<LCD_RS) | (0<<LCD_D7) | (0<<LCD_D6) | (0<<LCD_D5) | (0<<LCD_D4);
}

void print_voltage(uint16_t ad_r)
{
	char write_string[50];
	int voltage = ((uint32_t)ad_r*5000) / 1023;
	sprintf(write_string,"%d.%03d",voltage/1000, voltage % 1000);
	lcd_set_cursor_position(0);
	lcd_write_string(write_string);
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
	lcd_init();
	uart0_init(BAUD9600);
	adc_init();
	sei();
    /* Replace with your application code */
    while (1)
    {
		if(task_1ms == TRUE)
		{

			task_1ms = FALSE;
		}
		
		if(task_10ms == TRUE)
		{
			//PORTF ^= 0x01;
			task_10ms = FALSE;
		}
		
		if(task_100ms == TRUE)
		{
			//6. feladat 
			//if(pd0_pushed)
			//{
				//if(time_1!=0 && time_2==0) time_2=timer_cnt;
				//if(time_1==0) time_1=timer_cnt;
				//PORTA ^=0x01;
				//pd0_pushed=FALSE;
			//}
			//
			////6. feladat
			//char write_string[50];
			//sprintf(write_string,"%d",time_2-time_1);
			//lcd_set_cursor_position(0);
			//lcd_write_string(write_string);
			
			//ADCSRA |= (1<<ADSC);
			//PORTA = ad_result;
			//PORTF ^= 0X02;
			//char write_string[50];
			//sprintf(write_string,"%4d",ad_result);
			//lcd_set_cursor_position(0);
			//lcd_write_string(write_string);
			//uart_write_string(write_string);
			//uart_write_string("\r\n");
			
			//1. feladat
			
			//print_voltage(ad_result);
			//ADCSRA |= (1<<ADSC);
			
			//2. feladat
			//char write_string[50];
			//uint8_t int_part = (uint8_t)M_PI;
			//uint32_t float_part = (M_PI-int_part)*1000000;
			//sprintf(write_string,"%d.%ld",int_part,float_part);
			//lcd_set_cursor_position(0);
			//lcd_write_string(write_string);
			
			//3. feladat
			//lcd_set_cursor_position(0);
			//uint8_t valtozo = 0b01011011;
			//for(uint8_t i=0; i<8; i++)
			//{
				//if(valtozo & (1<<(7-i))
				//{
					//lcd_write_char('1');
				//}
				//else
				//{
					//lcd_write_char('0');
				//}
			//}
			
			//4. feladat
			//char write_string[50];
			//uint16_t hexa_valtozo = 0x2FA2;
			//sprintf(write_string,"%x",hexa_valtozo);
			//lcd_set_cursor_position(0);
			//lcd_write_string(write_string);
			
			
			//8. feladat
			if(!(PIND & (1<<PD1)))
			{
				szam_valaszto++;
				if(szam_valaszto>1) szam_valaszto=0;
			}
			
			char write_string[50];
			
			switch(szam_valaszto)
			{
				case SZAM1:
				{
					szam1=ad_result/4;
					break;
				}
				case SZAM2:
				{
					szam2=ad_result/4;
					break;
				}
			}
			
			
			
			switch(muveleti_jel)
			{
				case OSSZEADAS:
				{
					sprintf(write_string,"%3d+%3d=%3d",szam1,szam2,szam1+szam2);
					break;
				}
				case KIVONAS:
				{
					sprintf(write_string,"%3d-%3d=%3d",szam1,szam2,szam1-szam2);
					break;
				}
				case SZORZAS:
				{
					sprintf(write_string,"%3d*%3d=%5d",szam1,szam2,szam1*szam2);
					break;
				}
				case OSZTAS:
				{
					float eredmeny = szam1/szam2;
					
					break;
				}
			}
			
			
			
			
			lcd_set_cursor_position(0);
			lcd_write_string(write_string);
			
			if(pd0_pushed)
			{
				muveleti_jel++;
				if(muveleti_jel>4) muveleti_jel =1;
				pd0_pushed=FALSE;
			}
			
			ADCSRA |= (1<<ADSC);
			
			
			
			
			
			
			task_100ms = FALSE;
		}
		
		if(task_500ms == TRUE)
		{
			//char szoveg[] = "szoveg a kijelzon";
			//lcd_write_string(szoveg);
			//lcd_set_cursor_position(0);
			//lcd_clear_display();
			//lcd_write_string("masik szoveg");
			
			PORTF ^= 0x04;
			task_500ms = FALSE;
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
	if((timer_cnt % 10) == 0) task_10ms = TRUE;
	if((timer_cnt % 100) == 0) task_100ms =TRUE;
	if((timer_cnt % 500) == 0) task_500ms = TRUE;
}

ISR(INT0_vect) //ext 0 interrupt
{
	//_delay_ms(40);
	pd0_pushed = TRUE;
	//PORTA = PORTA ^ 0x01;
}

ISR(USART0_RX_vect)
{
	char c = UDR0;
	if(c == 0x7F)
		lcd_clear_display();
	else
		lcd_write_char(c);
}

ISR(ADC_vect)
{
	ad_result = ADC;
}
