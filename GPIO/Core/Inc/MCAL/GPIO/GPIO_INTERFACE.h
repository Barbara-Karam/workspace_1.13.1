/*******************************************************
 * Author  : Abdullah Waheed
 * File    : GPIO_INTERFACE.h
 *******************************************************/

#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include "MCAL/GPIO/GPIO_PRIVATE.h"

void GPIO_SetPinDir(PORTS port, PINS pin, DIR dir);
void GPIO_WritePin(PORTS port, PINS pin, VALUE val);

Error_Value GPIO_readpin(PORTS port , PINS pin) ;



#endif /* GPIO_INTERFACE_H */
