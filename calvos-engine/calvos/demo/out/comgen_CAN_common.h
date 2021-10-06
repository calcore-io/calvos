/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		comgen_CAN_common.h                                           */
/** \brief     	Header file for common CAN definitions.
 *  \details   	Declares data structures, functions, etc. common for CAN (common
 *  			to multiple networks, nodes, etc.)
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
#ifndef COMGEN_CAN_COMMON_H
#define COMGEN_CAN_COMMON_H

#include "calvos.h"

/* TX Types definitions */
#define kTxSpontan			0u
#define kTxCyclic			1u
#define kTxCyclicSpontan	2u
#define kTxBAF				3u

/* Message direction definitions */
#define kDirTX		0u
#define kDirRX		1u

/* Length of CAN transmission queue */
#define kCANtxQueueLen		(5u)

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
	uint32_t timeout_timer;
	// NodeUint32 timeout_queue;
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

/* Exported Prototypes */
/* ------------------- */
/* Queueing Functions */
extern CalvosError can_txQueueEnqueue(CANtxQueue* queue, const CANtxMsgStaticData* node);
extern const CANtxMsgStaticData* can_txQueueGetHead(CANtxQueue* queue);
extern CalvosError can_txQueueDequeue(CANtxQueue* queue, const CANtxMsgStaticData* node);
extern CalvosError can_txQueueInit(CANtxQueue* queue);

extern void can_clearAllAvlblFlags(const CANrxMsgStaticData* msg_struct);
extern const CANrxMsgStaticData* can_traverseRxSearchTree(uint32_t msg_id, \
		const CANrxMsgStaticData* root, \
		uint32_t guard);

extern CalvosError can_commonTransmitMsg(const CANtxMsgStaticData* msg_struct, \
		  CANtxQueue* queue, \
		  CANhalTxFunction can_hal_tx_function, \
		  const CANtxMsgStaticData** transmitting_msg);

extern void can_commonConfirmTxMsg(const CANtxMsgStaticData* transmitting_msg, \
		uintNat_t check_msg_id, uint32_t txd_msg_id);

#endif /* COMGEN_CAN_COMMON_H */
