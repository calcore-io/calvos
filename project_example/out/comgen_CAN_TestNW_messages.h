/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		LIN_mstr_network.h
 *  \brief     	Header file for CAN network definitions of a Node.
 *  \details   	Part of the comgen component. Contains symbols definitions for 
 * 				CAN messages, signals, etc.
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
#ifndef COMGEN_CAN_TESTNW_MESSAGES_H
#define COMGEN_CAN_TESTNW_MESSAGES_H

#include "comgen_CAN_common.h"
#include "comgen_CAN_common_network.h"
#define CAN_MSG_DLEN_MESSAGE1	(8u)
#define CAN_MSG_DLEN_MESSAGE2	(8u)
#define CAN_MSG_DLEN_MESSAGE3	(5u)
#define CAN_MSG_DLEN_MESSAGE4	(8u)
#define CAN_MSG_DLEN_MESSAGE5	(8u)
#define CAN_MSG_DLEN_Cano_Msg	(7u)
#define CAN_MSG_DLEN_Pseudeo_Cano_Msg	(8u)
#define CAN_MSG_DLEN_Sig_Macros	(8u)
#define CAN_MSG_DLEN_MNU_MSG	(5u)

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

#define CAN_MSGS_TOTAL_DLEN (CAN_MSG_DLEN_MESSAGE1 + \ 
			CAN_MSG_DLEN_MESSAGE2 + \ 
			CAN_MSG_DLEN_MESSAGE3 + \ 
			CAN_MSG_DLEN_MESSAGE4 + \ 
			CAN_MSG_DLEN_MESSAGE5 + \ 
			CAN_MSG_DLEN_Cano_Msg + \ 
			CAN_MSG_DLEN_Pseudeo_Cano_Msg + \ 
			CAN_MSG_DLEN_Sig_Macros + \ 
			CAN_MSG_DLEN_MNU_MSG )

#if CAN_MSGS_TOTAL_DLEN < 0xFF
	typedef T_UBYTE T_BUFF_SIZE;
#elif CAN_MSGS_TOTAL_DLEN < 0xFFFF
	typedef T_UWORD T_BUFF_SIZE;
#else
	typedef T_ULONG T_BUFF_SIZE;
#endif

hoola

typedef enum{
	CAN_MSG_MESSAGE1,
	CAN_MSG_MESSAGE2,
	CAN_MSG_MESSAGE3,
	CAN_MSG_MESSAGE4,
	CAN_MSG_MESSAGE5,
	CAN_MSG_Cano_Msg,
	CAN_MSG_Pseudeo_Cano_Msg,
	CAN_MSG_Sig_Macros,
	CAN_MSG_MNU_MSG,
	CAN_MSGS_NUMBER_OF
}E_CAN_MSGS;

/*
#if E_LIN_N_OF_MSGS < 0xFF
	typedef T_UBYTE T_LINNMSG;
#elif E_LIN_N_OF_MSGS < 0xFFFF
	typedef T_UWORD T_LINNMSG;
#else
	typedef T_ULONG T_LINNMSG;
#endif
*/

typedef union {
	struct{
		T_UBYTE bi_MESSAGE1 : 1;
		T_UBYTE bi_MESSAGE2 : 1;
		T_UBYTE bi_MESSAGE3 : 1;
		T_UBYTE bi_MESSAGE4 : 1;
		T_UBYTE bi_MESSAGE5 : 1;
		T_UBYTE bi_Cano_Msg : 1;
		T_UBYTE bi_Pseudeo_Cano_Msg : 1;
		T_UBYTE bi_Sig_Macros : 1;
		T_UBYTE bi_MNU_MSG : 1;
		T_UBYTE reserved: 1;
	}b;
	T_UBYTE all[2];
}U_CAN_MSG_FLAGS;

U_CAN_MSG_FLAGS ru_LINmsgsStatusFLag;

#define GET_CAN_MSG_MSG_FLAG(msg)			( (ru_LINmsgsStatusFLag.all[((E_CAN_MSG_MSGS)msg >> 3u)] >> ((E_CAN_MSG_MSGS)msg % 8u)) & 1u )
#define SET_CAN_MSG_MSG_FLAG(msg,val)		(val) ? (ru_LINmsgsStatusFLag.all[((E_CAN_MSG_MSGS)msg >> 3u)] |= (T_UBYTE)(1u << ((E_CAN_MSG_MSGS)msg % 8u))) \
												:(ru_LINmsgsStatusFLag.all[((E_CAN_MSG_MSGS)msg >> 3u)] &= ~( (T_UBYTE)(1u << ((E_CAN_MSG_MSGS)msg % 8u))) )

extern const S_LIN_MSG_STATIC_DATA rs_LINMSTRmsgsStaticData[E_LIN_N_OF_MSGS];

extern void linmstr_RxGetMsgData(E_LINMSTR_MSGS le_msg, T_UBYTE * lpub_data);
extern void linmstr_TxUpdateMsgData(E_LINMSTR_MSGS le_msg, T_UBYTE * lpub_data);
extern void linmstr_clearDataBuffer();

#endif /* COMGEN_CAN_NODE_NETWORK_H */
