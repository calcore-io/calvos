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

/* -------------------------------------------------------------------------- */
// 		Network Messages
/* -------------------------------------------------------------------------- */

/* Macros for message(s) length */
/* [[[cog

TAB_SPACE = 4

# Code for individual CAN msgs lenght

# Calculate padding spaces
macro_prefix = "CAN_" + network.id_string + "_MSG_DLEN_"
macro_names = []
macro_values = []
for message in network.messages.values():
	macro_name = macro_prefix + message.name
	macro_value = "(" + str(message.len) + "u)"
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

/* Macro for getting the sum of the CAN msgs lenght. */
/* This to be used for defining the unified CAN data buffer. */

/* [[[cog
macro_name = "CAN_" + network.id_string + "_MSGS_TOTAL_LEN"

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

/* -------------------------------------------------------------------------- */
// 		Signal TYPES
/* -------------------------------------------------------------------------- */

/* [[[cog
for enum_type in network.enum_types.values():
	cog.out("typedef enum{\n\t")
	counter = 0
	code_string = ""
	for enum_entry in enum_type.enum_entries:
		counter += 1
		code_string += enum_entry
		if enum_type.enum_entries[enum_entry] is not None:
			code_string += " = " + str(enum_type.enum_entries[enum_entry])
		if counter < len(enum_type.enum_entries):
			#Append a comma, new line and tab for all entries except the last one
			code_string += ",\n\t"
	cog.outl(code_string)
	cog.outl("}t_" + enum_type.name + ";\n")
]]] */
// [[[end]]]

/* -------------------------------------------------------------------------- */
// 		Message structures (layout of signals)
/* -------------------------------------------------------------------------- */

