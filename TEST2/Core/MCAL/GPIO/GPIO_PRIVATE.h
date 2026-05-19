/*******************************************************
 * Author  : Abdullah Waheed
 * File    : GPIO_registers.h
 * Brief   : GPIO Registers Definitions for STM32F103C8
 *******************************************************/

#ifndef GPIO_PRIVATE_H
#define GPIO_PRIVATE_H


#include "STD_TYPES.h"

/* ======================================================= */
/*   Base addresses for GPIO ports (STM32F103C8)           */
/* ======================================================= */
#define GPIOA_BASE   0x40010800U
#define GPIOB_BASE   0x40010C00U
#define GPIOC_BASE   0x40011000U
#define GPIOD_BASE   0x40011400U
#define GPIOE_BASE   0x40011800U

/* ======================================================= */
/*   GPIOA Registers                                       */
/* ======================================================= */
#define GPIOX_CRL    ( 0x00 )
#define GPIOX_CRH    ( 0x04 )
#define GPIOX_IDR    ( 0x08 )
#define GPIOX_ODR    ( 0x0C )
#define GPIOX_BSRR   ( 0x10 )
#define GPIOX_BRR    ( 0x14 )
#define GPIOX_LCKR   ( 0x18 )



/* ======================================================= */
/*   GPIO Pin Numbers (0..15)                              */
/* ======================================================= */
typedef enum
{
    PIN0  = 0,
    PIN1  = 1,
    PIN2  = 2,
    PIN3  = 3,
    PIN4  = 4,
    PIN5  = 5,
    PIN6  = 6,
    PIN7  = 7,
    PIN8  = 8,
    PIN9  = 9,
    PIN10 = 10,
    PIN11 = 11,
    PIN12 = 12,
    PIN13 = 13,
    PIN14 = 14,
    PIN15 = 15
} PINS;

/* ======================================================= */
/*   GPIO Port Numbers (A..E)                              */
/* ======================================================= */
typedef enum
{
    PORTA = 0,
    PORTB = 1,
    PORTC = 2,
    PORTD = 3,
    PORTE = 4
} PORTS;

/* ======================================================= */
/*   GPIO Direction                                        */
/* ======================================================= */
typedef enum
{
    /* ---- INPUT MODES ---- */
    GPIO_MODE_INPUT_ANALOG       = 0x0U,  // 0000: Analog input
    GPIO_MODE_INPUT_FLOATING     = 0x4U,  // 0100: Floating input
    GPIO_MODE_INPUT_PU_PD        = 0x8U,  // 1000: Input with pull-up / pull-down

    /* ---- OUTPUT MODES ---- */
    GPIO_MODE_OUTPUT_PP_10MHZ    = 0x1U,  // 0001: Output push-pull, 10 MHz
    GPIO_MODE_OUTPUT_PP_2MHZ     = 0x2U,  // 0010: Output push-pull, 2 MHz
    GPIO_MODE_OUTPUT_PP_50MHZ    = 0x3U,  // 0011: Output push-pull, 50 MHz

    GPIO_MODE_OUTPUT_OD_10MHZ    = 0x5U,  // 0101: Output open-drain, 10 MHz
    GPIO_MODE_OUTPUT_OD_2MHZ     = 0x6U,  // 0110: Output open-drain, 2 MHz
    GPIO_MODE_OUTPUT_OD_50MHZ    = 0x7U,  // 0111: Output open-drain, 50 MHz

    /* ---- ALTERNATE FUNCTION MODES ---- */
    GPIO_MODE_AF_PP_10MHZ        = 0x9U,  // 1001: Alternate function push-pull, 10 MHz
    GPIO_MODE_AF_PP_2MHZ         = 0xAU,  // 1010: Alternate function push-pull, 2 MHz
    GPIO_MODE_AF_PP_50MHZ        = 0xBU,  // 1011: Alternate function push-pull, 50 MHz

    GPIO_MODE_AF_OD_10MHZ        = 0xDU,  // 1101: Alternate function open-drain, 10 MHz
    GPIO_MODE_AF_OD_2MHZ         = 0xEU,  // 1110: Alternate function open-drain, 2 MHz
    GPIO_MODE_AF_OD_50MHZ        = 0xFU   // 1111: Alternate function open-drain, 50 MHz

} DIR;

/* ======================================================= */
/*   GPIO State                                            */
/* ======================================================= */
typedef enum
{
    LOW  = 0,
    HIGH = 1
} VAL;



#endif /* GPIO_PRIVATE_H */
