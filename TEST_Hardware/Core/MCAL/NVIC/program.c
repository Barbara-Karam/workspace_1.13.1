/*
 * program.c
 *
 *  Created on: Sep 18, 2025
 *      Author: brbar
 */
#include "LIB/BIT_MATH.h"
#include "LIB/STD_TYPES.h"

#include "MCAL/NVIC/NVIC_private.h"
#include "MCAL/NVIC/NVIC_config.h"
#include "MCAL/NVIC/NVIC_interface.h"


void MNVIC_VidEnableInterrupt       ( u8 Copy_u8INTID )
{
	if (Copy_u8INTID <= 31)
	{
	SET_BIT(NVIC_ISER0,Copy_u8INTID);
	}
	else if (Copy_u8INTID > 31 && Copy_u8INTID <= 63)
	{
	SET_BIT(NVIC_ISER1,(Copy_u8INTID - 32));
	}
	else if (Copy_u8INTID > 63 && Copy_u8INTID <= 67)
	{
	SET_BIT(NVIC_ISER2,(Copy_u8INTID - 64));
	}
	else
	{
		//warning
	}
}

uint8   MNVIC_u8GetActiveFlag          ( u8 Copy_u8INTID )
{
	uint8 Local_FlagValue = 0;
	if (Copy_u8INTID <= 31)
		{
		Local_FlagValue = GET_BIT(NVIC_ISER0,Copy_u8INTID);
		}
		else if (Copy_u8INTID > 31 && Copy_u8INTID <= 63)
		{
		Local_FlagValue = GET_BIT(NVIC_ISER1,(Copy_u8INTID - 32));
		}
		else if (Copy_u8INTID > 63 && Copy_u8INTID <= 67)
		{
		Local_FlagValue = GET_BIT(NVIC_ISER2,(Copy_u8INTID - 64));
		}
		else
		{
			//warning
		}
	return Local_FlagValue;
}
