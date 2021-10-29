# -*- coding: utf-8 -*-
""" CalvOS Code Generation Module.

Module for code generation utilities.

@author: Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.0.1'
from math import floor
__date__ = '2020-09-29'
__updated__ = '2020-09-29'

import cogapp as cog
import pyexcel as pe
import re
import time
import os
import shutil
import pathlib as pl
import math

import calvos.common.logsys as lg
import calvos.__version__ as ver

# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "cg"

log = lg.log_system
log.add_logger(LOGGER_LABEL)

def log_debug(message):
    log.debug(LOGGER_LABEL, message)
    
def log_info(message):
    log.info(LOGGER_LABEL, message)
    
def log_warn(message):
    log.warning(LOGGER_LABEL, message)
    
def log_error(message):
    log.error(LOGGER_LABEL, message)
    
def log_critical(message):
    log.critical(LOGGER_LABEL, message)
# --------------------------------------------------------------------------------------------------

#==============================================================================
# Functions, data for code generation
#==============================================================================
calvos_path = None
calvos_project_path = None

MCU_word_size = 32
Compiler_max_size = 64
little_endian = False
pack_struct_tag = " __attribute__((packed))"

#dt in this context stands for "data type" prefix

dt = {"uint8"   :   "T_UBYTE",     "int8"   :   "T_BYTE", \
      "uint16"  :   "T_UWORD",     "int16"  :   "T_WORD", \
      "uint32"  :   "T_ULONG",     "int32"  :   "T_LONG", \
      "uint64"  :   "T_UDLONG",    "int64"  :   "T_DLONG",
      "float"   :   "T_FLOAT",    "double"  :   "T_DFLOAT"}

dt_compiler = {"uint8"   :   "unsigned char",     "int8"   :   "signed char", \
      "uint16"  :   "unsigned short",     "int16"  :   "signed short", \
      "uint32"  :   "unsigned int",     "int32"  :   "signed int", \
      "uint64"  :   "unsigned long long int",    "int64"  :   "signed long long int",
      "float"   :   "float",    "double"  :   "double"}

# { "type key" : [size, "i(integer)|f(float)", True(signed)|False(unsigned)]}
dt_info = {"uint8" : [8, "i", False],   "int8" : [8, "i", True], \
          "uint16" : [16, "i", False],  "int16" : [16, "i", True], \
          "uint32" : [32, "i", False],  "int32" : [32, "i", True], \
          "uint64" : [64, "i", False],  "int64" : [64, "i", True], \
          "float" : [32, "f", False],   "double" : [64, "f", True]}

#gl in this context stands for "general parameter"
gp = {"px_enum"    :    "E_",   "px_enum_el"    :   "E_",
      "sx_enum"    :    "",     "sx_enum_el"    :   "",
      "px_struct"  :    "S_",   "px_struct_el"  :   "S_",
      "sx_struct"  :    "",     "sx_struct_el"  :   "",
      "px_type"    :    "T_",   "sx_type"       :   "",
      "little_endian"   :   False,  "MCU_id"    :   "AVR",
      "MCU_word_lenght" :   32,     "Compiler"  :   "",       
      "includes"   :    "",     "gen_typedef"   :   True}

#==============================================================================
# Prefixes to use for signal parts.
gen_part_prefixes = ["","part_", "ele_"]
PRFX_DEFAULT = 0
    
#==============================================================================
def calculate_base_type_len(data_size):
    """ Calculate length of type that can hold the input size.
    Returns the size of the "base" data type that can contain the input data size, if
    data_size is greater than the maximum variable size of the compiler then
    returns None.
    """
    #TODO: compiler dependent function
    return_value = None
    
    exponent = math.ceil(math.log(data_size, 2))
    base = int(math.pow(2, exponent))
    
    if base <= Compiler_max_size:
        if base < 8:
            # Minimum base length is 8 bits (1 byte).
            base = 8
        return_value = base
    
    return return_value

#==============================================================================
def calculate_data_base_type_len(data_value):
    """ Calculate length of type that can hold the input data value.
    Returns the size of the "base" data type that can contain the input data value, if
    value is greater than the maximum variable size of the compiler then
    returns None.
    """
    #TODO: compiler dependent function
    return_value = None
    
    if data_value > 255:
        exponent = math.ceil(math.log(data_value, 2))
        if exponent <= 64:
            base = calculate_base_type_len(exponent)
            return_value = base
    else:
        # Assume minimum data size of 8 bits
        return_value = 8
    
    return return_value


#==============================================================================
def to_hex_string_with_suffix(input_constant, suffix_len = None, padding = False):
    """ Returns a string for the given number in hex and with unsigned suffix. """
    # TODO: This is compiler-dependent, adapt this function so that it takes that
    # in consideration.
    return_string = ""

    max_64 = 18446744073709551616 #pow(2,64)
    
    if type(input_constant) is not int:
        if is_hex_string(input_constant):
            input_constant = int(str(input_constant),0)

    input_constant_len = calculate_data_base_type_len(input_constant)
        
    if suffix_len is not None and suffix_len > input_constant_len:
        length_to_check = suffix_len
    else:
        length_to_check = input_constant_len
    
    if length_to_check <= 8:
        if padding is True:
            return_string = "0x{:02x}".format(input_constant)
        else:
            return_string = "0x{:x}".format(input_constant)
        return_string += "u"
    elif length_to_check > 8 and length_to_check <= 16:
        if padding is True:
            return_string = "0x{:04x}".format(input_constant)
        else:
            return_string = "0x{:x}".format(input_constant)
        return_string += "u"
    elif length_to_check > 16 and length_to_check <= 32:
        if padding is True:
            return_string = "0x{:08x}".format(input_constant)
        else:
            return_string = "0x{:x}".format(input_constant)
        return_string += "ul"
    elif length_to_check > 32 and length_to_check <= 64:
        if padding is True:
            return_string = "0x{:016x}".format(input_constant)
        else:
            return_string = "0x{:x}".format(input_constant)
        return_string += "ull"
    else:
        log_warn("Passed suffix lenght '%s' or input length '%s' exceeds 64-bits." \
                 % (str(suffix_len), str(input_constant)))
        if input_constant > max_64:
            return_string = "None"
        else:
            return_string = str(input_constant)

    return return_string

def shifter_string_with_suffix(shifting_bits):
    """ Returns a string for the given shifting bits with unsigned suffix. """
    # TODO: This is compiler-dependent, adapt this function so that it takes that
    # in consideration.
    return_string = str(shifting_bits)
    
    if shifting_bits <= 16:
        return_string += "u"
    elif shifting_bits > 16 and shifting_bits <= 32:
        return_string += "ul"
    elif shifting_bits > 32 and shifting_bits <= 64:
        return_string += "ull"
    else:
        log_warn("Passed input '%s' exceeds 64-bits." % str(shifting_bits))
        return_string += "ull"
    
    return return_string

#==============================================================================
def get_bit_mask(n_of_bits, start_bit = 0, inverse = False, length = None):
    """ Returns an integer mask. """
    bits_mask_str = ""
    if inverse is False:
        zero_char = "0"
        one_char = "1"
    else:
        zero_char = "1"
        one_char = "0"
     
    if length is not None:
        max_len = calculate_base_type_len(length)
    else:
        max_len = calculate_base_type_len(n_of_bits+start_bit)
     
    first_bit_position = start_bit
    last_bit_position = first_bit_position + n_of_bits - 1
    for i in range(max_len,0,-1):
        if (i-1) >= first_bit_position and (i-1) <= last_bit_position:
            # Put a "one_char"
            bits_mask_str += one_char
        else:
            # Put a "zero_char"
            bits_mask_str += zero_char
         
    bits_mask = int(bits_mask_str, 2)
         
    return bits_mask

#==============================================================================
def get_bit_mask_str_binary(n_of_bits, start_bit = 0, inverse = False, length = None):
    """ Returns an string mask. 
        
        if lenght is provided then the mask string will always be of byte-size
        enough to cover lenght.
    """
    bits_mask_str = ""
    if inverse is False:
        zero_char = "0"
        one_char = "1"
    else:
        zero_char = "1"
        one_char = "0"
     
    if length is not None:
        curr_len = length
    else:
        curr_len = n_of_bits+start_bit

    # Calculate max lenght and put it in complete bytes    
    max_len = floor(curr_len/8)
    if curr_len % 8 > 0:
        max_len += 1
    max_len = int(max_len*8)
    
    last_bit_position = start_bit + n_of_bits - 1

    for i in range(max_len):
        if i >= start_bit and i <= last_bit_position:
            bits_mask_str = one_char + bits_mask_str
        else:
            bits_mask_str = zero_char + bits_mask_str
         
    return bits_mask_str

#==============================================================================
def get_bit_mask_str_hex(n_of_bits, start_bit = 0, inverse = False, length = None):
    """ Returns an string mask in hex format. 
        
        if lenght is provided then the mask string will always be of byte-size
        enough to cover lenght.
    """
    bits_mask_str = get_bit_mask_str_binary(n_of_bits, start_bit, inverse, length);
    bit_mask_hex = hex(int(bits_mask_str, 2))

    return bit_mask_hex

#==============================================================================
#get_dtk -> "get data type key" (previously "g_dt_k")
def split_bit_len_in_base_types(size_in_bits):
    """ Given a lenght, split this lenght on pieces so that each piece is of a base type lenght.
    
    Return a list of pieces with format:
    [size of piece 1, size of piece 2, ..., size of piece X]
    """
    pieces_sizes = []
    remaining_bits = size_in_bits
    piece_size = remaining_bits
    while remaining_bits > 8 and calculate_base_type_len(remaining_bits) > remaining_bits:
        # Attempt with the lower base type
        piece_size = calculate_base_type_len(piece_size) / 2
        remaining_bits -= piece_size
        if remaining_bits < 0:
            remaining_bits += piece_size
            piece_size = calculate_base_type_len(piece_size) / 2
            pieces_sizes.append(int(piece_size))
            remaining_bits -= piece_size
        else:
            pieces_sizes.append(int(piece_size))
    else:
        pieces_sizes.append(int(remaining_bits))
    
    return pieces_sizes
        
#==============================================================================
def split_bit_str_in_base_types(size_in_bits, bits_string):
    """ Given a string of binary number, split it in pieces of base type lengths.
    
    Return a list of lists with format:
    [ [size in bits LSB, sub string LSB], [size in bits x, sub string x],
    ..., [size in bits MSB, sub string MSB] ]
    """
    
    pieces_sizes_list = split_bit_len_in_base_types(size_in_bits)
    pieces_out = []
    remaining_str = bits_string
    
    for piece_size in pieces_sizes_list:
        # Form "item" list with format [size in bits][bit_string]
        if len(remaining_str) >= piece_size:
            # Generate sub-string and consume the characters
            item_str = remaining_str[-piece_size:]
            remaining_str = remaining_str[:-piece_size]
            item_lst = [piece_size, item_str]
            pieces_out.append(item_lst)
        else:
            item_str = remaining_str[-piece_size:]
            item_lst = [len(item_str), item_str]
            pieces_out.append(item_lst)
            break
        
    return pieces_out

#==============================================================================
#get_dtk -> "get data type key" (previously "g_dt_k")
def round_to_bytes(size_in_bits):
    """ Will get the nubmer of full bytes necessary to contain the given number of bits.
    """
    n_of_bytes = floor(size_in_bits / 8)
    if size_in_bits % 8 > 0:
        n_of_bytes += 1
    
    return int(n_of_bytes)

#==============================================================================
#get_dtk -> "get data type key" (previously "g_dt_k")
def get_dtk(size, d_type = "i", is_signed = False):
    """ Returns the "key" of the data type specified in the input.
    
    Args:
        size (int): size of the data type, should be bigger than 0 and smaller
            than or equal to 64.
        d_type (str): input tha tells if data type is integer (value "i"),
            or is of float type (value "f"). If not specified will assume "i".
        is_signed: "True" if data type is signed, "False" if unsigned. If
            not specified will assume "False".
    
    Returns:
        str: Returns the Key corresponding to the input data type if it is 
            a valid specification. This key is taken from the "dt_info" 
            dictionary. If input specification is not valid returns "none".
    """
    return_value = None
    
    if size > 0 and size <= 64:
        
        container_size = calculate_base_type_len(size)
        
        for data_type_key in dt_info:
            if dt_info[data_type_key][0] == container_size \
            and dt_info[data_type_key][1] == d_type \
            and dt_info[data_type_key][2] == is_signed:
                return_value = data_type_key
                break
        
        if return_value is None:
            log_warn(("Provided data type information is not valid. "
                      + "Size ='%d', type = '%s', signed = '%s'")
                       % (size, d_type, is_signed))
        
    else:
        log_warn(("Provided data type size shall be from 1 upto 64."
                  + "Provided size ='%d'") % size)
    
    return return_value

#==============================================================================
def get_dtv(size, d_type = "i", is_signed = False):
    """ Returns the "value" of the data type specified in the input.
    
    Args:
        size (int): size of the data type, should be bigger than 0 and smaller
            than or equal to 64.
        d_type (str): input tha tells if data type is integer (value "i"),
            or is of float type (value "f"). If not specified will assume "i".
        is_signed: "True" if data type is signed, "False" if unsigned. If
            not specified will assume "False".
    
    Returns:
        str: Returns the data type string value as configured in "dt_info" 
            dictionary. If input specification is not valid returns "none".
    """
    return_value = None
    
    if size > 0 and size <= 64:
        
        container_size = calculate_base_type_len(size)
        
        for data_type_key in dt_info:
            if dt_info[data_type_key][0] == container_size \
            and dt_info[data_type_key][1] == d_type \
            and dt_info[data_type_key][2] == is_signed:
                return_value = dt[data_type_key] 
                break
        
        if return_value is None:
            log_warn(("Provided data type information is not valid. "
                      + "Size ='%d', type = '%s', signed = '%s'")
                       % (size, d_type, is_signed))
        
    else:
        log_warn(("Provided data type size shall be from 1 upto 64."
                  + "Provided size ='%d'") % size)
    
    return return_value

#==============================================================================
def update_type_param(parameter, value):
    """ Update the given data type parameter with the given value """
    if parameter in dt:
        dt.update({parameter : value})
    else:
        pass
        #TODO: warning?

#==============================================================================
def update_general_param(parameter, value):
    """ Update the given data type parameter with the given value """
    if parameter in gp:
        gp.update({parameter : value})
    else:
        pass
        #TODO: warning?

#==============================================================================
def get_param_category(parameter):
    """ Returns "1" if parameter is "type", "2" if it is "general" and None otherwise """
    return_value = None
    if parameter in dt:
        return_value = 1
    elif parameter in gp: 
        return_value = 2
    else:
        pass
    
    return return_value

#==============================================================================
def parse_codegen_spreadsheet(input_file):
    """ Parses an spreadsheet containing code generation parameters. """
    book = pe.get_book(file_name=str(input_file))

    # -----------------------
    # Parse selected Setup
    # -----------------------
    working_sheet = book["Setup"]
    setup = working_sheet[0,1]
    
    working_sheet.name_columns_by_row(2)
    setup_found = False
    for row in working_sheet:
        # Check if selected setup exists. If it does extract the rest of the
        # setuo datam, otherwise, warn the user.
        if setup == row[working_sheet.colnames.index("Setup Name")]:
            # Setup name found
            setup_found = True
            break
    
    if setup_found:
        params_set = row[working_sheet.colnames.index("Parameters Set")]
        compiler_name = row[working_sheet.colnames.index("Compiler")]
        MCU_name = row[working_sheet.colnames.index("MCU")]
        settings_name = row[working_sheet.colnames.index("Settings")]
        
        # -----------------------
        # Parse Parameters
        # -----------------------
        # Check if parameter set exists
        working_sheet = book["Parameters"]
        working_sheet.name_columns_by_row(0)
        
        if params_set in working_sheet.colnames:
            # Parameter set exists, continue processing parameters...
            for row in working_sheet:
            # Check if parameter exist. If it does, update the value, otherwise
            # warn the user.
                param = row[working_sheet.colnames.index("Parameter")]
                param_category = get_param_category(param)
                if param_category == 1:
                    # Parameter is of "data type" type. Update value if it is not
                    # empty.
                    param_value = str(row[working_sheet.colnames.index(params_set)])
                    if param_value != "":
                        dt.update({param : param_value})
                elif param_category == 2:
                    # Parameter is of "general" type. Update value if it is not
                    # empty.
                    param_value = str(row[working_sheet.colnames.index(params_set)])
                    if param_value != "":
                        dt.update({param : param_value})
                else:
                    # Parameter not defined internally.
                    log_warn(("Parameter \"" + param \
                      + "\" meaningless. Parameter is ignored.")) 
        else:
            log_warn(("Selected Parameters Set \"" + params_set \
                      + "\" of setup \"" + setup + "\" not found in input file. " \
                      + "Will assume default values for all parameters.")) 
    else:
        log_warn(("Selected setup: \"" + setup \
                      + "\" not found in setup names."))    

#==============================================================================
# General utilities
#==============================================================================  

#==============================================================================              
def is_valid_identifier(input_string):
    """ Validates if the input string is a C-language identifier.
    
    Args:
        input_string (str): input string to be checked.
    
    Returns:
        Boolean: Returns True if input string has a C-language identifier syntax,
        returns False otherwise.
    """
    
    identifier = re.compile(r"^[a-zA-Z_][a-zA-Z0-9_]*\Z", re.UNICODE)
    if re.match(identifier, input_string):
        return_value = True
    else:
        return_value = False
    return return_value

#==============================================================================
def is_hex_string(input_string):
    """ Validates if the input string is an hexadecimal number starting with 0x. 
    
    Args:
        input_string (str): input string to be checked.
    
    Returns:
        Boolean: Returns True if input string is hexadecimal number,
            returns False otherwise.
    """
    
    return_value = False
    hex_number = re.compile(r"^(?:0x)[0-9a-fA-F]+\Z", re.UNICODE)
    if re.match(hex_number, input_string):
        return_value = True
    return return_value

#==============================================================================
def get_valid_number(input_string):
    """ Returns the decimal number from the input string in dec or hex. 
    
    Args:
        input_string (str): input string to be checked.
    
    Returns:
        return_value: Integer value corresponding to the input string. Returns
        'None' if input doesn't represent a decimal or hex (0x) number.
    """
    return_value = None
    if str(input_string).isnumeric():
        return_value = int(input_string)
    else:
        #Check if string is hexadecimal
        if is_hex_string(input_string):
            #Remove 0x prefix
            input_string = input_string[2:]
            #convert to decimal
            return_value = int(input_string,16)
    #Return value (None if no valid dec or hex number in input string)
    return return_value
        
#==============================================================================
def ranges_overlap(range1_init, range1_end, range2_init, range2_end):
    """ Returns true if the input ranges overlap each other """
    return_value = False
    if (range2_init >= range1_init and \
        range2_init <= range1_end) or \
       (range2_end >= range1_init and \
        range2_end <= range1_end):
        #Ranges do overlap each other
        return_value = True
    return return_value

#==============================================================================
def parse_special_string(input_string):
    """ Returns a dictionary of the parsed special string input.
    
    Parses an string with following format:
        'identifier1 (number1), identifier2, identifier3 (number3), ...'
    and returns a dictionary with the same data:
        {{identifier1 : number1},
         {identifier2 : None},
         {identifier3 : number3}}
    Throw warnings if the input string doesn't comply with above's format.
    Note: identifiers can't be duplicated otherwise a warning will be thrown.
    
    Args:
        input_string (str): input string to be parsed.
    
    Returns:
        return_dictionary: dictionary equivalent to the parsed input string.
    """
    return_dictionary = {}
    #Elements separator is a comma
    string_elements = input_string.split(",")
    
    if len(string_elements) == 0:
        #Error, string_elements can't be of size 0.
        log_warn("Invalid input string format")
    if len(string_elements) > 0 and string_elements[0] != "":
        # Navigate through each eleement and check syntax
        # Valid syntax is: "text1 (number1)" or "text1"
        for string_element in string_elements:
            #Check format compliance
            regex = re.compile(r"(.+)\((.+)\)")
            regex_matches = re.findall(regex,str(string_element))                
            if len(regex_matches) == 0:
                #No parentheses found, means user didn't pass an explicit
                #numeric value. Now check if at least the symbol string is found
                #Remove leading and trailing spaces
                string_element = str(string_element).strip()
                #Check compliance with C-language identifier
                if is_valid_identifier(string_element):
                    #Valid symbol string, check if it is duplicated
                    if string_element not in return_dictionary:
                        #Not duplicated symbol string, add it to the output
                        return_dictionary.update({string_element : None})
                    else:
                        #warning, duplicated symbol string
                        log_warn(("Duplicated symbol: \"" \
                            + string_element + "\" ignored."))
                else:
                    #warning, invalid input string
                    log_warn(("Invalid input string format. Symbols shall" \
                        + " comply with C-language identifier syntax."))
            else:
                #First captured group should be the symbol string
                symbol_string = regex_matches[0][0]
                symbol_string = str(symbol_string).strip()
                #Second captured group should be the symbol numeric value
                symbol_value = regex_matches[0][1]
                symbol_value = str(symbol_value).strip()   
                #Validate symbol string
                if is_valid_identifier(symbol_string):
                    #Valid symbol string, check if it is duplicated
                    if symbol_string not in return_dictionary:
                        #Not duplicated symbol string, now validate symbol value
                        #TODO: maybe using get_valid_number?
                        if symbol_value.isnumeric():
                            #Valid symbol value. Add the entry to the output
                            return_dictionary.update({symbol_string : symbol_value})
                        else:
                            log_warn(("Invalid symbol numeric value: \"" \
                                + symbol_value + "\" of symbol: \"" \
                                + symbol_string + "\". Element ignored.")) 
                    else:
                        log_warn(("Duplicated symbol: \"" \
                            + string_element + "\" ignored."))
                else:
                    log_warn(("Invalid input string format. Symbols shall" \
                        + " comply with C-language identifier syntax."))
    else:
        #error, enum value string not valid
        log_warn("Invalid input string format")
    return return_dictionary

#==============================================================================
def get_special_string(input_dictionary):
    """ Returns a special string based on the input dictionary.
    
    Returns an string with following format:
        'identifier1 (number1), identifier2, identifier3 (number3), ...'
    based on the input dictionary with the same data:
        {{identifier1 : number1},
         {identifier2 : None},
         {identifier3 : number3}}
    
    Args:
        input_string (str): input string to be parsed.
    
    Returns:
        return_dictionary: dictionary equivalent to the parsed input string.
    """
    output_string = ""
    if len(input_dictionary) > 0:
        for key in input_dictionary:
            output_string = output_string + str(key)
            if input_dictionary[key] is not None:
                output_string = output_string + " (" + str(input_dictionary[key]) + ")"
            output_string = output_string + ", "
        output_string = output_string[:-2]
    return output_string
    
#==============================================================================
def get_local_time_formatted():
    """ Gets current local time and returns it in a formatted string """
    curren_time = time.localtime() # Gets current local time
    return_string = str(curren_time[0])+"." \
                + str(curren_time[1])+"." \
                + str(curren_time[2])+"::" \
                + str(curren_time[3])+":" \
                + str(curren_time[4])+":" \
                + str(curren_time[5])
    return return_string

#==============================================================================
def get_unsigned_suffix(number):
    """ Returns the unsigned suffix for the given number. """
    #TODO: solve compiler dependencies for suffix u, ul, ull
    return_str = ""
    if number <= 16:
        return_str = "u"
    elif number > 16 and number <= 32:
        return_str = "ul"
    else:
        return_str = "ull"
        
    return return_str

#==============================================================================
def str_to_multiline(input_str, max_chars, starting_char = None, \
                     separator = "..", divisors = ["/", "\\"], \
                     include_divisor = True):
    """ Returns a multiline string for the given input string. """
    return_str = ""
    file_str_lst = []
    # Split string as per divisor's list
    for divisor in divisors:
        file_str_lst = input_str.split(divisor)
        if len(file_str_lst) > 1:
            # Divisor found. Add separator back if needed
            if include_divisor is True:
                for i, element in enumerate(file_str_lst):
                    if i < len(file_str_lst) - 1:
                        file_str_lst[i] += divisor
            break
        # else: if divisor not found try with next one.
    
    if len(file_str_lst) > 1:
        # Split input_str into multi-lines according to max character
        
        # Calculate padding bits
        padding = ""
        if starting_char is not None:
            for i in range(starting_char):
                padding += " "
            
        line_str = ""
        for i, element in enumerate(file_str_lst):
            if i == 0:
                # Always insert first element
                line_str = element
            elif i < (len(file_str_lst) - 1):
                if len(line_str) + len(element) <= max_chars:
                    line_str += element
                else:
                    # Insert a separator and reset line_str
                    line_str += separator + "\n" + padding
                    return_str += line_str
                    line_str = element
            else:
                if len(line_str) + len(element) <= max_chars:
                    return_str += line_str + element
                else:
                    # Insert a separator and reset line_str
                    return_str += line_str + separator + "\n" + padding + element
    else:
        return_str = input_str
    
    return return_str

#==============================================================================
def gen_padding(starting_pos, last_occupied_pos = None):
    """ Generates a string of blank spaces for padding. """
    
    padding_str = ""
    if last_occupied_pos is not None and last_occupied_pos > starting_pos:
        padding = 1
        log_warn( \
            "Minimal padding: last occupied position '%d' is bigger than starting position '%d."\
            % (last_occupied_pos, starting_pos))
    elif last_occupied_pos is not None:
        padding = starting_pos - last_occupied_pos
    else:
        padding = starting_pos
    
    for i in range(padding):
        padding_str += " "
    
    return padding_str

#==============================================================================
def get_str_max_len(input_strings):
    """ Returns the size of the longest string within a list of strings. """
    max_len = 0
    for string in input_strings:
        if len(string) > max_len:
            max_len = len(string)
    return max_len
    
                 
#==============================================================================
def string_to_path(path_string):
    ''' Returns a "path" object for the given string. '''
    # libpath requires regular diagonal, not reverse diagonal
    if type(path_string) is not str:
        path_string = str(path_string)
    path_string = path_string.replace("\\", "/")
    
    try:
        return_path = pl.Path(path_string)
    except Exception as e:
        return_path = None
        log_error(("Can't convert input string '%s' to a Path object. Assumed None. " \
            + "Pathlib error: '%s'") % (str(path_string), str(e)))
    
    return return_path

    
def folder_exists(path):
    ''' Returns "True" if the given path is an existing folder. '''
    return_value = False
    
    if type(path) is str:
        path = string_to_path(path)
     
    if path.exists() and path.is_dir():
        return_value = True
    
    return return_value

def delete_folder_contents(folder):
    """ Deletes contents of the given folder.
    """
    if folder_exists(folder) is True:
        for filename in os.listdir(folder):
            file_path = os.path.join(folder, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    delete_folder_contents(file_path)
                    shutil.rmtree(file_path)
                    log_debug('Deleted element %s' % file_path)
            except Exception as e:
                log_error('Failed to delete folder %s. Reason: %s' % (file_path, e))

def create_folder(folder):
    """ Creates the specified folder.
    """
    if folder_exists(folder) is False:
        try:
            folder.mkdir()
            log_debug("Folder created: %s" % folder)
        except Exception as e:
            log_error('Failed to create folder %s. Reason: %s' % (folder, e))
    else:
        log_info("Folder to be created '%s' already exists." % folder)
        
def delete_file(file_name):
    """ deletes the specified file.
    """
    try:
        os.unlink(file_name)
    except Exception as e:
        log_error('Failed to delete %s. Reason: %s' % (file_name, e))
        
def file_exists(file_name):
    """ Returns True if the specified file exists.
    """
    return_value = False
    file_name = string_to_path(file_name)
    if file_name.is_file() is True:
        return_value = True
        
    return return_value

def resolve_wildcards(in_str, wildcards):
    """ Return the input string with the wildcard(s) resolved """
    out_str = in_str
    if type(wildcards) is dict:
        for wildcard, value in wildcards.items():
            out_str = out_str.replace("${"+wildcard+"}", str(value))
    else:
        log_warn("Input wildcards shall be a dictionary.")
        
    return out_str

def get_calvos_version_str():
    return_str = str(ver.__version__)
    return return_str

def C_license():
    """ Returns a string with the license in C comment format. """
    return_str = """/*  This file is part of calvOS project <https://github.com/calcore-io/calvos>.
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
 *  along with calvOS.  If not, see <https://www.gnu.org/licenses/>. 
 *
 * [calvOS engine version: """+get_calvos_version_str()+"]%s*/""" \
    % (" " * (50 - len(get_calvos_version_str())))
    return return_str

class GenParam():
    """ . """
    def __init__(self, name, param_data = None, default_idx = 0, description = ""):
        self.name = str(name)
        self.description = description
        self.pl = []    # Parameter body as a list
        self.pd = {}    # Parameter body as dictionary
        self.pg = None  # Generic type
        self.DEFAULT = 0
        self.type = None   # 0 - generic, 1 - list, 2 - dictionary
        
        
        if type(param_data) is list:
            self.pl = param_data
            self.PL_DEFAULT = default_idx
            self.type = 1
        elif type(param_data) is dict:
            self.pd = param_data
            if default_idx is str:
                self.PD_DEFAULT = default_idx # A key is expected for dictionaries
            else:
                # TODO: flag error, a default index string is required
                pass
            self.type = 2
        else:
            self.pg = param_data
            self.type = 0
            
        
    def get_idx(self, index = None):
        """ Get param data value. """
        return_value = None
        if index == None:
            index == self.DEFAULT
            
        if self.type == 0:
            return_value = self.pg
        elif self.type == 1 and index < len(self.pl):
            return_value = self.pl[index]
        elif self.type == 2 and index in self.pd:
            #TODO: Check if it is Ok to check None "in" dictionary
            return_value = self.pd[index]
        else:
            pass
        
        return return_value
    
    def set_idx(self, index, value):
        """ . """
        if self.type == 0:
            self.pg = value
        elif self.type == 1 and index < len(self.pl):
            self.pl[index] = value
        elif self.type == 2 and index in self.pd:
            self.pd.update({index : value})
        else:
            #TODO: flag error setting value
            pass
    
    def get_len(self):
        """ Get param data lenght. """
        return_value = None
        
        if self.type == 0:
            return_value = 1
        elif self.type == 1:
            return_value = len(self.pl)
        elif self.type == 2:
            return_value = len(self.pd)
        else:
            pass
        
        return return_value
        
class GenParams():
    """ . """
    def __init__(self, name, calvos_module = None, description = ""):
        self.name = str(name)
        self.module = calvos_module
        self.description = description
        
        self.p = {} # Param objects
    
    def add_p(self, name, param_data = None, default_idx = 0, \
                      description = ""):
        """ Adds a new parameter with the specified data.
        """
        self.p.update({name : GenParam(name, param_data, default_idx, description)})
        
    def get_p(self, name, idx = None):
        """ Gets the value of the specified parameter and index.
        """
        return_data = None
        if name in self.p:
            return_data = self.p[name].get_idx(idx)
            
        return return_data
    
    def get_len(self,name):
        """ . """
        return_data = None
        if name in self.p:
            return_data = self.p[name].get_len()
        
        return return_data

class CogSources():
    """ Models the set of Cog source files for code generation of a given module. """
    def __init__(self, module, **kwargs):
        self.module = module
        self.obj = kwargs.get('obj', None)
        self.var = kwargs.get('var', None)
        self.gen_path = kwargs.get('gen_path', None)
        
        self.sources = {} # {source_id : CogSrc object}
        
    class CogSrc():
        """ Models a single C-code source file. """
        def __init__(self, source_id, cog_in_file, cog_out_file = None, is_header = False, \
                     relations = [], dparams = {}, ** kwargs):
            self.source_id = source_id
            self.cog_in_file = cog_in_file
            # cog_out_file equal to None means that out file is same than cog_in_file
            self.cog_out_file = cog_out_file
            self.is_header = is_header
            self.additional_info = []
            
            self.relations = relations # List of CogSrcRel objects
            
            self.generated = False
            
            self.dparams = dparams # Params in the form of dictionary
            
            self.generated = False
            
            self.user_code = kwargs.get('user_code', False)
            
    class CogSrcRel():
        """ Models a source code relation for include purposes. """
        def __init__(self, module="", source_id=""):
            self.module = module
            self.source_id = source_id

class TreeNode:
    def __init__(self, val, prev = None, next = None):
        self.val = val
        self.prev = prev
        self.next = next

def inorderTree(root, result = []): 
    return_val = []
    if root: 
        inorderTree(root.prev, result) 
        return_val.append(root.val)
        if root.prev is not None:
            return_val.append(root.prev.val)
        else:
            return_val.append(None)
        if root.next is not None:
            return_val.append(root.next.val)
        else:
            return_val.append(None)
        result.append(return_val)
        inorderTree(root.next, result)
    
        
def formTree(ordered_list, root = None):
    if len(ordered_list) == 1:
        node = TreeNode(ordered_list[0])
        return node
    else:
        mid_idx = math.floor(len(ordered_list)/2)
        if root is None:
            root = TreeNode(ordered_list[mid_idx])
        else:
            root.val = ordered_list[mid_idx]
        # left list
        left_list = ordered_list[:mid_idx]
        root.prev = formTree(left_list, root.prev)
        # right list
        if len(ordered_list) > 2:
            right_list = ordered_list[mid_idx+1:]
            root.next = formTree(right_list, root.next)
        else:
            pass
        
        return root
        

#===================================================================================================
def cog_generator(input_file, out_dir, work_dir, gen_path, variables = None):
    """ Invoke cog generator for the specified file.
    """
    # Setup input and output files
    # ----------------------------       
    cog_input_file = gen_path / input_file
    #remove "cog_" prefix to output file names
    cog_output_file_str = input_file[4:]
    #remove "static" prefix if present.
    if cog_output_file_str.find("static_", 0, 7) == 0:
        cog_output_file_str = cog_output_file_str[7:]
    cog_output_file = out_dir / cog_output_file_str
    
    # Get project pickle full file name  
    project_pickle = project_object.get_work_file_path("common.project", "project_pickle")
        
    # Invoke code generation
    # ----------------------           
    cog_arguments = ['dummy_argument', \
               '-d', \
               '-D', 'input_worksheet=' + input_file, \
               '-D', 'project_working_dir=' + str(work_dir), \
               '-D', 'cog_proj_pickle_file=' + str(project_pickle),
               '-o', str(cog_output_file), \
               str(cog_input_file) ]
    
    # Append additional variables if required
    if variables is not None:
        for variable in variables:
            cog_arguments.append('-D')
            cog_arguments.append(str(variable))
            
    # Call cogapp engine
    cog_return = cog.Cog().main(cog_arguments)
    if cog_return == 0:
        log_info("Code generation successful: '%s'" % (str(out_dir / cog_output_file_str)))
        print("INFO: code generation successful: ",cog_output_file)
    else:
        log_error("Code generation error for '%s'. Cogapp return code: '%s'" \
                 % ((str(out_dir / cog_output_file_str)), str(cog_return)))
        print("INFO: code generation return value: ",cog_return)


project_object = None
#===================================================================================================
def load_input(input_file, input_type, params, project_obj):
    """ Loads input file and returns the corresponding object. """
    del input_type, params # Unused parameters
    
    global project_object
    
    project_object = project_obj
    parse_codegen_spreadsheet(input_file)
    # This function for this specific module returns a dummy object.
    return 0

#===================================================================================================
def generate(input_object, out_path, working_path, calvos_path, params):
    """ Generate C code for the given object """

    cog_files_path = calvos_path / "common" / "gen" / "codegen"
    
    #----------------------------------------------------------------------
    # Generate types file
    #----------------------------------------------------------------------
    cog_file_name = "cog_calvos_types.h"
    cog_generator(cog_file_name, out_path, working_path, cog_files_path)
    
    cog_file_name = "cog_static_calvos.h"
    cog_generator(cog_file_name, out_path, working_path, cog_files_path)
    
    cog_file_name = "cog_USER_general_defs.h"
    cog_generator(cog_file_name, out_path, working_path, cog_files_path)