/* [[[cog
# Get structure of all network messages

messages_layouts = network.get_messages_structures()

for message in messages_layouts:

	# Message structures
	# ------------------
	# Print structure comment
	cog.outl("\n/"+chr(42)+" Structure for network \""+ network.id_string
			+ "\" message \"" + message.name + "\"." + chr(42) + "/")

	# Print structure "header"
	cog.outl("typedef union{\n\tstruct"+cg.pack_struct_tag+"{")
	
	# If message is cannonical, don't use bitfields
	if message.is_cannonical:
		for signal in message.signals:
			signal_container_len = cg.calculate_base_type_len(signal.fragments[1])
			remaining_bits = signal.fragments[1]


			if signal_container_len > remaining_bits:
				prefix_number = 0
				prefix = cg.gen_part_prefixes[cg.PRFX_DEFAULT]

				msg_signals_temp = network.get_signals_of_message(message.name)
				msg_signals = []
				for msg_signal in msg_signals_temp:
					msg_signals.append(msg_signal.name)

				# Signal needs to be partitioned
				# TODO: This fragmentation logic of cannonican messages with
				# length of bytes not fitting exactly a data type can be moved
				# to function get_messages_structures.
				while signal_container_len > 8 \
				and signal_container_len > remaining_bits:
					signal_container_len = int(signal_container_len / 2)
					remaining_bits -= signal_container_len

					# Set fragment name
					fragment_name = str(signal.fragments[0]) + "_" \
						+ prefix + str(prefix_number)
					# Check if fragment name doesn't duplicate a signal
					# name, if so, try different part prefixes.
					prefix_idx = 0
					while fragment_name in msg_signals:
						# Fragment name duplicates an existing signal name
						# try different part prefix
						prefix_idx += 1
						if prefix_idx > (len(cg.gen_part_prefixes) - 1):
							# All prefixes exhausted. Use a disruptive
							# one so that it is evident and user can
							# manually name the signal parts in the C code.
							log_warn(("Can't generate unique name" \
							   + " for part "+ fragment_name \
							   + "\" of signal \"" + signal_name \
							   + "\" since it duplicates with existing" \
							   + "signal in same message."))

							prefix = "#"
							fragment_name = str(signal.fragments[0]) + "_" \
								+ prefix + str(prefix_number)
							break;
						else:
							log_warn(("Signal part \"" + fragment_name \
							   + "\" of signal \"" + signal_name \
							   + "\" duplicates with name of existing " \
							   + "signal in same message." \
							   + " Attempting with prefix \"" \
							   + cg.gen_part_prefixes[prefix_idx] + "\""))

							# Try new prefix
							prefix = cg.gen_part_prefixes[prefix_idx]
							fragment_name = str(signal.fragments[0]) + "_" \
								+ prefix + str(prefix_number)

					prefix_number += 1
					cog.outl("\t\t" + cg.get_dtv(signal_container_len) \
						+ " " + str(fragment_name) + ";")
			else:
				cog.outl("\t\t" + cg.get_dtv(signal.fragments[1]) \
						+ " " + str(signal.fragments[0]) + ";")
	else:
		# Bitfield is required for non-cannonical messages
		for signal in message.signals:
			cog.outl("\t\t" + cg.get_dtv(message.base_type_len) \
					+ " " + str(signal.fragments[0]) \
					+ " : " + str(signal.fragments[1]) + ";")
	
	# Print structure "tail"
	cog.outl("\t} s;")
	cog.outl("\t"+cg.get_dtv(8)+" all[CAN_" + network.id_string \
			+ "_MSG_DLEN_" + message.name + "];")
	cog.outl("}S_" + message.name + ";\n")

	# Signal read / write macros
	# --------------------------
	message_signals = network.get_signals_of_message(message.name)
	
	# Print read macro comment
	cog.outl("/"+chr(42)+" Macros for reading/writing signals of network \"" \
			+ network.id_string + "\" message \"" + message.name \
			+ "\". " + chr(42) + "/")
	cog.outl("")

	array_str = "msg_buffer"
	data_in_str = "data"

	for signal in message_signals:
		def_read_array = "CAN_" + network.id_string + "_get_ptr_" \
						+ signal.name + "(" + array_str + ")"
		def_read = "CAN_"+network.id_string+"_extract_" \
						+ signal.name + "(" + array_str + ")"
		def_get = "CAN_"+network.id_string+"_get_" \
						+ signal.name + "(" + array_str + ")"
		def_get_direct = "CAN_"+network.id_string+"_get_direct_" \
						+ signal.name + "()"

		def_write_array = "CAN_" + network.id_string + "_update_" \
						+ signal.name + "(" + array_str + "," + data_in_str + ")"
		def_write = "CAN_"+network.id_string+"_write_" \
						+ signal.name + "(" + array_str + "," + data_in_str + ")"
		def_update = "CAN_"+network.id_string+"_update_" \
						+ signal.name + "(" + array_str + "," + data_in_str + ")"
		def_update_direct = "CAN_"+network.id_string+"_update_direct_" \
						+ signal.name + "(" + data_in_str + ")"

		max_len = cg.get_str_max_len([def_read_array, def_read, \
									def_get, def_get_direct, \
									def_write_array, def_write, def_update, \
									def_update_direct])
		# From longest string add TAB_SPACE spaces for padding
		max_len += TAB_SPACE

		# Calculate padding
		pad_read_array = cg.gen_padding(max_len, len(def_read_array))
		pad_read = cg.gen_padding(max_len, len(def_read))
		pad_get = cg.gen_padding(max_len, len(def_get))
		pad_get_direct = cg.gen_padding(max_len, len(def_get_direct))

		pad_write_array = cg.gen_padding(max_len, len(def_write_array))
		pad_write = cg.gen_padding(max_len, len(def_write))
		pad_update = cg.gen_padding(max_len, len(def_update))
		pad_update_direct = cg.gen_padding(max_len, len(def_update_direct))

		# Print read macro comment
		cog.outl("/"+chr(42)+" Macros for reading signal \"" \
			+ signal.name + "\". " + chr(42) + "/")

		# Signal read macros
		# ------------------
		if signal.is_array():
			cog.outl("#define "+ def_read_array \
					+ pad_read_array \
					+ "( &"+array_str+"[" + str(signal.start_byte) + "] )" )
		else:
			signal_access = network.get_signal_abstract_read(signal.name)

			if len(signal_access.pieces) > 0 :
				macro_str = ""
				for i, piece in enumerate(signal_access.pieces):
					# Add signal casting if piece base size is bigger than 8-bits
					if piece.len > 8:
						macro_piece = "*("+cg.get_dtv(piece.len)+"*)&"+array_str \
							+ "[" + str(piece.abs_byte) + "]"
					else:
						macro_piece = array_str + "[" + str(piece.abs_byte) + "]"

					# Add inner mask if required
					if piece.mask_inner is not None:
						macro_piece = "(" + macro_piece + ") & " \
							+ cg.to_hex_string_with_suffix(piece.mask_inner)

					# Add right shifting if required (shift inner)
					if piece.shift_inner is not None:
						macro_piece = "(" + macro_piece + ") >> " \
							+ cg.shifter_string_with_suffix(piece.shift_inner)

					if piece.mask_outer is not None:
						macro_piece = "(" + macro_piece + ") & " \
							+ cg.to_hex_string_with_suffix(piece.mask_outer)

					# Add signal casting if signal base size is bigger than 8-bits
					if signal_access.signal_base_len > 8:
						macro_piece = "(" \
							+ cg.get_dtv(signal_access.signal_base_len) \
							+ ")(" + macro_piece + ")"

					if piece.shift_outer is not None:
						macro_piece += " << " \
							+ cg.shifter_string_with_suffix(piece.shift_outer)

					if len(signal_access.pieces) > 1:
						if i == 0:
							macro_str = "("
						if i < (len(signal_access.pieces) - 1):
							macro_str += "(" + macro_piece + ") \\\n"+cg.gen_padding(max_len+8)+"| "
						else:
							macro_str += "(" + macro_piece + "))"
					else:
							macro_str += "(" + macro_piece + ")"
			else:
				macro_str = "ERROR, invalid signal '" \
						+ signal.name + "' access structure."
				# TODO: logging system for this file
				log_warn("Invalid signal '%s' access structure" & signal.name)

			cog.outl("#define " + def_read + pad_read + macro_str)

			cog.outl("#define " + def_get + pad_get \
				+ "(" + def_read.replace("("+array_str+")","("+array_str+".all)") + ")" )

			cog.outl("#define "+def_get_direct + pad_get_direct \
				+ "(" + def_read.replace("("+array_str+")","(unified_buffer)") + ")" )
	
		# Signal write macros
		# -------------------

		cog.outl("/"+chr(42)+" Macros for writing signal \"" \
			+ signal.name + "\". " + chr(42) + "/")
		if signal.is_array():
			cog.outl("#define " + def_write_array + pad_write_array \
					+"( memcpy(&"+array_str+"["+str(signal.start_byte) \
					+ "],&values, CAN_" + network.id_string \
					+ "_MSG_DLEN_" + message.name + ")" )
		else:
			pass

			signal_access = network.get_signal_abstract_write(signal.name)

			if len(signal_access.pieces) > 0 :
				macro_str = ""
				for i, piece in enumerate(signal_access.pieces):
					macro_str_clear = ""
					macro_str_write = ""
					macro_str_assign = ""

					# Add signal casting if piece base size is bigger than 8-bits
					if piece.len > 8:
						macro_str_assign = "*("+cg.get_dtv(piece.len)+"*)&"+array_str \
							+ "[" + str(piece.abs_byte) + "]"
					else:
						macro_str_assign = array_str + "[" + str(piece.abs_byte) + "]"

					# Mask out bits to be written
					if piece.mask_inner is not None:
						macro_str_clear = macro_str_assign + " & " \
							+ cg.to_hex_string_with_suffix(piece.mask_inner, \
									piece.len)
					else:
						macro_str_clear = ""

					# Shifting part of the "write" macro depends on the chunk number
					if i == 0:
						# For first chunk, "write" macro shall do a left shifting
						if piece.shift_inner is not None:
							macro_str_write = data_in_str + " << " \
								+ cg.shifter_string_with_suffix(piece.shift_inner)
						else:
							macro_str_write = data_in_str
					else:
						# For subsequent chunks, "write" macro shall do a
						# right shiftings
						if piece.shift_inner is not None:
							macro_str_write = data_in_str + " >> " \
								+ cg.shifter_string_with_suffix(piece.shift_inner)
						else:
							macro_str_write = data_in_str

					# Add masking part of "write" macro
					if piece.mask_outer is not None:
						macro_str_write = "(" + macro_str_write + ") & " \
							+ cg.to_hex_string_with_suffix(piece.mask_outer)

					# Form piece string
					if len(signal_access.pieces) > 1:
						if i == 0:
							macro_str = macro_str_assign + " = "
						else:
							macro_str += macro_str_assign + " = "

						if macro_str_clear != "":
							macro_str += "(" + macro_str_clear + ") | " \
									+ "(" + macro_str_write + ");"
						else:
							macro_str += macro_str_write + ";"

						if i < (len(signal_access.pieces) - 1):
							macro_str += " \\\n" + cg.gen_padding(max_len+8)
					else:
						macro_str = macro_str_assign + " = "
						if macro_str_clear != "":
							macro_str += "(" + macro_str_clear + ") | "
						macro_str += "(" + macro_str_write + ");"
			else:
				macro_str = "ERROR, invalid signal '" \
						+ signal.name + "' access structure."
				# TODO: logging system for this file
				log_warn("Invalid signal '%s' access structure" & signal.name)

			cog.outl("#define " + def_write + pad_write + macro_str)
			cog.outl("#define " + def_update + pad_update \
				+ def_write.replace("("+array_str+",","("+array_str+".all,"))
			cog.outl("#define " + def_update_direct + pad_update_direct \
				+ def_write.replace("("+array_str+",","(unified_buffer"))
		
		cog.outl("")
]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
