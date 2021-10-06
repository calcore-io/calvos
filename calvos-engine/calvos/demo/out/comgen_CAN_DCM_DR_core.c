/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		comgen_CAN_DCM_DR_core.c                                      */
/** \brief     	Source file for CAN Core functionality.
 *  \details   	Implements the CAN core functionality for the handling of the
 *  			defined messages/signals in the given network and given node.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-15
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
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): 2021.10.6::0:10:26
 * Generated from following source(s):
 *     Network file: "G:\devproj\github\calvos_0_4\calvos\calvos-engine\calvos\..
                      demo\usr_in\template - CAN Network Definition.ods"
 *     Network name: "CAN-B"
 *     Network id: "B"
 *     Network date: "12/Mar/2021"
 *     Network version: "2"
 -----------------------------------------------------------------------------*/
#include <string.h>
#include "calvos.h"
#include "comgen_CAN_DCM_DR_core.h"
#include "comgen_CAN_network.h"
#include "comgen_CAN_DCM_DR_callbacks.h"
#include "comgen_CAN_DCM_DR_hal.h"

/* Exported Function Prototypes */

/* Private Function Prototypes */

/* Private macro definitions */

/* Private type definitions */

/* Exported data */

/* Rx message data buffer */
uint8_t can_DCM_DR_RxDataBuffer[kCAN_DCM_DR_RxMsgsTotalLen];

/* Rx available flags buffer */
uint8_t can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_len];

/* RX available flags */
/* RX signal availability flag indexes */
#define kCAN_DCM_DR_sig_avlbl_idx_BCM_5_MC             (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_SysPwrMode           (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_BCM_5_CMAC           (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_BCM_2_MC             (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_ConfortCmd           (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_BCM_2_CMAC           (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_VehSpeed             (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_BRAKE_2_MC           (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_BRAKE_2_CRC          (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_ShiftLeverPos        (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_TRANSM_2_MC          (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_TRANSM_2_CRC         (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_ShiftLeverEng        (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_TRANSM_4_MC          (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_TRANSM_4_CRC         (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_WindowPos      (0u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_LockingReq     (1u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_WindowOp       (2u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_DoorLockSts    (3u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_MC             (4u)
#define kCAN_DCM_DR_sig_avlbl_idx_DCU_2_CRC            (5u)

/* Array of Rx messages dynamic data */
CANrxMsgDynamicData can_DCM_DR_rxMsgDynamicData[kCAN_DCM_DR_nOfRxMsgs];

