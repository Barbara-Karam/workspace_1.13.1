/*
 * RCC_program.c
 *
 *  Created on: Dec 15, 2024
 *      Author: user
 */


#include "STD_types.h"
#include "BitMath.h"

#include "RCC_interface.h"
#include "RCC_private.h"
#include "RCC_config.h"

void RCC_voidInitSysClock(void)
{
	#if     RCC_CLOCK_TYPE == RCC_HSE_CRYSTAL
		RCC_CR   = 0x00010000; /* Enable HSE with no bypass */
		RCC_CFGR = 0x00000001;

	#elif   RCC_CLOCK_TYPE == RCC_HSE_RC
		RCC_CR   = 0x00050000; /* Enable HSE with bypass */
		RCC_CFGR = 0x00000001;

	#elif   RCC_CLOCK_TYPE == RCC_HSI
		RCC_CR   = 0x00000081; /* Enable HSI + Trimming = 0 */
		RCC_CFGR = 0x00000000;

	#elif   RCC_CLOCK_TYPE == RCC_PLL
		#if   RCC_PLL_INPUT == RCC_PLL_IN_HSI_DIV_2

		#elif RCC_PLL_INPUT == RCC_PLL_IN_HSE_DIV_2

		#elif RCC_PLL_INPUT == RCC_PLL_IN_HSE

		#endif

	#else
		#error("You choose Wrong Clock type")
	#endif
}



void RCC_voidEnableClock(u8 BusId, u8 PerId)
{
	if (PerId <= 31)
	{
		switch (BusId)
		{
			case RCC_AHB  : SET_BIT(RCC_AHBENR  ,PerId);   break;
			case RCC_APB1 : SET_BIT(RCC_APB1ENR ,PerId);   break;
			case RCC_APB2 : SET_BIT(RCC_APB2ENR ,PerId);   break;
		}
	}

	else
	{
		/* Return Error */
	}

}


void RCC_voidDisableClock(u8 BusId, u8 PerId)
{
	if (PerId <= 31)
	{
		switch (BusId)
		{
			case RCC_AHB  : CLR_BIT(RCC_AHBENR  ,PerId);   break;
			case RCC_APB1 : CLR_BIT(RCC_APB1ENR ,PerId);   break;
			case RCC_APB2 : CLR_BIT(RCC_APB2ENR ,PerId);   break;
		}
	}

	else
	{
		/* Return Error */
	}

}

