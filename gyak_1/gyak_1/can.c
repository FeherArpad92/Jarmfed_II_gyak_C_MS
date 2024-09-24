/******************************************************************************
* Include files
******************************************************************************/
#include "can.h"
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
* Function:         void can_init(void)
* Description:      CAN periféria inicializálása
* Input:
* Output:
* Notes:
******************************************************************************/
void can_init(void)
{
	CANGCON = (1<<ENASTB); //Enable CAN peripheral
	CANGIE = (1<<ENRX); //Enable receive interrupt
	CANIE2 = (1<<IEMOB0); //Enable MOB0 interrputs
	
	CANBT1 = 0x02; // Set CAN baud rate to 500K, based on datasheet page 268. (Table 19.2)
	CANBT2 = 0x04;
	CANBT3 = 0x13;
	
	for(uint8_t i=0; i<15; i++)
	{
		CANPAGE = i << 4;	// Set initial value to all MOb registers
		CANSTMOB = 0; // CAN MOb Status Register
		CANCDMOB = 0; // CAN MOb Control and DLC Register
		CANIDT4 = 0; // CAN Identifier Tag Registers 4,3,2,1
		CANIDT3 = 0; 
		CANIDT2 = 0; 
		CANIDT1 = 0; 
		CANIDM4 = 0; // CAN Identifier Mask Registers 4,3,2,1
		CANIDM3 = 0; 
		CANIDM2 = 0;
		CANIDM1 = 0;
		CANSTMH = 0; // CAN Time Stamp Registers 
		CANSTML = 0;
		CANMSG = 0; //CAN Data Message Register
	}
	CANGIE |= (1<<ENIT); // Enable all CAN interrupts
}

/******************************************************************************
* Function:			void CAN_ReceiveEnableMob(uint8_t page, uint32_t id, uint8_t dlc)
* Description:		MOB inicializálása fogadásra
* Input:
* Output:
* Notes:
******************************************************************************/
void CAN_ReceiveEnableMob(uint8_t page, uint32_t id, uint8_t extended, uint8_t dlc)
{

	CANPAGE = page << 4;  // MOb0
	CANSTMOB = 0x00;	  // Clear status flags

	CANIDM4 = 0xFF;		  // Full filtering
	CANIDM3 = 0xFF;
	CANIDM2 = 0xFF;
	CANIDM1 = 0xFF;

	if(extended)
	{
		CANIDT4 = id << 3;
		CANIDT3 = id >> 5;
		CANIDT2 = id >> 13;
		CANIDT1 = id >> 21;
	}
	else
	{
		CANIDT2 = id << 5;
		CANIDT1 = id >> 3;
	}

	CANCDMOB = (1 << CONMOB1) | (extended<<IDE) | dlc; // Enable Rx, extended ID, set DLC to 8
}

/******************************************************************************
* Function:			void CAN_SendMob(uint8_t page, uint32_t id, uint8_t extended, uint8_t dlc)
* Description:		Transmit data frame on MOb
* Input:			
* Output:
* Notes:
******************************************************************************/
void CAN_SendMob(uint8_t page, uint32_t id, uint8_t extended, uint8_t dlc, uint8_t *data)
{
	CANPAGE = page << 4;   // MOb1
	CANSTMOB = 0x00;	// Clear status flags

	if(extended)
	{
		CANIDT4 = id << 3;
		CANIDT3 = id >> 5;
		CANIDT2 = id >> 13;
		CANIDT1 = id >> 21;
	}
	else
	{
		CANIDT2 = id << 5;
		CANIDT1 = id >> 3;
	}
	
	
	CANCDMOB |= (extended<<IDE); //Extended ID

	for (uint8_t i = 0; i < dlc; i++) CANMSG = *(data+i);
	
	CANCDMOB = (1 << CONMOB0) | dlc; // Enable Tx, set DLC to 8

}