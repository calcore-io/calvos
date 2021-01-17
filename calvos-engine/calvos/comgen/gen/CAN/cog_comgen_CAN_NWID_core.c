/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import json
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
/** \brief     	Source file for CAN Core functionality.
 *  \details   	Implements the CAN core functionality for the handling of the
 *  			defined messages/signals in the given network.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-15
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

/* Private macro definitions */

/* Private type definitions */
typede struct{
	uint32_t current_idx;
	uint32_t lower_idx;
	uint32_t upper_idx;
}RxSearchNodeVals;

typedef struct{
	NodeUint32 node;
	/* Range of indexes within CANmsgStaticData_t to navigate looking for
	 * the rx message. */
	RxSearchNodeVals vals;
}RxSearchTreeNode;

typedef struct{
	T_native use_current_idx;
	RxSearchNodeVals vals;
}RxSearchResult;

/* Exported data */

const CANrxMsgStaticData can_CT_static_rx_msg_data[kCAN_CT_nOfRxMsgs] = [\
		{kCAN_CT_msgId_MESSAGE1,	kCAN_CT_timeout_MESSAGE1, &can_rx_callback_MESSAGE1, &can_timeout_callback_MESSAGE1, \
				{kFalse, kCAN_CT_msgLen_MESSAGE1}},
		{kCAN_CT_msgId_MESSAGE2,	kCAN_CT_timeout_MESSAGE2, &can_rx_callback_MESSAGE2, &can_timeout_callback_MESSAGE2, \
				{kFalse, kCAN_CT_msgLen_MESSAGE2}}]

CANrxMsgDynamicData can_CT_dynamic_rx_msg_data[kCAN_CT_nOfRxMsgs];

/* Private data */

/* Rx messages navigation tree */
/* Node at index is the top node of the tree */
# define kCAN_N_OF_RX_NODES		6u
const RxSearchTreeNode rx_search_nodes[kCAN_N_OF_RX_NODES] = [\
	{{0x50,		&rx_search_nodes[1],	&rx_search_nodes[2]},	{7,		5,	9}},
	{{0x40,		&rx_search_nodes[3],	&rx_search_nodes[4]},	{5,		4,	6}},
	{{0x60,		&rx_search_nodes[5],	NULL				},	{9,		8,	10}},
	{{0x35,		NULL			   ,	NULL				},	{7,		5,	9}},
	{{0x45,		NULL			   ,	NULL				},	{7,		5,	9}},
	{{0x55,		NULL			   ,	NULL				},	{7,		5,	9}}]

/* Exported Function Prototypes */
void CAN_coreInit();
void CAN_processRxMessage();

/* Private Function Prototypes */
void CAN_clearMsgDynamicData();
CalvosError CAN_traverseRxSearchTree(uint32_t, RxSearchResult);
CANmsgFound CAN_getMsgIdx(uint32_t, uint32_t *);
void CAN_filterRxMsg(uint32_t);

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/
void CAN_clearMsgDynamicData(){
	/* Initialize msg dynamic data */
		CANmsgDynamicData clear_data;

		clear_data.timeout_queue = 0;
		clear_data.timeout_time = 0;
		clear_data.period_queue = 0;
		clear_data.period_time = 0;
		clear_data.data.available = 0;
		clear_data.data.timed_out = 0;

		for(uint32_t i=0; i<kCAN_CT_nOfMsgs; i++){
			can_CT_dynamic_msg_data[i] = clear_data;
		}
}

/* ===========================================================================*/
/** Function for traversing a CAN RX msg search tree.
 *
 * More detailed description of what this function does.
 *
 * @param msg_id 	ID of the message to look for.
 * @param result	Contains the indexes where to search for the id within the
 *  				message static data array.
 * @return	Returns @c kNoError if traversing the tree was Ok, returns @c kError
 *  				otherwise.
 * ===========================================================================*/
CalvosError CAN_traverseRxSearchTree(uint32_t msg_id, RxSearchResult* result, \
		RxSearchTreeNode* search_tree){
	uint32_t guard = kCAN_N_OF_RX_NODES;
	CalvosError return_value = kError;
	/* Search will always starts at index 0 so that index should be
	 * the top node. */
	RxSearchTreeNode current_node = search_tree[0];

	while(kTrue and guard > 0){
		/* guard variable is to avoid an endless loop in case of a malformed
		 * tree (missing some leave nodes). Tree traverse can't be of more
		 * than kCAN_N_OF_RX_NODES nodes. */
		guard -= 1;
		/* Keep looking for msg_id until the msg_id is found or a leave node is
		 * reached. In either case a break statement shall quit the loop. */
		if(current_node.node.val == msg_id){
			/* Message found! */
			*result.vals.current_idx = current_node.vals.current_idx;
			*result.use_current_idx = kTrue;
			return_value = kNoError;
			break;
		}else if(msg_id < current_node.node.val){
			/* See if a lower node exists */
			if(current_node.node.prev != NULL){
				/* Assign new node for the search. */
				current_node = current_node.node.prev;
			}else{
				/* No previous node. Means this is a leave node. */
				*result.vals.lower_idx = current_node.vals.lower_idx;
				*result.vals.upper_idx = current_node.vals.upper_idx;
				*result.use_current_idx = kFalse;
				return_value = kNoError;
				break;
			}
		}else( /* msg_id > current_node.node.val */
			/* See if a upper node exists */
			if(current_node.node.next != NULL){
				/* Assign new node for the search. */
				current_node = current_node.node.next;
			}else{
				/* No next node. Means this is a leave node. */
				*result.vals.lower_idx = current_node.vals.lower_idx;
				*result.vals.upper_idx = current_node.vals.upper_idx;
				*result.use_current_idx = kFalse;
				return_value = kNoError;
				break;
			}

		}
	}
	return return_value;
}


CANmsgFound CAN_getRxMsgIdx(uint32_t msg_id, uint32_t* msg_idx){
	CANmsgFound return_val;
	RxSearchResult search_result;

	return_val = kMsgNotFound;

	if(CAN_traverseRxSearchTree(msg_id, &search_result, rx_search_nodes) \
	!= kError){
		if(search_result.use_current_idx){
			*msg_idx = search_result.vals.current_idx;
			return_val = kMsgFound;
		}else{
			/* Get indexes to search for msg id within the static data array */
			for(uint32_t i=search_result.vals.lower_idx; \
			i <= search_result.vals.upper_idx; i++){
				if(can_CT_static_rx_msg_data[i].id == msg_id){
					return_val = kMsgFound;
					*msg_idx = i;
					break;
				}
			}
		}
	}

	return return_val;
}

void CAN_processRxMessage(msg_id){
	CANmsgFound msg_found;
	uint32_t msg_idx;

	/* TODO: If multiple search trees (for example a search tree for < 20ms
	 * messages, another for 20 to 100 ms, another for more than 100ms, etc.)
	 * adjust following logic to search for all trees. */
	msg_found = CAN_getRxMsgIdx(msg_id, &msg_idx);
	if(msg_found == kMsgFound){
		/* Set available flags */
		can_CT_dynamic_rx_msg_data[msg_idx].available.all = kAllOnes32;
		/* clear timeout flag */
		can_CT_dynamic_rx_msg_data[msg_idx].timedout = kFalse;
		/* Invoke rx callback */
		if(CANrxMsgStaticData[msg_idx].timeout_callback != NULL){
			(*CANrxMsgStaticData[msg_idx].timeout_callback)();
		}
	}
}


void CAN_coreInit(){
	/* Initialize msg dynamic data */
	CAN_clearMsgDynamicData();
}


/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
