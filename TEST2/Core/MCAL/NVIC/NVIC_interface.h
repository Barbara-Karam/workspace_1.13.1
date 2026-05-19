/*
 * NVIC_interface.h
 *
 *  Created on: Sep 20, 2025
 *      Author: brbar
 */

#ifndef MCAL_NVIC_NVIC_INTERFACE_H_
#define MCAL_NVIC_NVIC_INTERFACE_H_

//options

//G is number of groups, S is number of subgroups
typedef enum
{
	OPTION1_16G = 0x5FA0300,
	OPTION2_8G_2S = 0x5FA0400,
	OPTION3_4G_4S = 0x5FA0500,
	OPTION4_2G_8S = 0x5FA0600,
	OPTION5_16S = 0x5FA0700,

}PRIORITY_MODE;


//functions

void MNVIC_VidEnableInterrupt       ( uint8 Copy_u8INTID );

void MNVIC_VidDisableInterrupt      ( uint8 Copy_u8INTID );

void MNVIC_VidSetPendingFlag        ( uint8 Copy_u8INTID );

void MNVIC_VidClearPendingFlag      ( uint8 Copy_u8INTID );

uint8 MNVIC_u8GetPendingFlag       ( uint8 Copy_u8INTID );

uint8 MNVIC_u8GetActiveFlag          ( uint8 Copy_u8INTID );

void  MNVIC_vidSetPriority(uint8 Copy_u8INTID, PRIORITY_MODE Copy_u8MODE, uint8 Copy_u8GroupPriority, uint8 Copy_u8SubPriority);




#endif /* MCAL_NVIC_NVIC_INTERFACE_H_ */