/* Array of Rx messages static data */
const CANrxMsgStaticData can_DCM_DR_rxMsgStaticData[kCAN_DCM_DR_nOfRxMsgs] = {\
		{kCAN_msgId_TRANSM_2,	kCAN_DCM_DR_msgTimeout_TRANSM_2,	&can_DCM_DR_TRANSM_2_rx_callback,	&can_DCM_DR_TRANSM_2_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[0],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_TRANSM_2],	kCAN_DCM_DR_avlbl_slot_len_TRANSM_2,	 \
				&can_DCM_DR_rxMsgDynamicData[0],	{8u,	kFalse},	NULL,	NULL}, \
		{kCAN_msgId_TRANSM_4,	kCAN_DCM_DR_msgTimeout_TRANSM_4,	&can_DCM_DR_TRANSM_4_rx_callback,	&can_DCM_DR_TRANSM_4_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[8],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_TRANSM_4],	kCAN_DCM_DR_avlbl_slot_len_TRANSM_4,	 \
				&can_DCM_DR_rxMsgDynamicData[1],	{8u,	kFalse},	&can_DCM_DR_rxMsgStaticData[0],	&can_DCM_DR_rxMsgStaticData[2]}, \
		{kCAN_msgId_BCM_5,	kCAN_DCM_DR_msgTimeout_BCM_5,	&can_DCM_DR_BCM_5_rx_callback,	&can_DCM_DR_BCM_5_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[16],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_BCM_5],	kCAN_DCM_DR_avlbl_slot_len_BCM_5,	 \
				&can_DCM_DR_rxMsgDynamicData[2],	{8u,	kFalse},	NULL,	NULL}, \
		{kCAN_msgId_DCU_2,	kCAN_DCM_DR_msgTimeout_DCU_2,	&can_DCM_DR_DCU_2_rx_callback,	&can_DCM_DR_DCU_2_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[24],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_DCU_2],	kCAN_DCM_DR_avlbl_slot_len_DCU_2,	 \
				&can_DCM_DR_rxMsgDynamicData[3],	{8u,	kFalse},	&can_DCM_DR_rxMsgStaticData[1],	&can_DCM_DR_rxMsgStaticData[5]}, \
		{kCAN_msgId_BCM_2,	kCAN_DCM_DR_msgTimeout_BCM_2,	&can_DCM_DR_BCM_2_rx_callback,	&can_DCM_DR_BCM_2_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[32],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_BCM_2],	kCAN_DCM_DR_avlbl_slot_len_BCM_2,	 \
				&can_DCM_DR_rxMsgDynamicData[4],	{8u,	kFalse},	NULL,	NULL}, \
		{kCAN_msgId_BRAKE_2,	kCAN_DCM_DR_msgTimeout_BRAKE_2,	&can_DCM_DR_BRAKE_2_rx_callback,	&can_DCM_DR_BRAKE_2_timeout_callback,	 \
				&can_DCM_DR_RxDataBuffer[40],	&can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_idx_BRAKE_2],	kCAN_DCM_DR_avlbl_slot_len_BRAKE_2,	 \
				&can_DCM_DR_rxMsgDynamicData[5],	{8u,	kFalse},	&can_DCM_DR_rxMsgStaticData[4],	NULL}};

/* Rx search tree starting index */
#define kCAN_DCM_DR_RxSearchStartIdx		(3u)

/* Tx message data buffer */
uint8_t can_DCM_DR_TxDataBuffer[kCAN_DCM_DR_TxMsgsTotalLen];

/* Array of Tx messages dynamic data */
CANtxMsgDynamicData can_DCM_DR_txMsgDynamicData[kCAN_DCM_DR_nOfTxMsgs];

/* Tx transmission queue */
CANtxQueue can_DCM_DR_txQueue;

/* Message being transmitted */
const CANtxMsgStaticData* can_DCM_DR_transmittingMsg;

/* Array of Tx messages static data */
const CANtxMsgStaticData can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_nOfTxMsgs] = {\
		{kCAN_msgId_DCU_1,	kCAN_msgTxPeriod_DCU_1,	&can_DCM_DR_DCU_1_tx_callback,	 \
				&can_DCM_DR_TxDataBuffer[0],	&can_DCM_DR_txMsgDynamicData[0],	{8u,	kFalse,	kCAN_msgTxType_DCU_1}}};

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* ===========================================================================*/
/** Function for processing reception of a CAN msg.
 *
 * Verifies if received message belongs to this node and if so, signal its
 * reception and invokes the corresponding callback.
 * This function is typically called from the CAN RX ISR.
 *
 * @param msg_id 	Id of the received message.
 * @param data_in 	Pointer to the message's received data.
 *
 * IMPORTANT: This function needs to be called by user's application upon
 * indication of successful reception by the CAN HAL (typically within ISR
 * context).
 * ===========================================================================*/
