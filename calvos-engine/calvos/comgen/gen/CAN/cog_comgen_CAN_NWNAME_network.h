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
/*  This file is part of calvOS project.
 *
 *  calvOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  calvOS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with calvOS.  If not, see <https://www.gnu.org/licenses/>. */
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

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))
]]] */
// [[[end]]]
/* [[[cog
# Print generation information
curren_time = time.localtime() # Gets current local time
return_string = str(curren_time[1])+"." \
				+ str(curren_time[2])+"." \
				+ str(curren_time[0])+"::" \
				+ str(curren_time[3])+":" \
				+ str(curren_time[4])+":" \
				+ str(curren_time[5])
cog.outl("/"+chr(42)+"-----------------------------------------------------------------------------")
cog.outl(" * This file was generated on (mm.dd.yyyy::hh:mm:ss): " + return_string)
cog.outl(" * Generated from following source(s):")
cog.outl(" * 	Network file: \"" + str(input_worksheet) + "\"")
cog.outl(" * 	Network name: \"" + str(network.name) + "\"")
cog.outl(" * 	Network date: \"" + str(network.date) + "\"")
cog.outl(" * 	Network version: \"" + str(network.version) + "\"")
cog.outl(" -----------------------------------------------------------------------------"+chr(42)+"/")
]]] */
// [[[end]]]
/* [[[cog
# Print include guards
cog.outl("#ifndef COMGEN_CAN_"+network.name.upper()+"_NETWORK_H")
cog.outl("#define COMGEN_CAN_"+network.name.upper()+"_NETWORK_H")
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
    cog.outl("#define CAN_" + network.name + "_MSG_DLEN_" + message.name \
			+ "\t(" + str(message.len) + "u)")
#print(code_string)
]]] */
// [[[end]]]

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

