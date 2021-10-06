/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		comgen_CAN_network.h                                          */
/** \brief     	Header file CAN Signals/messages definitions.
 *  \details   	Contains data structures to ease the handling of the CAN
 *				signals/messages for a given network.
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
#ifndef COMGEN_CAN_NETWORK_H
#define COMGEN_CAN_NETWORK_H

#include "calvos.h"

/* -------------------------------------------------------------------------- */
// 		Network Messages
/* -------------------------------------------------------------------------- */
/* Message(s) IDs */
#define kCAN_msgId_BCM_5       (0x110u)
#define kCAN_msgId_BCM_2       (0x252u)
#define kCAN_msgId_BRAKE_2     (0x342u)
#define kCAN_msgId_TRANSM_2    (0x102u)
#define kCAN_msgId_TRANSM_4    (0x104u)
#define kCAN_msgId_DCU_1       (0x201u)
#define kCAN_msgId_DCU_2       (0x202u)
#define kCAN_msgId_DCU_3       (0x203u)
#define kCAN_msgId_DCU_4       (0x204u)

/* Message(s) length */
#define kCAN_msgLen_BCM_5       (8u)
#define kCAN_msgLen_BCM_2       (8u)
#define kCAN_msgLen_BRAKE_2     (8u)
#define kCAN_msgLen_TRANSM_2    (8u)
#define kCAN_msgLen_TRANSM_4    (8u)
#define kCAN_msgLen_DCU_1       (8u)
#define kCAN_msgLen_DCU_2       (8u)
#define kCAN_msgLen_DCU_3       (8u)
#define kCAN_msgLen_DCU_4       (8u)

/* Message(s) Transmission types */
#define kCAN_msgTxType_BCM_5       (kTxCyclic)
#define kCAN_msgTxType_BCM_2       (kTxCyclic)
#define kCAN_msgTxType_BRAKE_2     (kTxCyclic)
#define kCAN_msgTxType_TRANSM_2    (kTxCyclic)
#define kCAN_msgTxType_TRANSM_4    (kTxCyclic)
#define kCAN_msgTxType_DCU_1       (kTxCyclic)
#define kCAN_msgTxType_DCU_2       (kTxCyclic)
#define kCAN_msgTxType_DCU_3       (kTxCyclic)
#define kCAN_msgTxType_DCU_4       (kTxCyclic)

/* Message(s) Transmission Period (in ticks of task) */
/* Task period is defined in config parameter 'CAN_tx_task_period' */
#define kCAN_msgTxPeriod_BCM_5       (50u)
#define kCAN_msgTxPeriod_BCM_2       (50u)
#define kCAN_msgTxPeriod_BRAKE_2     (10u)
#define kCAN_msgTxPeriod_TRANSM_2    (2u)
#define kCAN_msgTxPeriod_TRANSM_4    (2u)
#define kCAN_msgTxPeriod_DCU_1       (10u)
#define kCAN_msgTxPeriod_DCU_2       (10u)
#define kCAN_msgTxPeriod_DCU_3       (10u)
#define kCAN_msgTxPeriod_DCU_4       (10u)

/* Message(s) Raw Data Initial Values */
#define kCAN_TxDataInitVal		(0x00u)
#define kCAN_RxDataInitVal		(0x00u)


/* -------------------------------------------------------------------------- */
// 		Signal TYPES
/* -------------------------------------------------------------------------- */

typedef enum{
	kCAN_B_PM_SNA = 0,
	kCAN_B_OFF = 1,
	kCAN_B_ACC = 2,
	kCAN_B_RUN = 3,
	kCAN_B_CRANK = 4
}t_SysPwrMode;

typedef enum{
	kCAN_B_No_Cmd,
	kCAN_B_LockCmd,
	kCAN_B_UnlockAllCmd,
	kCAN_B_UnlockDrvrCmd
}t_ConfortCmd;

typedef enum{
	kCAN_B_PARK = 0,
	kCAN_B_DRIVE_1 = 1,
	kCAN_B_DRIVE_2 = 2,
	kCAN_B_DRIVE_3 = 3,
	kCAN_B_NEUTRAL = 4,
	kCAN_B_MANUAL = 5,
	kCAN_B_REVERSE = 6,
	kCAN_B_SNA = 7
}t_ShiftLeverPos;

