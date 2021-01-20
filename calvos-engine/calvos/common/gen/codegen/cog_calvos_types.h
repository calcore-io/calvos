/* [[[cog
import cog
import math
import pickle as pic
import pathlib as plib


import calvos.common.codegen as cg

from cog_codegen import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = "calvos_types.h"
padding = 80 - (19 + len(file_name))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for calvos type definitions.
 *  \details   	Contains typedef to be used across all calvos related source
 *				code.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-13
 *  \copyright 	2021 Carlos Calvillo.
 */
/*============================================================================*/
/* [[[cog
cog.outl(cg.C_license())
]]] */
// [[[end]]]
/*============================================================================*/
/* [[[cog

# Print generation information
cog.outl(C_gen_info("File Name"))
]]] */
// [[[end]]]
#ifndef CALVOS_TYPES_H
#define CALVOS_TYPES_H

#define kTrue 	1u
#define kFalse 	0u

/* [[[cog

# Definition of basic data types
cog.outl("/"+chr(42)+" Definition of basic data types "+chr(42)+"/")
for key, data_type in cg.dt_compiler.items():
	typedef_str = None
	if key in cg.dt:
		if data_type != cg.dt[key] and data_type is not None:
			typedef_str = "typedef " + data_type + " " + cg.dt[key] + ";"
	else:
		typedef_str = "#error Data type string not found for '%s'" % data_type
		log_warn("Data type string not found for '%s'" % data_type)
	if typedef_str is not None:
		cog.outl(typedef_str)

 ]]] */
// [[[end]]]

typedef uint32_t intNative_t;
typedef uintX_t /* [[[cog cog.outl("hey") ]]] */ g // [[[end]]];

/* Error symbols */
typedef enum{
	kNoError = 0,
	kError
}CalvosError;

typedef void (*Callback)(void);

typedef struct{
	uint32_t value;
	NodeUint32 * prev;
	NodeUint32 * next;
}NodeUint32;

typedef struct{
	uint8_t value;
	NodeUint8 * prev;
	NodeUint8 * next;
}NodeUint8;

typedef struct{
	uint16_t value;
	NodeUint16 * prev;
	NodeUint16 * next;
}NodeUint16;

typedef struct{
	uint64_t value;
	NodeUint64 * prev;
	NodeUint64 * next;
}NodeUint64;

typedef struct{
	void * value;
	NodeVoidPtr * prev;
	NodeVoidPtr * next;
}NodeVoidPtr;

/* Number of bits depend on target machine/compiler */
typedef union{
	struct{
		flag0 : 1;
		flag1 : 1;
		flag2 : 1;
		flag3 : 1;
		flag4 : 1;
		flag5 : 1;
		flag6 : 1;
		flag7 : 1;
		flag8 : 1;
		flag9 : 1;
		flag10 : 1;
		flag11 : 1;
		flag12 : 1;
		flag13 : 1;
		flag14 : 1;
		flag15 : 1;
		flag16 : 1;
		flag17 : 1;
		flag18 : 1;
		flag19 : 1;
		flag20 : 1;
		flag21 : 1;
		flag22 : 1;
		flag23 : 1;
		flag24 : 1;
		flag25 : 1;
		flag26 : 1;
		flag27 : 1;
		flag28 : 1;
		flag29 : 1;
		flag30 : 1;
		flag31 : 1;
	}flags;
	uint32_t all;
}FlagsNative;

typedef union{
	struct{
		flag0 : 1;
		flag1 : 1;
		flag2 : 1;
		flag3 : 1;
		flag4 : 1;
		flag5 : 1;
		flag6 : 1;
		flag7 : 1;
	}flags;
	uint8_t all;
}Flags8;

typedef union{
	struct{
		flag0 : 1;
		flag1 : 1;
		flag2 : 1;
		flag3 : 1;
		flag4 : 1;
		flag5 : 1;
		flag6 : 1;
		flag7 : 1;
		flag8 : 1;
		flag9 : 1;
		flag10 : 1;
		flag11 : 1;
		flag12 : 1;
		flag13 : 1;
		flag14 : 1;
		flag15 : 1;
	}flags;
	uint16_t all;
}Flags16;

#define kAllOnes8 	0xFFu
#define kAllOnes16 	0xFFFFu
#define kAllOnes32 	0xFFFFFFFFul
#define kAllOnes64 	0xFFFFFFFFFFFFFFFFull

#endif /* CALVOS_TYPES_H */
