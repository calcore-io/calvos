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

class MsgConstantData:
	def __init__(self, name = "", len = None):
		self.name = name
		self.len = len
		self.pieces = [] #List of size of pieces in bits, e.g., [32,16,8,...]
		self.signals = [] #list of SignalConstantData objects

class SignalConstantData:
	def __init__(self, name = "", len = None):
		self.name = name
		self.len = len
		self.clr_in_msg_mask = None
		self.pieces = [] #list of SignalConstantDataPiece objects

class SignalConstantDataPiece:
	def __init__(self, len = None, val = None):
		self.len = len
		self.val = val
		self.clr_mask = None
		self.mask = None

# Get structure of all network messages
sym_len_prefix = "kCAN_" + net_name_str + "SIG_LEN_"
sym_bit_prefix = "kCAN_" + net_name_str + "SIG_START_BIT_"
sym_byte_prefix = "kCAN_" + net_name_str + "SIG_START_BYTE_"
sym_abs_bit_prefix = "kCAN_" + net_name_str + "SIG_ABS_START_BIT_"

sym_mask_sig = "kCAN_" + net_name_str + "SIG_MASK"

sym_mask_sig_clr = "kCAN_" + net_name_str + "SIG_MASK_CLR"
sym_mask_sig_clr_in_msg = "kCAN_" + net_name_str + "SIG_MASK_CLR_IN_MSG_"

sym_init_value = "kCAN_" + net_name_str + "SIG_INIT_VAL_"

datat_symb_prfx = network.get_simple_param("CAN_enum_sym_prefix")
if datat_symb_prfx == "$":
	datat_symb_prfx = ""
else:
	datat_symb_prfx = \
		cg.resolve_wildcards(datat_symb_prfx, {"NWID":network.id_string})


sym_msg_clr_init = "kCAN_" + net_name_str + "MSG_CLR_INITS_"
sym_msg_clr_init_piece = "kCAN_" + net_name_str + "MSG_CLR_INITS"

sym_msg_init_vals = "kCAN_" + net_name_str + "MSG_INIT_VALS_"
sym_msg_init_vals_piece = "kCAN_" + net_name_str + "MSG_INIT_VALS_"

sym_set_msg_inits = "CAN_" + net_name_str + "write_msg_init_vals_"

#for i in range(1,65):
#	if i == i:
#		print(" i = ",i," pieces: ", cg.split_bit_len_in_base_types(i))
#		mask = cg.get_bit_mask_str_binary(i)
#		#mask = "000000000000rqponmlkjihgfedcba0987654321"
#		#print("mask: ", mask)
#		print(cg.split_bit_str_in_base_types(i,mask))
#		for piece in cg.split_bit_str_in_base_types(i,mask):
#			print(hex(int(piece[1],2)))

msgs = []

for message in network.messages.values():
	signals = network.get_signals_of_message(message.name)

	msg = MsgConstantData(message.name, message.len)
	base_pieces = cg.split_bit_len_in_base_types(message.len*8)
	for base_piece in base_pieces:
		msg.pieces.append(base_piece)

	for signal_obj in signals:

		signal = SignalConstantData(signal_obj.name, signal_obj.len)
		base_pieces = cg.split_bit_len_in_base_types(signal_obj.len)
		reamining_len = signal_obj.len
		for base_piece in base_pieces:
			piece = SignalConstantDataPiece(base_piece, 0)
			if reamining_len > base_piece:
				mask = cg.get_bit_mask_str_hex(base_piece)
				clr_mask = cg.get_bit_mask_str_hex(base_piece, 0, True, base_piece)
				reamining_len -= base_piece
			else:
				mask = cg.get_bit_mask_str_hex(reamining_len)
				clr_mask = cg.get_bit_mask_str_hex(reamining_len, 0, True, base_piece)
				reamining_len = 0

			piece.mask = mask
			piece.clr_mask = clr_mask

			signal.pieces.append(piece)

		signal.clr_in_msg_mask = cg.get_bit_mask_str_hex(signal.len,signal_obj.get_abs_start_bit(),True,message.len*8)
		msg.signals.append(signal)

	msgs.append(msg)