typedef enum{
	kCAN_B_SHIFT_LEVER_NOT_ENGAGED,
	kCAN_B_SHIFT_LEVER_ENGAGED
}t_ShiftLeverEng;

typedef enum{
	kCAN_B_IN_BETWEEN,
	kCAN_B_COMPLETELY_OPEN,
	kCAN_B_COMPLETELY_CLOSED,
	kCAN_B_ERROR
}t_WindowPos;

typedef enum{
	kCAN_B_NO_LOCKING_REQ,
	kCAN_B_LOCK_REQ,
	kCAN_B_UNLOCK_REQ
}t_LockingReq;

typedef enum{
	kCAN_B_WINDOW_IDLE,
	kCAN_B_WINDOW_UP,
	kCAN_B_WINDOW_DOWN
}t_WindowOp;

typedef enum{
	kCAN_B_DOOR_LOCK,
	kCAN_B_DOOR_UNLOCK,
	kCAN_B_DOOR_UNKNOWN,
	kCAN_B_DOOR_ERROR
}t_DoorLockSts;

typedef enum{
	kCAN_B_WINDOW_NO_REQ,
	kCAN_B_WINDOW_UP_REQ,
	kCAN_B_WINDOW_DOWN_REQ
}t_WindowControl;

typedef enum{
	kCAN_B_REAR_WINDOW_UNBLOCK,
	kCAN_B_REAR_WINDOW_BLOCK
}t_RearWindowLock;


/* -------------------------------------------------------------------------- */
// 		Message structures (layout of signals)
/* -------------------------------------------------------------------------- */


/* Structure for network "B" message "BCM_2".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t BCM_2_MC;
		uint8_t reserved_0;
		uint8_t ConfortCmd;
		uint8_t reserved_1;
		uint32_t BCM_2_CMAC;
	} s;
	uint8_t all[kCAN_msgLen_BCM_2];
}S_BCM_2;

/* Macros for reading/writing signals of network "B" message "BCM_2". */

/* Macros for reading signal "BCM_2_MC". */
#define CAN_extract_BCM_2_MC(msg_buffer)            (msg_buffer[0])
#define CAN_get_BCM_2_MC(msg_buffer)                (CAN_extract_BCM_2_MC(msg_buffer.all))
/* Macros for writing signal "BCM_2_MC". */
#define CAN_write_BCM_2_MC(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_BCM_2_MC(msg_buffer,data)        CAN_write_BCM_2_MC(msg_buffer.all,data)

/* Macros for reading signal "ConfortCmd". */
#define CAN_extract_ConfortCmd(msg_buffer)            (msg_buffer[2])
#define CAN_get_ConfortCmd(msg_buffer)                (CAN_extract_ConfortCmd(msg_buffer.all))
/* Macros for writing signal "ConfortCmd". */
#define CAN_write_ConfortCmd(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_ConfortCmd(msg_buffer,data)        CAN_write_ConfortCmd(msg_buffer.all,data)

/* Macros for reading signal "BCM_2_CMAC". */
#define CAN_extract_BCM_2_CMAC(msg_buffer)            ((uint32_t)(*(uint32_t*)&msg_buffer[4]))
#define CAN_get_BCM_2_CMAC(msg_buffer)                (CAN_extract_BCM_2_CMAC(msg_buffer.all))
/* Macros for writing signal "BCM_2_CMAC". */
#define CAN_write_BCM_2_CMAC(msg_buffer,data)         *(uint32_t*)&msg_buffer[4] = (data);
#define CAN_update_BCM_2_CMAC(msg_buffer,data)        CAN_write_BCM_2_CMAC(msg_buffer.all,data)


/* Structure for network "B" message "BCM_5".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t BCM_5_MC;
		uint8_t reserved_0;
		uint8_t reserved_1;
		uint8_t SysPwrMode;
		uint32_t BCM_5_CMAC;
	} s;
	uint8_t all[kCAN_msgLen_BCM_5];
}S_BCM_5;

/* Macros for reading/writing signals of network "B" message "BCM_5". */

