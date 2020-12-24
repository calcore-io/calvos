/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		comgen_CAN_callbacks.H
 *  \brief     	Header file for CAN network callbacks.
 *  \details   	Part of the comgen component. Contains function declarations 
 * 				for callbacks from the CAN network. These callbacks include 
 *				include message receptions/transmissions, CAN error callbacks, 
 *				etc.
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
#ifndef COMGEN_CAN_CALLBACKS_H_
#define COMGEN_CAN_CALLBACKS_H_

#include "comgen_CAN_common.h"
#include "comgen_CAN_network.h"
#include "comgen_CAN_signals.h"

/* [[[cog
for message in network.messages:
	cog.outl("extern void can_msg_callback_" + str(message) + "();")
]]] */
// [[[end]]]

#endif /* COMGEN_CAN_CALLBACKS_H_ */
