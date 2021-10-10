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
/** \brief     	Source file for CAN node network signal access functions.
 *  \details   	Definitions for functions to have direct access to signals from
 *  			the data buffers. Functions will be generated in this file
 *  			only if configurable parameter "CAN_protect_direct_access" is
 *  			"True". Otherwise, these functions are not required and direct
 *  			access will be provided directly by macros.
 *  \author    	Carlos Calvillo
 *  \version   	0.1
 *  \date      	2021-10-10
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
#include "calvos.h"
#include <string.h> /* For inclusion of memcpy function */
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

read_function_prefix = "can_" + net_name_str + node_name_str + "read_"
write_function_prefix = "can_" + net_name_str + node_name_str + "write_"

read_array_function_prefix = "can_" + net_name_str + node_name_str + "read_array_"
write_array_function_prefix = "can_" + net_name_str + node_name_str + "write_array_"

 ]]] */
// [[[end]]]

/* [[[cog

# Get parameter for direct access protection
direct_access_protected = subnet.get_simple_param("CAN_protect_direct_access")


macro_prefix = "kCAN_" + net_name_str + "msgLen_"

# Names of direct access macros

def_get_direct_prefix = "CAN_"+net_name_str+node_name_str+"get_direct_"
def_update_direct_prefix = "CAN_"+net_name_str+node_name_str+"update_direct_"

def_read_array_prefix = "CAN_" + net_name_str + node_name_str+"get_ptr_direct_"
def_write_array_prefix = "CAN_" + net_name_str + node_name_str+"update_ptr_direct_"

# Definition of Functions

def get_read_signal_header(signal_name, signal_data_type):
	return_str = """
/"""+chr(42)+""" ==========================================================================="""+chr(42)+"""/
/"""+chr(42)+chr(42)+""" Function for reading signal """+signal_name+""".
 """+chr(42)+"""
 """+chr(42)+""" This function returns the value of signal """+signal_name+""" reading it
 """+chr(42)+""" directly from the RX data buffer.
 """+chr(42)+""" @return"""+chr(9)+"""Returns value of signal """+signal_name+""".
 """+chr(42)+""" ==========================================================================="""+chr(42)+"""/"""
	return return_str[1:]

def get_read_signal_signature(signal_name, signal_data_type):
	return_str = signal_data_type + " " + read_function_prefix + signal_name+"()"
	return return_str

def get_read_signal_body(signal_name, signal_data_type):
	return_str = chr(9)+signal_data_type+" value;"+chr(13)+chr(13)+chr(9)
	if direct_access_protected is True:
		return_str += "CALVOS_CRITICAL_ENTER();"+chr(13)+chr(9)
	return_str += "value = "+def_get_direct_prefix+signal_name+"();"+chr(13)+chr(9)
	if direct_access_protected is True:
		return_str += "CALVOS_CRITICAL_EXIT();"+chr(13)+chr(9)
	return_str += chr(13)+chr(9)+"return value;"
	return return_str

def get_write_signal_header(signal_name, signal_data_type):
	return_str = """
/"""+chr(42)+""" ==========================================================================="""+chr(42)+"""/
/"""+chr(42)+chr(42)+""" Function for writing signal """+signal_name+""".
 """+chr(42)+"""
 """+chr(42)+""" This function writes the value of signal """+signal_name+""" directly to
 """+chr(42)+""" the TX data buffer.
 """+chr(42)+"""
 """+chr(42)+""" @param value"""+chr(9)+"""Value to be written to the signal.
 """+chr(42)+""" ==========================================================================="""+chr(42)+"""/"""
	return return_str[1:]

def get_write_signal_signature(signal_name, signal_data_type):
	return_str = "void " + write_function_prefix + signal_name + "(" + signal_data_type +" value)"
	return return_str

def get_write_signal_body(signal_name, signal_data_type):
	return_str = chr(9)
	if direct_access_protected is True:
		return_str += "CALVOS_CRITICAL_ENTER();"+chr(13)+chr(9)
	return_str += def_update_direct_prefix+signal_name+"(value);"
	if direct_access_protected is True:
		return_str += chr(13)+chr(9)+"CALVOS_CRITICAL_EXIT();"
	return return_str



def get_read_array_header(signal_name):
	return_str = """
/"""+chr(42)+""" ==========================================================================="""+chr(42)+"""/
/"""+chr(42)+chr(42)+""" Function for reading signal """+signal_name+""" of type 'array'.
 """+chr(42)+"""
 """+chr(42)+""" This function returns the value of signal """+signal_name+""" reading it
 """+chr(42)+""" directly from the RX data buffer and copies it into the passed array.
 """+chr(42)+""" @param dest_array"""+chr(9)+"""Data read from signal will be written in this destination array.
 """+chr(42)+""" ==========================================================================="""+chr(42)+"""/"""
	return return_str[1:]

def get_read_array_signature(signal_name):
	return_str = "void " + read_array_function_prefix + signal_name+"(uint8_t * dest_array)"
	return return_str

def get_read_array_body(signal_name, array_len_in_bytes):
	return_str = ""
	if direct_access_protected is True:
		return_str += chr(9)+"CALVOS_CRITICAL_ENTER();"+chr(13)+chr(9)
	return_str += "memcpy(dest_array, "+def_read_array_prefix+signal_name+"(),"+str(array_len_in_bytes)+");"
	if direct_access_protected is True:
		return_str += chr(13)+chr(9)+"CALVOS_CRITICAL_EXIT();"
	return return_str

def get_write_array_header(signal_name):
	return_str = """
/"""+chr(42)+""" ==========================================================================="""+chr(42)+"""/
/"""+chr(42)+chr(42)+""" Function for writing signal """+signal_name+""" of type 'array'.
 """+chr(42)+"""
 """+chr(42)+""" This function writes the value of signal """+signal_name+""" directly to
 """+chr(42)+""" the TX data buffer based on the provided input array.
 """+chr(42)+"""
 """+chr(42)+""" @param src_array"""+chr(9)+"""Value written to signal will be taken from this source array.
 """+chr(42)+""" ==========================================================================="""+chr(42)+"""/"""
	return return_str[1:]

def get_write_array_signature(signal_name):
	return_str = "void " + write_array_function_prefix + signal_name + "(uint8_t * src_array)"
	return return_str

def get_write_array_body(signal_name):
	return_str = chr(9)
	if direct_access_protected is True:
		return_str += "CALVOS_CRITICAL_ENTER();"+chr(13)+chr(9)
	return_str += def_write_array_prefix+signal_name+"(src_array);"
	if direct_access_protected is True:
		return_str += chr(13)+chr(9)+"CALVOS_CRITICAL_EXIT();"
	return return_str

 ]]] */