/* Macros for reading signal "BCM_5_MC". */
#define CAN_extract_BCM_5_MC(msg_buffer)            (msg_buffer[0])
#define CAN_get_BCM_5_MC(msg_buffer)                (CAN_extract_BCM_5_MC(msg_buffer.all))
/* Macros for writing signal "BCM_5_MC". */
#define CAN_write_BCM_5_MC(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_BCM_5_MC(msg_buffer,data)        CAN_write_BCM_5_MC(msg_buffer.all,data)

/* Macros for reading signal "SysPwrMode". */
#define CAN_extract_SysPwrMode(msg_buffer)            (msg_buffer[3])
#define CAN_get_SysPwrMode(msg_buffer)                (CAN_extract_SysPwrMode(msg_buffer.all))
/* Macros for writing signal "SysPwrMode". */
#define CAN_write_SysPwrMode(msg_buffer,data)         msg_buffer[3] = (data);
#define CAN_update_SysPwrMode(msg_buffer,data)        CAN_write_SysPwrMode(msg_buffer.all,data)

/* Macros for reading signal "BCM_5_CMAC". */
#define CAN_extract_BCM_5_CMAC(msg_buffer)            ((uint32_t)(*(uint32_t*)&msg_buffer[4]))
#define CAN_get_BCM_5_CMAC(msg_buffer)                (CAN_extract_BCM_5_CMAC(msg_buffer.all))
/* Macros for writing signal "BCM_5_CMAC". */
#define CAN_write_BCM_5_CMAC(msg_buffer,data)         *(uint32_t*)&msg_buffer[4] = (data);
#define CAN_update_BCM_5_CMAC(msg_buffer,data)        CAN_write_BCM_5_CMAC(msg_buffer.all,data)


/* Structure for network "B" message "BRAKE_2".*/
typedef union{
	struct __attribute__((packed)){
		uint32_t reserved_0 : 16;
		uint32_t VehSpeed : 10;
		uint32_t reserved_1 : 6;
		uint32_t reserved_2 : 16;
		uint32_t BRAKE_2_MC : 8;
		uint32_t BRAKE_2_CRC : 8;
	} s;
	uint8_t all[kCAN_msgLen_BRAKE_2];
}S_BRAKE_2;

/* Macros for reading/writing signals of network "B" message "BRAKE_2". */

/* Macros for reading signal "VehSpeed". */
#define CAN_extract_VehSpeed(msg_buffer)            ((uint16_t)((*(uint16_t*)&msg_buffer[2]) & 0x3ffu))
#define CAN_get_VehSpeed(msg_buffer)                (CAN_extract_VehSpeed(msg_buffer.all))
/* Macros for writing signal "VehSpeed". */
#define CAN_write_VehSpeed(msg_buffer,data)         *(uint16_t*)&msg_buffer[2] = (*(uint16_t*)&msg_buffer[2] & 0xfc00u) | ((data) & 0x3ffu);
#define CAN_update_VehSpeed(msg_buffer,data)        CAN_write_VehSpeed(msg_buffer.all,data)

/* Macros for reading signal "BRAKE_2_MC". */
#define CAN_extract_BRAKE_2_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_BRAKE_2_MC(msg_buffer)                (CAN_extract_BRAKE_2_MC(msg_buffer.all))
/* Macros for writing signal "BRAKE_2_MC". */
#define CAN_write_BRAKE_2_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_BRAKE_2_MC(msg_buffer,data)        CAN_write_BRAKE_2_MC(msg_buffer.all,data)

/* Macros for reading signal "BRAKE_2_CRC". */
#define CAN_extract_BRAKE_2_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_BRAKE_2_CRC(msg_buffer)                (CAN_extract_BRAKE_2_CRC(msg_buffer.all))
/* Macros for writing signal "BRAKE_2_CRC". */
#define CAN_write_BRAKE_2_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_BRAKE_2_CRC(msg_buffer,data)        CAN_write_BRAKE_2_CRC(msg_buffer.all,data)


/* Structure for network "B" message "DCU_1".*/
typedef union{
	struct __attribute__((packed)){
		uint32_t DCU_1_WindowPos : 8;
		uint32_t DCU_1_LockingReq : 8;
		uint32_t DCU_1_WindowOp : 8;
		uint32_t DCU_1_RearWindowLock : 8;
		uint32_t DCU_1_DoorLockSts : 8;
		uint32_t DCU_1_WdwCtrl_RR : 2;
		uint32_t DCU_1_WdwCtrl_RL : 2;
		uint32_t DCU_1_WdwCtrl_Pas : 2;
		uint32_t reserved_0 : 2;
		uint32_t DCU_1_MC : 8;
		uint32_t DCU_1_CRC : 8;
	} s;
	uint8_t all[kCAN_msgLen_DCU_1];
}S_DCU_1;

