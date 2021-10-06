/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		USER_comgen_CAN_DCM_PS_hal.h                                  */
/** \brief     	Header file for CAN Hardware Abstraction Layer functions.
 *  \details   	Contains delcarations of functions for CAN HAL integration.
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
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): 2021.10.5::23:23:57
 * Generated from following source(s):
 *     Network file: "G:\devproj\github\calvos_0_4\calvos\calvos-engine\calvos\..
                      demo\usr_in\template - CAN Network Definition.ods"
 *     Network name: "CAN-B"
 *     Network id: "B"
 *     Network date: "12/Mar/2021"
 *     Network version: "2"
 -----------------------------------------------------------------------------*/
#ifndef USER_COMGEN_CAN_DCM_PS_HAL_H
#define USER_COMGEN_CAN_DCM_PS_HAL_H

#include "calvos.h"
#include "comgen_CAN_common.h"
#include "comgen_CAN_network.h"

/* CAN HAL functions */
extern CalvosError can_DCM_PS_HALtransmitMsg(const CANtxMsgStaticData* msg_info);
extern void can_DCM_PS_HALconfirmTxMsg(void);
extern void can_DCM_PS_HALreceiveMsg(uint32_t msg_id, uint8_t* data_in, uint8_t data_len);
extern void can_DCM_PS_HALinit(void);

#endif /* USER_COMGEN_CAN_DCM_PS_HAL_H */
