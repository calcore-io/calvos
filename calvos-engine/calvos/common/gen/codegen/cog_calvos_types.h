/* [[[cog
import cog
import math
import pickle as pic
import pathlib as plib


import calvos.common.codegen as cg

from cog_codegen import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))
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
 *  \details   	Contains typedefs to be used across all calvos related source
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

#include "general_defs.h"

#define kTrue 	1u
#define kFalse 	0u

/* TODO: Un-comment definitions for uint32_t and int32_t temporarily done for compatibility with NXP S32 SDK */
/* [[[cog

# Definition of basic data types
cog.outl("/"+chr(42)+" Definition of basic data types "+chr(42)+"/")
for key, data_type in cg.dt_compiler.items():
	typedef_str = None
	if key in cg.dt:
		if data_type != cg.dt[key] and data_type is not None:
			# TODO: In S32 Studio, uint32_t and int32_t are already defined
			if cg.dt[key] == "uint32_t" or cg.dt[key] == "int32_t":
				comment_out = "// "
			else:
				comment_out = ""
			typedef_str = comment_out+"typedef " + data_type + " " + cg.dt[key] + ";"
	else:
		typedef_str = "#error Data type string not found for '%s'" % data_type
		log_warn("Data type string not found for '%s'" % data_type)
	if typedef_str is not None:
		cog.outl(typedef_str)

 ]]] */
// [[[end]]]

/* MCU dependant symbols */
/* [[[cog

# Definition of basic data types
sym_word_size_name = "MCU_WORD_SIZE"
sym_word_size_val = project.get_simple_param_val("common.codegen", "codegen_word_size")

code_str = "#define " + sym_word_size_name + "\t\t" + str(sym_word_size_val)
cog.outl(code_str)

# Definition of native data types
sym_u_native_name = "uintNat_t"
cog.outl("typedef " +  cg.get_dtv(sym_word_size_val, "i", False) + " " + sym_u_native_name + ";")

sym_native_name = "intNat_t"
cog.outl("typedef " +  cg.get_dtv(sym_word_size_val, "i", True) + " " + sym_native_name + ";")

 ]]] */
// [[[end]]]

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
