/*
 * DIO_program.c
 *
 *  Created on: Dec 15, 2024
 *      Author: user
 */
#include "BitMath.h"
#include "STD_types.h"
#include "DIO_interface.h"
#include "DIO_private.h"
#include "DIO_config.h"

void DIO_voidSetPortDirection(u8 Copy_u8PORT, u8 Copy_u8MODE) {
    // Arrays to store the control register pointers
    volatile u32* GPIOx_CRL;
    volatile u32* GPIOx_CRH;

    // Select the appropriate GPIO port control registers
    switch (Copy_u8PORT) {
        case GPIOA: GPIOx_CRL = &GPIOA_CRL; GPIOx_CRH = &GPIOA_CRH; break;
        case GPIOB: GPIOx_CRL = &GPIOB_CRL; GPIOx_CRH = &GPIOB_CRH; break;
        case GPIOC: GPIOx_CRL = &GPIOC_CRL; GPIOx_CRH = &GPIOC_CRH; break;
        case GPIOD: GPIOx_CRL = &GPIOD_CRL; GPIOx_CRH = &GPIOD_CRH; break;
        case GPIOE: GPIOx_CRL = &GPIOE_CRL; GPIOx_CRH = &GPIOE_CRH; break;
        case GPIOF: GPIOx_CRL = &GPIOF_CRL; GPIOx_CRH = &GPIOF_CRH; break;
        case GPIOG: GPIOx_CRL = &GPIOG_CRL; GPIOx_CRH = &GPIOG_CRH; break;
        default: return; // Handle invalid port case, if needed
    }

    // Set the mode for the lower 8 pins (CRL register)
    for (u8 i = 0; i < 8; i++) {
        *GPIOx_CRL &= ~(0xF << (i * 4));     // Clear the 4 bits for the pin
        *GPIOx_CRL |= (Copy_u8MODE << (i * 4)); // Set the mode
    }

    // Set the mode for the upper 8 pins (CRH register)
    for (u8 i = 0; i < 8; i++) {
        *GPIOx_CRH &= ~(0xF << (i * 4));     // Clear the 4 bits for the pin
        *GPIOx_CRH |= (Copy_u8MODE << (i * 4)); // Set the mode
    }
}