/* [[[cog
# Code for CAN_MSGS_TOTAL_DLEN
code_string = "#define CAN_" + network.name + "_MSGS_TOTAL_DLEN ("
for message in network.messages.values():
    code_string += "CAN_" + network.name + "_MSG_DLEN_" + message.name \
			+ " + \\ \n\t\t\t"
code_string = code_string[:-8] + ")"
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
	cog.outl("\n/"+chr(42)+" Structure for network \""+ network.name 
			+ "\" message \"" + message.name + "\"." + chr(42) + "/")

	# Print structure "header"
	cog.outl("typedef union{\n\tstruct{")
	
	# If message is cannonical, don't use bitfields
	if message.is_cannonical:
		for signal in message.signals:
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
	cog.outl("\t"+cg.get_dtv(8)+" all[CAN_" + network.name \
			+ "_MSG_DLEN_" + message.name + "];")
	cog.outl("}S_" + message.name + ";\n")

	# Signal read / write macros
	# --------------------------
	message_signals = network.get_signals_of_message(message.name)
	
	# Print read macro comment
	cog.outl("/"+chr(42)+" Macros for reading/writing signals of network \"" \
			+ network.name + "\" message \"" + message.name \
			+ "\". " + chr(42) + "/")
			
	for signal in message_signals:	
		base_len = cg.calculate_base_type_len(signal.len)
		
		if signal.is_array():
			cog.outl("#define CAN_"+network.name+"_get_ptr_" \
					+ signal.name + "(msg)\t\t( &msg.all[" \
					+ str(signal.start_byte) + "] )" )
		
		else:
			array_byte = signal.start_byte
			current_byte = 0
			remaining_bits = signal.len
			macro_str = ""
			
			# Get string of LSByte 
			if signal.start_bit > 0:
				macro_str = "( (" + cg.get_dtv(base_len) \
					+ ")msg.all[" + str(array_byte) + "] >> " \
					+ str(signal.start_bit) + "u )"
			else:
				macro_str = "( (" + cg.get_dtv(base_len) \
					+ ")msg.all[" + str(array_byte) + "] ) "
#			consumed_bits = 8 - signal.start_bit
			
			if signal.len <= (8 - signal.start_bit):
				# Only one "byte" needed for this signal's macro
				mask = int(255 << signal.len).to_bytes(8,byteorder='big')
				mask = hex(mask[-1] ^ 255)
			
				if int(mask,0) != 0 and int(mask,0) != 255: 
					macro_str = "(" + cg.get_dtv(base_len) \
						+ ")(" + macro_str + " & " + str(mask) + ")"	
				elif int(mask,0) == 255:
					macro_str = "(" + cg.get_dtv(base_len) \
						+ ")(" + macro_str + ")"
				else:
					cog.outl("# Test Else First piece, mask: "+ mask)
					# Mask of 0 means nothing is needed here.
					pass
			else:
				# Multiple bytes may be needed
				
				consumed_bits = 8 - signal.start_bit
				remaining_bits -= consumed_bits
				# Next bytes will be taken in two parts. First part
				# will try to fill the previously started byte.
				# Second part will start a new byte.
				first_part_len = signal.start_bit
				# TODO: for to_bytes assuming max 8 bytes for a variable size,
				# make this compiler dependant not fixed to 8.
				first_part_mask = int(255 << first_part_len).to_bytes(8,byteorder='big')
				first_part_mask = hex(first_part_mask[-1] ^ 255) 
				
				second_part_len = 8 - signal.start_bit
				second_part_mask = int(255 << first_part_len).to_bytes(8,byteorder='big')
				second_part_mask = hex(second_part_mask[-1])
				
				while remaining_bits > 0:
					# New piece to add, more will be needed.
					if remaining_bits > first_part_len:
						mask = first_part_mask
						remaining_bits -= first_part_len
					else:
						# Last piece to add
						last_mask = int(255 << remaining_bits).to_bytes(8,byteorder='big')
						last_mask = hex(last_mask[-1] ^ 255)
						mask = last_mask
						remaining_bits -= remaining_bits
						if int(mask,0) != 0 and int(mask,0) != 255: 
							first_piece_str = "( ((" + cg.get_dtv(base_len) \
								+ ")msg.all[" + str(array_byte + current_byte + 1) \
								+ "] & "+ str(mask) +"u) << " \
								+ str((8 * current_byte)+(8-signal.start_bit)) \
								+ "u )"
							macro_str = first_piece_str + " \\ \n\t\t\t\t| " + macro_str
						elif int(mask,0) == 255:
							first_piece_str = "( ((" + cg.get_dtv(base_len) \
								+ ")msg.all[" + str(array_byte + current_byte + 1) \
								+ "]) << " \
								+ str((8 * current_byte)+(8-signal.start_bit)) \
								+ "u )"
							macro_str = first_piece_str + " \\ \n\t\t\t\t| " + macro_str
						else:
							# Mask of 0 means nothing is needed here.
							pass
#					macro_str = first_piece_str + " \\ \n\t\t\t\t| " + macro_str
					# See if second piece is needed
					# Add new piece, more will be needed.
					if remaining_bits > 0:
						if remaining_bits > second_part_len:
							mask = second_part_mask
							remaining_bits -= second_part_len
						else:
							# Last piece to add
							last_mask = int(255 << remaining_bits).to_bytes(8,byteorder='big')
							last_mask = hex(last_mask[-1] ^ 255)
							mask = last_mask
							remaining_bits -= remaining_bits
							
						
						if int(mask,0) != 0 and int(mask,0) != 255:
							second_piece_str = "( ((" + cg.get_dtv(base_len) \
								+ ")msg.all[" + str(array_byte + current_byte + 1) \
								+ "] & " + str(mask) \
								+ "u) << " + str(8 * (current_byte + 1) - first_part_len) + ")"
							macro_str = second_piece_str + " \\ \n\t\t\t\t| " + macro_str
						elif int(mask,0) == 255:
							second_piece_str = "( ((" + cg.get_dtv(base_len) \
								+ ")msg.all[" + str(array_byte + current_byte + 1) \
								+ "] ) << " \
								+ str(8 * (current_byte + 1) - first_part_len) + ")"
							macro_str = second_piece_str + " \\ \n\t\t\t\t| " + macro_str
						else:
							# Mask of 0 means nothing is needed here.
							pass 
					
					current_byte += 1
				
			cog.outl("#define CAN_" + network.name + "_get_"+signal.name+"(msg)" \
					+ "\t\t( "+macro_str+" )")
	
	
		# Signal write macros
		# -------------------
		if signal.is_array():
			cog.outl("#define CAN_"+network.name+"_update_" \
					+ signal.name + "(msg, values)\t\t( memcpy(&msg.all["+str(signal.start_byte) \
					+ "],&values, CAN_" + network.name + "_MSG_DLEN_" + message.name + ")" )
		else:
			pass
		
		cog.outl("")
]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" COMGEN_CAN_"+network.name.upper() \
		+"_NETWORK_H" + chr(42) + "/")
]]] */
// [[[end]]]
