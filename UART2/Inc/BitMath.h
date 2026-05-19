/*
 * BitMath.h
 *
 * Created: 13/8/2004 2:42:31 AM
 *  Author: Youssef Seyam
 */


#ifndef BITMATH_H_
#define BITMATH_H_

#define SET_BIT(reg,bit) reg |=(1<<(bit))
#define CLR_BIT(reg,bit) reg &=(~(1<<(bit)))
#define GET_BIT(reg,bit) ((reg>>bit) & 1 )
#define TGL_BIT(reg,bit) reg^=(1<<(bit))






#endif /* BITMATH_H_ */