void DIO_voidSetPinDirection (u8 Copy_u8PORT , u8 Copy_u8PIN , u8 Copy_u8MODE){
	switch(Copy_u8PORT){
	case GPIOA:
		if (Copy_u8PIN <8)
		{
			GPIOA_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOA_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOA_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOA_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOB:
		if (Copy_u8PIN < 8)
		{
			GPIOB_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOB_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <=15)
		{
			Copy_u8PIN -= 8;
			GPIOB_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOB_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOC:
		if (Copy_u8PIN < 8)
		{
			GPIOC_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOC_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOC_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOC_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOD:
		if (Copy_u8PIN < 8)
		{
			GPIOD_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOD_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOD_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOD_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOE:
		if (Copy_u8PIN < 8)
		{
			GPIOE_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOE_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOE_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOE_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOF:
		if (Copy_u8PIN < 8)
		{
			GPIOF_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOF_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOF_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOF_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;

	case GPIOG:
		if (Copy_u8PIN < 8)
		{
			GPIOG_CRL  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOG_CRL  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		else if (Copy_u8PIN <16)
		{
			Copy_u8PIN -= 8;
			GPIOG_CRH  &= ~(( 0b1111 )  << ( Copy_u8PIN * 4 ));
			GPIOG_CRH  |= (Copy_u8MODE) << ( Copy_u8PIN * 4 );

		}
		break;
	default: break;





	}
}

void DIO_voidSetPortValue(u8 Copy_u8PORT, u8 Copy_u8VALUE) {
    switch (Copy_u8PORT) {
        case GPIOA:
            if (Copy_u8VALUE) {
                GPIOA_BSRR = 0x0000FFFF;
            } else {
                GPIOA_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOB:
            if (Copy_u8VALUE) {
                GPIOB_BSRR = 0x0000FFFF;
            } else {
                GPIOB_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOC:
            if (Copy_u8VALUE) {
                GPIOC_BSRR = 0x0000FFFF;
            } else {
                GPIOC_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOD:
            if (Copy_u8VALUE) {
                GPIOD_BSRR = 0x0000FFFF;
            } else {
                GPIOD_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOE:
            if (Copy_u8VALUE) {
                GPIOE_BSRR = 0x0000FFFF;
            } else {
                GPIOE_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOF:
            if (Copy_u8VALUE) {
                GPIOF_BSRR = 0x0000FFFF;
            } else {
                GPIOF_BSRR = 0xFFFF0000;
            }
            break;
        case GPIOG:
            if (Copy_u8VALUE) {
                GPIOG_BSRR = 0x0000FFFF;
            } else {
                GPIOG_BSRR = 0xFFFF0000;
            }
            break;
        default: break;
    }
}
/* why this not working ?
 *  if (Copy_u8VALUE == 1)
 	 switch (Copy_u8PORT){
 	  case GPIOA: GPIOA_BSRR = 0xFFFF0000; break;
 	  case GPIOB: GPIOB_BSRR = 0xFFFF0000; break;
 	  case GPIOC: GPIOC_BSRR = 0xFFFF0000; break;
 	  case GPIOD: GPIOD_BSRR = 0xFFFF0000; break;
 	  case GPIOE: GPIOE_BSRR = 0xFFFF0000; break;
 	  case GPIOF: GPIOF_BSRR = 0xFFFF0000; break;
 	  case GPIOG: GPIOG_BSRR = 0xFFFF0000; break;
 	  default: break;
 	  }
 	 }
	else if (Copy_u8VALUE == 0) {
	 switch (Copy_u8PORT){
	  case GPIOA: GPIOA_BSRR = 0x0000FFFF; break;
	  case GPIOB: GPIOB_BSRR = 0x0000FFFF; break;
	  case GPIOC: GPIOC_BSRR = 0x0000FFFF; break;
	  case GPIOD: GPIOD_BSRR = 0x0000FFFF; break;
	  case GPIOE: GPIOE_BSRR = 0x0000FFFF; break;
	  case GPIOF: GPIOF_BSRR = 0x0000FFFF; break;
	  case GPIOG: GPIOG_BSRR = 0x0000FFFF; break;
	  default: break;
	 }
	}
 *
 *
 */
void DIO_voidSetPinValue (u8 Copy_u8PORT , u8 Copy_u8PIN , u8 Copy_u8VALUE){
	if(Copy_u8VALUE == 1){
		switch(Copy_u8PORT){
			case GPIOA:	SET_BIT(GPIOA_ODR,Copy_u8PIN); 	break;
			case GPIOB:	SET_BIT(GPIOB_ODR,Copy_u8PIN);	break;
			case GPIOC:	SET_BIT(GPIOC_ODR,Copy_u8PIN);	break;
			case GPIOD:	SET_BIT(GPIOD_ODR,Copy_u8PIN);	break;
			case GPIOE:	SET_BIT(GPIOE_ODR,Copy_u8PIN);	break;
			case GPIOF:	SET_BIT(GPIOF_ODR,Copy_u8PIN);	break;
			case GPIOG:	SET_BIT(GPIOG_ODR,Copy_u8PIN);	break;
			default: break;

		}
	}
	else if (Copy_u8VALUE == 0){
		switch(Copy_u8PORT){
			case GPIOA:	CLR_BIT(GPIOA_ODR,Copy_u8PIN); 	break;
			case GPIOB:	CLR_BIT(GPIOB_ODR,Copy_u8PIN);	break;
			case GPIOC:	CLR_BIT(GPIOC_ODR,Copy_u8PIN);	break;
			case GPIOD:	CLR_BIT(GPIOD_ODR,Copy_u8PIN);	break;
			case GPIOE:	CLR_BIT(GPIOE_ODR,Copy_u8PIN);	break;
			case GPIOF:	CLR_BIT(GPIOF_ODR,Copy_u8PIN);	break;
			case GPIOG:	CLR_BIT(GPIOG_ODR,Copy_u8PIN);	break;
			default: break;


		}

	}



}

u8 DIO_u8GetPortValue(u8 Copy_u8PORT){
	switch(Copy_u8PORT){
			case GPIOA:	 return GPIOA_IDR;	break;
			case GPIOB:	 return GPIOB_IDR;	break;
			case GPIOC:	 return GPIOC_IDR;	break;
			case GPIOD:	 return GPIOD_IDR;	break;
			case GPIOE:	 return GPIOE_IDR;	break;
			case GPIOF:	 return GPIOF_IDR;	break;
			case GPIOG:	 return GPIOG_IDR;	break;
			default: return 0;
		}

}

u8 DIO_u8GetPinValue (u8 Copy_u8PORT , u8 Copy_u8PIN ){
	switch(Copy_u8PORT){
		case GPIOA:	 return GET_BIT(GPIOA_IDR,Copy_u8PIN);	break;
		case GPIOB:	 return GET_BIT(GPIOB_IDR,Copy_u8PIN);	break;
		case GPIOC:	 return GET_BIT(GPIOC_IDR,Copy_u8PIN);	break;
		case GPIOD:	 return GET_BIT(GPIOD_IDR,Copy_u8PIN);	break;
		case GPIOE:	 return GET_BIT(GPIOE_IDR,Copy_u8PIN);	break;
		case GPIOF:	 return GET_BIT(GPIOF_IDR,Copy_u8PIN);	break;
		case GPIOG:	 return GET_BIT(GPIOG_IDR,Copy_u8PIN);	break;
		default: return 0;
	}
}

void DIO_voidTogglePinValue (u8 Copy_u8PORT , u8 Copy_u8PIN ){
	switch(Copy_u8PORT){
		case GPIOA:	TGL_BIT(GPIOA_ODR,Copy_u8PIN); 	break;
		case GPIOB:	TGL_BIT(GPIOB_ODR,Copy_u8PIN);	break;
		case GPIOC:	TGL_BIT(GPIOC_ODR,Copy_u8PIN);	break;
		case GPIOD:	TGL_BIT(GPIOD_ODR,Copy_u8PIN);	break;
		case GPIOE:	TGL_BIT(GPIOE_ODR,Copy_u8PIN);	break;
		case GPIOF:	TGL_BIT(GPIOF_ODR,Copy_u8PIN);	break;
		case GPIOG:	TGL_BIT(GPIOG_ODR,Copy_u8PIN);	break;
		default: break;

	}



}