/* Macros for reading/writing signals of network "B" message "DCU_1". */

/* Macros for reading signal "DCU_1_WindowPos". */
#define CAN_extract_DCU_1_WindowPos(msg_buffer)            (msg_buffer[0])
#define CAN_get_DCU_1_WindowPos(msg_buffer)                (CAN_extract_DCU_1_WindowPos(msg_buffer.all))
/* Macros for writing signal "DCU_1_WindowPos". */
#define CAN_write_DCU_1_WindowPos(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_DCU_1_WindowPos(msg_buffer,data)        CAN_write_DCU_1_WindowPos(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_LockingReq". */
#define CAN_extract_DCU_1_LockingReq(msg_buffer)            (msg_buffer[1])
#define CAN_get_DCU_1_LockingReq(msg_buffer)                (CAN_extract_DCU_1_LockingReq(msg_buffer.all))
/* Macros for writing signal "DCU_1_LockingReq". */
#define CAN_write_DCU_1_LockingReq(msg_buffer,data)         msg_buffer[1] = (data);
#define CAN_update_DCU_1_LockingReq(msg_buffer,data)        CAN_write_DCU_1_LockingReq(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_WindowOp". */
#define CAN_extract_DCU_1_WindowOp(msg_buffer)            (msg_buffer[2])
#define CAN_get_DCU_1_WindowOp(msg_buffer)                (CAN_extract_DCU_1_WindowOp(msg_buffer.all))
/* Macros for writing signal "DCU_1_WindowOp". */
#define CAN_write_DCU_1_WindowOp(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_DCU_1_WindowOp(msg_buffer,data)        CAN_write_DCU_1_WindowOp(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_RearWindowLock". */
#define CAN_extract_DCU_1_RearWindowLock(msg_buffer)            (msg_buffer[3])
#define CAN_get_DCU_1_RearWindowLock(msg_buffer)                (CAN_extract_DCU_1_RearWindowLock(msg_buffer.all))
/* Macros for writing signal "DCU_1_RearWindowLock". */
#define CAN_write_DCU_1_RearWindowLock(msg_buffer,data)         msg_buffer[3] = (data);
#define CAN_update_DCU_1_RearWindowLock(msg_buffer,data)        CAN_write_DCU_1_RearWindowLock(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_DoorLockSts". */
#define CAN_extract_DCU_1_DoorLockSts(msg_buffer)            (msg_buffer[4])
#define CAN_get_DCU_1_DoorLockSts(msg_buffer)                (CAN_extract_DCU_1_DoorLockSts(msg_buffer.all))
/* Macros for writing signal "DCU_1_DoorLockSts". */
#define CAN_write_DCU_1_DoorLockSts(msg_buffer,data)         msg_buffer[4] = (data);
#define CAN_update_DCU_1_DoorLockSts(msg_buffer,data)        CAN_write_DCU_1_DoorLockSts(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_WdwCtrl_RR". */
#define CAN_extract_DCU_1_WdwCtrl_RR(msg_buffer)            ((msg_buffer[5]) & 0x03u)
#define CAN_get_DCU_1_WdwCtrl_RR(msg_buffer)                (CAN_extract_DCU_1_WdwCtrl_RR(msg_buffer.all))
/* Macros for writing signal "DCU_1_WdwCtrl_RR". */
#define CAN_write_DCU_1_WdwCtrl_RR(msg_buffer,data)         msg_buffer[5] = (msg_buffer[5] & 0xfcu) | ((data) & 0x03u);
#define CAN_update_DCU_1_WdwCtrl_RR(msg_buffer,data)        CAN_write_DCU_1_WdwCtrl_RR(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_WdwCtrl_RL". */
#define CAN_extract_DCU_1_WdwCtrl_RL(msg_buffer)            (((msg_buffer[5]) >> 2u) & 0x03u)
#define CAN_get_DCU_1_WdwCtrl_RL(msg_buffer)                (CAN_extract_DCU_1_WdwCtrl_RL(msg_buffer.all))
/* Macros for writing signal "DCU_1_WdwCtrl_RL". */
#define CAN_write_DCU_1_WdwCtrl_RL(msg_buffer,data)         msg_buffer[5] = (msg_buffer[5] & 0xf3u) | ((data << 2u) & 0x0cu);
#define CAN_update_DCU_1_WdwCtrl_RL(msg_buffer,data)        CAN_write_DCU_1_WdwCtrl_RL(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_WdwCtrl_Pas". */
#define CAN_extract_DCU_1_WdwCtrl_Pas(msg_buffer)            (((msg_buffer[5]) >> 4u) & 0x03u)
#define CAN_get_DCU_1_WdwCtrl_Pas(msg_buffer)                (CAN_extract_DCU_1_WdwCtrl_Pas(msg_buffer.all))
/* Macros for writing signal "DCU_1_WdwCtrl_Pas". */
#define CAN_write_DCU_1_WdwCtrl_Pas(msg_buffer,data)         msg_buffer[5] = (msg_buffer[5] & 0xcfu) | ((data << 4u) & 0x30u);
#define CAN_update_DCU_1_WdwCtrl_Pas(msg_buffer,data)        CAN_write_DCU_1_WdwCtrl_Pas(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_MC". */
#define CAN_extract_DCU_1_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_DCU_1_MC(msg_buffer)                (CAN_extract_DCU_1_MC(msg_buffer.all))
/* Macros for writing signal "DCU_1_MC". */
#define CAN_write_DCU_1_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_DCU_1_MC(msg_buffer,data)        CAN_write_DCU_1_MC(msg_buffer.all,data)

