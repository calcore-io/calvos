/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		comgen_CAN_node_network.c
 *  \brief     	Source file for CAN network definitions of a Node.
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
 *  CalvOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CalvOS is distributed in the hope that it will be useful,
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

#TODO: Following line is windows dependant, make it OS independant (care on backslash)
cog_serialized_network_file = str(project_working_dir) + "\\" + str(cog_pickle_file_name)

try:
	with open(cog_serialized_network_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_serialized_network_file, e)) 
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
#include "comgen_CAN_network.h"
#include "comgen_CAN_callbacks.h"
#include <cstring>  /* For inclusion of the memcpy function */

/* Buffer to contain all data for the messages being handled by the Master node */
T_UBYTE CAN_msgs_data_buffer[CAN_MSGS_TOTAL_DLEN];

/* Static Information of messages for the LIN master */
const S_CAN_MSG_STATIC_DATA kCAN_msgs_static_data[E_CAN_N_OF_MSGS] = {
	/* msgID, msgDir, msgDataLen, MsgCallback, MsgBuffDataStartPtr	*/
	/* [[[cog
	data_start_byte = 0
	count = 0
	for message in network.messages.values():
		count += 1
		
		code_string = "{ CAN_MSG_ID_" + message.name + ",\tTX,\tCAN_MSG_DLEN_" + message.name \
		+ ",\tcanil_callback_" + message.name \
		+ ",\t&CAN_msgs_data_buffer[" + str(data_start_byte) + "]}"
		
		data_start_byte += message.len
		
		if count != len(network.messages):
			#if this is not the last item, append a comma to the code string
			code_string += ",\t// Info for msg: "+ message.name
			
		else:
			#This is the last item, do not append a comma
			code_string += "\t// Info for msg: "+ message.name
		
		cog.outl(code_string)
	]]] */
	// [[[end]]]
};


/***************************************************************************//**
 * Brief function description, needs to end with a dot.
 *
 * Detailed function description of what the function does or other useful 
 * information. This description can span to multiple lines.
 *
 * @param paramname	Param description that can also span to multiple lines
 *					if required.
 * @return Description of what is returned.
 ******************************************************************************/
void linmstr_RxGetMsgData(E_LINMSTR_MSGS le_msg, T_UBYTE * lpub_data)
{
	/* TODO: Enter critical section */
	memcpy(lpub_data,CAN_msgs_static_data[le_msg].MsgBuffDataStartPtr,CAN_msgs_static_data[le_msg].msgDataLen);
	/* TODO: Exit critical section */
}

/***************************************************************************//**
 * Brief function description, needs to end with a dot.
 *
 * Detailed function description of what the function does or other useful 
 * information. This description can span to multiple lines.
 *
 * @param paramname	Param description that can also span to multiple lines
 *					if required.
 * @return Description of what is returned.
 ******************************************************************************/
void linmstr_TxUpdateMsgData(E_LINMSTR_MSGS le_msg, T_UBYTE * lpub_data)
{
	/* TODO: Enter critical section */
	memcpy(CAN_msgs_static_data[le_msg].MsgBuffDataStartPtr,lpub_data,CAN_msgs_static_data[le_msg].msgDataLen);
	/* TODO: Exit critical section */
}

/***************************************************************************//**
 * Brief function description, needs to end with a dot.
 *
 * Detailed function description of what the function does or other useful 
 * information. This description can span to multiple lines.
 *
 * @param paramname	Param description that can also span to multiple lines
 *					if required.
 * @return Description of what is returned.
 ******************************************************************************/
void linmstr_clearDataBuffer()
{
	T_UWORD i;
	for(i=0; i<CAN_MSGS_TOTAL_DLEN; i++)
	{
		CAN_msgs_data_buffer[i] = 0;
	}
}
