/*******************************************************
 * Author  : Abdullah Waheed
 * File    : GPIO_PROG.c
 *******************************************************/
#include "LIBS/STD_TYPES.h"
#include "LIBS/BIT_MATH.h"
#include "GPIO_PRIVATE.h"
#include "GPIO_CONFIG.h"
#include "GPIO_INTERFACE.h"


void GPIO_SetPinDir(PORTS port, PINS pin, DIR dir)
{
	if ( pin < 8 )
	{
	   (*(volatile uint32*) (GPIOA_BASE + (0X400 * port) + GPIOX_CRL )) |= (dir << (pin*4)) ;
	}

	else if ( pin >= 8 )
	{
		   (*(volatile uint32*) (GPIOA_BASE + (0X400 * port) + GPIOX_CRH )) |= (dir << ((pin-8)*4)) ;

	}
}


void GPIO_WritePin(PORTS port, PINS pin, VALUE val)
{

	if ( val == HIGH )
	SET_BIT((*(volatile uint32*) (GPIOA_BASE + (0X400 * port) + GPIOX_ODR )), pin) ;
	else if ( val == LOW )
	CLR_BIT((*(volatile uint32*) (GPIOA_BASE + (0X400 * port) + GPIOX_ODR )),pin);

}

VALUE GPIO_readpin(PORTS port , PINS pin)
{
	 return  ( (  (*(volatile uint32*) (GPIOA_BASE + (0X400 * port) + GPIOX_IDR )) >> pin ) & 1 ) ;

}


