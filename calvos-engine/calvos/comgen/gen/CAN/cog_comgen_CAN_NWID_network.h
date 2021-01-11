/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file 		CAN_signals.h
 *  \brief     	Header file CAN Signals definitions.
 *  \details   	Contains data structures to ease the handling of the CAN 
 *				signals.
 *  \author    	Carlos Calvillo
 *  \authors	Carlos Calvillo
 *  \version   	1.0
 *  \date      	1990-2011
 *  \copyright 	GNU Public License v3.
 */
/*============================================================================*/
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

cog.outl(cg.C_license())

]]] */
// [[[end]]]
/*============================================================================*/
/* [[[cog
try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))

# Print generation information
cog.outl(C_gen_info(input_worksheet,network))
]]] */
// [[[end]]]
/* [[[cog
# Print include guards
cog.outl("#ifndef COMGEN_CAN_"+network.id_string.upper()+"_NETWORK_H")
cog.outl("#define COMGEN_CAN_"+network.id_string.upper()+"_NETWORK_H")
]]] */
// [[[end]]]

#include "LIN_common_network.h"

/* -------------------------------------------------------------------------- */
// 		Network Messages
/* -------------------------------------------------------------------------- */
/* [[[cog
# Code for individual CAN msgs lenght
code_string = ""
for message in network.messages.values():
    cog.outl("#define CAN_" + network.id_string + "_MSG_DLEN_" + message.name \
			+ "\t(" + str(message.len) + "u)")
]]] */
// [[[end]]]

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

/* [[[cog
# Code for CAN_MSGS_TOTAL_DLEN
code_string = "#define CAN_" + network.id_string + "_MSGS_TOTAL_DLEN ("
for message in network.messages.values():
    code_string += "CAN_" + network.id_string + "_MSG_DLEN_" + message.name \
			+ " \\ \n\t\t\t + "
code_string = code_string[:-10] + ")"
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
#Get structure of all network messages

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

	array_str = "msg_buffer"

	for signal in message_signals:	
		if signal.is_array():
			cog.outl("#define CAN_"+network.id_string+"_get_ptr_" \
					+ signal.name + "(msg)\t\t( &"+array_str+"[" \
					+ str(signal.start_byte) + "] )" )
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

					# Add right shifting if required
					if piece.shift_inner is not None:
						macro_piece += " >> " \
							+ cg.shifter_string_with_suffix(piece.shift_inner)

					# Add signal casting if signal base size is bigger than 8-bits
					if signal_access.signal_base_len > 8:
						macro_piece = "(" \
							+ cg.get_dtv(signal_access.signal_base_len) \
							+ ")(" + macro_piece + ")"

					if piece.mask_inner is not None:
						macro_piece = "(" + macro_piece + ") & " \
							+ cg.to_hex_string_with_suffix(piece.mask_inner)

					if piece.shift_outer is not None:
						macro_piece += " << " \
							+ cg.shifter_string_with_suffix(piece.shift_outer)



					if len(signal_access.pieces) > 1:
						if i == 0:
							macro_str = "("
						if i < (len(signal_access.pieces) - 1):
							macro_str += "(" + macro_piece + ") \\ \n\t\t\t\t\t\t\t\t\t| "
						else:
							macro_str += "(" + macro_piece + ")"
					else:
							macro_str += "(" + macro_piece + ")"
			else:
				macro_str = "ERROR, invalid signal '" \
						+ signal.name + "' access structure."
				# TODO: logging system for this file
				log_warn("Invalid signal '%s' access structure" & signal.name)

			cog.outl("#define CAN_"+network.id_string+"_extract_" \
					+ signal.name + "("+array_str+")\t\t" + macro_str)

			cog.outl("#define CAN_"+network.id_string+"_get_" \
					+ signal.name + "(msg)\t\t" \
					+ "(CAN_"+network.id_string+"_extract_" \
					+ signal.name + "(msg.all))")

			cog.outl("#define CAN_"+network.id_string+"_get_direct_" \
					+ signal.name + "()\t\t" \
					+ "(CAN_"+network.id_string+"_extract_" \
					+ signal.name + "(unified_buffer))")
	
		# Signal write macros
		# -------------------
		if signal.is_array():
			cog.outl("#define CAN_"+network.id_string+"_update_" \
					+ signal.name + "(msg, values)\t\t( memcpy(&"+array_str+"["+str(signal.start_byte) \
					+ "],&values, CAN_" + network.id_string + "_MSG_DLEN_" + message.name + ")" )
		else:
			pass
		
		cog.outl("")
]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" COMGEN_CAN_"+network.id_string.upper() \
		+"_NETWORK_H" + chr(42) + "/")
]]] */
// [[[end]]]
