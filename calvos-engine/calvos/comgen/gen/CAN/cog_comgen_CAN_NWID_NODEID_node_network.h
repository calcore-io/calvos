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

from cog_CAN import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
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
/** \brief     	Header file CAN Signals/Messages definitions of a given node.
 *  \details   	Declares macros, functions, etc. for the handling of signals and
 *  			messages for a given node taking into account the messages/
 *  			signals direction (TX/RX) with respect to the node.
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
 ]]] */
// [[[end]]]

#include "calvos.h"
#include <string.h>

/* -------------------------------------------------------------------------- */
// 		Network Messages
/* -------------------------------------------------------------------------- */
/* Message(s) direction */
/* [[[cog

list_of_tx_msgs = []
list_of_rx_msgs = []

# Calculate padding spaces
macro_prefix = "kCAN_" + net_name_str + node_name_str + "msg_dir_"
macro_names = []
macro_values = []
for message in subnet.messages.values():
	macro_name = macro_prefix + message.name
	if subnet.get_message_direction(node_name,message.name) == nw.CAN_TX:
		list_of_tx_msgs.append(message.name)
		macro_value = "(kDirTX)"
	elif subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
		list_of_rx_msgs.append(message.name)
		macro_value = "(kDirRX)"
	else:
		macro_value = None
	if macro_value is not None:
		macro_names.append(macro_name)
		macro_values.append(macro_value)

max_len = cg.get_str_max_len(macro_names)
max_len += TAB_SPACE

for i, macro_name in enumerate(macro_names):
	code_string = "#define " + macro_name \
				+ cg.gen_padding(max_len, len(macro_name)) + macro_values[i]
	cog.outl(code_string)

]]] */
// [[[end]]]

/* Total data Length of TX message(s) */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	# Calculate padding spaces
	macro_prefix = "kCAN_" + net_name_str + "msgLen_"
	macro_names = []
	for message_name in list_of_tx_msgs:
		macro_name = macro_prefix + message_name
		macro_names.append(macro_name)
	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	# Total name macro
	macro_name = "kCAN_" + net_name_str + node_name_str + "TxMsgsTotalLen"
	code_string = "#define " + macro_name + cg.gen_padding(TAB_SPACE)
	padding_str = cg.gen_padding(len(code_string))
	code_string += "("
	for i, macro_name in enumerate(macro_names):
		if i < len(macro_names) - 1:
			code_string += macro_name + " \\\n" + padding_str + "+ "
		else:
			code_string += macro_name + ")"

	cog.outl(code_string)
]]] */
// [[[end]]]

/* Total data Length of RX message(s) */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Calculate padding spaces
	macro_prefix = "kCAN_" + net_name_str + "msgLen_"
	macro_names = []
	for message_name in list_of_rx_msgs:
		macro_name = macro_prefix + message_name
		macro_names.append(macro_name)
	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	# Total name macro
	macro_name = "kCAN_" + net_name_str +  node_name_str + "RxMsgsTotalLen"
	code_string = "#define " + macro_name + cg.gen_padding(TAB_SPACE)
	padding_str = cg.gen_padding(len(code_string))
	code_string += "("
	for i, macro_name in enumerate(macro_names):
		if i < len(macro_names) - 1:
			code_string += macro_name + " \\\n" + padding_str + "+ "
		else:
			code_string += macro_name + ")"

	cog.outl(code_string)
]]] */
// [[[end]]]

/* Totals numbers for TX / RX message(s) */
/* [[[cog
macro_name = "kCAN_" + net_name_str +  node_name_str + "NofTxMsgs"
macro_value = str(len(list_of_tx_msgs))
#cog.outl("#define "+macro_name+"\t("+macro_value+")")
macro_name = "kCAN_" + net_name_str +  node_name_str + "NofRxMsgs"
macro_value = str(len(list_of_rx_msgs))
#cog.outl("#define "+macro_name+"\t("+macro_value+")")
]]] */
// [[[end]]]

