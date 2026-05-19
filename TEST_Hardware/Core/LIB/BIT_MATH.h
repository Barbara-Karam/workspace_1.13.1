/*
 * BIT_MATHS.h
 *
 *  Created on: Sep 15, 2025
 *      Author: brbar
 */

#ifndef LIB_BIT_MATH_H_
#define LIB_BIT_MATH_H_

// set bit to 1
#define SET_BIT(REG,BIT)  ( REG |=  (1 << BIT))
// clear bit or make it 0
#define CLR_BIT(REG,BIT)  ( REG &= ~(1 << BIT))
// toggle bit : if 0 -> 1, if 1 -> 0
#define TOG_BIT(REG,BIT)  ( REG ^=  (1 << BIT))
// read bit
#define GET_BIT(REG,BIT)  ((REG >> BIT) & 0x01U)



#endif /* LIB_BIT_MATH_H_ */
