/*
 * NVIC_interface.h
 *
 *  Created on: Sep 20, 2025
 *      Author: brbar
 */

#ifndef MCAL_NVIC_NVIC_INTERFACE_H_
#define MCAL_NVIC_NVIC_INTERFACE_H_

void MNVIC_VidEnableInterrupt       ( u8 Copy_u8INTID );

void MNVIC_VidDisableInterrupt      ( u8 Copy_u8INTID );

void MNVIC_VidSetPendingFlag        ( u8 Copy_u8INTID );

void MNVIC_VidClearPendingFlag      ( u8 Copy_u8INTID );

uint8   MNVIC_u8GetActiveFlag          ( u8 Copy_u8INTID );


#endif /* MCAL_NVIC_NVIC_INTERFACE_H_ */
