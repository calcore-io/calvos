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
/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e)) 
]]] */
// [[[end]]]
/* [[[cog
# Print generation information
curren_time = time.localtime() # Gets current local time
return_string = str(curren_time[1])+"." \
				+ str(curren_time[2])+"." \
				+ str(curren_time[0])+"::" \
				+ str(curren_time[3])+":" \
				+ str(curren_time[4])+":" \
				+ str(curren_time[5])
cog.outl("/"+chr(42)+"-----------------------------------------------------------------------------")
cog.outl(" * This file was generated on (mm.dd.yyyy::hh:mm:ss): " + return_string)
cog.outl(" * Generated from following source(s):")
cog.outl(" * 	Network file: \"" + str(input_worksheet) + "\"")
cog.outl(" * 	Network name: \"" + str(network.name) + "\"")
cog.outl(" * 	Network date: \"" + str(network.date) + "\"")
cog.outl(" * 	Network version: \"" + str(network.version) + "\"")
cog.outl(" -----------------------------------------------------------------------------"+chr(42)+"/")
]]] */
// [[[end]]]
/* [[[cog
cog.outl("#ifndef COMGEN_CAN_"+network.name.upper()+"_MESSAGES_H")
cog.outl("#define COMGEN_CAN_"+network.name.upper()+"_MESSAGES_H")
]]] */
// [[[end]]]

#include "comgen_CAN_common.h"
#include "comgen_CAN_common_network.h"
/* [[[cog
# Code for individual CAN msgs lenght
code_string = ""
for message in network.messages.values():
    cog.outl("#define CAN_MSG_DLEN_" + message.name + "\t(" + str(message.len) + "u)")
#print(code_string)
]]] */
// [[[end]]]

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

/* [[[cog
# Code for CAN_MSGS_TOTAL_DLEN
code_string = "#define CAN_MSGS_TOTAL_DLEN ("
for message in network.messages.values():
    code_string += "CAN_MSG_DLEN_" + message.name + " + \\ \n\t\t\t"
code_string = code_string[:-8] + ")"
cog.outl(code_string)
]]] */
// [[[end]]]

#if CAN_MSGS_TOTAL_DLEN < 0xFF
	typedef T_UBYTE T_BUFF_SIZE;
#elif CAN_MSGS_TOTAL_DLEN < 0xFFFF
	typedef T_UWORD T_BUFF_SIZE;
#else
	typedef T_ULONG T_BUFF_SIZE;
#endif

/* [[[cog
cog.outl("hoola")
]]] */
// [[[end]]]

typedef enum{
	/* [[[cog
	for message in network.messages:
		cog.outl("CAN_MSG_" + message + ",")
	cog.outl("CAN_MSGS_NUMBER_OF")
	]]] */
	// [[[end]]]
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
		/* [[[cog
		byte_array_lenght = math.ceil(len(network.messages) / 8)
		if byte_array_lenght > 1:
			reserved_bits = (len(network.messages) % 8)
		else:
			reserved_bits = 8 - len(network.messages)
		for message in network.messages:
			cog.outl("T_UBYTE bi_" + message + " : 1;")
		if reserved_bits > 0:	
			cog.outl("T_UBYTE reserved: " + str(reserved_bits) + ";")
		]]] */
		// [[[end]]]
	}b;
	/* [[[cog 
	cog.out("T_UBYTE all[" + str(byte_array_lenght) + "];")
	]]]*/
	/*[[[end]]]*/
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
