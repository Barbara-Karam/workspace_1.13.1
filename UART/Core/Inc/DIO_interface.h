/*
 * DIO_interface.h
 *
 *  Created on: Dec 15, 2024
 *      Author: user
 */

#ifndef DIO_INTERFACE_H
#define DIO_INTERFACE_H
#include "STD_types.h"

#define HIGH    1
#define LOW     0

#define GPIOA   0
#define GPIOB   1
#define GPIOC   2
#define GPIOD   3
#define GPIOE   4
#define GPIOF   5
#define GPIOG   6

#define PIN0    0
#define PIN1    1
#define PIN2    2
#define PIN3    3
#define PIN4    4
#define PIN5    5
#define PIN6    6
#define PIN7    7
#define PIN8    8
#define PIN9    9
#define PIN10    10
#define PIN11    11
#define PIN12    12
#define PIN13    13
#define PIN14    14
#define PIN15    15

#define INPUT_ANALOG              0b0000
#define INPUT_FLOATING            0b0100
#define INPUT_PULLUP_PULLDOWN     0b1000

//For Speed 10
#define OUTPUT_SPEED_10MHZ_PP     0b0001
#define OUTPUT_SPEED_10MHZ_OD     0b0101
#define OUTPUT_SPEED_10MHZ_AFPP   0b1001
#define OUTPUT_SPEED_10MHZ_AFOD   0b1101

//For Speed 2
#define OUTPUT_SPEED_2MHZ_PP      0b0010
#define OUTPUT_SPEED_2MHZ_OD      0b0110
#define OUTPUT_SPEED_2MHZ_AFPP    0b1010
#define OUTPUT_SPEED_2MHZ_AFOD    0b1110

//For Speed 2
#define OUTPUT_SPEED_50MHZ_PP     0b0011
#define OUTPUT_SPEED_50MHZ_OD     0b0111
#define OUTPUT_SPEED_50MHZ_AFPP   0b1011
#define OUTPUT_SPEED_50MHZ_AFOD   0b1111

void DIO_voidSetPinDirection (u8 Copy_u8PORT , u8 Copy_u8PIN , u8 Copy_u8MODE);

void DIO_voidSetPortDirection (u8 Copy_u8PORT , u8 Copy_u8MODE );

void DIO_voidSetPortValue (u8 Copy_u8PORT , u8 Copy_u8VALUE);

void DIO_voidSetPinValue (u8 Copy_u8PORT , u8 Copy_u8PIN , u8 Copy_u8VALUE);

u8 DIO_u8GetPortValue(u8 Copy_u8PORT);

u8 DIO_u8GetPinValue (u8 Copy_u8PORT , u8 Copy_u8PIN );

void DIO_voidTogglePinValue (u8 Copy_u8PORT , u8 Copy_u8PIN );





#endif /* DIO_INTERFACE_H_ */
