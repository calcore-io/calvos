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
/** \brief     	Source file for CAN Core functionality.
 *  \details   	Implements the CAN core functionality for the handling of the
 *  			defined messages/signals in the given network.
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
node_name_str = node_name + "_"

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
extern void CAN_coreInit();
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_rx_proc_func_name = "can_" + net_name_str + node_name_str + "processRxMessage"
	sym_rx_proc_func_args = "(uint32_t msg_id, uint8_t * data_in)"
	sym_rx_proc_func_return = "void"

	code_str = "extern "+sym_rx_proc_func_return+" "+sym_rx_proc_func_name+sym_rx_proc_func_args+";"
	cog.outl(code_str)
 ]]] */
// [[[end]]]

/* Private Function Prototypes */
void CAN_clearMsgDynamicData();
CalvosError CAN_traverseRxSearchTree(uint32_t, RxSearchResult);
CANmsgFound CAN_getMsgIdx(uint32_t, uint32_t *);
void CAN_filterRxMsg(uint32_t);


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

/* Array of Rx messages dynamic data */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_dyn_data_type = "CANrxMsgDynamicData"
	sym_dyn_data_name = "can_" + net_name_str + node_name_str + "rxMsgDynamicData"
	code_str = sym_dyn_data_type+" "+sym_dyn_data_name+"["+sym_rx_msgs+"];"
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
	sym_rx_stat_data_type = "CANrxMsgStaticData"
	sym_rx_stat_data_name = "can_" + net_name_str + node_name_str \
		+ "rxMsgStaticData"

	cog.outl("const "+sym_rx_stat_data_type+" "+sym_rx_stat_data_name+"["+sym_rx_msgs+"] = [\\")

	callback_prefix = "can_" + net_name_str + node_name_str
	callback_rx_sufix = "_rx_callback"
	callback_tout_sufix = "_timeout_callback"

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
		array_data.append("&" + sym_rx_data_name + "["+ str(data_idx) +"]")
		data_idx += subnet.messages[msg_name].len
		# Pointer to dynamic data
		array_data.append("&" + sym_dyn_data_name + "["+ str(i) +"]")
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
		SUB_STRUCT_BEGIN = 6
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
			code_string += "]"
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

/* Constant for clearing Rx dynamic data */
/* [[[cog
if len(list_of_rx_msgs) > 0:
	sym_data_name = "CANrxMsgDynamicData can_" + net_name_str + node_name_str \
		+ "rxMsgDynamicData_clear"

	cog.outl("const "+sym_data_name+" = {0,0,{0,NULL,NULL}};")
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

/* Array of Tx messages static data */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_data_name = "CANtxMsgStaticData can_" + net_name_str + node_name_str + "txMsgStaticData"
	sym_data_len = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"

	cog.outl("const "+sym_data_name+"["+sym_data_len+"] = [\\")

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
		array_data.append("&" + sym_tx_data_name + "["+ str(data_idx) +"]")
		data_idx += subnet.messages[msg_name].len
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
		SUB_STRUCT_BEGIN = 4
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
			code_string += "]"
		cog.outl("\t\t"+code_string)
 ]]] */
// [[[end]]]

/* Array of Tx messages dynamic data */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_data_name = "CANtxMsgDynamicData can_" + net_name_str + node_name_str + "txMsgDynamicData"
	sym_data_len = "kCAN_" + net_name_str + node_name_str + "nOfTxMsgs"

	cog.outl("const "+sym_data_name+"["+sym_data_len+"];")
]]] */
// [[[end]]]

/* Constant for clearing Tx dynamic data */
/* TODO: Can't really clear all flags with a simple 0 */
/* [[[cog
if len(list_of_tx_msgs) > 0:
	sym_data_name = "CANtxMsgDynamicData can_" + net_name_str + node_name_str + "rxMsgDynamicData_clear"
	cog.outl("const "+sym_data_name+" = {0,0,0,{0,NULL,NULL}};")
]]] */
// [[[end]]]

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* ===========================================================================*/
/** Function for processing a CAN RX msg.
 *
 * Checks if the received msg ID belongs to this node. If so invoke callbacks,
 * and set corresponding flags and data.
 *
 * @param msg_id 	ID of the message tha was received.
 * @param data_in	Pointer to the message's data that was received.
 * ===========================================================================*/
/* [[[cog
if len(list_of_tx_msgs) > 0:
	code_str = sym_rx_proc_func_return+" "+sym_rx_proc_func_name+sym_rx_proc_func_args+"{"
	cog.outl(code_str)

	invoke_search = "msg_static_data = can_traverseRxSearchTree(msg_id, \\\n\t\t\t&"\
		+sym_rx_stat_data_name+"["+sym_search_start_idx+"],  \\\n\t\t\t"+sym_rx_msgs+");"

	function_body = """
	CANrxMsgStaticData* msg_static_data;
	// Search for message to see if its suscribed by this node.
	"""+invoke_search+"""
	if(msg_static_data != NULL){
		// Copy data to buffer
		memcpy(msg_static_data->data, data_in, msg_static_data->fields.len)
		// Set available flags
		msg_static_data->dyn->available.all = kAllOnes32;
		// clear timeout flag
		msg_static_data->dyn->timedout = kFalse;
		// Invoke rx callback
		if(msg_static_data->rx_callback != NULL){
			(msg_static_data->rx_callback)();
		}
	}
}
	"""
	function_body = function_body[1:]
	cog.outl(function_body)
]]] */
// [[[end]]]

void CAN_clearMsgDynamicData(){
	/* Initialize msg dynamic data */
		CANmsgDynamicData clear_data;

		clear_data.timeout_queue = 0;
		clear_data.timeout_time = 0;
		clear_data.period_queue = 0;
		clear_data.period_time = 0;
		clear_data.data.available = 0;
		clear_data.data.timed_out = 0;

		for(uint32_t i=0; i<kCAN_CT_nOfMsgs; i++){
			can_CT_dynamic_msg_data[i] = clear_data;
		}
}

void CAN_coreInit(){
	/* Initialize msg dynamic data */
	CAN_clearMsgDynamicData();
}


