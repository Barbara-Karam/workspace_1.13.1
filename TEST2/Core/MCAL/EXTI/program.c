/*
 * program.c
 *
 *  Created on: Sep 18, 2025
 *      Author: brbar
 */
#include "BIT_MATH.h"
#include "STD_TYPES.h"

#include "EXTI/EXTI_private.h"
#include "EXTI/EXTI_config.h"
#include "EXTI/EXTI_interface.h"


static void (*EXTI_CallBack_pf[16]) (void);

void EXTI_VidInitEdge( EXTILine Copy_EXTILine , EXTIMode Copy_u8EXTIMode ){

	switch(Copy_u8EXTIMode)
	{

	case RISING_EDGE:

		CLR_BIT( EXTI_FTSR , Copy_EXTILine );
		SET_BIT( EXTI_RTSR  , Copy_EXTILine );
		break;


	case FALLING_EDGE:
		CLR_BIT( EXTI_RTSR  , Copy_EXTILine );
		SET_BIT( EXTI_FTSR , Copy_EXTILine );

		break;


	case ON_CHANGE:

		SET_BIT( EXTI_RTSR  , Copy_EXTILine );
		SET_BIT( EXTI_FTSR , Copy_EXTILine );

		break;
	}

	EXTI_VidEnableEXTI(Copy_EXTILine );

}




void EXTI_VidEnableEXTI( uint8 LOC_u8EXTILine){

	SET_BIT( EXTI_IMR , LOC_u8EXTILine );

}



void EXTI_VidDisableEXTI( uint8 LOC_u8EXTILine){

	CLR_BIT( EXTI_IMR , LOC_u8EXTILine );

}

void EXTI_VidSoftWareTrigger( uint8 LOC_u8EXTILine){

	SET_BIT( EXTI_SWIER , LOC_u8EXTILine );

}
void EXTI_VidClearPendingFlag( uint8 LOC_u8EXTILine ){
	SET_BIT( EXTI_PR , LOC_u8EXTILine );
}


void EXTI_VoidSetCallBack(void (*ptr) (void),EXTILine Line)
{
	if (ptr != NULL){
		EXTI_CallBack_pf[Line]= ptr;

	}
	else
	{
		//error
	}
}
void EXTI0_IRQHandler(void){

	EXTI_CallBack_Pf[0]();
	EXTI_VidClearPendingFlag(EXTI_LINE0);

}
