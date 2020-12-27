/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		CAN_signals.h
 *  \brief     	Header file CAN Signals definitions.
 *  \details   	Contains data structures to ease the handling of the CAN 
 *				signals.
 *  \author    	Carlos Calvillo
 *  \authors	Carlos Calvillo
 *  \version   	1.0
 *  \date      	1990-2011
 *  \copyright 	GNU Public License v3.
 */
/*============================================================================*/
/*  This file is part of calvOS project.
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
 * This file was generated on (mm.dd.yyyy::hh:mm:ss): 12.27.2020::17:19:13
 * Generated from following source(s):
 * 	Network file: "G:\devproj\github\calvos\calvos\project_example\usr_in\CAN_Network_Definition.ods"
 * 	Network name: "TestNW"
 * 	Network date: ""
 * 	Network version: ""
 -----------------------------------------------------------------------------*/
#ifndef COMGEN_CAN_TESTNW_NETWORK_H
#define COMGEN_CAN_TESTNW_NETWORK_H

#include "LIN_common_network.h"

/* -------------------------------------------------------------------------- */
// 		Network Messages
/* -------------------------------------------------------------------------- */
#define CAN_TestNW_MSG_DLEN_MESSAGE1	(8u)
#define CAN_TestNW_MSG_DLEN_MESSAGE2	(8u)
#define CAN_TestNW_MSG_DLEN_MESSAGE3	(5u)
#define CAN_TestNW_MSG_DLEN_MESSAGE4	(8u)
#define CAN_TestNW_MSG_DLEN_MESSAGE5	(8u)
#define CAN_TestNW_MSG_DLEN_Cano_Msg	(7u)
#define CAN_TestNW_MSG_DLEN_Pseudeo_Cano_Msg	(8u)
#define CAN_TestNW_MSG_DLEN_Sig_Macros	(8u)
#define CAN_TestNW_MSG_DLEN_MNU_MSG	(5u)

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

#define CAN_TestNW_MSGS_TOTAL_DLEN (CAN_TestNW_MSG_DLEN_MESSAGE1 + \ 
			CAN_TestNW_MSG_DLEN_MESSAGE2 + \ 
			CAN_TestNW_MSG_DLEN_MESSAGE3 + \ 
			CAN_TestNW_MSG_DLEN_MESSAGE4 + \ 
			CAN_TestNW_MSG_DLEN_MESSAGE5 + \ 
			CAN_TestNW_MSG_DLEN_Cano_Msg + \ 
			CAN_TestNW_MSG_DLEN_Pseudeo_Cano_Msg + \ 
			CAN_TestNW_MSG_DLEN_Sig_Macros + \ 
			CAN_TestNW_MSG_DLEN_MNU_MSG )

/* -------------------------------------------------------------------------- */
// 		Signal TYPES
/* -------------------------------------------------------------------------- */

typedef enum{
	Lock = 0,
	Accessory = 1,
	Run = 2,
	Start = 3,
	SNA = 7
}t_IgnitionState;

typedef enum{
	VALUE1,
	VALUE2,
	VALUE3,
	VALUE4
}t_Type_Signal1;

typedef enum{
	Lock,
	Accessory = 1
}t_IgnitionState2;

typedef enum{
	one,
	two,
	three,
	four,
	five,
	six,
	seven,
	eigth
}t_IgnitionState3;

typedef enum{
	Hola
}t_IgnitionState4;

typedef enum{
	MNU_1,
	MNU_2 = 26,
	MNU_3,
	MNU_4 = 2
}t_MNU;


/* -------------------------------------------------------------------------- */
// 		Message structures (layout of signals)
/* -------------------------------------------------------------------------- */


/* Structure for network "TestNW" message "Cano_Msg".*/
typedef union{
	struct{
		uint16_t var_int16;
		uint8_t var_char;
		uint8_t reserved_0;
		uint8_t var_long32;
		uint8_t reserved_1;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_Cano_Msg];
}S_Cano_Msg;

/* Macros for reading/writing signals of network "TestNW" message "Cano_Msg". */
#define CAN_TestNW_get_var_int16(msg)		( ( ((uint16_t)msg.all[1] ) << 8) \ 
				| ( (uint16_t)msg.all[0] )  )

#define CAN_TestNW_get_var_char(msg)		( (uint8_t)(( (uint8_t)msg.all[2] ) ) )

#define CAN_TestNW_get_var_long32(msg)		( (uint8_t)(( (uint8_t)msg.all[4] ) ) )


