/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import json
import pathlib as plib

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

from cog_CAN import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project_obj = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = cog_output_file
padding = 80 - (19 + len(cog_output_file))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Source file for CAN user callbacks.
 *  \details   	Implements the CAN core functionality for the handling of the
 *  			defined messages/signals in the given network.
 *  \author    	Carlos Calvillo
 *  \version   	0.1
 *  \date      	2021-02-01
 *  \copyright 	2021 Carlos Calvillo.
 */
/*============================================================================*/
/* [[[cog
cog.outl(cg.C_license())
]]] */
// [[[end]]]
/*============================================================================*/
/* [[[cog

# Print generation information
cog.outl(C_gen_info(input_worksheet,network))
]]] */
// [[[end]]]
/* [[[cog
# Definition of Functions
 ]]] */
// [[[end]]]
#include "calvos.h"
/* [[[cog
# Generate include statements if required

if 'include_var' in locals():
	includes = json.loads(include_var)
	for include in includes:
		cog.outl("#include \"" + include + "\"")
 ]]] */
// [[[end]]]
/* [[[cog
TAB_SPACE = 4
network_name = network.id_string
net_name_str = network_name + "_"
 ]]] */
// [[[end]]]


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
/* [[[cog
sym_hal_transmit_return = "CalvosError"
sym_hal_transmit_name = "can_"+net_name_str+"HALtransmitMsg"
sym_hal_transmit_args = "(CANtxMsgStaticData* msg_info)"

code_str = sym_hal_transmit_return+" "+sym_hal_transmit_name+sym_hal_transmit_args+"{\n"
cog.outl(code_str)
sym_can_transmit_body="""
	CalvosError return_value = kError;
	// Write HAL code to transmit a CAN message. Information about the message
	// can be extracted from the provided msg_info structure.
"""
sym_can_transmit_body = sym_can_transmit_body[1:]+"}"
cog.outl(sym_can_transmit_body)
]]] */
// [[[end]]]
