/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib

import calvos.utils.time as tmr
import calvos.common.codegen as cg

from cog_time import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project_obj = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_proj_pickle_file, e))
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = cog_output_file
padding = 80 - (19 + len(cog_output_file))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for Timer definitions.
 *  \details   	Contains definitions for timers.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-26
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
cog.outl(C_gen_info(input_worksheet,project_obj))
]]] */
// [[[end]]]
/* [[[cog
# Print include guards
guard_symbol = cog_output_file.replace(".","_")
guard_symbol = guard_symbol.upper()

cog.outl("#ifndef "+guard_symbol)
cog.outl("#define "+guard_symbol)
]]] */
// [[[end]]]
/* [[[cog

# Definition of Functions
 ]]] */
// [[[end]]]

#include "calvos.h"
/* [[[cog
# Generate include statements if required

if 'include_var' in locals():
	includes = json.loads(include_var)
	for include in includes:
		cog.outl("#include \"" + include + "\"")
 ]]] */
// [[[end]]]

typedef uint8_t timrIdx; // Depends on total of timers


typedef enum{
	kTmrState_STOPPED,
	kTmrState_RUNNING,
	kTmrState_EXPIRED
}tmrState;

typedef struct{
	void * static_data;
	void * dynamic_data;
	tmrTimerNode * next;
}tmrTimerNode;

/* Types for Simple Timers */
/* ----------------------- */
typedef struct{
	Callback expired_callback;
}tmrTimerStaticData;

typedef struct{
	tmrState state;
}tmrTimerDynamicFields;

/* Types for Simple Timers - periodic - */
/* ------------------------------------ */
typedef struct{
	uint8_t inf_reloads : 1; // Infinite reloads
	uint8_t reloads : 7;
}tmrTimerTstaticFields;

typedef struct{
	tmrTimerTstaticFields data;
	uint32_t period;
	Callback expired_callback;
}tmrTimerTstaticData;

typedef struct{
	uint8_t running_cycle : 1;
}tmrTimerTdynamicFields;

typedef struct{
	tmrState state;
	tmrTimerDynamicFields data;
}tmrTimerTdynamicFields;

/* Types for Full Timers */
/* --------------------- */
/* More flexible in run-time */
typedef struct{
	Callback expired_callback;
}tmrFullTimerStaticData;

typedef struct{
	uint8_t inf_reloads : 1; // Infinite reloads
	uint8_t reloads : 7;
}tmrFullTimerDynamicFields;

typedef struct{
	tmrState state;
	uint32_t period;
}tmrFullTimerDynamicData;


/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
