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
/** \brief     	Common CAN functions definitions.
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

/* ===========================================================================*/
/** Function for traversing a CAN RX msg search tree.
 *
 * More detailed description of what this function does.
 *
 * @param msg_id 	ID of the message to look for.
 * @param root		Pointer to the search tree's root node.
 * @param guard		Integer value to act as a guard to avoid endless loops.
 * 					Shall set to be equal to the max number of Rx messages of
 * 					the given node.
 * @return	Returns a pointer to the message's static data if message was found
 * 			while traversing the tree. Returns @c NULL if message was not found.
 * ===========================================================================*/
CANrxMsgStaticData* can_traverseRxSearchTree(uint32_t msg_id, \
											 CANrxMsgStaticData* root, \
											 uint32_t guard){
	CANrxMsgStaticData* return_node = NULL;
	CANrxMsgStaticData* current_node = root;

	while(guard > 0){
		/* guard variable is to avoid an endless loop in case of a malformed
		 * tree (missing some leave nodes). Tree traverse can't be of more
		 * than number of rx messages in the node. */
		guard -= 1;
		if(msg_id == current_node->id){
			/* Message found. Return current node address. */
			return_node = current_node;
			break;
		}else if(msg_id < current_node->id){
			/* Search in previous tree node if not Null */
			if(current_node->prev != NULL){
				current_node = current_node->prev;
			}else{
				/* if a leave node is found (NULL) then end search since id
				 * was not found. */
				break;
			}
		}else{ /* msg_id > current_node->id */
			/* Search in next tree node if not Null */
			if(current_node->next != NULL){
				current_node = current_node->next;
			}else{
				/* if a leave node is found (NULL) then end search since id
				 * was not found. */
				break;
			}
		}
	}

	return return_node;
}



