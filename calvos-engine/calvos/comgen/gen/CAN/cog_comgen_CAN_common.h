/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

from cog_CAN import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))
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
/** \brief     	Header file CAN Signals definitions.
 *  \details   	Contains data structures to ease the handling of the CAN
 *				signals.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2020-11-15
 *  \copyright 	2020 Carlos Calvillo.
 */
/*============================================================================*/
/* [[[cog
cog.outl(cg.C_license())
]]] */
// [[[end]]]
/*============================================================================*/
/* [[[cog

# Print generation information
cog.outl(C_gen_info(input_worksheet,network))
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

/* TX Types definitions */
/* [[[cog
# Generate include statements if required
cog.outl("#include kTxSpontan\t\t\t" + str(nw.SPONTAN) + "u")
cog.outl("#include kTxCyclic\t\t\t" + str(nw.CYCLIC) + "u")
cog.outl("#include kTxCyclicSpontan\t" + str(nw.CYCLIC_SPONTAN) + "u")
cog.outl("#include kTxBAF\t\t\t\t" + str(nw.BAF) + "u")
 ]]] */
// [[[end]]]

/* Message direction definitions */
/* [[[cog
# Generate include statements if required
cog.outl("#include kDirTX\t\t" + str(nw.CAN_TX) + "u")
cog.outl("#include kDirRX\t\t" + str(nw.CAN_RX) + "u")
 ]]] */
// [[[end]]]

/* Message direction definitions */
typedef enum{
	kMsgNotFound = 0,
	kMsgFound
}CANmsgFound;

/* Types for CAN messages static data */
typedef struct{
	uint8_t len : 4;
	uint8_t id_is_extended : 1;
}CANrxMsgStaticFields;

typedef struct{
	uint32_t id;
	uint32_t timeout;
	Callback rx_callback;
	Callback timeout_callback;
	CANrxMsgStaticFields data;
	CANrxMsgStaticData * next;
	CANrxMsgStaticData * prev;
}CANrxMsgStaticData;

typedef struct{
	uint8_t len : 4;
	uint8_t id_is_extended : 1;
	uint8_t tx_type : 2;
}CANtxMsgStaticFields;

typedef struct{
	uint32_t id;
	uint32_t period;
	Callback tx_callback;
	CANtxMsgStaticFields data;
}CANtxMsgStaticData;

/* Types for CAN messages dynamic data */
typedef struct{
	FlagsNative available;
	intNative_t timedout;
	NodeUint32 timeout_queue;
}CANrxMsgDynamicData;

typedef union{
	FlagsNative transmitted;
	intNative_t BAF_active;
	NodeUint32 period_queue;
}CANtxMsgDynamicFields;

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