void can_DCM_DR_processRxMessage(uint32_t msg_id, uint8_t * data_in, uint8_t data_len){
	const CANrxMsgStaticData* msg_static_data;
	// Search for message to see if its suscribed by this node.
	msg_static_data = can_traverseRxSearchTree(msg_id, \
			&can_DCM_DR_rxMsgStaticData[kCAN_DCM_DR_RxSearchStartIdx],  \
			kCAN_DCM_DR_nOfRxMsgs);
	if(msg_static_data != NULL){
		// Consider message as valid if it matches the expected length
		if(data_len == msg_static_data->fields.len){
			if(data_len != 0){
				// Copy data to buffer
				memcpy(msg_static_data->data, data_in, msg_static_data->fields.len);
				// Set Signals available flags
				if(msg_static_data->sig_avlbl_buf_len == 1){
					*msg_static_data->sig_avlbl_flags = kAllOnes8;
				}else{
					for(uint32_t i=0; i < msg_static_data->sig_avlbl_buf_len; i++){
						msg_static_data->sig_avlbl_flags[i] = kAllOnes8;
					}
				}
			}

			// Set Message available flags
			msg_static_data->dyn->available.all = kAllOnes32;

			// clear timeout flag
			msg_static_data->dyn->timedout = kFalse;
			// Invoke rx callback
			if(msg_static_data->rx_callback != NULL){
				(msg_static_data->rx_callback)();
			}
		}
	}
}
	

/* ===========================================================================*/
/** Function for cyclic processing of CAN RX messages.
 *
 * Keeps track of message timeouts if defined for this node.
 * IMPORTANT: This function needs to be called by user's application in a
 * periodic task with the period indicated in this function's name.
 * ===========================================================================*/
void can_task_20ms_DCM_DR_rxProcess(void){

	// TODO: Implement this as a timer wheel for efficiency
	// TODO: Consider separating data structures for timeout so that memory for that
	// is only used for messages with defined timeout and not for all.

	for(uint32_t i = 0u; i < kCAN_DCM_DR_nOfRxMsgs; i++){
		// Check if message has a timeout defined
		if(can_DCM_DR_rxMsgStaticData[i].timeout > 0u){
			// Only increment timer if message hasn't timed out
			CALVOS_CRITICAL_ENTER();
			if(!can_DCM_DR_rxMsgStaticData[i].dyn->timedout){
				CALVOS_CRITICAL_EXIT();
				// Increment timer (up-counter since they are initialized with zero)
				can_DCM_DR_rxMsgStaticData[i].dyn->timeout_timer++;
				if(can_DCM_DR_rxMsgStaticData[i].dyn->timeout_timer \
				>= can_DCM_DR_rxMsgStaticData[i].timeout){
					// Set timeout flag
					CALVOS_CRITICAL_ENTER();
					can_DCM_DR_rxMsgStaticData[i].dyn->timedout = kTrue;
					CALVOS_CRITICAL_EXIT();
					// Call timeout callback if not NULL
					if(can_DCM_DR_rxMsgStaticData[i].timeout_callback != NULL){
						(can_DCM_DR_rxMsgStaticData[i].timeout_callback)();
					}
					// Reset timer
					can_DCM_DR_rxMsgStaticData[i].dyn->timeout_timer = 0u;
				}
			}else{
				CALVOS_CRITICAL_EXIT();
			}
		}
	}

}

/* ===========================================================================*/
/** Function for transmitting a CAN msg.
 *
 * Triggers the transmission of the specified CAN message.
 *
 * @param msg_idx 	index of the message to be transmitted.
 * @Return 		Returns @c kNoError if message was triggered for transmission
 * 				by HAL or if it was successfully queued for a transmission
 * 				retry. Returns @c kError otherwise (HAL busy, queue full, wrong
 * 				message index provided).
 * ===========================================================================*/
CalvosError can_DCM_DR_transmitMsg(CAN_DCM_DR_txMsgs msg_idx){
	CalvosError return_value = kError;

	// Trigger CAN transmission to HAL
	if(msg_idx < kCAN_DCM_DR_nOfTxMsgs){
		return_value = can_commonTransmitMsg(&can_DCM_DR_txMsgStaticData[msg_idx], \
								  &can_DCM_DR_txQueue, \
								  &can_DCM_DR_HALtransmitMsg, \
								  &can_DCM_DR_transmittingMsg);
	}

	return return_value;
}
	