/* Macros for reading signal "DCU_1_CRC". */
#define CAN_extract_DCU_1_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_DCU_1_CRC(msg_buffer)                (CAN_extract_DCU_1_CRC(msg_buffer.all))
/* Macros for writing signal "DCU_1_CRC". */
#define CAN_write_DCU_1_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_DCU_1_CRC(msg_buffer,data)        CAN_write_DCU_1_CRC(msg_buffer.all,data)


/* Structure for network "B" message "DCU_2".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t DCU_2_WindowPos;
		uint8_t DCU_2_LockingReq;
		uint8_t DCU_2_WindowOp;
		uint8_t reserved_0;
		uint8_t DCU_2_DoorLockSts;
		uint8_t reserved_1;
		uint8_t DCU_2_MC;
		uint8_t DCU_2_CRC;
	} s;
	uint8_t all[kCAN_msgLen_DCU_2];
}S_DCU_2;

/* Macros for reading/writing signals of network "B" message "DCU_2". */

/* Macros for reading signal "DCU_2_WindowPos". */
#define CAN_extract_DCU_2_WindowPos(msg_buffer)            (msg_buffer[0])
#define CAN_get_DCU_2_WindowPos(msg_buffer)                (CAN_extract_DCU_2_WindowPos(msg_buffer.all))
/* Macros for writing signal "DCU_2_WindowPos". */
#define CAN_write_DCU_2_WindowPos(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_DCU_2_WindowPos(msg_buffer,data)        CAN_write_DCU_2_WindowPos(msg_buffer.all,data)

/* Macros for reading signal "DCU_2_LockingReq". */
#define CAN_extract_DCU_2_LockingReq(msg_buffer)            (msg_buffer[1])
#define CAN_get_DCU_2_LockingReq(msg_buffer)                (CAN_extract_DCU_2_LockingReq(msg_buffer.all))
/* Macros for writing signal "DCU_2_LockingReq". */
#define CAN_write_DCU_2_LockingReq(msg_buffer,data)         msg_buffer[1] = (data);
#define CAN_update_DCU_2_LockingReq(msg_buffer,data)        CAN_write_DCU_2_LockingReq(msg_buffer.all,data)

/* Macros for reading signal "DCU_2_WindowOp". */
#define CAN_extract_DCU_2_WindowOp(msg_buffer)            (msg_buffer[2])
#define CAN_get_DCU_2_WindowOp(msg_buffer)                (CAN_extract_DCU_2_WindowOp(msg_buffer.all))
/* Macros for writing signal "DCU_2_WindowOp". */
#define CAN_write_DCU_2_WindowOp(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_DCU_2_WindowOp(msg_buffer,data)        CAN_write_DCU_2_WindowOp(msg_buffer.all,data)