for message in msgs:
	cog.outl("\n/"+chr(42)+" Signals of message \"" + message.name + "\" " + chr(42) + "/\n")

	msg_base_type = cg.get_dtv(cg.calculate_base_type_len(message.len*8))

	for signal in message.signals:
		signal_obj = network.signals[signal.name]
		name = signal_obj.name
		sig_len = str(signal_obj.len)
		sig_start_bit = str(signal_obj.start_bit)
		sig_start_byte = str(signal_obj.start_byte)
		sig_abs_start_bit = str(signal_obj.get_abs_start_bit())
		sig_init = str(signal_obj.init_value)

		cog.outl("#define "+sym_len_prefix+name+"\t\t("+sig_len+"u)")
		cog.outl("#define "+sym_bit_prefix+name+"\t\t("+sig_start_bit+"u)")
		cog.outl("#define "+sym_byte_prefix+name+"\t\t("+sig_start_byte+"u)")
		cog.outl("#define "+sym_abs_bit_prefix+name+"\t\t("+sig_abs_start_bit+"u)")

		cog.outl("#define "+sym_mask_sig_clr_in_msg+name+"\t\t(("+msg_base_type+")"+cg.to_hex_string_with_suffix(signal.clr_in_msg_mask)+")")

#		# Signal Masks
#		if len(signal.pieces) > 1:
#			for i, piece in enumerate(signal.pieces):
#				cog.outl("#define "+sym_mask_sig+str(i)+"_"+name+"\t\t("+piece.mask+")")
#		else:
#			cog.outl("#define "+sym_mask_sig+"_"+name+"\t\t("+signal.pieces[0].mask+")")
#
#		# Signal Clear Masks
#		if len(signal.pieces) > 1:
#			for i, piece in enumerate(signal.pieces):
#				cog.outl("#define "+sym_mask_sig_clr+str(i)+"_"+name+"\t\t("+piece.clr_mask+")")
#		else:
#			cog.outl("#define "+sym_mask_sig_clr+"_"+name+"\t\t("+signal.pieces[0].clr_mask+")")
#

		if signal_obj.init_value is not None:
			sig_init_val = "#error 'wrong signal init value'"
			if signal_obj.is_enum_type is True:
				sig_init_val = datat_symb_prfx + signal_obj.init_value
			elif signal_obj.is_scalar() is True:
				sig_init_val = cg.to_hex_string_with_suffix(signal_obj.init_value, signal_obj.len)
			elif signal_obj.is_array() is True:
				byte_val = hex(signal_obj.init_value)
				byte_val = byte_val[2:]
				if len(byte_val) == 1:
					byte_val = "0" + byte_val
				sig_init_val = "0x"
				for i in range(int(signal_obj.len/8)):
					sig_init_val = sig_init_val + byte_val
				sig_init_val = cg.to_hex_string_with_suffix(sig_init_val, signal_obj.len)

			cog.outl("#define "+sym_init_value+name+"\t\t"+"("+sig_init_val+")")

		cog.outl()

	if network.message_has_init_values(message.name) is True:
		# Msg clear init macro
		sym_msg_clr_macro_name = sym_msg_clr_init+message.name
		sym_macro_value = "("
		sig_with_init_idx = 0

		init_signals = network.get_signals_with_init_values(message.name)
		for i, init_signal in enumerate(init_signals):
			if i > 0:
				sym_macro_value += "| "
			sym_macro_value += sym_mask_sig_clr_in_msg + init_signal.name
			if i < len(init_signals)-1:
				sym_macro_value += " \\\n\t\t\t\t\t\t\t\t\t\t\t"
		sym_macro_value += ")"

		cog.outl("#define "+sym_msg_clr_macro_name+"\t\t"+sym_macro_value)

		cog.outl()

		# Msg clear init macro pieces
		if len(message.pieces) > 1:
			next_shifting = 0
			for i, piece in enumerate(message.pieces):
				sym_macro_name = sym_msg_clr_init_piece+str(i)+"_"+message.name
				if next_shifting > 0:
					sym_macro_value = "(("+cg.get_dtv(piece)+")((("+msg_base_type+")"+sym_msg_clr_macro_name+" >> " + str(next_shifting) + "u) & " + cg.get_bit_mask_str_hex(piece) + "))"
				else:
					sym_macro_value = "(("+cg.get_dtv(piece)+")(("+msg_base_type+")"+sym_msg_clr_macro_name+" & " + cg.get_bit_mask_str_hex(piece) + "))"
				cog.outl("#define "+sym_macro_name+"\t\t"+sym_macro_value)
				next_shifting += piece

		cog.outl()

		# Msg Init vals macro
		sym_msg_init_macro_name = sym_msg_init_vals+message.name
		sym_macro_value = "( "
		for i, init_signal in enumerate(init_signals):
			if i > 0:
				sym_macro_value += "| "
			if init_signal.get_abs_start_bit() > 0:
				sym_macro_value += "(("+msg_base_type+")" + sym_init_value + init_signal.name + " << " + sym_abs_bit_prefix + init_signal.name + ")"
			else:
				sym_macro_value += "("+msg_base_type+")"+sym_init_value + init_signal.name
			if i < len(init_signals)-1:
				sym_macro_value += " \\\n\t\t\t\t\t\t\t\t\t\t\t"
		sym_macro_value += ")"

		cog.outl("#define "+sym_msg_init_macro_name+"\t\t"+sym_macro_value)

		cog.outl()

		# Msg init values macro pieces
		if len(message.pieces) > 1:
			next_shifting = 0
			for i, piece in enumerate(message.pieces):
				sym_macro_name = sym_msg_init_vals_piece+str(i)+"_"+message.name
				if next_shifting > 0:
					sym_macro_value = "(("+cg.get_dtv(piece)+")((("+msg_base_type+")"+sym_msg_init_macro_name+" >> " + str(next_shifting) + "u) & " + cg.get_bit_mask_str_hex(piece) + "))"
				else:
					sym_macro_value = "(("+cg.get_dtv(piece)+")(("+msg_base_type+")"+sym_msg_init_macro_name+" & " + cg.get_bit_mask_str_hex(piece) + "))"
				cog.outl("#define "+sym_macro_name+"\t\t"+sym_macro_value)
				next_shifting += piece

		cog.outl()


		# Write Msg Init values
		sym_macro_name = sym_set_msg_inits + message.name + "(data_buffer)"

		sym_macro_value = ""
		if len(message.pieces) > 1:
			# Clear data
			byte_idx = 0
			for i, piece in enumerate(message.pieces):
				sym_macro_value += "*("+cg.get_dtv(piece)+"*)(&data_buffer["+str(byte_idx)+"]) &= "+sym_msg_clr_init_piece+str(i)+"_"+message.name+";"
				sym_macro_value += " \\\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
				byte_idx += int(piece/8)

			# Write init data
			byte_idx = 0
			for i, piece in enumerate(message.pieces):
				sym_macro_value += "*("+cg.get_dtv(piece)+"*)(&data_buffer["+str(byte_idx)+"]) |= "+sym_msg_init_vals_piece+str(i)+"_"+message.name+";"
				if i < len(message.pieces)-1:
					sym_macro_value += " \\\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
				byte_idx += int(piece/8)
		else:
			sym_macro_value += "*("+cg.get_dtv(message.len*8)+"*)(&data_buffer[0]) &= "+sym_msg_clr_init+message.name+";"
			sym_macro_value +=  " \\\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t"
			sym_macro_value += "*("+cg.get_dtv(message.len*8)+"*)(&data_buffer[0]) |= "+sym_msg_init_vals+message.name+";"

		cog.outl("#define "+sym_macro_name+"\t\t"+sym_macro_value)




]]] */
// [[[end]]]


/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
