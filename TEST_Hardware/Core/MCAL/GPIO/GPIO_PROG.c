/*******************************************************
 * Author  : Abdullah Waheed
 * File    : GPIO_PROG.c
 *******************************************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "GPIO_PRIVATE.h"
#include "GPIO_CONFIG.h"
#include "GPIO_INTERFACE.h"

void GPIO_PinSetDir(PORTS	PORT , PINS PIN , DIR dir  )
{
	if ( PIN < PIN8 )
	{
		 *((volatile uint32 *)(GPIOA_BASE + GPIOX_CRL + (0X400 * PORT )))  &= ~(0XF << (PIN*4)) ;
           *((volatile uint32 *)(GPIOA_BASE + GPIOX_CRL + (0X400 * PORT )))  |= (dir << (PIN*4)) ;
	}
	else if (PIN >= 8 )
	{
		 *((volatile uint32 *)(GPIOA_BASE + GPIOX_CRH + (0X400 * PORT )))  &= ~(0XF << ((PIN-8)*4)) ;
          *((volatile uint32 *)(GPIOA_BASE + GPIOX_CRH + (0X400 * PORT )))  |= (dir << ((PIN-8)*4)) ;
	}
}

void GPIO_PinSetVal(PORTS PORT , PINS PIN , VAL val)
{
	if (val == HIGH)
	 *( (volatile uint32 *)(GPIOA_BASE + (PORT * 0X400) + GPIOX_ODR ))  |=(1<<PIN) ;
	else if ( val == LOW)
	 *( (volatile uint32 *)(GPIOA_BASE + (PORT * 0X400) + GPIOX_ODR ))  &=~(1<<PIN) ;

}

VAL GPIO_PinGetVal( PORTS PORT , PINS PIN )
{
	return GET_BIT(	 *((volatile uint32 *) (GPIOA_BASE + (PORT*0X400 ) + GPIOX_IDR ) ), PIN ) ;
}



void GPIO_SetPortDir(PORTS PORT , DIR dir )
{
	for(PINS PIN = PIN0 ; PIN <= PIN15 ; PIN++)
	{
		GPIO_PinSetDir(PORT , PIN , dir) ;
	}
}

void GPIO_SetPortVal(PORTS PORT , uint16 val )
{
	*( (volatile uint32 *)(GPIOA_BASE + (PORT * 0X400) + GPIOX_ODR ))  = val ;

}
