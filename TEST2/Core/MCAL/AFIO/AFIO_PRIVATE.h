/*
 * AFIO_PRIVATE.h
 *
 *  Created on: Sep 23, 2025
 *      Author: brbar
 */

#ifndef AFIO_PRIVATE_H
#define AFIO_PRIVATE_H

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#define AFIO_Base_Address    0x40010000

 /*       REGISTERS ADDRESSES DEFINTION        */
#define AFIO_EVCR               *((uint32*)(AFIO_Base_Address  + 0x00))
#define AFIO_MAPR               *((uint32*)(AFIO_Base_Address  + 0x04))
#define AFIO_EXTICR1            *((uint32*)(AFIO_Base_Address  + 0x08))
#define AFIO_EXTICR2            *((uint32*)(AFIO_Base_Address  + 0x0C))
#define AFIO_EXTICR3            *((uint32*)(AFIO_Base_Address  + 0x10))
#define AFIO_EXTICR4            *((uint32*)(AFIO_Base_Address  + 0x14))
#define AFIO_MAPR2              *((uint32*)(AFIO_Base_Address  + 0x1C))

#endif /* MCAL_AFIO_AFIO_PRIVATE_H_ */