/* Macros for reading signal "DCU_2_DoorLockSts". */
#define CAN_extract_DCU_2_DoorLockSts(msg_buffer)            (msg_buffer[4])
#define CAN_get_DCU_2_DoorLockSts(msg_buffer)                (CAN_extract_DCU_2_DoorLockSts(msg_buffer.all))
/* Macros for writing signal "DCU_2_DoorLockSts". */
#define CAN_write_DCU_2_DoorLockSts(msg_buffer,data)         msg_buffer[4] = (data);
#define CAN_update_DCU_2_DoorLockSts(msg_buffer,data)        CAN_write_DCU_2_DoorLockSts(msg_buffer.all,data)

/* Macros for reading signal "DCU_2_MC". */
#define CAN_extract_DCU_2_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_DCU_2_MC(msg_buffer)                (CAN_extract_DCU_2_MC(msg_buffer.all))
/* Macros for writing signal "DCU_2_MC". */
#define CAN_write_DCU_2_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_DCU_2_MC(msg_buffer,data)        CAN_write_DCU_2_MC(msg_buffer.all,data)

/* Macros for reading signal "DCU_2_CRC". */
#define CAN_extract_DCU_2_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_DCU_2_CRC(msg_buffer)                (CAN_extract_DCU_2_CRC(msg_buffer.all))
/* Macros for writing signal "DCU_2_CRC". */
#define CAN_write_DCU_2_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_DCU_2_CRC(msg_buffer,data)        CAN_write_DCU_2_CRC(msg_buffer.all,data)


/* Structure for network "B" message "DCU_3".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t DCU_3_WindowPos;
		uint8_t reserved_0;
		uint8_t DCU_3_WindowOp;
		uint8_t reserved_1;
		uint8_t DCU_3_DoorLockSts;
		uint8_t reserved_2;
		uint8_t DCU_3_MC;
		uint8_t DCU_3_CRC;
	} s;
	uint8_t all[kCAN_msgLen_DCU_3];
}S_DCU_3;

/* Macros for reading/writing signals of network "B" message "DCU_3". */

/* Macros for reading signal "DCU_3_WindowPos". */
#define CAN_extract_DCU_3_WindowPos(msg_buffer)            (msg_buffer[0])
#define CAN_get_DCU_3_WindowPos(msg_buffer)                (CAN_extract_DCU_3_WindowPos(msg_buffer.all))
/* Macros for writing signal "DCU_3_WindowPos". */
#define CAN_write_DCU_3_WindowPos(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_DCU_3_WindowPos(msg_buffer,data)        CAN_write_DCU_3_WindowPos(msg_buffer.all,data)

/* Macros for reading signal "DCU_3_WindowOp". */
#define CAN_extract_DCU_3_WindowOp(msg_buffer)            (msg_buffer[2])
#define CAN_get_DCU_3_WindowOp(msg_buffer)                (CAN_extract_DCU_3_WindowOp(msg_buffer.all))
/* Macros for writing signal "DCU_3_WindowOp". */
#define CAN_write_DCU_3_WindowOp(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_DCU_3_WindowOp(msg_buffer,data)        CAN_write_DCU_3_WindowOp(msg_buffer.all,data)

/* Macros for reading signal "DCU_3_DoorLockSts". */
#define CAN_extract_DCU_3_DoorLockSts(msg_buffer)            (msg_buffer[4])
#define CAN_get_DCU_3_DoorLockSts(msg_buffer)                (CAN_extract_DCU_3_DoorLockSts(msg_buffer.all))
/* Macros for writing signal "DCU_3_DoorLockSts". */
#define CAN_write_DCU_3_DoorLockSts(msg_buffer,data)         msg_buffer[4] = (data);
#define CAN_update_DCU_3_DoorLockSts(msg_buffer,data)        CAN_write_DCU_3_DoorLockSts(msg_buffer.all,data)

