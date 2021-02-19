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
/** \brief     	Common CAN functions definitions.
 *  \details   	Implements common functionality for CAN (common to multiple
 *  			networks, nodes, etc.)
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
/* =============================================================================
 * 	Macros and typedefs
 * ===========================================================================*/

/* =============================================================================
 * 	Exported definitions
 * ===========================================================================*/

/* =============================================================================
 * 	Private definitions
 * ===========================================================================*/

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

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
const CANrxMsgStaticData* can_traverseRxSearchTree(uint32_t msg_id, \
											 const CANrxMsgStaticData* root, \
											 uint32_t guard){
	const CANrxMsgStaticData* return_node = NULL;
	const CANrxMsgStaticData* current_node = root;

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
			if(current_node->searchPrev != NULL){
				current_node = current_node->searchPrev;
			}else{
				/* if a leave node is found (NULL) then end search since id
				 * was not found. */
				break;
			}
		}else{ /* msg_id > current_node->id */
			/* Search in next tree node if not Null */
			if(current_node->searchNext != NULL){
				current_node = current_node->searchNext;
			}else{
				/* if a leave node is found (NULL) then end search since id
				 * was not found. */
				break;
			}
		}
	}

	return return_node;
}

/* ===========================================================================*/
/** Function to enqueue a CAN tx message to the transmission queue.
 *
 * @param queue 	Pointer to the queue to operate with.
 * @param root		Pointer to node to be enqueued.
 * @return	Returns @c kNoError if node was enqueued successfully. Returns
 * 			@c kError if queue was full and, hence, node was not enqueued.
 * ===========================================================================*/
CalvosError can_txQueueEnqueue(CANtxQueue* queue, const CANtxMsgStaticData* node){
	CalvosError return_value = kError;

	/* Queue node at the tail if there is still empty space in the queue */
	if(queue->length < kCANtxQueueLen){
		queue->tail->dyn->txQueueNext = node;
		queue->tail = node;
		queue->length++;
		return_value = kNoError;
	}

	return return_value;
}

/* ===========================================================================*/
/** Function to get the head node of a CAN tx queue.
 *
 * @param queue 	Pointer to the queue to operate with.
 * @return	Returns a pointer to the CAN message statuc data structure at the
 * 			head of the queue.
 * ===========================================================================*/
const CANtxMsgStaticData* can_txQueueGetHead(CANtxQueue* queue){
	const CANtxMsgStaticData* return_value = NULL;

	/* Queue node at the tail if there is still empty space in the queue */
	if(queue->length > 0){
		return_value = queue->head;
	}

	return return_value;
}

/* ===========================================================================*/
/** Function to dequeue a CAN tx message from the transmission queue.
 *
 * @param queue 	Pointer to the queue to operate with.
 * @param node		Pointer to the dequeued node if success.
 * @return	Returns @c kNoError if node was dequeued successfully. Returns
 * 			@c kError if queue was empty and, hence, no node was dequeued.
 * ===========================================================================*/
CalvosError can_txQueueDequeue(CANtxQueue* queue, const CANtxMsgStaticData* node){
	CalvosError return_value = kError;

	/* Dequeue node at the head if queue is not empty. */
	if(queue->head != NULL){
		if(node != NULL){
			node = queue->head;
		}
		queue->head = queue->head->dyn->txQueueNext;
		queue->length--;
		return_value = kNoError;
	}

	return return_value;
}

/* ===========================================================================*/
/** Function to initialize a CAN transmisison queue.
 *
 * @param queue 	Pointer to the queue to operate with..
 * @return	Returns @c kNoError if queue was initialized successfully. Returns
 * 			@c kError if provided queue was NULL.
 * ===========================================================================*/
CalvosError can_txQueueInit(CANtxQueue* queue){
	CalvosError return_value = kError;

	/* Initialize given queue if not NULL */
	if(queue != NULL){
		queue->head = NULL;
		queue->tail = NULL;
		queue->length = 0;
		return_value = kNoError;
	}

	return return_value;
}

/* ===========================================================================*/
/** Function for clearing all available flags of a received CAN msg.
 *
 * Clears all the available flags of a CAN RX message.
 *
 * @param msg_idx 	Index of the node's message (refer to enumeration
 * 					CAN_NWID_NODEID_rxMsgs).
 * @param msg_idx_max 	Max number of Rx messages in the node (refer to
 * 						constant kCAN_NWID_NODEID_nOfRxMsgs for the given node).
 * @param msg_struct 	Pointer to the node's messages static data structure.
 * ===========================================================================*/
void can_clearAllAvlblFlags(const CANrxMsgStaticData* msg_struct){
	// Clear Message available flags
	msg_struct->dyn->available.all = 0;
	// Clear Signals available flags
	if(msg_struct->sig_avlbl_buf_len == 1){
		*msg_struct->sig_avlbl_flags = 0;
	}else{
		for(uint32_t i=0; i < msg_struct->sig_avlbl_buf_len; i++){
			msg_struct->sig_avlbl_flags[i] = 0;
		}
	}
}

/* ===========================================================================*/
/** Function for transmitting a CAN msg given its data structure.
 *
 * Triggers the transmission of the specified CAN message.
 *
 * @param msg_struct 	Pointer to the transmitting message's static data.
 * @param queue 		Pointer to the transmitting queue.
 * @Return 		Returns @c kNoError if message was triggered for transmission
 * 				by HAL or if it was successfully queued for a transmission
 * 				retry. Returns @c kError otherwise (HAL busy, queue full).
 * ===========================================================================*/
CalvosError can_commonTransmitMsg(const CANtxMsgStaticData* msg_struct, \
								  CANtxQueue* queue, \
								  CANhalTxFunction can_hal_tx_function, \
								  const CANtxMsgStaticData* transmitting_msg){
	CalvosError return_value = kError;
	CalvosError local_return_value;

	// Trigger CAN transmission to HAL if it is not being transmitted or
	// pending for transmission
	if((msg_struct->dyn->state != kCANtxState_transmitting) \
	&& (msg_struct->dyn->state != kCANtxState_queued)){
		local_return_value = (*can_hal_tx_function)(msg_struct);
		if(local_return_value == kNoError){
			// Message successfully triggered for transmission from HAL
			msg_struct->dyn->state = kCANtxState_transmitting;
			transmitting_msg = msg_struct;
			return_value = kNoError;
		}else{
			// If queue is not NULL, queue message for a later transmission (retry)
			if(queue != NULL){
				local_return_value = can_txQueueEnqueue(queue, msg_struct);
				if(local_return_value == kNoError){
					// Queue succeeded
					msg_struct->dyn->state = kCANtxState_queued;
					return_value = kNoError;
				}else{
					msg_struct->dyn->state = kCANtxState_requested;
				}
			}else{
				msg_struct->dyn->state = kCANtxState_requested;
			}
		}
	}
	return return_value;
}

/* ===========================================================================*/
/** Function for confirming transmission of CAN message.
 *
 * @param transmitting_msg 	Pointer to the transmitting message's static data.
 * @Return 		Returns @c kNoError if provided @c transmitting_msg is not NULL.
 * 				Returns @c kError otherwise.
 * ===========================================================================*/
void can_commonConfirmTxMsg(const CANtxMsgStaticData* transmitting_msg){

	if(transmitting_msg != NULL){
		transmitting_msg->dyn->state = kCANtxState_transmited;
		// Clears transmitting message pointer
		transmitting_msg = NULL;
	}
}
