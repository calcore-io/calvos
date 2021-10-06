/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		comgen_CAN_DCM_DR_core.h                                      */
/** \brief     	Header file for CAN core functionality.
 *  \details   	Declares functions and macros for the CAN core functionality of
 *  			a given network and a given node.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2020-11-15
 *  \copyright 	2020 Carlos Calvillo.
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
#ifndef COMGEN_CAN_DCM_DR_CORE_H
#define COMGEN_CAN_DCM_DR_CORE_H

#include "calvos.h"
#include "comgen_CAN_common.h"
#include "comgen_CAN_network.h"
#include "comgen_CAN_DCM_DR_node_network.h"

extern uint8_t can_DCM_DR_RxDataBuffer[kCAN_DCM_DR_RxMsgsTotalLen];
extern uint8_t can_DCM_DR_avlbl_buffer[kCAN_DCM_DR_avlbl_buffer_len];
extern const CANrxMsgStaticData can_DCM_DR_rxMsgStaticData[kCAN_DCM_DR_nOfRxMsgs];
extern uint8_t can_DCM_DR_TxDataBuffer[kCAN_DCM_DR_TxMsgsTotalLen];
extern const CANtxMsgStaticData can_DCM_DR_txMsgStaticData[kCAN_DCM_DR_nOfTxMsgs];
extern const CANtxMsgStaticData* can_DCM_DR_transmittingMsg;

extern void can_DCM_DR_processRxMessage(uint32_t msg_id, uint8_t * data_in, uint8_t data_len);
extern CalvosError can_DCM_DR_transmitMsg(CAN_DCM_DR_txMsgs msg_idx);

extern void can_task_10ms_DCM_DR_txProcess(void);
extern void can_DCM_DR_txRetry(void);
extern void can_DCM_DR_signalsInit(void);
extern void can_DCM_DR_coreInit(void);



#endif /* COMGEN_CAN_DCM_DR_CORE_H */