/* Structure for network "TestNW" message "MESSAGE1".*/
typedef union{
	struct{
		uint32_t Signal_2A_0 : 8;
		uint32_t Signal_2A_1 : 8;
		uint32_t Signal_2A_2 : 8;
		uint32_t Signal_2A_3 : 8;
		uint32_t reserved_0 : 9;
		uint32_t Signal_3A : 4;
		uint32_t reserved_1 : 3;
		uint32_t Signal_5 : 8;
		uint32_t Signal_1 : 1;
		uint32_t reserved_2 : 4;
		uint32_t Signal_4 : 3;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MESSAGE1];
}S_MESSAGE1;

/* Macros for reading/writing signals of network "TestNW" message "MESSAGE1". */
#define CAN_TestNW_get_ptr_Signal_2A(msg)		( &msg.all[0] )
#define CAN_TestNW_update_Signal_2A(msg, values)		( memcpy(&msg.all[0],&values, CAN_TestNW_MSG_DLEN_MESSAGE1)

#define CAN_TestNW_get_Signal_3A(msg)		( (uint8_t)(( (uint8_t)msg.all[5] >> 1u ) & 0xf) )

#define CAN_TestNW_get_Signal_5(msg)		( (uint8_t)(( (uint8_t)msg.all[6] ) ) )

#define CAN_TestNW_get_Signal_1(msg)		( (uint8_t)(( (uint8_t)msg.all[7] )  & 0x1) )

#define CAN_TestNW_get_Signal_4(msg)		( (uint8_t)(( (uint8_t)msg.all[7] >> 5u ) & 0x7) )


/* Structure for network "TestNW" message "MESSAGE2".*/
typedef union{
	struct{
		uint32_t reserved_0 : 3;
		uint32_t Signal_2 : 5;
		uint32_t reserved_1 : 24;
		uint32_t reserved_2 : 9;
		uint32_t Signal_3 : 4;
		uint32_t reserved_3 : 3;
		uint32_t Signal_5A : 5;
		uint32_t reserved_4 : 3;
		uint32_t Signal_1A : 1;
		uint32_t reserved_5 : 4;
		uint32_t Signal_4A : 3;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MESSAGE2];
}S_MESSAGE2;

/* Macros for reading/writing signals of network "TestNW" message "MESSAGE2". */
#define CAN_TestNW_get_Signal_2(msg)		( (uint8_t)(( (uint8_t)msg.all[0] >> 3u ) & 0x1f) )

#define CAN_TestNW_get_Signal_3(msg)		( (uint8_t)(( (uint8_t)msg.all[5] >> 1u ) & 0xf) )

#define CAN_TestNW_get_Signal_5A(msg)		( (uint8_t)(( (uint8_t)msg.all[6] )  & 0x1f) )

#define CAN_TestNW_get_Signal_1A(msg)		( (uint8_t)(( (uint8_t)msg.all[7] )  & 0x1) )

#define CAN_TestNW_get_Signal_4A(msg)		( (uint8_t)(( (uint8_t)msg.all[7] >> 5u ) & 0x7) )


/* Structure for network "TestNW" message "MESSAGE3".*/
typedef union{
	struct{
		uint8_t SignalC : 1;
		uint8_t SignalD : 1;
		uint8_t SignalF : 1;
		uint8_t SignalE : 1;
		uint8_t reserved_0 : 4;
		uint8_t SignalB : 1;
		uint8_t SignalA : 1;
		uint8_t reserved_1 : 6;
		uint8_t reserved_2 : 8;
		uint8_t reserved_3 : 8;
		uint8_t reserved_4 : 8;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MESSAGE3];
}S_MESSAGE3;

/* Macros for reading/writing signals of network "TestNW" message "MESSAGE3". */
#define CAN_TestNW_get_SignalC(msg)		( (uint8_t)(( (uint8_t)msg.all[0] )  & 0x1) )

#define CAN_TestNW_get_SignalD(msg)		( (uint8_t)(( (uint8_t)msg.all[0] >> 1u ) & 0x1) )

#define CAN_TestNW_get_SignalF(msg)		( (uint8_t)(( (uint8_t)msg.all[0] >> 2u ) & 0x1) )

#define CAN_TestNW_get_SignalE(msg)		( (uint8_t)(( (uint8_t)msg.all[0] >> 3u ) & 0x1) )

#define CAN_TestNW_get_SignalB(msg)		( (uint8_t)(( (uint8_t)msg.all[1] )  & 0x1) )

#define CAN_TestNW_get_SignalA(msg)		( (uint8_t)(( (uint8_t)msg.all[1] >> 1u ) & 0x1) )


/* Structure for network "TestNW" message "MESSAGE4".*/
typedef union{
	struct{
		uint32_t sadfsdf : 2;
		uint32_t reserved_0 : 30;
		uint32_t reserved_1 : 32;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MESSAGE4];
}S_MESSAGE4;

