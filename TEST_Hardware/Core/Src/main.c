/*
 ******************************************************************************
 * @file           : main.c
 * @author         : Abdullah waheed
 * @brief          : Main program body
 ******************************************************************************
 */

#include <stdint.h>
#include "RCC/RCC_INTERFACE.h"
#include "GPIO/GPIO_INTERFACE.h"

void Delay(volatile uint32_t count);

int main(void)
{
    RCC_Init();
    RCC_EnableClock(RCC_APB2, 2);   // GPIOA clock
    GPIO_SetPortDir(PORTA,GPIO_MODE_OUTPUT_PP_50MHZ);

    while (1)
    {
    	GPIO_SetPortVal(PORTA, 0xFFFF) ;
    	GPIO_SetPortVal(PORTB, 0xFFFF) ;
    	GPIO_PinSetVal(PORTC, PIN13, HIGH);
    	GPIO_PinSetVal(PORTC, PIN14, HIGH);
    	GPIO_PinSetVal(PORTC, PIN15, HIGH);

    }
}

void Delay(volatile uint32_t count)
{
	count*=75 ;
    while(count--)
    {
        __asm("NOP");   // no operation (just waste 1 cycle)
    }
}

