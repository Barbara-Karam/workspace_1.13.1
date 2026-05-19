/*
 * program.c
 *
 *  Created on: Sep 18, 2025
 *      Author: brbar
 */
#include "BIT_MATH.h"
#include "STD_TYPES.h"

#include "NVIC/NVIC_private.h"
#include "NVIC/NVIC_config.h"
#include "NVIC/NVIC_interface.h"


void MNVIC_VidEnableInterrupt       ( uint8 Copy_u8INTID )
{
	if (Copy_u8INTID <= 31)
	{
	SET_BIT(NVIC_ISER0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
	{
	SET_BIT(NVIC_ISER1,(Copy_u8INTID - 32));
	}

}

void MNVIC_VidDisableInterrupt       ( uint8 Copy_u8INTID )
{
	if (Copy_u8INTID <= 31)
	{
	SET_BIT(NVIC_ICER0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
	{
	SET_BIT(NVIC_ICER1,(Copy_u8INTID - 32));
	}

}

void MNVIC_VidSetPendingFlag       ( uint8 Copy_u8INTID )
{
	if (Copy_u8INTID <= 31)
	{
	SET_BIT(NVIC_ISPR0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
	{
	SET_BIT(NVIC_ISPR1,(Copy_u8INTID - 32));
	}
}

void MNVIC_VidClearPendingFlag       ( uint8 Copy_u8INTID )
{
	if (Copy_u8INTID <= 31)
	{
	SET_BIT(NVIC_ICPR0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
	{
	SET_BIT(NVIC_ICPR1,(Copy_u8INTID - 32));
	}

}

uint8 MNVIC_u8GetPendingFlag       ( uint8 Copy_u8INTID )
{
	uint8 Local_FlagValue = 0;
	if (Copy_u8INTID <= 31)
	{
		Local_FlagValue = GET_BIT(NVIC_ISPR0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
	{
		Local_FlagValue = GET_BIT(NVIC_ISPR1,(Copy_u8INTID - 32));
	}
	else
	{
		//warning
	}
	return Local_FlagValue;
}



uint8   MNVIC_u8GetActiveFlag          ( uint8 Copy_u8INTID )
{
	uint8 Local_FlagValue = 0;
	if (Copy_u8INTID <= 31)
		{
		Local_FlagValue = GET_BIT(NVIC_ISER0,Copy_u8INTID);
		}
		else if (Copy_u8INTID > 31 && Copy_u8INTID <= 59)
		{
		Local_FlagValue = GET_BIT(NVIC_ISER1,(Copy_u8INTID - 32));
		}
		else
		{
			//warning
		}
	return Local_FlagValue;
}

void	MNVIC_vidSetPriority(uint8 Copy_u8INTID, PRIORITY_MODE Copy_u8MODE, uint8 Copy_u8GroupPriority, uint8 Copy_u8SubPriority)
{
	//choose mode
	SCB_AIRCR |= Copy_u8MODE;
	//choose priority
    if(Copy_u8INTID >= 0)
     {
        NVIC_IPR0[Copy_u8INTID] = ((Copy_u8SubPriority | (Copy_u8GroupPriority << ((Copy_u8MODE - OPTION1_16G)/256)))<< 4);
     }
     else
     {
     	//error
     }
}
