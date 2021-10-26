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
		project = pic.load(f)
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
/** \brief     	Source file for CAN Core functionality.
 *  \details   	Implements the CAN core functionality for the handling of the
 *  			defined messages/signals in the given network and given node.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-15
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
#include <string.h>
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

# Sort RX messages by ID
sorted_rx_msgs = {} # Expects {msg_name : msg_id}
for msg_name in list_of_rx_msgs:
	sorted_rx_msgs.update({msg_name : subnet.messages[msg_name].id})
sorted_rx_msgs = dict(sorted(sorted_rx_msgs.items(), key=lambda kv: kv[1]))

 ]]] */
// [[[end]]]

/* Exported Function Prototypes */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# RX Processing Function
	# ----------------------
	sym_rx_proc_func_name = "can_" + net_name_str + node_name_str + "processRxMessage"
	sym_rx_proc_func_args = "(uint32_t msg_id, uint8_t * data_in, uint8_t data_len)"
	sym_rx_proc_func_return = "void"

if len(list_of_tx_msgs) > 0:
	# TX Processing Function
	# ----------------------
	sym_enum_name = "CAN_" + net_name_str + node_name_str + "txMsgs"

	sym_transmit_func_name = "can_"+net_name_str+node_name_str+"transmitMsg"
	sym_transmit_func_args = "("+sym_enum_name+" msg_idx)"
	sym_transmit_func_return = "void"
 ]]] */
// [[[end]]]

/* Private Function Prototypes */

/* Private macro definitions */

/* Private type definitions */

/* Exported data */

/* Rx message data buffer */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_rx_msgs = "kCAN_" + net_name_str + node_name_str + "nOfRxMsgs"

	sym_rx_data_len = "kCAN_" + net_name_str +  node_name_str + "RxMsgsTotalLen"
	sym_rx_data_name = "can_" + net_name_str +  node_name_str + "RxDataBuffer"
	code_str = cg.get_dtv(8) + " " + sym_rx_data_name + "[" + sym_rx_data_len + "];"

	cog.outl(code_str)
]]] */
// [[[end]]]

/* Rx available flags buffer */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_avlbl_buffer_name = "can_" + net_name_str + node_name_str + "avlbl_buffer"
	sym_avlbl_buff_len = "kCAN_" + net_name_str + node_name_str + "avlbl_buffer_len"
	code_str = cg.get_dtv(8) + " " + sym_avlbl_buffer_name + "[" + sym_avlbl_buff_len + "];"
	cog.outl(code_str)
]]] */
// [[[end]]]

/* RX available flags */
/* RX signal availability flag indexes */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Estimate size of
	sym_sig_idx_pfx = "kCAN_" + net_name_str + node_name_str + "sig_avlbl_idx_"
	macro_names = []
	macro_values = []
	for message in subnet.messages.values():
		if subnet.get_message_direction(node_name,message.name) == nw.CAN_RX:
			message_signals = subnet.get_signals_of_message(message.name)
			index = 0
			for signal in message_signals:
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
		cog.outl(code_string)
]]] */
// [[[end]]]

/* Array of Rx messages dynamic data */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_rx_dyn_data_type = "CANrxMsgDynamicData"
	sym_rx_dyn_data_name = "can_" + net_name_str + node_name_str + "rxMsgDynamicData"
	code_str = sym_rx_dyn_data_type+" "+sym_rx_dyn_data_name+"["+sym_rx_msgs+"];"
	cog.outl(code_str)
]]] */
// [[[end]]]

