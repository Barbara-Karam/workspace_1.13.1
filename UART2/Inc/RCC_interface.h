/*
 * RCC_interface.h
 *
 *  Created on: Dec 15, 2024
 *      Author: user
 */

#ifndef RCC_INTERFACE_H
#define RCC_INTERFACE_H

#define RCC_AHB       0
#define RCC_APB1      1
#define RCC_APB2      2

void RCC_voidInitSysClock(void);
void RCC_voidEnableClock(u8 BusId, u8 PerId);
void RCC_voidDisableClock(u8 BusId, u8 PerId);

#endif /* RCC_INTERFACE_H_ */
