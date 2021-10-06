/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		comgen_CAN_common.c                                           */
/** \brief     	Common CAN functions definitions.
 *  \details   	Implements common functionality for CAN (common to multiple
 *  			networks, nodes, etc.)
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-12
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
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): 2021.10.5::23:23:57
 * Generated from following source(s):
 *     Network file: "G:\devproj\github\calvos_0_4\calvos\calvos-engine\calvos\..
                      demo\usr_in\template - CAN Network Definition.ods"
 *     Network name: "CAN-B"
 *     Network id: "B"
 *     Network date: "12/Mar/2021"
 *     Network version: "2"
 -----------------------------------------------------------------------------*/
#include "calvos.h"
#include "comgen_CAN_common.h"
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
	CALVOS_CRITICAL_ENTER();
	if(queue->length < kCANtxQueueLen){
		if(queue->length == 0){
			// List is empty, make tail equal to new node and tail equal to head
			queue->tail = node;
			//queue->tail->dyn->txQueueNext = NULL;
			queue->head = queue->tail;
		}else{
			// List is not empty, enqueue new node
			queue->tail->dyn->txQueueNext = node;
			queue->tail = node;
		}
		queue->length++;
		return_value = kNoError;
	}
	CALVOS_CRITICAL_EXIT();

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
	CALVOS_CRITICAL_ENTER();
	if(queue->length > 0){
		return_value = queue->head;
	}
	CALVOS_CRITICAL_EXIT();

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
	CALVOS_CRITICAL_ENTER();
	if((queue->length > 0) && (queue->head != NULL)){
		if(node != NULL){
			node = queue->head;
		}
		queue->length--;
		if(queue->length == 0){
			queue->head = NULL;
		}else{
			queue->head = queue->head->dyn->txQueueNext;
		}
		return_value = kNoError;
	}
	CALVOS_CRITICAL_EXIT();

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
								  const CANtxMsgStaticData** transmitting_msg){
	CalvosError return_value = kError;
	CalvosError local_return_value;

	// Trigger CAN transmission to HAL
	local_return_value = (*can_hal_tx_function)(msg_struct);
	if(local_return_value == kNoError){
		// Message successfully triggered for transmission from HAL
		msg_struct->dyn->state = kCANtxState_transmitting;
		*transmitting_msg = msg_struct;
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
			if(msg_struct->dyn->state != kCANtxState_queued){
				// If message was not queued, set it to requested
				// otherwise it will remain as queued
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
 * @param check_msg_id 	If kTrue, then tx message will be confirmed only if the
 * 						transmitted id provided in argument @c txd_msg_id
 * 						matches with the one transmitted by calvos engine.
 * 						Otherwise, confirmation will occur regardless of the id
 * 						transmitted by HAL. In this case ensure there is no
 * 						other SW components transmitting CAN messages other than
 * 						calvos engine.
 * @param txd_msg_id	Id of the message transmitted by the HAL. To be used for
 * 						message confirmation if @c check_msg_id is @c kTrue
 * @Return 		Returns @c kNoError if provided @c transmitting_msg is not NULL.
 * 				Returns @c kError otherwise.
 * ===========================================================================*/
void can_commonConfirmTxMsg(const CANtxMsgStaticData* transmitting_msg, \
							uintNat_t check_msg_id, uint32_t txd_msg_id){

	if(transmitting_msg != NULL){
		// Confirm message only if the transmitted ID matches in case
		// check_msg_id is set true. If check_msg_id is false then
		// last transmission will be confirmed but no TX callback triggered.
		if(!check_msg_id || txd_msg_id == transmitting_msg->id){
			transmitting_msg->dyn->state = kCANtxState_transmitted;
			// Invoke TX callback if defined and check_msg_id is true
			if((check_msg_id) && (transmitting_msg->tx_callback != NULL)){
				(transmitting_msg->tx_callback)();
			}
			// Clears transmitting message pointer
			transmitting_msg = NULL;
		}
	}
}
