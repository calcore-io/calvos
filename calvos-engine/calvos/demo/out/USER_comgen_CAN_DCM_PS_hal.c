/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		USER_comgen_CAN_DCM_PS_hal.c                                  */
/** \brief     	Source file for CAN Hardware Abstraction Layer functions.
 *  \details   	Defines functions that need to be filled with user code in order
 *  			to integrate the CAN interaction layer with the hardware
 *  			abstraction layer in the target MCU.
 *  \author    	Carlos Calvillo
 *  \version   	0.1
 *  \date      	2021-02-01
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
#include "calvos.h"
#include "comgen_CAN_common.h"
#include "comgen_CAN_DCM_PS_hal.h"
#include "comgen_CAN_DCM_PS_core.h"

/* Macro for confirming TX msg ID */
/* This macro is generated based on parameter "CAN_tx_confirm_msg_id" */
#define CAN_DCM_PS_CONFIRM_MSG_ID		kTrue

#if CAN_DCM_PS_CONFIRM_MSG_ID==kTrue
uint32_t can_DCM_PS_HALgetTxdMsgId(void);
#endif

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* ===========================================================================*/
/** Function for triggering a CAN msg transmission from HAL.
 *
 * This hardware-abstraction-layer function triggers the transmission of the
 * given CAN message in the target MCU.
 *
 * @param msg_info 	Pointer to the structure containing the message information.
 * @return 	Returns @c kNoError if transmission was accepted by the HAL, returns
 * 			@c kError if it was not accepted.
 * ===========================================================================*/
CalvosError can_DCM_PS_HALtransmitMsg(const CANtxMsgStaticData* msg_info){

	CalvosError return_value = kError;
	// Write HAL code to transmit a CAN message. Information about the message
	// can be extracted from the provided msg_info structure.
	#error "User code needed here. Remove this line when done."

	return return_value;
}

/* ===========================================================================*/
/** Function for getting the id of the message just transmitted by the HAL.
 *
 * This function returns the id of the CAN message just transmitted by the
 * target CAN HAL. This function is invoked by the TX confirmation function
 * "can_NWID_NODEID_HALconfirmTxMsg".
 * ===========================================================================*/
#if CAN_DCM_PS_CONFIRM_MSG_ID==kTrue
uint32_t can_DCM_PS_HALgetTxdMsgId(void){

	uint32_t txd_msg_id;
	// Write user code to return the ID of the CAN message just transmitted
	// by the CAN HAL.
	#error "User code needed here. Remove this line when done."

	return txd_msg_id;
}
#endif

/* ===========================================================================*/
/** Function for confirming transmission of CAN message for given node.
 *
 * This function shall be called when the target MCU confirms the transmission
 * of the lastly requested tx message.
 * ===========================================================================*/
void can_DCM_PS_HALconfirmTxMsg(void){

#if CAN_DCM_PS_CONFIRM_MSG_ID==kTrue
	uint32_t txd_msg_id;
	// Get ID of the message just transmitted
	txd_msg_id = can_DCM_PS_HALgetTxdMsgId();
	// Confirm TX message if ID matches
	can_commonConfirmTxMsg(can_DCM_PS_transmittingMsg, kTrue, txd_msg_id);
#else
	can_commonConfirmTxMsg(can_DCM_PS_transmittingMsg, kFalse, NULL);
#endif
}

/* ===========================================================================*/
/** Function for processing a received CAN msg from HAL.
 *
 * This hardware-abstraction-layer function shall be called when a CAN message
 * is received in the target MCU.
 *
 * @param msg_id 	ID of the received message.
 * @param data_in	Pointer to the received data.
 * @param data_len	Length of the received data. If it will depend on message
 * 					definition pass this parameter as zero.
 * ===========================================================================*/
void can_DCM_PS_HALreceiveMsg(uint32_t msg_id, uint8_t* data_in, uint8_t data_len){

	#warning "This function shall be called in user's CAN HAL rx ISR. Remove this line when done".
	// Call RX processor function
	can_DCM_PS_processRxMessage(msg_id, data_in, data_len);
}

/* ===========================================================================*/
/** Function for initializing CAN Hardware Abstraction Layer (HAL).
 *
 * This function is called by can_NWID_NODEID_coreInit function and is in charge
 * of initializing the CAN HAL for the target MCU.
 * ===========================================================================*/
void can_DCM_PS_HALinit(void){

	// Write user code to initialize CAN HAL in the target MCU.
	#error "User code needed here. Remove this line when done."
}
