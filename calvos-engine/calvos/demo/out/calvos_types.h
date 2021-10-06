/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		calvos_types.h                                                */
/** \brief     	Header file for calvos type definitions.
 *  \details   	Contains typedefs to be used across all calvos related source
 *				code.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-13
 *  \copyright 	2021 Carlos Calvillo.
 */
/*============================================================================*/
/*  This file is part of calvOS project <https://github.com/calcore-io/calvos>.
 *
 *  calvOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  calvOS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with calvOS.  If not, see <https://www.gnu.org/licenses/>. */
/*============================================================================*/
/*-----------------------------------------------------------------------------
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): 2021.10.5::23:23:58
 * Generated from following source(s):
 *     Config file: "File Name"
 -----------------------------------------------------------------------------*/
#ifndef CALVOS_TYPES_H
#define CALVOS_TYPES_H

#include "general_defs.h"

#define kTrue 	1u
#define kFalse 	0u

/* TODO: Un-comment definitions for uint32_t and int32_t temporarily done for compatibility with NXP S32 SDK */
/* Definition of basic data types */
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
// typedef unsigned int uint32_t;
// typedef signed int int32_t;
typedef unsigned long long int uint64_t;
typedef signed long long int int64_t;

/* MCU dependant symbols */
#define MCU_WORD_SIZE		32
typedef uint32_t uintNat_t;
typedef int32_t intNat_t;

/* Error symbols */
typedef enum{
	kNoError = 0,
	kError
}CalvosError;

typedef void (*Callback)(void);

typedef struct NodeUint32{
	uint32_t value;
	struct NodeUint32 * prev;
	struct NodeUint32 * next;
}NodeUint32;

typedef struct NodeUint8{
	uint8_t value;
	struct NodeUint8 * prev;
	struct NodeUint8 * next;
}NodeUint8;

typedef struct NodeUint16{
	uint16_t value;
	struct NodeUint16 * prev;
	struct NodeUint16 * next;
}NodeUint16;

typedef struct NodeUint64{
	uint64_t value;
	struct NodeUint64 * prev;
	struct NodeUint64 * next;
}NodeUint64;

typedef struct NodeVoidPtr{
	void * value;
	struct NodeVoidPtr * prev;
	struct NodeVoidPtr * next;
}NodeVoidPtr;

/* Number of bits depend on target machine/compiler */
typedef union{
	struct{
		uint32_t flag0 : 1;
		uint32_t flag1 : 1;
		uint32_t flag2 : 1;
		uint32_t flag3 : 1;
		uint32_t flag4 : 1;
		uint32_t flag5 : 1;
		uint32_t flag6 : 1;
		uint32_t flag7 : 1;
		uint32_t flag8 : 1;
		uint32_t flag9 : 1;
		uint32_t flag10 : 1;
		uint32_t flag11 : 1;
		uint32_t flag12 : 1;
		uint32_t flag13 : 1;
		uint32_t flag14 : 1;
		uint32_t flag15 : 1;
		uint32_t flag16 : 1;
		uint32_t flag17 : 1;
		uint32_t flag18 : 1;
		uint32_t flag19 : 1;
		uint32_t flag20 : 1;
		uint32_t flag21 : 1;
		uint32_t flag22 : 1;
		uint32_t flag23 : 1;
		uint32_t flag24 : 1;
		uint32_t flag25 : 1;
		uint32_t flag26 : 1;
		uint32_t flag27 : 1;
		uint32_t flag28 : 1;
		uint32_t flag29 : 1;
		uint32_t flag30 : 1;
		uint32_t flag31 : 1;
	}flags;
	uint32_t all;
}FlagsNative;

typedef union{
	struct{
		uint8_t flag0 : 1;
		uint8_t flag1 : 1;
		uint8_t flag2 : 1;
		uint8_t flag3 : 1;
		uint8_t flag4 : 1;
		uint8_t flag5 : 1;
		uint8_t flag6 : 1;
		uint8_t flag7 : 1;
	}flags;
	uint8_t all;
}Flags8;

typedef union{
	struct{
		uint16_t flag0 : 1;
		uint16_t flag1 : 1;
		uint16_t flag2 : 1;
		uint16_t flag3 : 1;
		uint16_t flag4 : 1;
		uint16_t flag5 : 1;
		uint16_t flag6 : 1;
		uint16_t flag7 : 1;
		uint16_t flag8 : 1;
		uint16_t flag9 : 1;
		uint16_t flag10 : 1;
		uint16_t flag11 : 1;
		uint16_t flag12 : 1;
		uint16_t flag13 : 1;
		uint16_t flag14 : 1;
		uint16_t flag15 : 1;
	}flags;
	uint16_t all;
}Flags16;

#define kAllOnes8 	0xFFu
#define kAllOnes16 	0xFFFFu
#define kAllOnes32 	0xFFFFFFFFul
#define kAllOnes64 	0xFFFFFFFFFFFFFFFFull

#endif /* CALVOS_TYPES_H */