/* Array of Rx messages static data */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	# Create search tree
	input = [*range(len(list_of_rx_msgs))]
	root = cg.formTree(input)
	search_tree = []
	cg.inorderTree(root, search_tree)

	# Create static array
	sym_rx_stat_data_type = "const CANrxMsgStaticData"
	sym_rx_stat_data_name = "can_" + net_name_str + node_name_str \
		+ "rxMsgStaticData"

	cog.outl(sym_rx_stat_data_type+" "+sym_rx_stat_data_name+"["+sym_rx_msgs+"] = {\\")

	callback_prefix = "can_" + net_name_str + node_name_str
	callback_rx_sufix = "_rx_callback"
	callback_tout_sufix = "_timeout_callback"

	sym_avlbl_buff_idx_pfx = "kCAN_" + net_name_str + node_name_str + "avlbl_buffer_idx_"
	sym_avlbl_size_idx_pfx = "kCAN_" + net_name_str + node_name_str + "avlbl_slot_len_"

	array_data = []
	data_idx = 0
	for i, msg_name in enumerate(sorted_rx_msgs.keys()):
		array_data.clear()
		# Msg ID
		array_data.append("kCAN_" + net_name_str + "msgId_" + msg_name)
		# Msg Timeout
		array_data.append("kCAN_" + net_name_str + node_name_str \
			+ "msgTimeout_" + msg_name)
		# Msg rx callback
		array_data.append("&" + callback_prefix + msg_name + callback_rx_sufix)
		# Msg timeout callback
		array_data.append("&" + callback_prefix + msg_name + callback_tout_sufix)
		# Data Buffer
		array_data.append(" \\\n\t\t\t\t&" + sym_rx_data_name + "["+ str(data_idx) +"]")
		data_idx += subnet.messages[msg_name].len
		# Available Flags Buffer
		array_data.append("&" + sym_avlbl_buffer_name + "["+ sym_avlbl_buff_idx_pfx + msg_name +"]")
		# Available Flags Buffer Length
		array_data.append(sym_avlbl_size_idx_pfx + msg_name)
		# Pointer to dynamic data
		array_data.append(" \\\n\t\t\t\t&" + sym_rx_dyn_data_name + "["+ str(i) +"]")
		# Msg Len
		array_data.append(str(subnet.messages[msg_name].len) + "u")
		# Msg extended id?
		if subnet.messages[msg_name].extended_id is True:
			array_data.append("kTrue")
		else:
			array_data.append("kFalse")

		prev_idx = search_tree[i][1]
		if prev_idx is not None:
			array_data.append("&"+sym_rx_stat_data_name+"["+str(prev_idx)+"]")
		else:
			array_data.append("NULL")
		next_idx = search_tree[i][2]
		if next_idx is not None:
			array_data.append("&"+sym_rx_stat_data_name+"["+str(next_idx)+"]")
		else:
			array_data.append("NULL")

		ALL_DATA_END = len(array_data)-1
		SUB_STRUCT_BEGIN = 8
		SUB_STRUCT_END = SUB_STRUCT_BEGIN + 1
		code_string = "{"
		for j, piece_str in enumerate(array_data):
			if j == SUB_STRUCT_BEGIN:
				code_string += "{"
			code_string += piece_str
			if j == SUB_STRUCT_END:
				code_string += "}"
			if (j < SUB_STRUCT_BEGIN and j < ALL_DATA_END) \
			or (j >= SUB_STRUCT_BEGIN and j < SUB_STRUCT_END) \
			or (j >= SUB_STRUCT_END and j < ALL_DATA_END):
				code_string += ",\t"
			if j == ALL_DATA_END:
				code_string += "}"
		if i < len(sorted_rx_msgs.keys()) - 1:
			code_string += ", \\"
		else:
			code_string += "};"
		cog.outl("\t\t"+code_string)
 ]]] */
// [[[end]]]

/* Rx search tree starting index */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	search_tree_start_idx = math.floor(len(list_of_rx_msgs)/2)
	# Create static array
	sym_search_start_idx = "kCAN_" + net_name_str + node_name_str +"RxSearchStartIdx"
	cog.outl("#define "+sym_search_start_idx+"\t\t("+str(search_tree_start_idx)+"u)")
 ]]] */
// [[[end]]]

/* Tx message data buffer */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_data_len = "kCAN_" + net_name_str +  node_name_str + "TxMsgsTotalLen"
	sym_tx_data_name = "can_" + net_name_str +  node_name_str + "TxDataBuffer"
	code_str = cg.get_dtv(8) + " " + sym_tx_data_name + "[" + sym_tx_data_len + "];"

	cog.outl(code_str)
]]] */
// [[[end]]]

