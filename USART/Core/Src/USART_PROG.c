/***************************************************************************************/
/****************************  IMT School Training Center ******************************/
/***************************************************************************************/
/** This file is developed by IMT School training center, All copyrights are reserved **/
/***************************************************************************************/

#include "STD_Types.h"
#include "BitMath.h"

#include "USART_INTERFACE.h"
#include "USART_CONFIG.h"
#include "USART_REG.h"

#define THRESHOLD_VALUE 9000000UL

void (*MUSART1_CallBack)(void);

void MUSART_VidInit(void)
{

// check if usart1 is enabled
#if USART1_STATUS == USART1_ENABLE
	MUSART1->SR = 0;
	// check for USART1 word length

	// check for USART1 parity

	// check for USART1 interrupt

	// check for USART1 transmitter and receiver

	// check for USART1 stop bits

	// set baud rate and update BRR register

	// enable USART1

#elif USART1_STATUS == USART1_DISABLE

#endif
}

void MUSART1_VidSendCharSync(u8 Copy_u8Char)
{
}

void MUSART1_VidSendStringSynch(u8 *Copy_ptrString)
{
}

u8 MUSART1_u8RecCharSynch(void)
{
}

u8 *MUSART1_PtrReadStringSynch(void)
{
}

void MUSART1_VidSetCallBack(void (*ptr)(void))
{
}

void USART1_IRQHandler(void)
{
}
