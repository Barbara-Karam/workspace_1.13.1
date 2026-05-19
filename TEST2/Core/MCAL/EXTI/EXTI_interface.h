/*
 * EXTI_interface.h
 *
 *  Created on: Sep 20, 2025
 *      Author: brbar
 */

#ifndef MCAL_EXTI_INTERFACE_H_
#define MCAL_EXTI_INTERFACE_H_
//EXTI Mode options
typedef enum{
	RISING_EDGE,
	FALLING_EDGE,
	ON_CHANGE,
}EXTIMode;

//EXTI LINES
typedef enum{
	Line0,
	Line1,
	Line2,
	Line3,
	Line4,
	Line5,
	Line6,
	Line7,
	Line8,
	Line9,
	Line10,
	Line11,
	Line12,
	Line13,
	Line14,
	Line15,

}EXTILine;

#define PORTA           0
#define PORTB           1
#define PORTC           2

#define EXTI0IRQn     6
#define EXTI1IRQn     7
#define EXTI2IRQn     8
#define EXTI3IRQn     9
#define EXTI4IRQn     10

#define EXTI5IRQn     23
#define EXTI6IRQn     23
#define EXTI7IRQn     23
#define EXTI8IRQn     23
#define EXTI9IRQn     23

#define EXTI10IRQn    40
#define EXTI11IRQn    40
#define EXTI12IRQn    40
#define EXTI13IRQn    40
#define EXTI14IRQn    40
#define EXTI15IRQn    40


void EXTI_VidInitEdge( EXTILine Copy_EXTILine , EXTIMode Copy_u8EXTIMode );
void EXTI_VidEnableEXTI( uint8 LOC_u8EXTILine);
void EXTI_VidDisableEXTI( uint8 LOC_u8EXTILine);
void EXTI_VidSoftWareTrigger( uint8 LOC_u8EXTILine);
void EXTI_VidClearPendingFlag( uint8 LOC_u8EXTILine );


void EXTI_VoidSetCallBack(void (*ptr) (void),EXTILine Line);
void EXTI0_IRQHandler(void);

#endif /* MCAL_EXTI_EXTI_INTERFACE_H_ */
