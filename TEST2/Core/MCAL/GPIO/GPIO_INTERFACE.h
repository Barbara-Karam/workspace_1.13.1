/*******************************************************
 * Author  : Abdullah Waheed
 * File    : GPIO_INTERFACE.h
 *******************************************************/

#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include "GPIO_PRIVATE.h"

void GPIO_PinSetDir(PORTS	PORT , PINS PIN , DIR dir  ) ;

void GPIO_PinSetVal(PORTS PORT , PINS PIN , VAL val) ;

VAL GPIO_PinGetVal( PORTS PORT , PINS PIN ) ;

void GPIO_SetPortDir(PORTS PORT , DIR dir );

void GPIO_SetPortVal(PORTS PORT , uint16 val ) ;

#endif /* GPIO_INTERFACE_H */
