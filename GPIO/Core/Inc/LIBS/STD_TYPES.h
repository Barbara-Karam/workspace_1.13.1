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
typedef unsigned char       u8;
typedef signed char         s8;

typedef unsigned short      u16;
typedef signed short        s16;

typedef unsigned long       u32;
typedef signed long         s32;

typedef unsigned long long  u64;
typedef signed long long    s64;

// Floating point
typedef float   f32;
typedef double  f64;

// Error state
typedef enum {
    E_OK = 0,
    E_NOT_OK
} Std_ReturnType;

#endif /* STD_TYPES_H */
