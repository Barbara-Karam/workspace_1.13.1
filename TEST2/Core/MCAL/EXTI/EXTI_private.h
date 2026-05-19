#ifndef EXTI_PRIVATE_H
#define EXTI_PRIVATE_H

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#define EXTI_Base_Address   0x40010400
 /*       REGISTERS ADDRESSES DEFINTION        */

#define EXTI_IMR            *((uint32*)(EXTI_Base_Address  + 0x00))
#define EXTI_EMR            *((uint32*)(EXTI_Base_Address  + 0x04))
#define EXTI_RTSR           *((uint32*)(EXTI_Base_Address  + 0x08))
#define EXTI_FTSR           *((uint32*)(EXTI_Base_Address  + 0x0C))
#define EXTI_SWIER          *((uint32*)(EXTI_Base_Address  + 0x10))
#define EXTI_PR             *((uint32*)(EXTI_Base_Address  + 0x14))

#endif /* EXTI_PRIVATE_H_ */