// [[[end]]]

/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* --------------------------- */
/* Signal reading functions    */
/* --------------------------- */
/* [[[cog

# Resolve enum symbols wildcards
datat_symb_prfx = network.get_simple_param("CAN_enum_sym_prefix")
if datat_symb_prfx == "$":
	datat_symb_prfx = ""
else:
	datat_symb_prfx = \
		cg.resolve_wildcards(datat_symb_prfx, {"NWID":network.id_string})

datat_type_name = network.get_simple_param("CAN_enum_type_prefix")

if len(list_of_rx_msgs) > 0:

	for message_name in list_of_rx_msgs:
		message_signals = subnet.get_signals_of_message(message_name)
		for signal in message_signals:
			signal_name = signal.name
			# Functions are not generated for signals of type array
			if signal.is_array() is False:
				# Resolve type name
				if datat_type_name == "" or  datat_type_name is None:
					type_name = signal_name
				else:
					type_name = \
						cg.resolve_wildcards(datat_type_name, {"DATATYPE":signal_name})

				# Generate read function
				cog.outl(get_read_signal_header(signal_name,type_name))
				cog.outl(get_read_signal_signature(signal_name,type_name)+"{")
				cog.outl(get_read_signal_body(signal_name,type_name)+chr(13)+"}")
				cog.outl("")
			else:
				array_len_in_bytes = int(signal.len/8)
				# Generate read function
				cog.outl(get_read_array_header(signal_name))
				cog.outl(get_read_array_signature(signal_name)+"{")
				cog.outl(get_read_array_body(signal_name,array_len_in_bytes)+chr(13)+"}")
				cog.outl("")

]]] */
// [[[end]]]

/* --------------------------- */
/* Signal writing functions    */
/* --------------------------- */
/* [[[cog

if len(list_of_tx_msgs) > 0:

	for message_name in list_of_tx_msgs:
		message_signals = subnet.get_signals_of_message(message_name)
		for signal in message_signals:
			# Functions are not generated for signals of type array
			signal_name = signal.name
			if signal.is_array() is False:
				# Resolve type name
				if datat_type_name == "" or  datat_type_name is None:
					type_name = signal_name
				else:
					type_name = \
						cg.resolve_wildcards(datat_type_name, {"DATATYPE":signal_name})

				# Generate write function
				cog.outl(get_write_signal_header(signal_name,type_name))
				cog.outl(get_write_signal_signature(signal_name,type_name)+"{")
				cog.outl(get_write_signal_body(signal_name,type_name)+chr(13)+"}")
				cog.outl("")
			else:
				# Generate write function
				cog.outl(get_write_array_header(signal_name))
				cog.outl(get_write_array_signature(signal_name)+"{")
				cog.outl(get_write_array_body(signal_name)+chr(13)+"}")
				cog.outl("")

]]] */
// [[[end]]]