/* Array of Tx messages dynamic data */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_dyn_data_name = "can_" + net_name_str + node_name_str + "txMsgDynamicData"
	sym_tx_dyn_data_type = "CANtxMsgDynamicData"
	sym_tx_dyn_data_len = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"
	source_str = sym_tx_dyn_data_type+" "+sym_tx_dyn_data_name+"["+sym_tx_dyn_data_len+"];"
	cog.outl(source_str)
]]] */
// [[[end]]]

/* Tx transmission queue */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_queue_name = "can_" + net_name_str +  node_name_str + "txQueue"
	sym_tx_queue_type = "CANtxQueue"
	code_str = sym_tx_queue_type+ " "+sym_tx_queue_name+";"
	cog.outl(code_str)
]]] */
// [[[end]]]

/* Message being transmitted */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_txing_msg_name = "can_" + net_name_str +  node_name_str + "transmittingMsg"
	sym_txing_msg_type = "const CANtxMsgStaticData*"
	code_str = sym_txing_msg_type+ " "+sym_txing_msg_name+";"
	cog.outl(code_str)
]]] */
// [[[end]]]

/* Array of Tx messages static data */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_stat_data_name = "can_" + net_name_str + node_name_str + "txMsgStaticData"
	sym_tx_stat_data_type = "const CANtxMsgStaticData"
	sym_tx_stat_data_len = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"

	cog.outl(sym_tx_stat_data_type+" "+sym_tx_stat_data_name+"["+sym_tx_stat_data_len+"] = {\\")

	callback_prefix = "can_" + net_name_str + node_name_str
	callback_tx_sufix = "_tx_callback"

	array_data = []
	data_idx = 0
	for i, msg_name in enumerate(list_of_tx_msgs):
		array_data.clear()
		# Msg ID
		array_data.append("kCAN_" + net_name_str + "msgId_" + msg_name)
		# Msg Tx Period
		array_data.append("kCAN_" + net_name_str + "msgTxPeriod_" + msg_name)
		# Msg tx callback
		array_data.append("&" + callback_prefix + msg_name + callback_tx_sufix)
		# Data Buffer
		array_data.append(" \\\n\t\t\t\t&" + sym_tx_data_name + "["+ str(data_idx) +"]")
		data_idx += subnet.messages[msg_name].len
		# Pointer to dynamic data
		array_data.append("&" + sym_tx_dyn_data_name + "["+ str(i) +"]")
		# Msg Len
		array_data.append(str(subnet.messages[msg_name].len) + "u")
		# Msg extended id?
		if subnet.messages[msg_name].extended_id is True:
			array_data.append("kTrue")
		else:
			array_data.append("kFalse")
		# Msg Tx Type
		array_data.append("kCAN_" + net_name_str + "msgTxType_" + msg_name)

		ALL_DATA_END = len(array_data)-1
		SUB_STRUCT_BEGIN = 5
		SUB_STRUCT_END = SUB_STRUCT_BEGIN + 2
		code_string = "{"
		for j, piece_str in enumerate(array_data):
			if j == SUB_STRUCT_BEGIN:
				code_string += "{"
			code_string += piece_str
			if j == SUB_STRUCT_END:
				code_string += "}"
			if (j < SUB_STRUCT_BEGIN and j < ALL_DATA_END) \
			or (j >= SUB_STRUCT_BEGIN and j < SUB_STRUCT_END) \
			or (j >= SUB_STRUCT_END and j < ALL_DATA_END):
				code_string += ",\t"
			if j == ALL_DATA_END:
				code_string += "}"
		if i < len(list_of_tx_msgs) - 1:
			code_string += ", \\"
		else:
			code_string += "};"
		cog.outl("\t\t"+code_string)
 ]]] */
// [[[end]]]

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* ===========================================================================*/
/** Function for processing reception of a CAN msg.
 *
 * Verifies if received message belongs to this node and if so, signal its
 * reception and invokes the corresponding callback.
 * This function is typically called from the CAN RX ISR.
 *
 * @param msg_id 	Id of the received message.
 * @param data_in 	Pointer to the message's received data.
 *
 * IMPORTANT: This function needs to be called by user's application upon
 * indication of successful reception by the CAN HAL (typically within ISR
 * context).
 * ===========================================================================*/