/* RX message(s) timeout */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Get timeout toleance parameter
	rx_proc_task = subnet.get_simple_param("CAN_rx_task_period")
	rx_timeout_tolerance = subnet.get_simple_param("CAN_rx_timeout_tolerance")
	if rx_timeout_tolerance < 0 or rx_timeout_tolerance > 100:
		rx_timeout_tolerance = 100
		log_warn("Config parameter 'CAN_rx_timeout_tolerance' wrong value '%s'. Assumed 100 (\%)." \
			% str(rx_timeout_tolerance))

	# Calculate padding spaces
	macro_prefix = "kCAN_" + net_name_str + node_name_str + "msgTimeout_"
	macro_names = []
	macro_values = []
	for message in subnet.messages.values():
		macro_name = macro_prefix + message.name
		if subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			message_timeout = subnet.get_message_timeout(node_name,message.name)
			if message_timeout is not None:
				message_timeout = int(message_timeout)
				timeout_in_ticks = round(int(message_timeout)/rx_proc_task)
				macro_value = "(" + str(timeout_in_ticks) + "u)"

				# Check if RX timeout tolerance is met
				upper_tol_perc = (100 + rx_timeout_tolerance) / 100
				lower_tol_perc = (100 - rx_timeout_tolerance) / 100

				if ((timeout_in_ticks * rx_proc_task) > (upper_tol_perc * message_timeout)) or \
				((timeout_in_ticks * rx_proc_task) < (lower_tol_perc * message_timeout)):
					log_warn(("Generated timeout of '%sms' for message '%s' doesn't met tolerance of +/-'%s' percent. " \
							 + "Consider changing period of Rx task in config parameter 'CAN_rx_task_period' " \
							 + "or the tolerance for RX timeouts in parameter 'CAN_rx_timeout_tolerance'.") \
							 % ((timeout_in_ticks * rx_proc_task), message.name, str(rx_timeout_tolerance)))
			else:
				macro_value = "(0u)"
			macro_names.append(macro_name)
			macro_values.append(macro_value)

	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	for i, macro_name in enumerate(macro_names):
		code_string = "#define " + macro_name \
					+ cg.gen_padding(max_len, len(macro_name)) + macro_values[i]
		cog.outl(code_string)

]]] */
// [[[end]]]

/* RX signal availability flag indexes */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Calculate padding spaces
	sym_sig_idx_pfx = "kCAN_" + net_name_str + node_name_str + "sig_avlbl_idx_"
	macro_names = []
	macro_values = []
	avlbl_flags_idx = {}
	for message in subnet.messages.values():
		if subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			message_signals = subnet.get_signals_of_message(message.name)
			index = 0
			for signal in message_signals:
				avlbl_flags_idx.update({signal.name : index})
				macro_name = sym_sig_idx_pfx + signal.name
				macro_value = "(" + str(index) + "u)"
				index += 1
				macro_names.append(macro_name)
				macro_values.append(macro_value)

	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	for i, macro_name in enumerate(macro_names):
		code_string = "#define " + macro_name \
					+ cg.gen_padding(max_len, len(macro_name)) + macro_values[i]
#		cog.outl(code_string)
]]] */
// [[[end]]]

/* RX signal availability flag buffer indexes*/
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Calculate padding spaces
	sym_avlbl_buff_idx_pfx = "kCAN_" + net_name_str + node_name_str + "avlbl_buffer_idx_"
	macro_names = []
	macro_values = []
	avlbl_msg_buff_idx = {}
	avlbl_buff_byte = 0
	for message in subnet.messages.values():
		if subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			avlbl_msg_buff_idx.update({message.name : avlbl_buff_byte})

			macro_name = sym_avlbl_buff_idx_pfx + message.name
			macro_value = "(" + str(avlbl_buff_byte) + "u)"

			message_signals = subnet.get_signals_of_message(message.name)
			avlbl_buff_byte += \
				int(cg.calculate_base_type_len(len(message_signals))/8)

			macro_names.append(macro_name)
			macro_values.append(macro_value)

	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	for i, macro_name in enumerate(macro_names):
		code_string = "#define " + macro_name \
					+ cg.gen_padding(max_len, len(macro_name)) + macro_values[i]
		cog.outl(code_string)
]]] */
// [[[end]]]

/* RX signal availability flags buffer slot sizes */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Calculate padding spaces
	sym_avlbl_size_idx_pfx = "kCAN_" + net_name_str + node_name_str + "avlbl_slot_len_"
	macro_names = []
	macro_values = []
	for message in subnet.messages.values():
		if subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			macro_name = sym_avlbl_size_idx_pfx + message.name

			message_signals = subnet.get_signals_of_message(message.name)
			avlbl_buff_len = \
				int(cg.calculate_base_type_len(len(message_signals))/8)
			macro_value = "(" + str(avlbl_buff_len) + "u)"

			macro_names.append(macro_name)
			macro_values.append(macro_value)

	max_len = cg.get_str_max_len(macro_names)
	max_len += TAB_SPACE

	for i, macro_name in enumerate(macro_names):
		code_string = "#define " + macro_name \
					+ cg.gen_padding(max_len, len(macro_name)) + macro_values[i]
		cog.outl(code_string)
]]] */
// [[[end]]]

