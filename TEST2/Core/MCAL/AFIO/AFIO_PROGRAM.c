/*
 * AFIO_PROGRAM.c
 *
 *  Created on: Sep 23, 2025
 *      Author: brbar
 */
#include "STD_TYPES.h"
#include "AFIO_INTERFACE.h"
#include "AFIO_PRIVATE.h"
void AFIO_VidSetEXTIConfiguration( uint8 LOC_u8EXTI_Line , uint8 LOC_u8PortMap )
{
	if (LOC_u8EXTI_Line <= 3)
	{
	AFIO_EXTICR1 &= ~(0x000F << ((LOC_u8EXTI_Line - 0)*4));
	AFIO_EXTICR1 |= LOC_u8PortMap<<((LOC_u8EXTI_Line - 0)*4);
	}
	else if (LOC_u8EXTI_Line <= 7)
    {
	AFIO_EXTICR2 &= ~(0x000F << ((LOC_u8EXTI_Line - 0)*4));
	AFIO_EXTICR2 |= LOC_u8PortMap<<((LOC_u8EXTI_Line - 4)* 4 );
	}

	else if (LOC_u8EXTI_Line <= 11)
	{
	AFIO_EXTICR3 &= ~(0x000F << ((LOC_u8EXTI_Line - 0)*4));
	AFIO_EXTICR3 |= LOC_u8PortMap<<((LOC_u8EXTI_Line - 8)* 4 );
	}
	else if (LOC_u8EXTI_Line <= 15)
	{
		AFIO_EXTICR4 &= ~(0x000F << ((LOC_u8EXTI_Line - 0)*4));
		AFIO_EXTICR4 |= LOC_u8PortMap<<((LOC_u8EXTI_Line - 12)* 4 );
	}

	else
	{
		//error
	}
}
//	 *((volatile uint32 *)(AFIO_EXTICR1 + (0x04 * PORT ))) |= LOC_u8PortMap<<(LOC_u8EXTI_Line*4);
