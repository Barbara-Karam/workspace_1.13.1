#ifndef NVIC_PRIVATE_H
#define NVIC_PRIVATE_H

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#define NVIC_Base_Address           0xE000E100
#define SCB_Base_Address            0xE000ED00
  /*       REGISTERS ADDRESSES DEFINTION        */

#define NVIC_ISER0          *((uint32*)(NVIC_Base_Address  + 0x000))
#define NVIC_ISER1          *((uint32*)(NVIC_Base_Address  + 0x004))
#define NVIC_ISER2          *((uint32*)(NVIC_Base_Address  + 0x008))


#define NVIC_ICER0          *((uint32*)(NVIC_Base_Address  + 0x080))
#define NVIC_ICER1          *((uint32*)(NVIC_Base_Address  + 0x084))
#define NVIC_ICER2          *((uint32*)(NVIC_Base_Address  + 0x088))


#define NVIC_ISPR0          *((uint32*)(NVIC_Base_Address  + 0x100))
#define NVIC_ISPR1          *((uint32*)(NVIC_Base_Address  + 0x104))
#define NVIC_ISPR2          *((uint32*)(NVIC_Base_Address  + 0x108))


#define NVIC_ICPR0          *((uint32*)(NVIC_Base_Address  + 0x180))
#define NVIC_ICPR1          *((uint32*)(NVIC_Base_Address  + 0x184))
#define NVIC_ICPR2          *((uint32*)(NVIC_Base_Address  + 0x188))


#define NVIC_IABR0          *((volatile uint32*)(NVIC_Base_Address  + 0x200))
#define NVIC_IABR1          *((volatile uint32*)(NVIC_Base_Address  + 0x204))
#define NVIC_IABR2          *((volatile uint32*)(NVIC_Base_Address  + 0x208))



/* i made a pointer to make it easy to transfer through different interrupts and assigning priority for each */
// pointer of type uint8 because each priority register. is 8 bits(1byte)
#define NVIC_IPR0           ((uint8*)(NVIC_Base_Address  + 0x300))

#define SCB_AIRCR           *((uint32*)(SCB_Base_Address  + 0x0C))


#endif /* NVIC_PRIVATE_H_ */
