/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		comgen_CAN_callbacks.c
 *  \brief     	Source file for CAN network callbacks.
 *  \details   	Part of the comgen component. Contains function definitions 
 * 				which are callbacks from the CAN network. These callbacks
 *				include message receptions/transmissions, CAN error callbacks, 
 *				etc.
 *				User is expected to fill-up the contents of these callbacks.
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
#include "LIN_mstr_il.h"
#include "LIN_mstr_core.h"
#include "LIN_mstr_sched.h"

#if COMPILE_LIN_MASTER_NODE

/* ------------------------------------------------------------------------- */
//	LIN Master Node Error Callbacks:
//	Called upon transmission or reception of a CAN msg.
//	These functions are called inside an ISR so shall be short.
/* ------------------------------------------------------------------------- */

void linmstr_timeout_error_callback()
{
	
}
void linmstr_checksum_error_callback()
{
	
}
void linmstr_ID_parity_error_callback()
{
	
}

/* ------------------------------------------------------------------------- */
//	Msg Callbacks:
//	Called upon transmission or reception of a CAN msg.
//	These functions are called inside an ISR so shall be short.
/* ------------------------------------------------------------------------- */
 
/* [[[cog

for message in network.messages:
	# Generate function header comment
	function_header_str = "/" + chr(42) \
		+ "*************************************************************************" \
		+ chr(42)+"//"+chr(42)+"*\n" \
		+ " * Callback function for CAN message " + message + ".\n" \
		+ """ *
 * This function is called when the message is received (if message is Rx) or
 * when the message is transmitted (if the message if Tx).
 * Function is called from ISR.
 *****************************************************************************""" \
		+ chr(42) + "/"
	cog.outl(function_header_str)
	# Generate function header comment
	cog.outl("void can_msg_callback_" + str(message) + "(){\n")
	cog.outl("\t// Place your code here...")
	cog.outl("}\n")
]]] */
// [[[end]]]

#endif