/* [[[cog
if len(list_of_rx_msgs) > 0:
	code_str = sym_rx_proc_func_return+" "+sym_rx_proc_func_name+sym_rx_proc_func_args+"{"
	cog.outl(code_str)

	invoke_search = "msg_static_data = can_traverseRxSearchTree(msg_id, \\\n\t\t\t&"\
		+sym_rx_stat_data_name+"["+sym_search_start_idx+"],  \\\n\t\t\t"+sym_rx_msgs+");"

	function_body = """
	const CANrxMsgStaticData* msg_static_data;
	// Search for message to see if its suscribed by this node.
	"""+invoke_search+"""
	if(msg_static_data != NULL){
		// Consider message as valid if it matches the expected length
		if(data_len == msg_static_data->fields.len){
			if(data_len != 0){
				// Copy data to buffer
				memcpy(msg_static_data->data, data_in, msg_static_data->fields.len);
				// Set Signals available flags
				if(msg_static_data->sig_avlbl_buf_len == 1){
					*msg_static_data->sig_avlbl_flags = kAllOnes8;
				}else{
					for(uint32_t i=0; i < msg_static_data->sig_avlbl_buf_len; i++){
						msg_static_data->sig_avlbl_flags[i] = kAllOnes8;
					}
				}
			}

			// Set Message available flags
			msg_static_data->dyn->available.all = kAllOnes32;

			// clear timeout flag and reset timeout timer
			msg_static_data->dyn->timeout_timer = 0;
			msg_static_data->dyn->timedout = kFalse;
			// Invoke rx callback
			if(msg_static_data->rx_callback != NULL){
				(msg_static_data->rx_callback)();
			}
		}
	}
}
	"""
	function_body = function_body[1:]
	cog.outl(function_body)
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for cyclic processing of CAN RX messages.
 *
 * Keeps track of message timeouts if defined for this node.
 * IMPORTANT: This function needs to be called by user's application in a
 * periodic task with the period indicated in this function's name.
 * ===========================================================================*/
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_rx_msg_idx_prefix = "kCAN_" + net_name_str + node_name_str + "rxMsgIdx_"
	rx_proc_task = network.get_simple_param("CAN_rx_task_period")

	sym_rx_proc_func_name = "can_task_"+str(rx_proc_task)+"ms_"+net_name_str+node_name_str+"rxProcess"
	code_str = "void "+sym_rx_proc_func_name+"(void){"
	cog.outl(code_str)

	code_strs = """
	for(uint32_t i = 0u; i < """+sym_rx_msgs+"""; i++){
		// Check if message has a timeout defined
		if("""+sym_rx_stat_data_name+"""[i].timeout > 0u){
			// Only increment timer if message hasn't timed out
			CALVOS_CRITICAL_ENTER();
			if(!"""+sym_rx_stat_data_name+"""[i].dyn->timedout){
				CALVOS_CRITICAL_EXIT();
				// Increment timer (up-counter since they are initialized with zero)
				"""+sym_rx_stat_data_name+"""[i].dyn->timeout_timer++;
				if("""+sym_rx_stat_data_name+"""[i].dyn->timeout_timer \\
				>= """+sym_rx_stat_data_name+"""[i].timeout){
					// Set timeout flag
					CALVOS_CRITICAL_ENTER();
					"""+sym_rx_stat_data_name+"""[i].dyn->timedout = kTrue;
					CALVOS_CRITICAL_EXIT();
					// Call timeout callback if not NULL
					if("""+sym_rx_stat_data_name+"""[i].timeout_callback != NULL){
						("""+sym_rx_stat_data_name+"""[i].timeout_callback)();
					}
					// Reset timer
					"""+sym_rx_stat_data_name+"""[i].dyn->timeout_timer = 0u;
				}
			}else{
				CALVOS_CRITICAL_EXIT();
			}
		}
	}\n"""

	function_body = """

	// TODO: Implement this as a timer wheel for efficiency
	// TODO: Consider separating data structures for timeout so that memory for that
	// is only used for messages with defined timeout and not for all.
"""+code_strs+"\n"

	function_body = function_body[1:]
	cog.outl(function_body+"}")
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for transmitting a CAN msg.
 *
 * Triggers the transmission of the specified CAN message.
 *
 * @param msg_idx 	index of the message to be transmitted.
 * @Return 		Returns @c kNoError if message was triggered for transmission
 * 				by HAL or if it was successfully queued for a transmission
 * 				retry. Returns @c kError otherwise (HAL busy, queue full, wrong
 * 				message index provided).
 * ===========================================================================*/
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_enum_name = "CAN_" + net_name_str + node_name_str + "txMsgs"
	sym_max_tx_msgs = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"
	sym_transmit_func_name = "can_"+net_name_str+node_name_str+"transmitMsg"
	sym_transmit_func_args = "("+sym_enum_name+" msg_idx)"

	sym_hal_transmit_name = "can_"+net_name_str+node_name_str+"HALtransmitMsg"

	code_str = "CalvosError "+sym_transmit_func_name+sym_transmit_func_args+"{"
	cog.outl(code_str)

	function_body = """
	CalvosError return_value = kError;

	// Trigger CAN transmission to HAL
	if(msg_idx < """+sym_max_tx_msgs+"""){
		return_value = can_commonTransmitMsg(&"""+sym_tx_stat_data_name+"""[msg_idx], \\
								  &"""+sym_tx_queue_name+""", \\
								  &"""+sym_hal_transmit_name+""", \\
								  &"""+sym_txing_msg_name+""");
	}

	return return_value;
}
	"""
	function_body = function_body[1:]
	cog.outl(function_body)
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for processing cyclic CAN TX messages.
 *
 * Triggers the transmission of CAN messages set as cyclic or cyclic+spontan.
 * IMPORTANT: This function needs to be called by user's application in a
 * periodic task with the period indicated in this function's name.
 * ===========================================================================*/
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_msg_idx_prefix = "kCAN_" + net_name_str + node_name_str + "txMsgIdx_"
	tx_proc_task = network.get_simple_param("CAN_tx_task_period")

	sym_tx_proc_func_name = "can_task_"+str(tx_proc_task)+"ms_"+net_name_str+node_name_str+"txProcess"
	code_str = "void "+sym_tx_proc_func_name+"(void){"
	cog.outl(code_str)

	code_strs = ""
	for message_name in list_of_tx_msgs:
		if subnet.messages[message_name].tx_period != 0 \
		and  (subnet.messages[message_name].tx_type == nw.tx_type_list[nw.CYCLIC] \
		or subnet.messages[message_name].tx_type == nw.tx_type_list[nw.CYCLIC_SPONTAN]):
			sym_tx_msg_idx_name = sym_tx_msg_idx_prefix + message_name
			# Reduce timer
			code_strs += """	// Increment timer (up-counter since they are initialized with zero)
	"""+sym_tx_stat_data_name+"["+sym_tx_msg_idx_name+"""].dyn->period_timer++;
	if("""+sym_tx_stat_data_name+"["+sym_tx_msg_idx_name+"""].dyn->period_timer \\
	>= """+sym_tx_stat_data_name+"["+sym_tx_msg_idx_name+"""].period){
		// Timer expired, trigger message transmission
		"""+sym_transmit_func_name+"("+sym_tx_msg_idx_name+""");
		// Reset timer
		"""+sym_tx_stat_data_name+"["+sym_tx_msg_idx_name+"""].dyn->period_timer = 0u;
	}\n"""


	function_body = """

	// TODO: Implement this as a timer wheel for efficiency
	// TODO: Group tx messages with same period in single timers rather than
	// individual ones for efficiency.

"""+code_strs+"\n"

	function_body = function_body[1:]
	cog.outl(function_body+"}")
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for processing CAN TX retry mechanism.
 *
 * Triggers transmission of queued CAN messages (retry mechanism).
 * IMPORTANT: This function needs to be called by user's application either in
 * a periodic task or right after a CAN transmission within ISR context.
 * ===========================================================================*/
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_tx_retry_func_name = "can_"+net_name_str+node_name_str+"txRetry"
	code_str = "void "+sym_tx_retry_func_name+"(void){"
	cog.outl(code_str)

	function_body = """
	const CANtxMsgStaticData* msg_to_retry;
	CalvosError return_value;

	// TODO: Implement max retries per message, etc.?
	// TODO: Implement an option to also "freeze" the data to the message that
	// was queued. Right now, the data will be taken from common buffer when the
	// message finally is accepted for transmission.

	msg_to_retry = can_txQueueGetHead(&"""+sym_tx_queue_name+""");
	if(msg_to_retry != NULL){
		// Attempt the re-transmission
		return_value = can_commonTransmitMsg(msg_to_retry, \\
								  NULL, \\
								  &"""+sym_hal_transmit_name+""", \\
								  &"""+sym_txing_msg_name+""");
		if(return_value == kNoError){
			// Transmission succeeded. Dequeue the message.
			can_txQueueDequeue(&"""+sym_tx_queue_name+""", NULL);
		}
	}
"""
	function_body = function_body[1:]
	cog.outl(function_body+"}")
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Set signals initial values.
 *
 * Initializes the signals with defined initial value if this is different than
 * zero (all signal data buffers are previously initialized with zeros).
 * ===========================================================================*/
