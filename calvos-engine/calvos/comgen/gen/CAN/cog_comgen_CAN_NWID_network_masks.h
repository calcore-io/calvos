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
/** \brief     	Header file CAN Signals/messages definitions.
 *  \details   	Contains data structures to ease the handling of the CAN
 *				signals/messages for a given network.
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

/* -------------------------------------------------------------------------- */
// 		Signal Masks and Symbols
/* -------------------------------------------------------------------------- */

/* [[[cog
# Get structure of all network messages
sym_bit_prefix = "kCAN_" + net_name_str + "SIG_START_BIT_"
sym_byte_prefix = "kCAN_" + net_name_str + "SIG_START_BYTE_"
sym_abs_bit_prefix = "kCAN_" + net_name_str + "SIG_ABS_START_BIT_"

sym_mask_sig_clr = "kCAN_" + net_name_str + "SIG_MASK_CLR"
sym_mask_sig_clr_in_msg = "kCAN_" + net_name_str + "SIG_MASK_CLR_IN_MSG_"

sym_init_value = "kCAN_" + net_name_str + "SIG_INIT_VAL_"

datat_symb_prfx = network.get_simple_param("CAN_enum_sym_prefix")
if datat_symb_prfx == "$":
	datat_symb_prfx = ""
else:
	datat_symb_prfx = \
		cg.resolve_wildcards(datat_symb_prfx, {"NWID":network.id_string})

#for i in range(1,65):
#	if i == i:
#		print(" i = ",i," pieces: ", cg.split_bit_len_in_base_types(i))
#		mask = cg.get_bit_mask_str_binary(i)
#		#mask = "000000000000rqponmlkjihgfedcba0987654321"
#		#print("mask: ", mask)
#		print(cg.split_bit_str_in_base_types(i,mask))
#		for piece in cg.split_bit_str_in_base_types(i,mask):
#			print(hex(int(piece[1],2)))

for message in network.messages.values():
	cog.outl(message.name)
	cog.outl(str(message.len))
	signals = network.get_signals_of_message(message.name)
	for signal in signals:
#		cog.outl("    - "+signal.name+", start byte: "+str(signal.start_byte)+", start bit: "+str(signal.start_bit)+", len: "+str(signal.len)+", abs bit: "+str(signal.get_abs_start_bit()))
#		mask = cg.get_bit_mask_str_binary(signal.len,signal.get_abs_start_bit(),True,message.len*8)
#		cog.outl("    -"+str(mask))
#		mask = cg.get_bit_mask_str_hex(signal.len,signal.get_abs_start_bit(),True,message.len*8)
#		cog.outl("    -"+str(mask))
#		mask = cg.get_bit_mask_str_hex(signal.len,signal.get_abs_start_bit(),True,message.len*8)
#		cog.outl("    -"+str(mask))

		cog.outl(sym_bit_prefix+signal.name+chr(9)+chr(9)+"("+str(signal.start_bit)+"u)")
		cog.outl(sym_byte_prefix+signal.name+chr(9)+chr(9)+"("+str(signal.start_byte)+"u)")
		cog.outl(sym_abs_bit_prefix+signal.name+chr(9)+chr(9)+"("+str(signal.get_abs_start_bit())+"u)")

		#Generate masks for clearing signal
		clr_mask = cg.get_bit_mask_str_binary(signal.len,0,True,cg.round_to_bytes(signal.len)*8)
		clr_mask_pieces = cg.split_bit_str_in_base_types(len(clr_mask),clr_mask)

		if len(clr_mask_pieces) > 1:
			for i, piece in enumerate(clr_mask_pieces):
				mask_in_hex = hex(int(piece[1],2))
				cog.outl(sym_mask_sig_clr+i+"_"+signal.name+chr(9)+chr(9)+"("+mask_in_hex+"u)")
		else:
			mask_in_hex = hex(int(clr_mask_pieces[0][1],2))
			cog.outl(sym_mask_sig_clr+"_"+signal.name+chr(9)+chr(9)+"("+mask_in_hex+"u)")

		#Generate masks for clearing signal within whole message data
		clr_mask = cg.get_bit_mask_str_binary(signal.len,signal.get_abs_start_bit(),True,message.len*8)
		cog.outl(sym_mask_sig_clr_in_msg+signal.name+chr(9)+chr(9)+"(0b"+clr_mask+"ull)")


		#Generate macro for initial value
		if signal.init_value is not None:
			sig_init_val = "#error 'wrong signal init value'"
			if signal.is_enum_type is True:
				sig_init_val = datat_symb_prfx + signal.init_value
			elif signal.is_scalar() is True:
				sig_init_val = cg.to_hex_string_with_suffix(signal.init_value, signal.len)
			elif signal.is_array() is True:
				byte_val = hex(signal.init_value)
				byte_val = byte_val[2:]
				if len(byte_val) == 1:
					byte_val = "0" + byte_val
				sig_init_val = "0x"
				for i in range(int(signal.len/8)):
					sig_init_val = sig_init_val + byte_val
				sig_init_val = cg.to_hex_string_with_suffix(sig_init_val, signal.len)

			cog.outl(sym_init_value+signal.name+chr(9)+chr(9)+"("+sig_init_val+")")

		cog.outl()
]]] */
// [[[end]]]


/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