/* ===========================================================================*/
/** Function for processing cyclic CAN TX messages.
 *
 * Triggers the transmission of CAN messages set as cyclic or cyclic+spontan.
 * IMPORTANT: This function needs to be called by user's application in a
 * periodic task with the period indicated in this function's name.
 * ===========================================================================*/
void can_task_10ms_DCM_DR_txProcess(void){

	// TODO: Implement this as a timer wheel for efficiency
	// TODO: Group tx messages with same period in single timers rather than
	// individual ones for efficiency.

	// Increment timer (up-counter since they are initialized with zero)
	can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_txMsgIdx_DCU_1].dyn->period_timer++;
	if(can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_txMsgIdx_DCU_1].dyn->period_timer \
	>= can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_txMsgIdx_DCU_1].period){
		// Timer expired, trigger message transmission
		can_DCM_DR_transmitMsg(kCAN_DCM_DR_txMsgIdx_DCU_1);
		// Reset timer
		can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_txMsgIdx_DCU_1].dyn->period_timer = 0u;
	}

}

/* ===========================================================================*/
/** Function for processing CAN TX retry mechanism.
 *
 * Triggers transmission of queued CAN messages (retry mechanism).
 * IMPORTANT: This function needs to be called by user's application either in
 * a periodic task or right after a CAN transmission within ISR context.
 * ===========================================================================*/
void can_DCM_DR_txRetry(void){
	const CANtxMsgStaticData* msg_to_retry;
	CalvosError return_value;

	// TODO: Implement max retries per message, etc.?
	// TODO: Implement an option to also "freeze" the data to the message that
	// was queued. Right now, the data will be taken from common buffer when the
	// message finally is accepted for transmission.

	msg_to_retry = can_txQueueGetHead(&can_DCM_DR_txQueue);
	if(msg_to_retry != NULL){
		// Attempt the re-transmission
		return_value = can_commonTransmitMsg(msg_to_retry, \
								  NULL, \
								  &can_DCM_DR_HALtransmitMsg, \
								  &can_DCM_DR_transmittingMsg);
		if(return_value == kNoError){
			// Transmission succeeded. Dequeue the message.
			can_txQueueDequeue(&can_DCM_DR_txQueue, NULL);
		}
	}
}

/* ===========================================================================*/
/** Set signals initial values.
 *
 * Initializes the signals with defined initial value if this is different than
 * zero (all signal data buffers are previously initialized with zeros).
 * ===========================================================================*/
void can_DCM_DR_signalsInit(void){
	// TODO: implement this function
}

/* ===========================================================================*/
/** Function for initialization of CAN core.
 *
 * Initializes data for CAN core functionality of current node.
 * IMPORTANT: This function needs to be called by user's application during SW
 * initialization phase.
 * ===========================================================================*/
void can_DCM_DR_coreInit(void){
	// Clear RX data buffer
	memset(&can_DCM_DR_RxDataBuffer,kCAN_RxDataInitVal,kCAN_DCM_DR_RxMsgsTotalLen);
	// Clear RX available flags buffer
	memset(&can_DCM_DR_avlbl_buffer,0u,kCAN_DCM_DR_avlbl_buffer_len);
	// Clear TX data buffer
	memset(&can_DCM_DR_TxDataBuffer,kCAN_TxDataInitVal,kCAN_DCM_DR_TxMsgsTotalLen);
	// Clear RX dynamic data
	memset(&can_DCM_DR_rxMsgDynamicData,0u,sizeof(CANrxMsgDynamicData)*(kCAN_DCM_DR_nOfRxMsgs));
	// Clear TX dynamic data
	memset(&can_DCM_DR_txMsgDynamicData,0u,sizeof(CANtxMsgDynamicData)*(kCAN_DCM_DR_nOfTxMsgs));

	// Init signal values
	can_DCM_DR_signalsInit();

	// Init TX queue
	can_txQueueInit(&can_DCM_DR_txQueue);

	// Init transmitting message
	can_DCM_DR_transmittingMsg = NULL;

	// Init CAN HAL
	can_DCM_DR_HALinit();
}