/* [[[cog
sym_init_sigs_ret = "void"
sym_init_sigs_name = "can_"+net_name_str+node_name_str+"signalsInit"
sym_init_sigs_args = "(void)"

code_str = sym_init_sigs_ret+" "+sym_init_sigs_name+sym_init_sigs_args+"{"
cog.outl(code_str)

# Write init values code string

function_body = """
	// TODO: implement this function
"""
function_body = function_body[1:]
cog.outl(function_body+"}")
]]] */
// [[[end]]]

/* ===========================================================================*/
/** Function for initialization of CAN core.
 *
 * Initializes data for CAN core functionality of current node.
 * IMPORTANT: This function needs to be called by user's application during SW
 * initialization phase.
 * ===========================================================================*/
/* [[[cog
if len(list_of_tx_msgs) > 0:

	sym_hal_init_name = "can_"+net_name_str+node_name_str+"HALinit"

	sym_tx_init_val = "kCAN_" + net_name_str + "TxDataInitVal"
	sym_rx_init_val = "kCAN_" + net_name_str + "RxDataInitVal"

	timeouts_initially_true = network.get_simple_param("CAN_init_timeouts_val")

	sym_core_init_name = "can_"+net_name_str+node_name_str+"coreInit"
	code_str = "void "+sym_core_init_name+"(void){"
	cog.outl(code_str)

	timeout_inits_str = ""
	if timeouts_initially_true is True:
		timeout_inits_str = """
	// Set initial timeout flags as true (param 'CAN_init_timeouts_val' is set to True)
	for(uint32_t i = 0; i < """+sym_rx_msgs+"""; i++){
		"""+sym_rx_dyn_data_name+"""[i].timedout = kTrue;
	}\n"""

	function_body = """

	// Clear RX data buffer
	memset(&"""+sym_rx_data_name+","+sym_rx_init_val+","+sym_rx_data_len+""");

	// Clear RX available flags buffer
	memset(&"""+sym_avlbl_buffer_name+",0u,"+sym_avlbl_buff_len+""");

	// Clear TX data buffer
	memset(&"""+sym_tx_data_name+","+sym_tx_init_val+","+sym_tx_data_len+""");

	// Clear RX dynamic data
	memset(&"""+sym_rx_dyn_data_name+",0u,sizeof("+sym_rx_dyn_data_type+")*("+sym_rx_msgs+"""));
	"""+timeout_inits_str+"""
	// Clear TX dynamic data
	memset(&"""+sym_tx_dyn_data_name+",0u,sizeof("+sym_tx_dyn_data_type+")*("+sym_tx_dyn_data_len+"""));

	// Init signal values
	"""+sym_init_sigs_name+"""();

	// Init TX queue
	can_txQueueInit(&"""+sym_tx_queue_name+""");

	// Init transmitting message
	"""+sym_txing_msg_name+""" = NULL;

	// Init CAN HAL
	"""+sym_hal_init_name+"""();
"""
	function_body = function_body[1:]
	cog.outl(function_body+"}")
]]] */
// [[[end]]]