/* Macros for reading signal "DCU_3_MC". */
#define CAN_extract_DCU_3_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_DCU_3_MC(msg_buffer)                (CAN_extract_DCU_3_MC(msg_buffer.all))
/* Macros for writing signal "DCU_3_MC". */
#define CAN_write_DCU_3_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_DCU_3_MC(msg_buffer,data)        CAN_write_DCU_3_MC(msg_buffer.all,data)

/* Macros for reading signal "DCU_3_CRC". */
#define CAN_extract_DCU_3_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_DCU_3_CRC(msg_buffer)                (CAN_extract_DCU_3_CRC(msg_buffer.all))
/* Macros for writing signal "DCU_3_CRC". */
#define CAN_write_DCU_3_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_DCU_3_CRC(msg_buffer,data)        CAN_write_DCU_3_CRC(msg_buffer.all,data)


/* Structure for network "B" message "DCU_4".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t DCU_4_WindowPos;
		uint8_t reserved_0;
		uint8_t DCU_4_WindowOp;
		uint8_t reserved_1;
		uint8_t DCU_4_DoorLockSts;
		uint8_t reserved_2;
		uint8_t DCU_4_MC;
		uint8_t DCU_4_CRC;
	} s;
	uint8_t all[kCAN_msgLen_DCU_4];
}S_DCU_4;

/* Macros for reading/writing signals of network "B" message "DCU_4". */

/* Macros for reading signal "DCU_4_WindowPos". */
#define CAN_extract_DCU_4_WindowPos(msg_buffer)            (msg_buffer[0])
#define CAN_get_DCU_4_WindowPos(msg_buffer)                (CAN_extract_DCU_4_WindowPos(msg_buffer.all))
/* Macros for writing signal "DCU_4_WindowPos". */
#define CAN_write_DCU_4_WindowPos(msg_buffer,data)         msg_buffer[0] = (data);
#define CAN_update_DCU_4_WindowPos(msg_buffer,data)        CAN_write_DCU_4_WindowPos(msg_buffer.all,data)

/* Macros for reading signal "DCU_4_WindowOp". */
#define CAN_extract_DCU_4_WindowOp(msg_buffer)            (msg_buffer[2])
#define CAN_get_DCU_4_WindowOp(msg_buffer)                (CAN_extract_DCU_4_WindowOp(msg_buffer.all))
/* Macros for writing signal "DCU_4_WindowOp". */
#define CAN_write_DCU_4_WindowOp(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_DCU_4_WindowOp(msg_buffer,data)        CAN_write_DCU_4_WindowOp(msg_buffer.all,data)

/* Macros for reading signal "DCU_4_DoorLockSts". */
#define CAN_extract_DCU_4_DoorLockSts(msg_buffer)            (msg_buffer[4])
#define CAN_get_DCU_4_DoorLockSts(msg_buffer)                (CAN_extract_DCU_4_DoorLockSts(msg_buffer.all))
/* Macros for writing signal "DCU_4_DoorLockSts". */
#define CAN_write_DCU_4_DoorLockSts(msg_buffer,data)         msg_buffer[4] = (data);
#define CAN_update_DCU_4_DoorLockSts(msg_buffer,data)        CAN_write_DCU_4_DoorLockSts(msg_buffer.all,data)

/* Macros for reading signal "DCU_4_MC". */
#define CAN_extract_DCU_4_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_DCU_4_MC(msg_buffer)                (CAN_extract_DCU_4_MC(msg_buffer.all))
/* Macros for writing signal "DCU_4_MC". */
#define CAN_write_DCU_4_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_DCU_4_MC(msg_buffer,data)        CAN_write_DCU_4_MC(msg_buffer.all,data)

/* Macros for reading signal "DCU_4_CRC". */
#define CAN_extract_DCU_4_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_DCU_4_CRC(msg_buffer)                (CAN_extract_DCU_4_CRC(msg_buffer.all))
/* Macros for writing signal "DCU_4_CRC". */
#define CAN_write_DCU_4_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_DCU_4_CRC(msg_buffer,data)        CAN_write_DCU_4_CRC(msg_buffer.all,data)


/* Structure for network "B" message "TRANSM_2".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t reserved_0;
		uint8_t reserved_1;
		uint8_t ShiftLeverPos;
		uint8_t reserved_2;
		uint8_t reserved_3;
		uint8_t reserved_4;
		uint8_t TRANSM_2_MC;
		uint8_t TRANSM_2_CRC;
	} s;
	uint8_t all[kCAN_msgLen_TRANSM_2];
}S_TRANSM_2;

/* Macros for reading/writing signals of network "B" message "TRANSM_2". */

