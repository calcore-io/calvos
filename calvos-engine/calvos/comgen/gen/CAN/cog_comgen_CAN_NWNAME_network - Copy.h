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
    cog.outl("#define CAN_" + network.name + "_MSG_DLEN_" + message.name + "\t(" + str(message.len) + "u)")
#print(code_string)
]]] */
// [[[end]]]

// Macro for getting the sum of the CAN msgs lenght.
// This to be used for defining the CAN data buffer.

/* [[[cog
# Code for CAN_MSGS_TOTAL_DLEN
code_string = "#define CAN_" + network.name + "_MSGS_TOTAL_DLEN ("
for message in network.messages.values():
    code_string += "CAN_" + network.name + "_MSG_DLEN_" + message.name + " + \\ \n\t\t\t"
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

#print(cg.get_dtk(32, "i", False))
#print(cg.dt[cg.get_dtk(32, "i", False)])
#print(cg.get_dtk(32,"i",  True))
#print(cg.dt[cg.get_dtk(32,"i",  True)])

# Get list of sorted signals. This list is expected to contain a list of signals
# for each message. Within these lists, the signals are ordered by their layout
# start byte and start bit.
sorted_signals = network.get_sorted_signals_by_layout(True)

#for test_message in sorted_signals:
#	print("Message: ", list(test_message[0].values())[0].message)
#	for signal in test_message:
#		print(" - Signal: ", list(signal.values())[0].name)

if len(sorted_signals) > 0:
	
	for list_by_message in sorted_signals:	

		message_name = list(list_by_message[0].values())[0].message
		message_len = network.messages[message_name].len
		
		last_reserved_field = 0

		cog.outl("typedef union{\n\tstruct{")
		
		# LOGIC FOR PARTITIONING SIGNAL INTO BYTES
		# ----------------------------------------
		next_start_bit = (list(list_by_message[0].values())[0].start_byte * 8) \
						+ list(list_by_message[0].values())[0].start_bit
		
		# Check if reserved fields are required
		if next_start_bit > 0:
			# Calculate empty space in the message
			empty_space_len = next_start_bit

			# Calculate size of leading reserved field bits
			empty_space_start_bit = 0
	
			splited_signal = nw.Network_CAN.split_signal(empty_space_start_bit, \
				empty_space_start_bit + empty_space_len - 1, 8)
			
			# Print all reserved fields
			for field in splited_signal:
				for each_unit in field:
					if each_unit is not None:
						cog.outl("\t\t" + cg.get_dtv(8) + " reserved_" \
								 + str(last_reserved_field) \
								 + " : " + str(each_unit) + ";")
						last_reserved_field += 1
						
		counter = 0
		# Start using type for unsigned int of 8 bits
		current_type = cg.get_dtk(8)
		current_type_len = cg.dt_info[current_type][0]
		
		for signal in list_by_message:
			counter += 1

			start_byte = list(signal.values())[0].start_byte
			start_bit = list(signal.values())[0].start_bit
			signal_len = int(list(signal.values())[0].len)
			signal_name = list(signal.values())[0].name
			signal_type = list(signal.values())[0].data_type
			
			absolute_start_bit = (start_byte * 8) + start_bit
			absolute_end_bit = absolute_start_bit + signal_len - 1
			
			if counter >= len(list_by_message):
				# This is the last signal in the message.
				# Assume "next start bit" is indeed the last
				# bit of the message.
				next_start_bit = message_len * 8
				next_type = cg.get_dtk(8)
			else:
				next_start_bit = (list(list_by_message[counter].values())[0].start_byte * 8) \
								+ list(list_by_message[counter].values())[0].start_bit
				next_len = list(list_by_message[counter].values())[0].len
				next_type = cg.get_dtk(int(next_len))
			
			next_type_len = cg.dt_info[next_type][0]

			# Generate Signal
			# ---------------
			if signal_type == nw.Network_CAN.data_types_list[1]:
				# Signal is of an "array" type. By default,
				# array type signals are arrays of unsigned bytes.
				array_bytes = int(signal_len / 8)
				cog.outl("\t\t" + cg.get_dtv(8) + " " + str(signal_name) \
				 + "[" + str(array_bytes) + "];")
			else:
			
				base_type_len = cg.calculate_base_type_len(signal_len)
				
				# Check if signal needs to be fragmented or no
				if (absolute_start_bit % 8) == 0 \
				and base_type_len == signal_len:
					# If signal starts at a position which is multiple of a byte
					# and its lenght matches an scalar size then the signal
					# doesn't need to be fragmented
					fragmented_signal = False
				else:
					# Signal may need to be fragmented, verify it by trying to
					# split the signal into leading, nominal and trailing fields.
					splited_signal = nw.Network_CAN.split_signal(absolute_start_bit, \
						absolute_end_bit, \
						base_type_len, current_type_len, next_type_len)
					
					if splited_signal[0][0] == None and splited_signal[2][0] == None \
					and len(splited_signal[1]) == 1:
						# If neither leading nor trailing bits are required and only 
						# one nominal field is required then the signal doesn't 
						# need to be fragmented.
						fragmented_signal = False
					else:
						# If either leading or trailing bits are required or more 
						# than one nominal fields are required then the signal 
						# needs to be fragmented.
						fragmented_signal = True
				
				if fragmented_signal:
					# Partition signal into segments in case the signal crosses
					# byte boundaries. If fragmented, segments will be always bytes
					# Segments will get a suffix _0, _1, ...
					field_position = 0
					fragment_counter = 0
					
#					cog.outl("Test: ")
#					cog.outl(str(splited_signal))
#					cog.outl(str(splited_signal[0][0]))
#					cog.outl(str(splited_signal[1][0]))
#					cog.outl(str(splited_signal[2][0]))
					
					if splited_signal[0][0] is not None \
					and splited_signal[1][0] is None \
					and splited_signal[2][0] is None:
						# If signal has only "leading field" it doesn't actually need
						# to be fragmented but a bit field needs to be used for it.
						field_data_type = cg.get_dtv(current_type_len)
						cog.outl("\t\t" + field_data_type + " " + signal_name \
											 + " : " + str(splited_signal[0][0]) + ";")
					else:
						# Generate the multiple fragments for the signal
						for field in splited_signal:
							# Three fields are expected. First one is the 
							# "leading field", second field is the "nominal field" 
							# and third field is the "trailing field".
							if field_position == 0:
								# Set lenght of "leading field" data type
								field_data_type = cg.get_dtv(current_type_len)
								field_data_type_len = current_type_len
							elif field_position == 1:
								# Set lenght of "nominal field" data type
								field_data_type = cg.get_dtv(base_type_len)
								field_data_type_len = base_type_len
							else:
								# Set lenght of "trailing field" data type
								field_data_type = cg.get_dtv(next_type_len)
								field_data_type_len = next_type_len
							for unit_len in field:
								if unit_len is not None:
									# Check if signal needs bitfield
									if field_data_type_len == unit_len \
									or field_position == 1:
										# For nominal fields, do not use bitfields
										cog.outl("\t\t" + field_data_type + " " \
												 + signal_name + "_" \
												 + str(fragment_counter) + ";")
									else:
										# For leading and trailing fields, use bitfields
										cog.outl("\t\t" + field_data_type + " " \
												 + signal_name + "_" \
												 + str(fragment_counter) \
												 + " : " + str(unit_len) + ";")
									fragment_counter += 1
							
							field_position += 1
				else:
					field_data_type = cg.get_dtv(base_type_len)
					# Signal doesn't need to be fragmented
					cog.outl("\t\t" + field_data_type + " " \
							 + signal_name + ";")
											 
#				cog.outl("\t\t" + cg.dt[current_type] + " " + str(signal_name) \
#				 + " : " + str(signal_len) + ";")
				
			# Check if reserved fields are required
			# -------------------------------------
			if next_start_bit > (absolute_end_bit + 1):
				# Calculate empty space in the message
				empty_start_bit = absolute_end_bit + 1 
				empty_space_len = next_start_bit - empty_start_bit

#				cog.outl("Last type: " + current_type + ", next type: " + next_type)
				
				splited_signal = nw.Network_CAN.split_signal(empty_start_bit, \
					empty_start_bit + empty_space_len - 1, \
					8)
				
				field_position = 0
				for field in splited_signal:
					# Three fields are expected. First one is the "leading field",
					# second field is the "nominal field" and third field is the
					# "trailing field".
					if field_position == 0:
						# Set lenght of "leading field" data type
						field_data_type = cg.get_dtv(current_type_len)
					elif field_position == 1:
						# Set lenght of "nominal field" data type
						field_data_type = cg.get_dtv(8)
					else:
						# Set lenght of "trailing field" data type
						field_data_type = cg.get_dtv(next_type_len)
					field_position += 1
					
					for each_unit in field:
						if each_unit is not None:
							cog.outl("\t\t" + field_data_type + " reserved_" \
									 + str(last_reserved_field) \
									 + " : " + str(each_unit) + ";")
							last_reserved_field += 1
#				# Print all reserved fields
#				for each_unit in range( len(splited_signal) ):
#					cog.outl("\t\tT_UBYTE reserved_" + str(last_reserved_field) \
#							 + " : " + str(splited_signal[each_unit]) + ";")
#					last_reserved_field += 1

			# Update current type with the next type
			current_type = next_type
			current_type_len = cg.dt_info[current_type][0]

		cog.outl("\t} s;")
		cog.outl("\tT_UBYTE all[CAN_" + network.name + "_MSG_DLEN_" + message_name + "];")
		cog.outl("}S_" + message_name + ";\n")
]]] */
// [[[end]]]

#endif /* COMGEN_CAN_SIGNALS_H_ */
