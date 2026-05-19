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
#include "AFIO/AFIO_INTERFACE.h"
#include "NVIC/NVIC_interface.h"
#include "EXTI/EXTI_interface.h"

void Delay(volatile uint32_t count);
void Led_toggle(void)
{
    // Toggle LED on PC13
    if(MGPIO_u8GetPinValue(GPIOC , PIN13) == HIGH)
    {
    	MGPIO_VidSetPinValue(GPIOC , PIN13, LOW);
    }
    else {
    	MGPIO_VidSetPinValue(GPIOC , PIN13, HIGH);
    }

}
int main(void)
{
	//RCC
    RCC_Init();

    RCC_EnableClock(RCC_APB2, 0);   // AFIO clock
    RCC_EnableClock(RCC_APB2, 2);   // GPIOA clock
    RCC_EnableClock(RCC_APB2, 4);   //GPIOC

    //GPIO
    GPIO_PinSetDir(PORTA,PIN0,GPIO_MODE_INPUT_PU_PD);
    GPIO_PinSetDir(PORTC,PIN13,GPIO_MODE_OUTPUT_PP_50MHZ);

    //AFIO
    AFIO_VidSetEXTIConfiguration( Line0 , PORTA );

    //EXTI
    EXTI_VidInitEdge( Line0 , FALLING_EDGE );
    EXTI_VoidSetCallBack( Led_toggle ,Line0);

    //NVIC
    __enable_irq();
    MNVIC_VidEnableInterrupt(EXTI0IRQn);


    while (1)
    {

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

