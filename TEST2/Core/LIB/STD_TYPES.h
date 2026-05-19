/*
 * STD_TYPES.h
 *
 *  Created on: Sep 15, 2025
 *      Author: brbar
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

// True and False
#ifndef TRUE
#define TRUE    (1u)
#endif

#ifndef FALSE
#define FALSE   (0u)
#endif

// Null pointer
#ifndef NULL
#define NULL    ((void*)0)
#endif

// Standard integer types
typedef unsigned char       uint8;
typedef signed char         sint8;

typedef unsigned short      uint16;
typedef signed short        sint16;

typedef unsigned long       uint32;
typedef signed long         sint32;

typedef unsigned long long  uint64;
typedef signed long long    sint64;

// Floating point
typedef float   f32;
typedef double  f64;

// Error state
typedef enum {
    E_OK = 0,
    E_NOT_OK
} Std_ReturnType;

#endif /* STD_TYPES_H */
