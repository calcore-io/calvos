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
		project = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_proj_pickle_file, e))
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
/** \brief     	Header file for CAN core functionality.
 *  \details   	Declares functions and macros for the CAN core functionality of
 *  			a given network and a given node.
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

/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Rx data buffer
	# --------------
	sym_rx_data_name = "can_" + net_name_str +  node_name_str + "RxDataBuffer"
	sym_rx_data_len = "kCAN_" + net_name_str +  node_name_str + "RxMsgsTotalLen"
	code_str = "extern "+cg.get_dtv(8)+" "+sym_rx_data_name+"["+sym_rx_data_len+"];"
	cog.outl(code_str)

	# Rx available buffer
	# -------------------
	sym_avlbl_buffer_name = "can_" + net_name_str + node_name_str + "avlbl_buffer"
	sym_avlbl_buff_len = "kCAN_" + net_name_str + "avlbl_buffer_len"
	code_str = "extern " + cg.get_dtv(8) + " " \
			   + sym_avlbl_buffer_name + "[" + sym_avlbl_buff_len + "];"
	cog.outl(code_str)

	# Rx static data
	# --------------
	sym_rx_stat_data_type = "const CANrxMsgStaticData"
	sym_rx_msgs = "kCAN_" + net_name_str + node_name_str + "nOfRxMsgs"
	sym_rx_stat_data_name = "can_" + net_name_str + node_name_str \
			+ "rxMsgStaticData"
	code_str = "extern "+sym_rx_stat_data_type+" "+sym_rx_stat_data_name+"["+sym_rx_msgs+"];"
	cog.outl(code_str)

if len(list_of_tx_msgs) > 0:
	# Tx data buffer
	# --------------
	sym_tx_data_name = "can_" + net_name_str +  node_name_str + "TxDataBuffer"
	sym_tx_data_len = "kCAN_" + net_name_str +  node_name_str + "TxMsgsTotalLen"
	code_str = "extern "+cg.get_dtv(8)+" "+sym_tx_data_name+"["+sym_tx_data_len+"];"
	cog.outl(code_str)

	# Tx static data
	# --------------
	sym_tx_stat_data_type = "const CANtxMsgStaticData"
	sym_tx_stat_data_name = "can_" + net_name_str + node_name_str + "txMsgStaticData"
	sym_tx_stat_data_len = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"
	code_str = "extern "+sym_tx_stat_data_type+" "+sym_tx_stat_data_name+"["+sym_tx_stat_data_len+"];"
	cog.outl(code_str)


	# Tx transmitting message
	# -----------------------
	sym_txing_msg_name = "can_" + net_name_str +  node_name_str + "transmittingMsg"
	sym_txing_msg_type = "const CANtxMsgStaticData*"
	code_str = "extern "+sym_txing_msg_type+" "+sym_txing_msg_name+";"
	cog.outl(code_str)

cog.outl("")
if len(list_of_rx_msgs) > 0:
	# RX Processing Function
	# ----------------------
	sym_rx_proc_func_name = "can_" + net_name_str + node_name_str + "processRxMessage"
	sym_rx_proc_func_args = "(uint32_t msg_id, uint8_t * data_in, uint8_t data_len)"
	sym_rx_proc_func_return = "void"

	code_str = "extern "+sym_rx_proc_func_return+" "+sym_rx_proc_func_name+sym_rx_proc_func_args+";"
	cog.outl(code_str)

if len(list_of_tx_msgs) > 0:
	# TX transmit Function
	# ----------------------
	sym_enum_name = "CAN_" + net_name_str + node_name_str + "txMsgs"

	sym_transmit_func_name = "can_"+net_name_str+node_name_str+"transmitMsg"
	sym_transmit_func_args = "("+sym_enum_name+" msg_idx)"
	sym_transmit_func_return = "CalvosError"

	code_str = "extern "+sym_transmit_func_return+" "+sym_transmit_func_name+sym_transmit_func_args+";"
	cog.outl(code_str)

	cog.outl("")

	# TX Processing Function
	# ----------------------
	tx_proc_task = network.get_simple_param("CAN_tx_task_period")

	sym_tx_proc_func_name = "can_task_"+str(tx_proc_task)+"ms_"+net_name_str+node_name_str+"txProcess"

	code_str = "extern void "+sym_tx_proc_func_name+"(void);"
	cog.outl(code_str)

	# TX Retry Function
	# ----------------------
	sym_tx_retry_func_name = "can_"+net_name_str+node_name_str+"txRetry"

	code_str = "extern void "+sym_tx_retry_func_name+"(void);"
	cog.outl(code_str)

# Init signals function
sym_init_sigs_ret = "void"
sym_init_sigs_name = "can_"+net_name_str+node_name_str+"signalsInit"
sym_init_sigs_args = "(void)"
code_str = "extern "+sym_init_sigs_ret+" "+sym_init_sigs_name+sym_init_sigs_args+";"
cog.outl(code_str)

# Core init function
sym_core_init_name = "can_"+net_name_str+node_name_str+"coreInit"
code_str = "extern void "+sym_core_init_name+"(void);"
cog.outl(code_str)
 ]]] */
// [[[end]]]



/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