/* Macros for reading signal "ShiftLeverPos". */
#define CAN_extract_ShiftLeverPos(msg_buffer)            (msg_buffer[2])
#define CAN_get_ShiftLeverPos(msg_buffer)                (CAN_extract_ShiftLeverPos(msg_buffer.all))
/* Macros for writing signal "ShiftLeverPos". */
#define CAN_write_ShiftLeverPos(msg_buffer,data)         msg_buffer[2] = (data);
#define CAN_update_ShiftLeverPos(msg_buffer,data)        CAN_write_ShiftLeverPos(msg_buffer.all,data)

/* Macros for reading signal "TRANSM_2_MC". */
#define CAN_extract_TRANSM_2_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_TRANSM_2_MC(msg_buffer)                (CAN_extract_TRANSM_2_MC(msg_buffer.all))
/* Macros for writing signal "TRANSM_2_MC". */
#define CAN_write_TRANSM_2_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_TRANSM_2_MC(msg_buffer,data)        CAN_write_TRANSM_2_MC(msg_buffer.all,data)

/* Macros for reading signal "TRANSM_2_CRC". */
#define CAN_extract_TRANSM_2_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_TRANSM_2_CRC(msg_buffer)                (CAN_extract_TRANSM_2_CRC(msg_buffer.all))
/* Macros for writing signal "TRANSM_2_CRC". */
#define CAN_write_TRANSM_2_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_TRANSM_2_CRC(msg_buffer,data)        CAN_write_TRANSM_2_CRC(msg_buffer.all,data)


/* Structure for network "B" message "TRANSM_4".*/
typedef union{
	struct __attribute__((packed)){
		uint8_t reserved_0;
		uint8_t reserved_1;
		uint8_t reserved_2;
		uint8_t reserved_3;
		uint8_t reserved_4;
		uint8_t ShiftLeverEng;
		uint8_t TRANSM_4_MC;
		uint8_t TRANSM_4_CRC;
	} s;
	uint8_t all[kCAN_msgLen_TRANSM_4];
}S_TRANSM_4;

/* Macros for reading/writing signals of network "B" message "TRANSM_4". */

/* Macros for reading signal "ShiftLeverEng". */
#define CAN_extract_ShiftLeverEng(msg_buffer)            (msg_buffer[5])
#define CAN_get_ShiftLeverEng(msg_buffer)                (CAN_extract_ShiftLeverEng(msg_buffer.all))
/* Macros for writing signal "ShiftLeverEng". */
#define CAN_write_ShiftLeverEng(msg_buffer,data)         msg_buffer[5] = (data);
#define CAN_update_ShiftLeverEng(msg_buffer,data)        CAN_write_ShiftLeverEng(msg_buffer.all,data)

/* Macros for reading signal "TRANSM_4_MC". */
#define CAN_extract_TRANSM_4_MC(msg_buffer)            (msg_buffer[6])
#define CAN_get_TRANSM_4_MC(msg_buffer)                (CAN_extract_TRANSM_4_MC(msg_buffer.all))
/* Macros for writing signal "TRANSM_4_MC". */
#define CAN_write_TRANSM_4_MC(msg_buffer,data)         msg_buffer[6] = (data);
#define CAN_update_TRANSM_4_MC(msg_buffer,data)        CAN_write_TRANSM_4_MC(msg_buffer.all,data)

/* Macros for reading signal "TRANSM_4_CRC". */
#define CAN_extract_TRANSM_4_CRC(msg_buffer)            (msg_buffer[7])
#define CAN_get_TRANSM_4_CRC(msg_buffer)                (CAN_extract_TRANSM_4_CRC(msg_buffer.all))
/* Macros for writing signal "TRANSM_4_CRC". */
#define CAN_write_TRANSM_4_CRC(msg_buffer,data)         msg_buffer[7] = (data);
#define CAN_update_TRANSM_4_CRC(msg_buffer,data)        CAN_write_TRANSM_4_CRC(msg_buffer.all,data)


#endif /* COMGEN_CAN_NETWORK_H */