/* RX signals availability buffer size */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_avlbl_buff_len = "kCAN_" + net_name_str + node_name_str + "avlbl_buffer_len"
	sym_avlbl_buff_len_val = str(avlbl_buff_byte)

	cog.outl("#define "+sym_avlbl_buff_len+"\t\t("+sym_avlbl_buff_len_val+"u)")
]]] */
// [[[end]]]

/* RX message(s) indexes - sorted by ID -*/
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Sort RX messages by ID to easy rx search later on
	sorted_rx_msgs = {} # Expects {msg_name : msg_id}
	for msg_name in list_of_rx_msgs:
		sorted_rx_msgs.update({msg_name : subnet.messages[msg_name].id})
	sorted_rx_msgs = dict(sorted(sorted_rx_msgs.items(), key=lambda kv: kv[1]))

	# Create RX enumeration
	enum_name = "CAN_" + net_name_str + node_name_str + "rxMsgs"
	cog.outl("typedef enum{")

	sym_rx_idx_pfx = "kCAN_" + net_name_str + node_name_str + "rxMsgIdx_"
	symbol_names = []
	for i, message_name in enumerate(sorted_rx_msgs.keys()):
		if i == 0:
			symbol_name = "\t" + sym_rx_idx_pfx + message_name + "=0,"
		else:
			symbol_name = "\t" + sym_rx_idx_pfx + message_name + ","
		cog.outl(symbol_name)
	cog.outl("\tkCAN_" + net_name_str + node_name_str + "nOfRxMsgs")
	cog.outl("}"+enum_name+";")
]]] */
// [[[end]]]

/* TX message(s) indexes */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	cog.outl("typedef enum{")
	enum_name = "CAN_" + net_name_str + node_name_str + "txMsgs"
	sym_tx_idx_pfx = "kCAN_" + net_name_str + node_name_str + "txMsgIdx_"
	for i, message_name in enumerate(list_of_tx_msgs):
		if i == 0:
			symbol_name = "\t" + sym_tx_idx_pfx + message_name + "=0,"
		else:
			symbol_name = "\t" + sym_tx_idx_pfx + message_name + ","
		cog.outl(symbol_name)
	cog.outl("\tkCAN_" + net_name_str + node_name_str + "nOfTxMsgs")
	cog.outl("}"+enum_name+";")

]]] */
// [[[end]]]

/* TX messages/signals direct access macros */

