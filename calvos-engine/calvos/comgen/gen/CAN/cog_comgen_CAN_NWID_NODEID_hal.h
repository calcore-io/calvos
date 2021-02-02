/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib
import json

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
/** \brief     	Header file CAN Signals definitions.
 *  \details   	Contains data structures to ease the handling of the CAN
 *				signals.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2020-11-15
 *  \copyright 	2020 Carlos Calvillo.
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
# Print include guards
guard_symbol = cog_output_file.replace(".","_")
guard_symbol = guard_symbol.upper()

cog.outl("#ifndef "+guard_symbol)
cog.outl("#define "+guard_symbol)
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

/* CAN HAL functions */
/* [[[cog
# HAL Transmit Function
# ---------------------
sym_hal_transmit_return = "CalvosError"
sym_hal_transmit_name = "can_"+net_name_str+node_name_str+"HALtransmitMsg"
sym_hal_transmit_args = "(CANtxMsgStaticData* msg_info)"

code_str = "extern "+sym_hal_transmit_return+" "+sym_hal_transmit_name+sym_hal_transmit_args+";"
cog.outl(code_str)

# HAL Confirm Transmit Function
# -----------------------------
sym_hal_confirm_tx_return = "void"
sym_hal_confirm_tx_name = "can_"+net_name_str+node_name_str+"HALconfirmTxMsg"
sym_hal_confirm_tx_args = "()"

code_str = "extern "+sym_hal_confirm_tx_return+" "+sym_hal_confirm_tx_name+sym_hal_confirm_tx_args+";"
cog.outl(code_str)

# HAL Receive Function
# --------------------
sym_hal_rx_return = "void"
sym_hal_rx_name = "can_"+net_name_str+node_name_str+"HALreceiveMsg"
sym_hal_rx_args = "(uint32_t msg_id, uint8_t* data_in, uint8_t data_len)"

code_str = "extern "+sym_hal_rx_return+" "+sym_hal_rx_name+sym_hal_rx_args+";"
cog.outl(code_str)
]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
