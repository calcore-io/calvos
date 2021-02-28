/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib
import json

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

from cog_CAN import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project = pic.load(f)
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
/** \brief     	Header file for common CAN definitions.
 *  \details   	Declares data structures, functions, etc. common for CAN (common
 *  			to multiple networks, nodes, etc.)
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-12
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
cog.outl("#define kTxSpontan\t\t\t" + str(nw.SPONTAN) + "u")
cog.outl("#define kTxCyclic\t\t\t" + str(nw.CYCLIC) + "u")
cog.outl("#define kTxCyclicSpontan\t" + str(nw.CYCLIC_SPONTAN) + "u")
cog.outl("#define kTxBAF\t\t\t\t" + str(nw.BAF) + "u")
 ]]] */
// [[[end]]]

/* Message direction definitions */
/* [[[cog
# Generate include statements if required
cog.outl("#define kDirTX\t\t" + str(nw.CAN_TX) + "u")
cog.outl("#define kDirRX\t\t" + str(nw.CAN_RX) + "u")
 ]]] */
// [[[end]]]

/* Length of CAN transmission queue */
/* [[[cog
# Generate include statements if required
sym_tx_queue_len_name = "kCANtxQueueLen"
sym_tx_queue_len_value = str(network.get_simple_param("CAN_tx_queue_len"))
code_str = "#define "+sym_tx_queue_len_name+"\t\t("+sym_tx_queue_len_value+"u)"
cog.outl(code_str)
 ]]] */
// [[[end]]]

/* TX states */
typedef enum{
	kCANtxState_idle = 0,
	kCANtxState_requested,
	kCANtxState_queued,
	kCANtxState_transmitting,
	kCANtxState_transmitted
}CANtxState;

/* Message direction definitions */
typedef enum{
	kMsgNotFound = 0,
	kMsgFound
}CANmsgFound;

/* Types for CAN messages dynamic data */
typedef struct{
	FlagsNative available;
	intNat_t timedout;
	NodeUint32 timeout_queue;
}CANrxMsgDynamicData;

typedef struct{
	CANtxState state;
	intNat_t BAF_active;
	uint32_t period_timer;	/* In ticks */
	const struct CANtxMsgStaticData* txQueueNext;
}CANtxMsgDynamicData;

/* Types for CAN messages static data */
typedef struct{
	uint8_t len : 4;
	uint8_t id_is_extended : 1;
}CANrxMsgStaticFields;

typedef struct CANrxMsgStaticData{
	uint32_t id;
	uint32_t timeout; /* Given in base time ticks */
	Callback rx_callback;
	Callback timeout_callback;
	uint8_t* data;
	uint8_t * sig_avlbl_flags;
	uint8_t sig_avlbl_buf_len;
	CANrxMsgDynamicData* dyn;
	CANrxMsgStaticFields fields;
	const struct CANrxMsgStaticData* searchPrev;
	const struct CANrxMsgStaticData* searchNext;
}CANrxMsgStaticData;

typedef struct{
	uint8_t len : 4;
	uint8_t id_is_extended : 1;
	uint8_t tx_type : 2;
}CANtxMsgStaticFields;

typedef struct CANtxMsgStaticData{
	uint32_t id;
	uint32_t period; /* Given in base time ticks */
	Callback tx_callback;
	uint8_t* data;
	CANtxMsgDynamicData* dyn;
	CANtxMsgStaticFields fields;
}CANtxMsgStaticData;

/* Declaration of TX transmission queue */
typedef struct{
	intNat_t length;
	const CANtxMsgStaticData* head;
	const CANtxMsgStaticData* tail;
}CANtxQueue;

/* HAL tx function typedef */
typedef CalvosError (*CANhalTxFunction)(const CANtxMsgStaticData* msg_info);

/* Queueing Functions */
CalvosError can_txQueueEnqueue(CANtxQueue* queue, const CANtxMsgStaticData* node);
const CANtxMsgStaticData* can_txQueueGetHead(CANtxQueue* queue);
CalvosError can_txQueueDequeue(CANtxQueue* queue, const CANtxMsgStaticData* node);
CalvosError can_txQueueInit(CANtxQueue* queue);
void can_clearAllAvlblFlags(const CANrxMsgStaticData* msg_struct);

/* Exported Prototypes */
extern const CANrxMsgStaticData* can_traverseRxSearchTree(uint32_t msg_id, \
		const CANrxMsgStaticData* root, \
		uint32_t guard);

extern CalvosError can_commonTransmitMsg(const CANtxMsgStaticData* msg_struct, \
		  CANtxQueue* queue, \
		  CANhalTxFunction can_hal_tx_function, \
		  const CANtxMsgStaticData** transmitting_msg);

extern void can_commonConfirmTxMsg(const CANtxMsgStaticData* transmitting_msg, \
		uintNat_t check_msg_id, uint32_t txd_msg_id);

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