/* [[[cog
sym_rx_data_name = "can_" + net_name_str +  node_name_str + "RxDataBuffer"
sym_rx_stat_data_name = "can_" + net_name_str + node_name_str \
	+ "rxMsgStaticData"
sym_rx_dyn_data_name = "can_" + net_name_str + node_name_str \
	+ "rxMsgDynamicData"
sym_tx_data_name = "can_" + net_name_str +  node_name_str + "TxDataBuffer"
sym_tx_stat_data_name = "can_" + net_name_str + node_name_str + "txMsgStaticData"

line_sep_str = " \\\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"

if len(subnet.messages) > 0:
	for message in subnet.messages.values():
		message_name = message.name
		message_signals = subnet.get_signals_of_message(message_name)

		if subnet.get_message_direction(node_name,message.name) == nw.CAN_TX:
			msg_is_tx = True

			sym_idx_pfx = sym_tx_idx_pfx+message_name
			msg_stat_data = sym_tx_stat_data_name+"["+sym_idx_pfx+"]"

			dir_str = "Tx"
		elif subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			msg_is_tx = False

			sym_idx_pfx = sym_rx_idx_pfx+message_name
			msg_stat_data = sym_rx_stat_data_name+"["+sym_idx_pfx+"]"

			dir_str = "Rx"
		else:
			msg_is_tx = None
			msg_stat_data = "#error"
			log_warn("Wrong direction for message '%s'.", message_name)

		# Print read macro comment
		cog.outl("/"+chr(42)+" ----------- Macros for "+dir_str+" message \"" + message.name \
				+ "\" ----------- " + chr(42) + "/")

		cog.outl("/"+chr(42)+chr(42)+chr(42)+" Message Level macros " \
			+ chr(42)+chr(42)+chr(42) + "/")
		# Message get macro.
		cog.outl("/"+chr(42)+" Get message data from unified buffer "+chr(42)+"/")
		macro_name = "CAN_" + net_name_str + node_name_str+"get_msg_"+message_name
		macro_args = "(data_struct)"
		macro_vals = "CALVOS_CRITICAL_ENTER();"+line_sep_str+"memcpy(data_struct.all,"+line_sep_str \
			+msg_stat_data+".data,"+line_sep_str \
			+msg_stat_data +".fields.len);"+line_sep_str \
			+"CALVOS_CRITICAL_EXIT();"

		code_str = "#define "+macro_name+macro_args+"\t\t"+macro_vals
		cog.outl(code_str+"\n")

		if msg_is_tx is True:
			# Message update macro is only for TX messages.
			cog.outl("/"+chr(42)+" Update message data to be transmitted "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"update_msg_"+message_name
			macro_args = "(data_struct)"
			macro_vals = "CALVOS_CRITICAL_ENTER();"+line_sep_str+"memcpy(" \
				+msg_stat_data+".data,"+line_sep_str+"data_struct.all,"+line_sep_str \
				+msg_stat_data +".fields.len);"+line_sep_str+"CALVOS_CRITICAL_EXIT();"

			code_str = "#define "+macro_name+macro_args+"\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

			# Get TX Message state macro
			cog.outl("/"+chr(42)+" Get message transmission state macro "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"get_tx_state_"+message_name
			macro_args = "()"
			macro_vals = "("+msg_stat_data+".dyn->state)"

			code_str = "#define "+macro_name+macro_args+"\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

		if msg_is_tx is False:
			# Message get available flags macro.
			cog.outl("/"+chr(42)+" Get message available flags "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"get_msg_avlbl_flags_"+message_name
			macro_vals = "("+ sym_rx_stat_data_name + "[" + sym_rx_idx_pfx \
				+ message_name + "].dyn->available)"
			code_str = "#define "+macro_name+"()"+"\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

			# Message clear available flags macro.
			cog.outl("/"+chr(42)+" Clear message available flags "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"clr_msg_avlbl_flags_"+message_name
			macro_vals = sym_rx_stat_data_name + "[" + sym_rx_idx_pfx \
				+ message_name + "].dyn->available.all = 0u;"
			code_str = "#define "+macro_name+"()"+"\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

			# Message clear all available flags macro.
			cog.outl("/"+chr(42)+" Clear all available flags (message and signals) "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"clr_all_avlbl_flags_"+message_name
			macro_vals = "can_clearAllAvlblFlags(&"+ sym_rx_stat_data_name + "[" + sym_rx_idx_pfx \
				+ message_name + "])"
			code_str = "#define "+macro_name+"()"+"\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

			# Message get timeout flag macro.
			cog.outl("/"+chr(42)+" Get message timeout flag "+chr(42)+"/")
			macro_name = "CAN_" + net_name_str + node_name_str+"get_timeout_flag_"+message_name
			macro_vals = "(" + sym_rx_dyn_data_name + "[" + sym_rx_idx_pfx \
				+ message_name + "].timedout)"
			code_str = "#define "+macro_name+"()"+"\t\t\t"+macro_vals
			cog.outl(code_str)
			cog.outl()

		# Direct signal access macros
		array_str = "msg_buffer"
		data_in_str = "data_in"

		cog.outl("/"+chr(42)+chr(42)+chr(42)+" Signal Level macros " + chr(42) +chr(42)+chr(42)+"/")

		for signal in message_signals:
			# Referenced names
			def_read = "CAN_"+net_name_str+"extract_" \
						+ signal.name + "(" + array_str + ")"
			def_write = "CAN_"+net_name_str+"write_" \
						+ signal.name + "(" + array_str + "," + data_in_str + ")"

			# New macro names
			def_read_array = "CAN_" + net_name_str + node_name_str+"get_ptr_direct_" \
							+ signal.name + "()"
			def_get_direct = "CAN_"+net_name_str+node_name_str+"get_direct_" \
							+ signal.name + "()"

			def_write_array = "CAN_" + net_name_str + node_name_str+"update_ptr_direct_" \
							+ signal.name + "("+ data_in_str + ")"
			def_update_direct = "CAN_"+net_name_str+node_name_str+"update_direct_" \
							+ signal.name + "(" + data_in_str + ")"

			sym_avlb_get = "CAN_" + net_name_str +node_name_str+ "get_avlbl_" \
							+ signal.name + "()"
			sym_avlb_clear = "CAN_" + net_name_str +node_name_str+ "clr_avlbl_" \
							+ signal.name + "()"

			max_len = cg.get_str_max_len([def_read_array, def_get_direct, \
										def_write_array, def_update_direct, \
										sym_avlb_get, sym_avlb_clear])

			# From longest string add TAB_SPACE spaces for padding
			max_len += TAB_SPACE

			# Calculate padding
			pad_read_array = cg.gen_padding(max_len, len(def_read_array))
			pad_get_direct = cg.gen_padding(max_len, len(def_get_direct))

			pad_write_array = cg.gen_padding(max_len, len(def_write_array))
			pad_update_direct = cg.gen_padding(max_len, len(def_update_direct))

			pad_avlb_get = cg.gen_padding(max_len, len(sym_avlb_get))
			pad_avlb_clear = cg.gen_padding(max_len, len(sym_avlb_clear))

			# Print read macro comment
			cog.outl("/"+chr(42)+" Macros for direct reading of signal \"" \
				+ signal.name + "\". " + chr(42) + "/")

			# Signal read direct macros
			# -------------------------
			if signal.is_array():
				cog.outl("#define "+ def_read_array \
						+ pad_read_array \
						+ "(&"+msg_stat_data+".data[" + str(signal.start_byte) + "])")
			else:
				cog.outl("#define "+def_get_direct + pad_get_direct \
					+ "(" + def_read.replace("("+array_str+")","("+msg_stat_data+".data)") + ")" )

			# Signal write direct macros
			# --------------------------
			# Direct writting of signals is only for TX messages.
			if msg_is_tx is True:
				cog.outl("/"+chr(42)+" Macros for direct writing of signal \"" \
					+ signal.name + "\". " + chr(42) + "/")
				if signal.is_array():
					signal_len_in_bytes = int(signal.len/8)
					cog.outl("#define " + def_write_array + pad_write_array \
							+"memcpy(&"+msg_stat_data+".data["+str(signal.start_byte) \
							+ "],"+data_in_str+","+str(signal_len_in_bytes)+")" )
				else:
					cog.outl("#define " + def_update_direct + pad_update_direct \
						+ def_write.replace("("+array_str,"("+msg_stat_data+".data"))

			cog.outl("")
			# Signal available flags macros
			# -----------------------------
			# Available macros are only for RX messages.
			cog.outl("/"+chr(42)+" Macros for available flags handling of signal \"" \
					+ signal.name + "\". " + chr(42) + "/")

			sym_avlbl_buffer_name = "can_" + net_name_str + node_name_str + "avlbl_buffer"

			# Available flags only apply for RX signals
			if msg_is_tx is False:
				working_byte = math.floor(avlbl_flags_idx[signal.name] / 8)
				working_bit = avlbl_flags_idx[signal.name] - (working_byte*8)
				working_clr_mask = cg.get_bit_mask(1, working_bit, True, 8)

				if working_bit > 0:
					macro_value = "((" + sym_avlbl_buffer_name \
						+ "[" + str(avlbl_msg_buff_idx[message.name] + working_byte) + "] >> " \
						+ cg.to_hex_string_with_suffix(working_bit) + ") & 0x01u)"
				else:
					macro_value = "(" + sym_avlbl_buffer_name \
					+ "[" + str(avlbl_msg_buff_idx[message.name] + working_byte) + "] & 0x01u)"

				cog.outl("#define "+sym_avlb_get+pad_avlb_get+macro_value)

				# Clear available macro
				macro_value = "(" + sym_avlbl_buffer_name \
					+ "[" + str(avlbl_msg_buff_idx[message.name] + working_byte) + "] = " \
					+ sym_avlbl_buffer_name \
					+ "[" + str(avlbl_msg_buff_idx[message.name] + working_byte) + "] & " \
					+ cg.to_hex_string_with_suffix(working_clr_mask) + ")"

				cog.outl("#define " + sym_avlb_clear + pad_avlb_clear + macro_value)

			cog.outl("")
]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