/* Macros for reading/writing signals of network "TestNW" message "MESSAGE4". */
#define CAN_TestNW_get_sadfsdf(msg)		( (uint8_t)(( (uint8_t)msg.all[0] )  & 0x3) )


/* Structure for network "TestNW" message "MESSAGE5".*/
typedef union{
	struct{
		uint32_t Aaddd : 2;
		uint32_t reserved_0 : 30;
		uint32_t reserved_1 : 32;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MESSAGE5];
}S_MESSAGE5;

/* Macros for reading/writing signals of network "TestNW" message "MESSAGE5". */
#define CAN_TestNW_get_Aaddd(msg)		( (uint8_t)(( (uint8_t)msg.all[0] )  & 0x3) )


/* Structure for network "TestNW" message "MNU_MSG".*/
typedef union{
	struct{
		uint8_t MNUsignal_0 : 8;
		uint8_t MNUsignal_1 : 8;
		uint8_t MNUsignal_part_2 : 8;
		uint8_t MNUsignal_3 : 8;
		uint8_t reserved_0 : 7;
		uint8_t MNUsignal_2 : 1;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_MNU_MSG];
}S_MNU_MSG;

/* Macros for reading/writing signals of network "TestNW" message "MNU_MSG". */
#define CAN_TestNW_get_MNUsignal(msg)		( ( ((uint32_t)msg.all[3] ) << 24) \ 
				| ( ((uint32_t)msg.all[2] ) << 16) \ 
				| ( ((uint32_t)msg.all[1] ) << 8) \ 
				| ( (uint32_t)msg.all[0] )  )

#define CAN_TestNW_get_MNUsignal_2(msg)		( (uint8_t)(( (uint8_t)msg.all[4] >> 7u ) & 0x1) )


/* Structure for network "TestNW" message "Pseudeo_Cano_Msg".*/
typedef union{
	struct{
		uint32_t var2_int16 : 16;
		uint32_t var2_char : 8;
		uint32_t reserved_0 : 8;
		uint32_t var2_long32 : 8;
		uint32_t reserved_1 : 18;
		uint32_t var2_bit : 1;
		uint32_t reserved_2 : 5;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_Pseudeo_Cano_Msg];
}S_Pseudeo_Cano_Msg;

/* Macros for reading/writing signals of network "TestNW" message "Pseudeo_Cano_Msg". */
#define CAN_TestNW_get_var2_int16(msg)		( ( ((uint16_t)msg.all[1] ) << 8) \ 
				| ( (uint16_t)msg.all[0] )  )

#define CAN_TestNW_get_var2_char(msg)		( (uint8_t)(( (uint8_t)msg.all[2] ) ) )

#define CAN_TestNW_get_var2_long32(msg)		( (uint8_t)(( (uint8_t)msg.all[4] ) ) )

#define CAN_TestNW_get_var2_bit(msg)		( (uint8_t)(( (uint8_t)msg.all[7] >> 2u ) & 0x1) )


/* Structure for network "TestNW" message "Sig_Macros".*/
typedef union{
	struct{
		uint32_t reserved_0 : 7;
		uint32_t signal_m_1 : 10;
		uint32_t reserved_1 : 14;
		uint32_t signal_m_2_0 : 1;
		uint32_t signal_m_2_1 : 1;
		uint32_t reserved_2 : 4;
		uint32_t signal_m_3 : 23;
		uint32_t reserved_3 : 3;
		uint32_t signal_m_4 : 1;
	} s;
	uint8_t all[CAN_TestNW_MSG_DLEN_Sig_Macros];
}S_Sig_Macros;

/* Macros for reading/writing signals of network "TestNW" message "Sig_Macros". */
#define CAN_TestNW_get_signal_m_1(msg)		( ( ((uint16_t)msg.all[2] & 0x1u) << 9u ) \ 
				| ( ((uint16_t)msg.all[1] & 0x80u) << 1) \ 
				| ( (uint16_t)msg.all[0] >> 7u ) )

#define CAN_TestNW_get_signal_m_2(msg)		( ( ((uint8_t)msg.all[4] & 0x1u) << 1u ) \ 
				| ( (uint8_t)msg.all[3] >> 7u ) )

#define CAN_TestNW_get_signal_m_3(msg)		( ( ((uint32_t)msg.all[7] & 0xfu) << 19u ) \ 
				| ( ((uint32_t)msg.all[6] & 0xe0u) << 11) \ 
				| ( ((uint32_t)msg.all[5] & 0xe0u) << 3) \ 
				| ( (uint32_t)msg.all[4] >> 5u ) )

#define CAN_TestNW_get_signal_m_4(msg)		( (uint8_t)(( (uint8_t)msg.all[7] >> 7u ) & 0x1) )


#endif /* COMGEN_CAN_TESTNW_NETWORK_H*/
