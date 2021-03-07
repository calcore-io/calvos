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
/** \brief     	Source file for CAN Hardware Abstraction Layer functions.
 *  \details   	Defines functions that need to be filled with user code in order
 *  			to integrate the CAN interaction layer with the hardware
 *  			abstraction layer in the target MCU.
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
if NWID_wildcard != "None":
	net_name_str = network_name + "_"
else:
	net_name_str = ""
if NODEID_wildcard != "None":
	node_name_str = node_name + "_"
else:
	node_name_str = ""

# Get subnetwork for this node
subnet = network.get_subnetwork([node_name])

list_of_tx_msgs = []
list_of_rx_msgs = []
for message in subnet.messages.values():
	if subnet.get_message_direction(node_name,message.name) == nw.CAN_TX:
		list_of_tx_msgs.append(message.name)
	elif subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
		list_of_rx_msgs.append(message.name)
	else:
		log_warn(("Message '%s' direction not determined for node '%s' " \
			+ "in network '%s'.") % (message.name, node_name, network_name))
 ]]] */
// [[[end]]]

/* Macro for confirming TX msg ID */
/* This macro is generated based on parameter "CAN_tx_confirm_msg_id" */
/* [[[cog
#  TX confirm Msg ID
# ------------------
sym_tx_id_needed_name = "CAN_" + net_name_str + node_name_str + "CONFIRM_MSG_ID"
tx_confirm_msg_id = network.get_simple_param("CAN_tx_confirm_msg_id")
if tx_confirm_msg_id is True:
	sym_tx_id_needed_val = "kTrue"
else:
	sym_tx_id_needed_val = "kFalse"

cog.outl("#define "+sym_tx_id_needed_name+"\t\t"+sym_tx_id_needed_val)
 ]]] */
// [[[end]]]

/* [[[cog
# HAL get transmitted ID function prototype
cog.outl("#if "+sym_tx_id_needed_name+"==kTrue")

sym_get_tx_id_return = "uint32_t"
sym_get_tx_id_name = "can_"+net_name_str+node_name_str+"HALgetTxdMsgId"
sym_get_tx_id_args = "(void)"

code_str = sym_get_tx_id_return+" "+sym_get_tx_id_name+sym_get_tx_id_args+";"
cog.outl(code_str)

cog.outl("#endif")
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
sym_hal_transmit_name = "can_"+net_name_str+node_name_str+"HALtransmitMsg"
sym_hal_transmit_args = "(const CANtxMsgStaticData* msg_info)"

code_str = sym_hal_transmit_return+" "+sym_hal_transmit_name+sym_hal_transmit_args+"{\n"
cog.outl(code_str)
sym_can_transmit_body="""
	CalvosError return_value = kError;
	// Write HAL code to transmit a CAN message. Information about the message
	// can be extracted from the provided msg_info structure.
	#error "User code needed here. Remove this line when done."

	return return_value;
"""
sym_can_transmit_body = sym_can_transmit_body[1:]+"}"
cog.outl(sym_can_transmit_body)
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for getting the id of the message just transmitted by the HAL.
 *
 * This function returns the id of the CAN message just transmitted by the
 * target CAN HAL. This function is invoked by the TX confirmation function
 * "can_NWID_NODEID_HALconfirmTxMsg".
 * ===========================================================================*/
/* [[[cog
# This function has a conditional compilation directive
cog.outl("#if "+sym_tx_id_needed_name+"==kTrue")

code_str = sym_get_tx_id_return+" "+sym_get_tx_id_name+sym_get_tx_id_args+"{\n"
cog.outl(code_str)

sym_get_tx_id_body="""
	uint32_t txd_msg_id;
	// Write user code to return the ID of the CAN message just transmitted
	// by the CAN HAL.
	#error "User code needed here. Remove this line when done."

	return txd_msg_id;
"""
sym_get_tx_id_body = sym_get_tx_id_body[1:]+"}"
cog.outl(sym_get_tx_id_body)

# Close conditional compilation directive
cog.outl("#endif")
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for confirming transmission of CAN message for given node.
 *
 * This function shall be called when the target MCU confirms the transmission
 * of the lastly requested tx message.
 * ===========================================================================*/
/* [[[cog
sym_txing_msg_name = "can_" + net_name_str +  node_name_str + "transmittingMsg"

sym_hal_confirm_tx_return = "void"
sym_hal_confirm_tx_name = "can_"+net_name_str+node_name_str+"HALconfirmTxMsg"
sym_hal_confirm_tx_args = "(void)"

code_str = sym_hal_confirm_tx_return+" "+sym_hal_confirm_tx_name+sym_hal_confirm_tx_args+"{\n"
cog.outl(code_str)

sym_hal_confirm_tx_body="""
#if """+sym_tx_id_needed_name+"""==kTrue
	uint32_t txd_msg_id;
	// Get ID of the message just transmitted
	txd_msg_id = """+sym_get_tx_id_name+"""();
	// Confirm TX message if ID matches
	can_commonConfirmTxMsg("""+sym_txing_msg_name+""", kTrue, txd_msg_id);
#else
	can_commonConfirmTxMsg("""+sym_txing_msg_name+""", kFalse, NULL);
#endif
"""
sym_hal_confirm_tx_body = sym_hal_confirm_tx_body[1:]+"}"
cog.outl(sym_hal_confirm_tx_body)
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for processing a received CAN msg from HAL.
 *
 * This hardware-abstraction-layer function shall be called when a CAN message
 * is received in the target MCU.
 *
 * @param msg_id 	ID of the received message.
 * @param data_in	Pointer to the received data.
 * @param data_len	Length of the received data. If it will depend on message
 * 					definition pass this parameter as zero.
 * ===========================================================================*/
/* [[[cog
sym_rx_proc_func_name = "can_" + net_name_str + node_name_str + "processRxMessage"

sym_hal_rx_return = "void"
sym_hal_rx_name = "can_"+net_name_str+node_name_str+"HALreceiveMsg"
sym_hal_rx_args = "(uint32_t msg_id, uint8_t* data_in, uint8_t data_len)"

code_str = sym_hal_rx_return+" "+sym_hal_rx_name+sym_hal_rx_args+"{\n"
cog.outl(code_str)

sym_can_transmit_body="""
	// Call RX processor function
	"""+sym_rx_proc_func_name+"""(msg_id, data_in, data_len);
"""
sym_can_transmit_body = sym_can_transmit_body[1:]+"}"
cog.outl(sym_can_transmit_body)
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for initializing CAN Hardware Abstraction Layer (HAL).
 *
 * This function is called by can_NWID_NODEID_coreInit function and is in charge
 * of initializing the CAN HAL for the target MCU.
 * ===========================================================================*/
/* [[[cog
sym_hal_init_return = "void"
sym_hal_init_name = "can_"+net_name_str+node_name_str+"HALinit"
sym_hal_init_args = "(void)"

code_str = sym_hal_init_return+" "+sym_hal_init_name+sym_hal_init_args+"{\n"
cog.outl(code_str)

sym_can_transmit_body="""
	// Write user code to initialize CAN HAL in the target MCU.
	#error "User code needed here. Remove this line when done."
"""
sym_can_transmit_body = sym_can_transmit_body[1:]+"}"
cog.outl(sym_can_transmit_body)
]]] */
// [[[end]]]
