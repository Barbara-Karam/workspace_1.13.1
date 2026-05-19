/*
 * AFIO_INTERFACE.h
 *
 *  Created on: Sep 23, 2025
 *      Author: brbar
 */

#ifndef MCAL_AFIO_AFIO_INTERFACE_H_
#define MCAL_AFIO_AFIO_INTERFACE_H_

//EXTI LINES
typedef enum{
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

//PORTS
#define AFIO_EXTI_PORTA  0b0000  // 0000 for GPIOA
#define AFIO_EXTI_PORTB  0b0001  // 0001 for GPIOB
#define AFIO_EXTI_PORTC  0b0010  // 0010 for GPIOC
#define AFIO_EXTI_PORTD  0b0011  // 0011 for GPIOD
#define AFIO_EXTI_PORTE  0b0100  // 0100 for GPIOE
#define AFIO_EXTI_PORTF  0b0101  // 0101 for GPIOF
#define AFIO_EXTI_PORTG  0b0110  // 0110 for GPIOG

void AFIO_VidSetEXTIConfiguration( uint8 LOC_u8EXTI_Line , uint8 LOC_u8PortMap );

#endif /* MCAL_AFIO_AFIO_INTERFACE_H_ */
