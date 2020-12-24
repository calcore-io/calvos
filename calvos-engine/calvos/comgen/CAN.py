# -*- coding: utf-8 -*-
"""
Created on Thu Sep  3 21:37:18 2020

@author: Carlos Calvillo
"""

    
import pyexcel as pe
import math
from lxml import etree as ET
import xml.dom.minidom
import re
import time
import warnings
import pickle as pic
import glob
import cogapp as cog
import pathlib as plib

import calvos.common.codegen as cg
import calvos.common.logsys as lg
from pickle import TRUE


# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "CAN"

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
# -----------------------------------------------------------------------------

def say_hello():
    print("Hello, I'm CAN")

DEFAULT = 0

#Pre-fixed data
data_types_list = ("scalar", "array") #First element is the default one
# If an element of data_type_list changes, update constants ARRAY, SCALAR, etc.
SCALAR = 0
ARRAY = 1
#List of allowed transmission types. First element of the list is 
#the default one. If an element of tx_type_list changes, update the related
#constants (SPONTAN, CYCLIC, etc) accordingly.
#TODO: pre-defined typelist wooudl be better as dictionary {name, description}
tx_type_list = ("spontan", "cyclic", "cyclic_spontan", "BAF")
SPONTAN = 0
CYCLIC = 1
CYCLIC_SPONTAN = 2
BAF = 3
    
#===================================================================================================
class Network_CAN:
    """ Class to model a CAN Network. 

    Attributes
    ----------
        enum_types : list
            List of user defined enumerated types
        custom_types :
            List of user defined non-enumerated types
        nodes: list 
            List of user defined network nodes
        messages : list
            List of user defined network messages
        signals : list
            List of user defined network signals
    """
    
    metadata_template_name = "TBD"
    metadata_template_version = "0.1.0"
    metadata_template_description = ""
    metadata_gen_source_file = ""
    
    #===============================================================================================    
    def __init__(self, name, description = "", version = "", date = ""):
        """ Class constructor.
        Parameters
        ----------
            name : str
                Name of the network
            description : str, optional
                Description of the network
            version : str, optional
                Version of the network (user defined)
            date : str, optional
                Date corresponding to the version of the network (user defined)
         """
        self.name = str(name)
        
        self.description = str(description)
        
        self.version = str(version)

        self.date = str(date)

        #Dynamic data
        self.enum_types = {} # {type name, type object}
        #TODO: Make APIs for adding and removing custom simple types?
        self.custom_types = {} # {type name, description}
        self.nodes = {} # {node name, node object}
        self.messages = {} # {message name, message object}
        self.signals = {}# {signal name, signal object}
        
        # Code generation parameters for CAN network
        self.gP = cg.GenParams("CAN_gen_params","comgen.CAN" \
            ,"Parameters for code generation of this CAN network.")
        
        self.gP.add_p("part_px", \
            ["","p_","part_","_"], 0, \
            "List of prefixes to use for naming signal parts when fragmented.")
        
    
        
    #===============================================================================================
    @staticmethod
    def split_signal(abs_start_bit, abs_end_bit, \
        nominal_type_len = 8, leading_type_len = None, trailing_type_len = None):
        """ Fragments a signal based on its layout information.
        
        Fragmentation of a signal may occur if signal crosses "type lenght" 
        boundaries.
        Signal could have a leading, nominal or tail sets of parts/fragments.
          - Leading fragments: fragments based on signal absolute bit position 
            and splitted according to the defined "trailing_type_len" lenght.
          - Nominal fragments: fragments based on signal absolute bit position
            and splitted according to the defined "nominal_type_len" lenght.
          - Tailing fragments: fragments based on signal absolute bit position
            and splitted according to the defined "trailing_type_len" lenght.
            
        Parameters
        ----------
            abs_start_bit : int
              The absolute start bit of a signal as per its layout definition
              of the signal's conveyor message.
            abs_end_bit : int
              The absolute end bit occupied by the signal as per its layout 
              definition of the signal's conveyor message.    
            nominal_type_len : int, optional
              Type lenght used to fragment the signal into its nominal parts.
              If no value is passed, will assume nominal_type_len as 8 bits.
            leading_type_len : int, optional
              Type lenght used to fragment the signal into its leading parts.
              If no value is passed, leading_type_len will assume same value
              as nominal_type_len.
            trailing_type_len : int, optional
              Type lenght used to fragment the signal into its leading parts.
              If no value is passed, trailing_type_len will assume same value
              as nominal_type_len.
       
        Returns
        -------
        list
            Will return a list (return_list) of lists with follwing format:
                return_list -> [leading_list, nominal_list, trailing_list]
            - leading_list is a list of the form:
                [leading_part_1, leading_part_2, ...]
              If no leading_part is identified then leading_list will be "None".
            - nominal_list is a list of the form:
                [nominal_part_1, nominal_part_2, ...]
              If no nominal_parts is identified then nominal_list will be "None".
            - trailing_list is a list of the form:
                [trailing_part_1, trailing_part_2, ...]
              If no tailing_parts is identified then trailing_list will be "None".
            
            Each element xxxx_part is a number conveying the lenght in bits 
            of the given part. This number is always nominal_type_len for the
            nominal parts but can be smaller than or equal to 
            leading_type_len/trailing_type_len for the leading/trailing parts
            correspondingly.
            
            Example:
                Signal_A start bit = 3, end bit = 28,
                
                for nominal_type_len = leading_type_len = trailing_type_len= 8
                return_list -> [[5],[8,8],[7]]
                
                for nominal_type_len=16, leading_type_len=trailing_type_len=8
                return_list -> [[5],[16],[7]]
                
                for nominal_type_len=trailing_type_len=8, leading_type_len=16
                return_list -> [[13],[8],[7]]
                
                Signal_B start bit = 8, end bit = 15,
                
                for nominal_type_len = leading_type_len = trailing_type_len= 8
                return_list -> [None,[8],None]
        """
        if leading_type_len is None:
            leading_type_len = nominal_type_len
        if trailing_type_len is None:
            trailing_type_len = nominal_type_len
            
        return_list = []
        leading_list = []
        nominal_list = []
        trailing_list = []
        
        signal_len = abs_end_bit - abs_start_bit + 1
        
        if signal_len > 0:
            #Calculate starting unit
            starting_unit = math.floor(abs_start_bit / leading_type_len) * leading_type_len
            
            # Calculate leading bits
            available_bits_in_unit = starting_unit + leading_type_len - abs_start_bit
            consumed_leading_bits = signal_len - available_bits_in_unit
            
            if consumed_leading_bits > 0:
                leading_len = available_bits_in_unit
            else:
                leading_len = signal_len
            
            if leading_len > 0:
                leading_list.append(leading_len) 
            else:
                leading_list.append(None)
            
            remaining_bits = signal_len - leading_len
            main_field_units = math.floor(remaining_bits / nominal_type_len)
            
            if main_field_units > 0:
                for each_unit in range(main_field_units):
                    nominal_list.append(nominal_type_len)
            else:
                nominal_list.append(None)
            
            # Calculate trailing field(s)
            remaining_bits = remaining_bits - (main_field_units * nominal_type_len)
            trailing_units = math.floor(remaining_bits / trailing_type_len)
            trailing_bits = remaining_bits % trailing_type_len
            
            if trailing_units > 0 or trailing_bits > 0:     
                for each_unit in range(trailing_units):
                    trailing_list.append(trailing_type_len)
                
                if trailing_bits > 0:
                    trailing_list.append(trailing_bits)
            else:
                trailing_list.append(None)
        else:
            leading_list.append(None)
            nominal_list.append(None)
            trailing_list.append(None)
            warnings.warn("End bit shall be greather or equal to the start bit." \
                            + " Current start bit: " + str(abs_start_bit)
                            + " Current end bit: " + str(abs_end_bit), UserWarning)
        
        return_list.append(leading_list)
        return_list.append(nominal_list)
        return_list.append(trailing_list)
        
        return return_list

    #===============================================================================================    
    def add_enum_type(self, name, enum_string, description = None):
        """ Adds a new enumerated type to the network. 
        
        Parameters
        ----------
            name : str
              Name of the enumerated type to be added to the network.
            enum_string : str
              Definition of the enumerated type to be added. This string needs
              to comply with the following syntax:
                  "symbol1 (int1), symbol2 (int2), symbol3 (int3), ..."
              - symbolX is a C-like identifier for the given enumerated symbol
              - (int1): explicit value to be assigned to symbolX -optional-
            
              Explicit values for symbols are optional so next are valid 
              examples of enum_string:
                  "symbol1, symbol2, symbol3, ..."
                  "symbol1, symbol2 (int2), symbol3, ..."
            description : str, optional
              Description of the enumerated type
              
        Yields
        ------
            If all passed parameters are valid, a new EnumType object with name
            "name" and description "description will be added to this network 
            with the symbols/values defined in "enum_string".
        """
        if cg.is_valid_identifier(name):
            #Verify that type to be added is not already existing
            if name not in self.enum_types:
                #Create new Enum Type object in the network
                #TODO: create first a temporal EnumType object then add it to
                # the network if it is valid...
                self.enum_types.update({name : self.EnumType(name, enum_string)}) 
                #Verify if the new enum tpye is not empty. If it is empty is because
                #the input string didn't have a correct format. Hence, remove the
                #empty object
                if len(self.enum_types[name].enum_entries) == 0:
                    self.enum_types.pop(name)
                    warnings.warn("Enum type: \"" + name + \
                          "\" not well formed. Removed from the network.", \
                          UserWarning)    
            else:
                warnings.warn("Duplicated Enum Type: \"" + name + \
                          "\" not added to the network.", \
                          UserWarning)
        else:
            #Warning, invalid node name
            warnings.warn("Invalid Enum Type: \"" + name + \
                          "\". Enum Types must comply with C-language identifier syntax.", \
                          UserWarning)
        
    #===============================================================================================
    def remove_enum_type(self, name):
        """ Removes the specified node from the network.
        
        Parameters
        ----------
            name : str
        """
        if name not in self.nodes:
            #Node doesn't exist
            warnings.warn("Enum Type \"" + name + \
                      "\" doesn't exist", \
                      UserWarning)
        else:
            self.nodes.pop(name)
    
    #===============================================================================================
    def add_node(self, name, description):
        """ Adds a new Node to the network. """
        if cg.is_valid_identifier(name):
            #Verify that node to be added is not already existing
            if name not in self.nodes:
                #Create new node object in the network
                self.nodes.update({name : self.Node(name, description)})
            else:
                warnings.warn("Duplicated Node: \"" + name + \
                          "\" not added to the network.", \
                          UserWarning)
        else:
            #Warning, invalid node name
#            print("Invalid node: "+name)
            warnings.warn("Invalid node name: \"" + name + \
                          "\". Node names must comply with C-language identifier syntax.", \
                          UserWarning)
    
    #=============================================================================================== 
    def remove_node(self, name):
        """ Removes the specified node from the network. """
        if name in self.nodes:
            self.nodes.pop(name)
        else:
            #Node doesn't exist
            warnings.warn("Node \"" + name + \
                      "\" doesn't exist", \
                      UserWarning)
    
    #===============================================================================================        
    def add_message(self, name, msg_id, length, extended_id = False, \
                     tx_type = None, tx_period = None, tx_baf_repeats = None, \
                     description = ""):
        """ Adds a new message to the network """
        if cg.is_valid_identifier(name):
            #Verify that message to be added is not already existing
            if name not in self.messages:
                raw_id = cg.get_valid_number(msg_id)
                if raw_id is not None:
                    #Verify that the message ID is not already taken
                    id_already_taken = False
                    for message in self.messages.values():
                        if message.id == msg_id:
                            id_already_taken = True
                            break;
                    
                    if not id_already_taken:
                        #Create new message object in the network
                        self.messages.update( {name : \
                            self.Message(name, msg_id, length, extended_id, tx_type, \
                                         tx_period, tx_baf_repeats, description)} )
                    else:
                        warnings.warn("Duplicated Message Id: \"" + str(msg_id) \
                              + "\". Message \"" + name + "\" not added to the network.", \
                              UserWarning)
                else:
                    warnings.warn("Invalid Message Id: \"" + str(msg_id) \
                          + "\". Message \"" + name + "\" not added to the network.", \
                          UserWarning)
            else:
                warnings.warn("Duplicated Message: \"" + name + \
                          "\" not added to the network.", \
                          UserWarning)
        else:
            #Warning, invalid message name
            warnings.warn("Invalid Message name: \"" + name + \
                          "\". Message names must comply with C-language identifier syntax.", \
                          UserWarning)
    
    #===============================================================================================
    def add_tx_message_to_node(self, node_name, message_name):
        """ Sets the publisher node of the specified message. """
        if node_name in self.nodes:
            if message_name in self.messages:
                #Check if message is not already assigned for TX to another node.
                if self.messages[message_name].get_publisher() is None or \
                    self.messages[message_name].get_publisher() == node_name:
                    # Add publisher to the given message
                    self.messages[message_name].set_publisher(node_name)
                else:
                    warnings.warn("Message: \"" + message_name \
                                  + "\" already assigned for TX to node: \"" \
                                  + self.messages[message_name].get_publisher() \
                                  + "\". No change performed.", UserWarning)
            else:
                warnings.warn("Message: \"" + message_name \
                          + "\" not defined. Define first the message in the network", \
                          UserWarning)
        else:
            warnings.warn("Node: \"" + node_name \
                          + "\" not defined. Define first the node in the network", \
                          UserWarning)
        
#    def remove_tx_message_to_node(self, node_name, message_name):
#        
#    def add_rx_message_to_node(self, node_name, message_name):
#    
    
    #===============================================================================================
    def add_rx_message_to_subscribers(self, message_name, subscribers_string):
        """ Adds the given message to the list of subscriber nodes. """
        if message_name in self.messages:
            #Parse subscribers string
            temporal_entries = cg.parse_special_string( str(subscribers_string))
            if len(temporal_entries) > 0: 
                #At least one syntax-compliant message found,
                #now check if the message(s) exist
                for node in temporal_entries:
                    if node in self.nodes:
                        #Check if message is not set as published by this node
                        published_by_this_node = False
                        for message in self.messages.values():
                            if message_name == message.name \
                                    and message.publisher == node:
                                published_by_this_node = True
                                break
                        if not published_by_this_node:    
                            #Add subscribed message and its timeout
                            self.nodes[node].add_subscriber(message_name, \
                                      temporal_entries[node])
                        else:
                            warnings.warn("Message \"" + message_name + \
                                      "\" already published by node \"" + node \
                                      + "\", message can't be published " \
                                      + "and subscribed by same node.", UserWarning)       
                    else:
                        warnings.warn("Node \"" + node + \
                                      "\" not defined.", UserWarning)
        else:
            warnings.warn("Message \"" + message_name + \
                  "\" not defined.", UserWarning)
    
    #===============================================================================================
    def add_rx_messages_to_node(self, node_name, message_name, timeout_ms):
        """ . """
        #TODO: This function has not been tested
        if node_name in self.nodes:
            if message_name in self.messages:
                #Message exists, add subscriber to Node
                self.nodes[node_name].add_subscriber(message_name,timeout_ms)
            else:
                warnings.warn("Message \"" + message_name + \
                              "\" not defined.", UserWarning)
        else:
            warnings.warn("Node \"" + node_name + \
                  "\" not defined.", UserWarning)

    #===============================================================================================
    def get_message_direction(self, node_name, message_name):
        """ Gets the direction ('Tx', 'Rx' or None) for the given node. """
        return_value = None
        if node_name in self.nodes and message_name in self-messages:
            if self.messages[message_name].publisher is not None:
                if self.messages[message_name].publisher == node_name:
                    # This message is a TX message for the given node
                    return_value = "Tx"
                elif node_name in self.nodes[node_name].subscribed_messages:
                    # This message is an RX message for the given node
                    return_value = "Rx"
        return return_value

#  
#    def remove_rx_message_to_node(self, node_name, message_name):
#           
        

    
    #===============================================================================================
    def get_signals_of_message(self, message_name):
        """ Returns a list of signals mapped to the given message.
        """
        return_data = [] # [signal_object_1, signal_object_2, ...]
        
        if message_name in self.messages:
            for signal in self.signals.values():
                if signal.message == message_name:
                    # Add this signal object to the return value
                    return_data.append(signal)
        else:
            return_data = None
            #TODO: warning, message not found.
        
        return return_data
    
    #===============================================================================================
    def message_layout_is_cannonical(self, message_name):
        """ Returns true if message layout doesn't need bitfields.
        """
        is_cannonical = True
        # Message layout is considered cannonical if:
        # - All signals have a size which is multiple of a byte, and
        # - All signals absolute start bit is multiple of a byte
        signals = self.get_signals_of_message(message_name)
        if signals is not None:
            for signal in signals:
                abs_start_bit = (signal.start_byte * 8) + signal.start_bit
                if (signal.len % 8 != 0) or (abs_start_bit % 8 != 0):
                    # Signal doesn't start at a bit position multiple of 8
                    # or its size is not multiple of 8.
                    is_cannonical = False
                    break
        else:
            #TODO: warning, message has no mapped signals
            is_cannonical = None
        
        return is_cannonical
            

#===================================================================================================
    def get_messages_structures(self):
        """ Gets layout structure of all messages.
        
        return
            array of MessageStructure objects.
        
        """               
        return_data = []
        
        # All signals in te network need to be sorted so that they are 
        # ordered by message, then by byte position, then by bit position.
        sorted_signals = self.get_sorted_signals_by_layout(True)

        # All messages structures will be returned.
        for list_by_message in sorted_signals:
            message_name = list(list_by_message[0].values())[0].message
            message_len = self.messages[message_name].len
            
            # msg_signals to be used to check for duplicated names later on when
            # generating signal fragments
            msg_signals_temp = self.get_signals_of_message(message_name)
            msg_signals = []
            for msg_signal in msg_signals_temp:
                msg_signals.append(msg_signal.name)
            
            # Check if message is canonical
            msg_is_cannonical = self.message_layout_is_cannonical(message_name)
            
            if msg_is_cannonical is True:
                # Append message
                return_data.append(self.MessageStructure(message_name,None,True))
                # Check if starting reserved fields are required by calculating
                # the start byte of first signal and filling up bytes
                # with "reserved" fields.
                next_start_byte = list(list_by_message[0].values())[0].start_byte
                
                reserved_counter = 0 # Required in case following 'for' doesn't run at least once
                
                for reserved_field in range(next_start_byte):
                    # Append reserved field to the message structure
                    return_data[-1].signals.append(\
                         self.MessageStructure.SignalStructure("reserved"))
                    # Set data of added field 
                    #[fragment_name, fragment_size, is_reserved]
                    fragment_name = "reserved_" + str(reserved_counter)
                    return_data[-1].signals[-1].fragments = \
                        [fragment_name, 8, True]
                    reserved_counter += 1
                 
                # Add signals and interleaved reserved fields if required
                sig_counter = 0
                for signal in list_by_message:       
                    start_byte = list(signal.values())[0].start_byte
                    signal_len = int(list(signal.values())[0].len)
                    signal_len_bytes = int(signal_len / 8)
                    signal_name = list(signal.values())[0].name
                    
                    current_end_byte = start_byte + signal_len_bytes - 1
                    
                    sig_counter += 1
                    
                    # Add current signal
                    return_data[-1].signals.append(\
                         self.MessageStructure.SignalStructure(signal_name))
                    # Set data of added signal 
                    #[signal_name, signal_len, is_reserved]
                    return_data[-1].signals[-1].fragments = \
                        [signal_name, signal_len, False]
                    
                    # Calculate start byte of next signal if any
                    if sig_counter < len(list_by_message):
                        # if this is not the last signal...
                        next_start_byte = \
                            list(list_by_message[sig_counter].values())[0].start_byte
                    else:
                        # If this is the last signal, check if reserved fields
                        # are required at the end of the message
                        # Assume "next start byte" is the last message byte
                        next_start_byte = message_len - 1 
                    
                    # Calculate empty bytes between signals or between
                    # last signal and end of message
                    if next_start_byte > current_end_byte:
                        empty_bytes = next_start_byte - current_end_byte - 1
                    else:
                        empty_bytes = 0   
                    # Fill "empty" bytes with reserved
                    # fields.    
                    for reserved_field in range(empty_bytes):
                        # Append reserved field to the message structure
                        return_data[-1].signals.append(\
                             self.MessageStructure.SignalStructure("reserved"))
                        # Set data of added field 
                        #[fragment_name, fragment_size, is_reserved]
                        #TODO: make "reserve" word configurable
                        fragment_name = "reserved_" + str(reserved_counter)
                        return_data[-1].signals[-1].fragments = \
                            [fragment_name, 8, True]
                        reserved_counter += 1
                                

            elif msg_is_cannonical is False:
                # Message is not cannonical, hence, a bitfield will need
                # to be generated.
                
                # Get default prefix to use for parts of fragmented signals
                prefix = cg.gen_part_prefixes[cg.PRFX_DEFAULT]
                
                # Calculate base data type lenght to use for the bitfield of
                # this message.
                if (message_len % 2) != 0:
                    # If message len is odd then use bytes
                    base_type_len = 8
                elif message_len == 2 or message_len == 6:
                    base_type_len = 16
                elif message_len == 4 or message_len == 8:
                    base_type_len = 32
                else:
                    # Should never get here (msg len can't be > 8 bytes)
                    #TODO: warning. Wrong lenght of message message_name
                    pass
                
                # Append new message structure to the return data
                return_data.append( \
                    self.MessageStructure(message_name,base_type_len,False))
                # Working message structure can be access with return_data[-1]
                
                # Reserved fields will be created as needed in empty spaces
                # between signals.
                last_reserved_field = 0
                
                # LOGIC FOR PARTITIONING SIGNAL INTO FRAGMENTS
                # --------------------------------------------
                # Calculate start bit of next signal.
                next_start_bit = (list(list_by_message[0].values())[0].start_byte * 8) \
                        + list(list_by_message[0].values())[0].start_bit
                
                # Check if reserved fields are required to be at the
                # beginning of the message structure.
                if next_start_bit > 0:
                    # Calculate empty space in the message
                    empty_space_len = next_start_bit
                    # Calculate size of leading reserved field bits
                    empty_space_start_bit = 0
                    # Split signal (reserved field) into fragments as required.
                    splited_signal = self.split_signal(empty_space_start_bit, \
                        empty_space_start_bit + empty_space_len - 1, base_type_len)
                    
                    # Create required reserved field fragments
                    for field in splited_signal:
                        for each_unit in field:
                            if each_unit is not None:
#                                print("        Reserved")
                                # Append signal fragment to the message structure
                                return_data[-1].signals.append(\
                                     self.MessageStructure.SignalStructure("reserved"))
                                
                                # Set data of added fragment 
                                #[fragment_name, fragment_size, is_reserved]
                                fragment_name = "reserved_" + str(last_reserved_field)
                                return_data[-1].signals[-1].fragments = \
                                    [fragment_name, each_unit, True]
                                # Increment reserved fields counter
                                last_reserved_field += 1
                
                counter = 0 # Signal counter
                # Start using type for unsigned int of 8 bits
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
                    else:
                        # Get start bit and lenght of next signal
                        next_start_bit = \
                            (list(list_by_message[counter].values())[0].start_byte * 8) \
                            + list(list_by_message[counter].values())[0].start_bit

                    #TODO: Clean-up code for current and next types and types len
                    # this is not applicable anymore, now using only bytes types
        
                    # Generate Signal
                    # ---------------
                    if signal_type == data_types_list[ARRAY]:
                        # Signal is of an "array" type. By default,
                        # array type signals are arrays of unsigned bytes.
                        # Create directly a fragment for each byte of the array.
                        array_bytes = int(signal_len / 8)
                        for array_byte in range(array_bytes):
#                            print("        Array")
                            # Append signal fragment to the message structure
                            return_data[-1].signals.append(\
                                 self.MessageStructure.SignalStructure(str(signal_name)))
                            
                            # Set data of added fragment 
                            #[fragment_name, fragment_size, is_reserved]
                            fragment_name = str(signal_name) + "_" \
                                + prefix + str(array_byte)
                            # Check if fragment name doesn't duplicate a signal
                            # name, if so, try different part prefixes.
                            prefix_idx = 0
#                            print("------ fragment_name: ",fragment_name)
#                            test_str = ""
#                            for test_sig in msg_signals:
#                                test_str += test_sig.name + ", "
#                            print("------ msg_signals: ", test_str)
                            while fragment_name in msg_signals:
                                # Fragment name duplicates an existing signal name
                                # try different part prefix
                                prefix_idx += 1
                                if prefix_idx > (len(cg.gen_part_prefixes) - 1):
                                    # All prefixes exhausted. Use a disruptive
                                    # one so that it is evident and user can
                                    # manually name the signal parts in the C code.
                                    warnings.warn("Can't generate unique name" \
                                       + " for part "+ fragment_name \
                                       + "\" of signal \"" + signal_name \
                                       + "\" since it duplicates with existing" \
                                       + "signal in same message.", \
                                      UserWarning)
                                    
                                    prefix = "#"
                                    fragment_name = str(signal_name) + "_" \
                                        + prefix + str(array_byte)
                                    
                                    break;
                                else:
                                    warnings.warn("Signal part \"" + fragment_name \
                                       + "\" of signal \"" + signal_name \
                                       + "\" duplicates with name of existing " \
                                       + "signal in same message." \
                                       + " Attempting with prefix \"" \
                                       + cg.gen_part_prefixes[prefix_idx] \
                                       + "\"", UserWarning)
                                    
                                    # Try new prefix
                                    prefix = cg.gen_part_prefixes[prefix_idx]
                                    fragment_name = str(signal_name) + "_" \
                                        + prefix + str(array_byte)
                                    
                                    
                            # Return prefix to default one
                            prefix = cg.gen_part_prefixes[cg.PRFX_DEFAULT]
                            
                            # Set fragment information
                            return_data[-1].signals[-1].fragments = \
                                [fragment_name, 8, False]
                    else:
                        # Signal is of a scalar type
                        
                        # Signal may need to be fragmented, verify it by trying to
                        # split the signal into leading, nominal and trailing fields.
                        splited_signal = \
                            self.split_signal(absolute_start_bit, \
                            absolute_end_bit, \
                            base_type_len)
                        
                        if splited_signal[0][0] == None \
                        and splited_signal[2][0] == None \
                        and len(splited_signal[1]) == 1:
                            # If neither leading nor trailing bits are required and only 
                            # one nominal field is required then the signal doesn't 
                            # need to be fragmented.
                            fragmented_signal = False
                            #TODO: Test this case of signal not being fragmented!!!!
                        else:
                            # If either leading or trailing bits are required or more 
                            # than one nominal fields are required then the signal 
                            # needs to be fragmented.
                            fragmented_signal = True
                        
                        if fragmented_signal:
                            # Partition signal into segments in case the signal crosses
                            # byte boundaries. Segments will get a suffix _0, _1, ...
                            fragment_counter = 0

                            if splited_signal[0][0] is not None \
                            and splited_signal[1][0] is None \
                            and splited_signal[2][0] is None:
                                # If signal has only "leading field" it doesn't actually need
                                # to be fragmented but a bit field needs to be used for it.
#                                print("        Only leading")
                                # Append signal fragment to the message structure
                                return_data[-1].signals.append(\
                                     self.MessageStructure.SignalStructure(str(signal_name)))
                                # Set data of added fragment 
                                #[fragment_name, fragment_size, data_type_len, is_reserved]
                                fragment_name = str(signal_name)
                                return_data[-1].signals[-1].fragments = \
                                        [fragment_name, splited_signal[0][0], False]
                            else:
                                # Generate the multiple fragments for the signal
                                for field in splited_signal:
                                    # Three fields are expected. First one is the 
                                    # "leading field", second field is the "nominal field" 
                                    # and third field is the "trailing field".
                                    for unit_len in field:
                                        if unit_len is not None:
#                                            print("        Fragment")
                                            # Append signal fragment to the message structure
                                            return_data[-1].signals.append( \
                                                 self.MessageStructure.SignalStructure( \
                                                                    str(signal_name)) )
                                            # Set data of added fragment 
                                            #[fragment_name, 
                                            # fragment_size, data_type_len, is_reserved]
                                            fragment_name = str(signal_name) \
                                                    + "_" + str(fragment_counter)
                                            
                                            # Check if fragment name doesn't duplicate a signal
                                            # name, if so, try different part prefixes.
                                            prefix_idx = 0
#                                            print("------ fragment_name: ",fragment_name)
#                                            test_str = ""
#                                            for test_sig in msg_signals:
#                                                test_str += test_sig.name + ", "
#                                            print("------ msg_signals: ", test_str)
                                            
                                            while fragment_name in msg_signals:
                                                # Fragment name duplicates an existing signal name
                                                # try different part prefix
                                                prefix_idx += 1
                                                if prefix_idx > (len(cg.gen_part_prefixes) - 1):
                                                    # All prefixes exhausted. Use a disruptive
                                                    # one so that it is evident and user can
                                                    # manually name the signal parts in the C code.
                                                    warnings.warn("Can't generate unique name" \
                                                       + " for part "+ fragment_name \
                                                       + "\" of signal \"" + signal_name \
                                                       + "\" since it duplicates with existing" \
                                                       + "signal in same message.", \
                                                      UserWarning)
                                                    
                                                    prefix = "#"
                                                    fragment_name = str(signal_name) + "_" \
                                                        + prefix + str(fragment_counter)
                                                    
                                                    break;
                                                else:
                                                    warnings.warn("Signal part \"" + fragment_name \
                                                       + "\" of signal \"" + signal_name \
                                                       + "\" duplicates with name of existing " \
                                                       + "signal in same message." \
                                                       + " Attempting with prefix \"" \
                                                       + cg.gen_part_prefixes[prefix_idx] \
                                                       + "\"", UserWarning)
                                                    
                                                    # Try new prefix
                                                    prefix = cg.gen_part_prefixes[prefix_idx]
                                                    fragment_name = str(signal_name) + "_" \
                                                        + prefix + str(fragment_counter)
                                            # Return prefix to default one
                                            prefix = cg.gen_part_prefixes[cg.PRFX_DEFAULT]
                                            
                                            # Set fragment information
                                            return_data[-1].signals[-1].fragments = \
                                                [fragment_name, unit_len, False]
                                
                                            fragment_counter += 1
                        else:                           
                            # Signal doesn't need to be fragmented
                            # Append signal to the message structure
#                            print("        Not fragmented")
                            return_data[-1].signals.append( \
                                 self.MessageStructure.SignalStructure(str(signal_name)))
                            # Set data of added fragment 
                            #[fragment_name, fragment_size, data_type_len, is_reserved]
                            return_data[-1].signals[-1].fragments = \
                                [str(signal_name), signal_len, False]
                        
                    # Check if reserved fields are required at the end of msg
                    # -------------------------------------------------------
                    if next_start_bit > (absolute_end_bit + 1):
                        # Calculate empty space in the end of the message
                        empty_start_bit = absolute_end_bit + 1 
                        empty_space_len = next_start_bit - empty_start_bit
                        
                        splited_signal = self.split_signal(empty_start_bit, \
                            empty_start_bit + empty_space_len - 1, \
                            base_type_len)

                        for field in splited_signal:
                            for unit_len in field:
                                if unit_len is not None:
#                                    print("        Reserved End")
                                    # Append signal fragment to the message structure
                                    return_data[-1].signals.append( \
                                         self.MessageStructure.SignalStructure("reserved"))
                                    # Set data of added fragment 
                                    #[fragment_name, fragment_size, data_type_len, is_reserved]
                                    fragment_name = "reserved_" \
                                            + str(last_reserved_field)
                                    return_data[-1].signals[-1].fragments = \
                                        [fragment_name, unit_len, True]
                                    
                                    last_reserved_field += 1
        return return_data
    
    #===============================================================================================  
    def add_signal(self, name, lenght, description = ""):
        """ Adds a new signal to the network. """
        if cg.is_valid_identifier(name):
            #Verify that signal to be added is not already existing
            if name not in self.signals:
                #Create new signal object in the network
                self.signals.update( {name : \
                                      self.Signal(name, lenght, description)} )
            else:
                warnings.warn("Duplicated Signal: \"" + name + \
                          "\" not added to the network.", \
                          UserWarning)
        else:
            #Warning, invalid signal name
            warnings.warn("Invalid signal name: \"" + name + \
                          "\". Signal names must comply with C-language identifier syntax.", \
                          UserWarning)
    
    #===============================================================================================        
    def add_signal_to_message(self, message_name, signal_name, start_byte, start_bit):
        """ Adds a signal to a message with the speficied layout information. """
        #Check if message exists
        if message_name in self.messages:
            #Check if signal exists
            if signal_name in self.signals:
                #Chcek if start byte and start bit are valid numbers (hex or dec)
                start_byte = cg.get_valid_number(start_byte)
                start_bit = cg.get_valid_number(start_bit)
                
                if start_byte != None and start_byte != None \
                        and self.signals[signal_name].len != None:
                            
                    new_absolute_start_bit = (start_byte * 8) + start_bit
                    new_absolute_end_bit = new_absolute_start_bit \
                                            + self.signals[signal_name].len - 1
                    #Check if signal fits in the message
                    if new_absolute_end_bit >= (self.messages[message_name].len * 8):
                        warnings.warn("Signal \"" + signal_name \
                                      + "\" doesn't fit in message \"" \
                                      + message_name + "\".", UserWarning)
                    else:
                        #check if signal space is not occupied by other signals
                        space_occupied = False
                        for signal in self.signals.values():
                            if signal.message == message_name:
                                #Current signal is assgined to the target message
                                #Check if space doesn't overlap with the new one
                                absolute_start_bit = (int(signal.start_byte) * 8) \
                                                    + int(signal.start_bit)
                                absolute_end_bit = absolute_start_bit \
                                                    + int(signal.len) - 1
                                
                                if cg.ranges_overlap(absolute_start_bit, \
                                                       absolute_end_bit, \
                                                       new_absolute_start_bit, \
                                                       new_absolute_end_bit) is True:
                                    space_occupied = True
                                    warnings.warn("Signal \"" + signal_name \
                                      + "\" overlaps with signal \"" \
                                      + signal.name + "\".", UserWarning)
                                    break
                        if space_occupied is False:
                            # If signal is of array type (data_types_list[ARRAY]) 
                            # then verify that aboslute start bit is byte-aligned 
                            # and that signal lenght is multiple of 8bits (1 byte)
                            start_bit_modulus = new_absolute_start_bit % 8
                            len_modulus = self.signals[signal_name].len % 8
                            #TODO: test this array logic
                            if (self.signals[signal_name].data_type == \
                            data_types_list[ARRAY] and start_bit_modulus == 0 \
                            and len_modulus == 0) \
                            or (self.signals[signal_name].data_type != \
                            data_types_list[ARRAY]):
                                #Set signal's conveyor message
                                self.signals[signal_name].set_conveyor_message(message_name)
                                #Set layout information
                                self.signals[signal_name].set_layout_info(start_byte, \
                                               start_bit)
                            else:
                                warnings.warn("Signal \"" + signal_name \
                                      + "\" is of array type and its start bit " \
                                      + "is not byte aligned or its lenght " \
                                      + "is not a multiple of 8-bits (1 byte)." \
                                      + "Signal not added to the network.", UserWarning)
                        else:
                            warnings.warn("Signal \"" + signal_name \
                                      + "\" Can't be added to message \"" \
                                      + message_name + "\" with current layout.", UserWarning)
                else:
                    warnings.warn("Wrong paratemer(s) for start_byte/start_bit/signal_lenght :" \
                                 + str(start_byte) + "/" + str(start_bit) + "/" \
                                 + str(self.signals[signal_name].len) \
                                 + " for signal \"" + signal_name + "\".", UserWarning) 
            else:
                warnings.warn("Signal \"" + signal_name + "\" not defined.", UserWarning)
        else:
            warnings.warn("Message \"" + message_name + "\" not defined.", UserWarning)
            
    #===============================================================================================    
    def set_signal_data_type(self, signal_name, data_type):
        if signal_name in self.signals:
            if data_type in data_types_list:
                # Data type of signal is a prefixed one (not custom)
                self.signals[signal_name].is_enum_type = False
                self.signals[signal_name].data_type = data_type 
            else:
                if data_type in self.enum_types:
                    # Data type of signal is a custom enum type
                    self.signals[signal_name].is_enum_type = True
                    self.signals[signal_name].data_type = data_type 
                else:
                    # Data type not defined.
                    warnings.warn("Undefined singal type: \"" + data_type \
                                     + "\" for signal \"" \
                                     + signal_name + "\".", UserWarning)
    
    #===============================================================================================
    def get_signal_conveyor_message(self, signal_name):
        """ Returns the name of the conveyor message of a signal. """
        #TODO: review this function
        return_value = None
        if signal_name in self.signals:
            return_value = self.signals[signal_name].message
        return return_value
    
    #===============================================================================================
    def get_sorted_signals_by_layout(self, grouped_by_message = False):
        """ Sorts the signals in this network according to their layout """
        return_list = {}
        sorted_signals_list = []
        
        # If input signals has elements do the sorting, otherwise return empty output.
        if len(self.signals) > 0:
            # Create working list of signals
            for signal in self.signals:
                if self.signals[signal].message is not None:
                    sorted_signals_list.append({signal : self.signals[signal]})
                else:
                    warnings.warn("Signal: \"" + signal \
                            + "\" doesn't have a publishing message.", UserWarning)
                
            # Sort signals by their messages
            sorted_signals_list = \
                sorted(sorted_signals_list, \
                       key= lambda signal_entry : list(signal_entry.values())[0].message)
                
                

            # Sort signals by layout within each message
            # Following code will create temporal individual lists of signals
            # for each message, then sort them. At the end all lists will be
            # glued back together.
            current_message = list(sorted_signals_list[0].values())[0].message
            signals_of_a_message = []
            list_of_list_of_signals = []
            counter = 0
            
            for signal in sorted_signals_list:
                
                counter += 1
                signal_object = list(signal.values())[0]
                signal_name = list(signal.keys())[0]
                
#                print("=== current msg: ",current_message ,"new msg: ",signal_object.message)
                
                # Check if we have changed to another message or not
                if signal_object.message == current_message:
                    new_message = False
                else:
                    # Jumping to a new message. Need to sort the list of the 
                    # current message and then start the one of the new message
                    new_message = True
    
                # For last item sort the list of current message and append it
                # to the list of lists
                if counter == len(sorted_signals_list):
                    last_signal = True
                else:
                    last_signal = False 
                # If from above code a sort and start new list is required
                # then do it.    
                
                if not new_message and not last_signal:
                    pre_add_signal = True
                    sort_and_append = False
                    post_add_signal = False
                    append_directly = False
                elif not new_message and last_signal:
                    pre_add_signal = True
                    sort_and_append = True
                    post_add_signal = False
                    append_directly = False
                elif new_message and not last_signal:
                    pre_add_signal = False
                    sort_and_append = True
                    post_add_signal = True
                    append_directly = False
                else: #new_byte and last_byte
                    pre_add_signal = False
                    sort_and_append = True
                    post_add_signal = True
                    append_directly = True
                                
                # Execute actions
                if pre_add_signal:
                    # Keep forming list of signals for a message
                    signals_of_a_message.append({signal_name : signal_object})
                if sort_and_append:
                    # 1) Sort the list
                    
#                    print("Pre SORT\n-----------------")
#                    print(signals_of_a_message)
                    signals_of_a_message = \
                        sorted(signals_of_a_message, \
                               key= lambda signal_entry : \
                               list(signal_entry.values())[0].start_byte)
#                    print("POST SORT\n-----------------")
#                    print(signals_of_a_message)
#                    print("-----------------")
                    # Now sort by byte and bit  
                    # ------------------------
                    # Now sort each byte by start_bit
                    # A new list for each byte will be created, then it will be
                    # sorted. At the end it will be glued back to the list of the
                    # message.
                    current_byte = list(signals_of_a_message[0].values())[0].start_byte
                    signals_by_start_byte = []
                    list_of_lists_by_byte = []
                    counter2 = 0
                    for signal_in_byte in signals_of_a_message:
                        counter2 += 1
                        signal_name2 = list(signal_in_byte.keys())[0]
                        signal_object2 = list(signal_in_byte.values())[0]
                        
                        # Check if we have changed to another byte or not
                        if signal_object2.start_byte == current_byte:
                            new_byte = False
                            
                        else:
                            # Jumping to a new byte. Defer appending this new signal
                            # until after the list for current byte is sorted
                            # and appended to the parent's list.
                            new_byte = True
                        # For last item sort the list of current byte and append it
                        # to the list of lists
                        if counter2 == len(signals_of_a_message):
                            last_byte = True
                        else:
                            last_byte = False
                        # If from above code a sort and start new list is required
                        # then do it.

                        if not new_byte and not last_byte:
                            pre_add_signal2 = True
                            sort_and_append2 = False
                            post_add_signal2 = False
                            append_directly2 = False
                        elif not new_byte and last_byte:
                            pre_add_signal2 = True
                            sort_and_append2 = True
                            post_add_signal2 = False
                            append_directly2 = False
                        elif new_byte and not last_byte:
                            pre_add_signal2 = False
                            sort_and_append2 = True
                            post_add_signal2 = True
                            append_directly2 = False
                        else: #new_byte and last_byte
                            pre_add_signal2 = False
                            sort_and_append2 = True
                            post_add_signal2 = True
                            append_directly2 = True
                        # Execute actions
                        if pre_add_signal2:
                            # Keep forming list of signals by start byte
                            signals_by_start_byte.append({signal_name2 : signal_object2})
                        if sort_and_append2:
                            # 1) Sort the list
                            signals_by_start_byte = \
                                sorted(signals_by_start_byte, \
                                       key= lambda signal_entry : \
                                       list(signal_entry.values())[0].start_bit)
                            # 2) Append list to the parent's list
                            list_of_lists_by_byte.append(signals_by_start_byte.copy())
                        if post_add_signal2:
                            current_byte = signal_object2.start_byte
                            signals_by_start_byte.clear()
                            signals_by_start_byte.append({signal_name2 : signal_object2})
                            
                        if append_directly2:
                            # 2) Append list to the parent's list
#                            signals_by_start_byte.clear()
#                            signals_by_start_byte.append({signal_name2 : signal_object2})
                            list_of_lists_by_byte.append(signals_by_start_byte.copy())

                    # glue_back the list
                    signals_of_a_message.clear()
                    for this_list in list_of_lists_by_byte:
                        for element in this_list:
                            signal_name3 = list(element.keys())[0]
                            signal_object3 = list(element.values())[0]
                            signals_of_a_message.append({signal_name3 : \
                                                         signal_object3})    
                    # ------------------------
                    
                    # 3) Append list to the parent's list
                    list_of_list_of_signals.append(signals_of_a_message.copy())
                if post_add_signal:
                    # Start the list of the new message
                    current_message = signal_object.message
                    signals_of_a_message.clear()
                    signals_of_a_message.append({signal_name : signal_object})
                    
                if append_directly:
                    # Append list to the parent's list
                    list_of_list_of_signals.append(signals_of_a_message.copy())
            

            
            if grouped_by_message is False:
                #"flatten" the list to be ready to be returned. This list will
                # contain each signal ordered by message and by layour (start 
                # byte, start bit)
                for this_list in list_of_list_of_signals:
                    for element in this_list:
                        signal_name = list(element.keys())[0]
                        signal_object = list(element.values())[0]
                        return_list.update({signal_name : signal_object})
            else:
                # Return list will contain a list for each found message
                # each of these lists is indeed another list with the
                # signals of that message ordered by layout.
                return_list = list_of_list_of_signals
            
            return return_list
    
    #===============================================================================================
    def sort_signals_by_layout(self):     
        """ Sorts the signals in this network object by their layout information. """
        sorted_signals = self.get_sorted_signals_by_layout()
        if len(sorted_signals) > 0:
            self.signals.clear()
            self.signals = sorted_signals.copy()
            
    #=============================================================================================== 
    #TODO: check usage and implementaiton of function "fragment_signal"
    def fragment_signal(self, signal_name, abs_start_bit, abs_end_bit, \
            nominal_type_len = 8, leading_type_len = None, trailing_type_len = None):
        """ Fragments a signal based on its size and layout information.
        
            If signal is
        """
        if leading_type_len is None:
            leading_type_len = nominal_type_len
        if trailing_type_len is None:
            trailing_type_len = nominal_type_len
            
        return_list = [] # [leading part, main part 1, main part 2..., trailing part]
        
        signal_len = abs_end_bit - abs_start_bit + 1
    
        #Calculate starting unit
        starting_unit = math.floor(abs_start_bit / leading_type_len) * leading_type_len
        
        # Calculate size of leading field bits
        if (starting_unit % leading_type_len) > 0:
            leading_reserved_len = leading_type_len - (starting_unit % leading_type_len)
        else:
            leading_reserved_len = 0
        return_list.append(leading_reserved_len)    
        
        remaining_bits = signal_len - leading_reserved_len
        main_field_units = math.floor(remaining_bits / nominal_type_len)
        trailing_reserved_len = remaining_bits % 8
        
        if main_field_units > 0:
            for each_unit in range(main_field_units):
                return_list.append(nominal_type_len)
        else:
            return_list.append(None)
        
        # Fill trailing field
        return_list.append(trailing_reserved_len)
        
        #cog.outl(" -- ---- Test: " + str(return_list))
        
        return return_list
#    
#    def set_signal_custom_type(self, signal_name, type_name):
#    
    #===============================================================================================    
    def gen_XML(self, output_file = None, gen_types = True, gen_nodes = True, \
                 gen_messages = True, gen_signals = True):
        """ Generates an XML describing the Network modeled by this object. """
        
        #GEnerate Network root node and its information
        XML_root = ET.Element("Network")
        XML_root.set("protocol","CAN")
        XML_level_1 = ET.Element("Name")
        XML_level_1.text = self.name
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Desc")
        XML_level_1.text = self.description
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Version")
        XML_level_1.text = self.version
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Date")
        XML_level_1.text = self.date
        XML_root.append(XML_level_1)
        
        if gen_types is True:
            #Generate nodes
            XML_level_1 = ET.Element("Nodes")
            for node in self.nodes.values():
                XML_level_2 = ET.Element("Node")
                
                XML_level_3 = ET.Element("Name")
                XML_level_3.text = node.name
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Desc")
                XML_level_3.text = node.description
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("NodeMessages")
                
                #Generate Rx Messages
                for message in node.subscribed_messages:
                    XML_level_4 = ET.Element("NodeMessage")
                    XML_level_4.set("dir","Rx")
                    timeout_ms = node.subscribed_messages[message]
                    if timeout_ms is not None:
                        XML_level_4.set("timeout_ms", str(timeout_ms)) 
                    else:
                        XML_level_4.set("timeout_ms","") 
                    XML_level_4.text = message
                    XML_level_3.append(XML_level_4)
                    
                #Generate Tx Messages
                for message in self.messages.values():
                    #Check if this node is publisher of this message
                    if message.publisher == node.name:
                        XML_level_4 = ET.Element("NodeMessage")
                        XML_level_4.set("dir","Tx")
                        XML_level_4.set("timeout_ms","") 
                        XML_level_4.text = message.name
                        XML_level_3.append(XML_level_4)
                
                XML_level_2.append(XML_level_3)
                
                XML_level_1.append(XML_level_2)
            XML_root.append(XML_level_1)    
        
        if gen_nodes is True:
            #Generate Messages
            XML_level_1 = ET.Element("Messages")
            for message in self.messages.values():
                XML_level_2 = ET.Element("Message")
                
                XML_level_3 = ET.Element("Name")
                XML_level_3.text = message.name
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Desc")
                XML_level_3.text = message.description
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Id")
                if message.extended_id:
                    XML_level_3.set("extended","yes")
                else:
                    XML_level_3.set("extended","no")
                XML_level_3.text = str(message.id)
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Len")
                XML_level_3.text = str(message.len)
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Transmission")
                XML_level_3.set("type",str(message.tx_type))
                
                XML_level_4 = ET.Element("Period_ms")
                if message.tx_period is not None:
                    XML_level_4.text = str(message.tx_period)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("BafRepeats")
                if message.tx_baf_repeats is not None:
                    XML_level_4.text = str(message.tx_baf_repeats)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                
                XML_level_2.append(XML_level_3)
                XML_level_1.append(XML_level_2)
            
            XML_root.append(XML_level_1)    
        
        #Generate Signals
        if gen_signals is True:
            XML_level_1 = ET.Element("Signals")
            for signal in self.signals.values():
                
                
                XML_level_2 = ET.Element("Signal")
                
                XML_level_3 = ET.Element("Name")
                XML_level_3.text = signal.name
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Desc")
                XML_level_3.text = signal.description
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Len")
                XML_level_3.text = str(signal.len)
                XML_level_2.append(XML_level_3)
                XML_level_3 = ET.Element("Data")
    
                XML_level_4 = ET.Element("Type")
                if signal.is_enum_type:
                    XML_level_4.set("enumType","yes")
                else:
                    XML_level_4.set("enumType","no")
                if signal.data_type != None:
                    XML_level_4.text = str(signal.data_type)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("InitValue")
                if signal.init_value != None:
                    XML_level_4.text = str(signal.init_value)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("FailSafeValue")
                if signal.fail_value != None:
                    XML_level_4.text = str(signal.fail_value)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("Offset")
                if signal.offset != None:
                    XML_level_4.text = str(signal.offset)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("Resolution")
                if signal.resolution != None:
                    XML_level_4.text = str(signal.resolution)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_4 = ET.Element("Unit")
                if signal.unit != None:
                    XML_level_4.text = str(signal.unit)
                else:
                    XML_level_4.text = ""
                XML_level_3.append(XML_level_4)
                XML_level_2.append(XML_level_3)
                XML_level_1.append(XML_level_2)
                
                #Signal Layout
                XML_level_3 = ET.SubElement(XML_level_2,"Layout")
                XML_level_4 = ET.SubElement(XML_level_3,"ConveyorMsg")
                XML_level_4.text = str(signal.message)
                XML_level_4 = ET.SubElement(XML_level_3,"StartByte")
                XML_level_4.text = str(signal.start_byte)
                XML_level_4 = ET.SubElement(XML_level_3,"StartBit")
                XML_level_4.text = str(signal.start_bit)
                
            
            XML_root.append(XML_level_1)  
        
        #Generate Type defs
        if gen_types is True:
            XML_level_1 = ET.SubElement(XML_root,"Typedefs")
            #Generate prefixed types
            for simple_type in data_types_list:
                XML_level_2 = ET.SubElement(XML_level_1,"Type")
                XML_level_2.set("enumerated","no")
                XML_level_3 = ET.SubElement(XML_level_2,"Name")
                XML_level_3.text = simple_type
            #Generate enum types
            for enum_type in self.enum_types.values():
                XML_level_2 = ET.SubElement(XML_level_1,"Type")
                XML_level_2.set("enumerated","yes")
                XML_level_3 = ET.SubElement(XML_level_2,"Name")
                XML_level_3.text = str(enum_type.name)
                XML_level_3 = ET.SubElement(XML_level_2,"Desc")
                XML_level_3.text = str(enum_type.description)
                #Generate enum type values
                for enum_entry in enum_type.enum_entries:
                    XML_level_3 = ET.SubElement(XML_level_2,"Value")
                    
                    if enum_type.enum_entries[enum_entry] is not None:
                        XML_level_3.set("number", \
                                             str(enum_type.enum_entries[enum_entry]))
                    else:
                        XML_level_3.set("number","")   
                    XML_level_3.text = enum_entry
        
        #Generate Metdata
        XML_level_1 = ET.SubElement(XML_root,"Metadata")
        
        XML_level_2 = ET.SubElement(XML_level_1,"TemplateName")
        XML_level_2.text = Network_CAN.metadata_template_name
        XML_level_2 = ET.SubElement(XML_level_1,"TemplateVersion")
        XML_level_2.text = Network_CAN.metadata_template_version
        XML_level_2 = ET.SubElement(XML_level_1,"TemplateDesc")
        XML_level_2.text = Network_CAN.metadata_template_description
    
#        XML_root.append(XML_level_1)
    
        print("Generating XML...")
        XML_string = ET.tostring(XML_root)
        
        XML_string = xml.dom.minidom.parseString(XML_string)
        XML_string = XML_string.toprettyxml()

        if output_file is None:
            print(XML_string)
        else:
            myfile = open(output_file, "w")
            myfile.write(XML_string)
        #TODO: save XML to file if output_file is different than None

    #===============================================================================================    
    def cog_generator(self, input_file, network_name, node_name, \
                             out_dir, work_dir, gen_path, pickle, \
                             variables = None):
        """ Invoke cog generator for the specified file.
        """
        # Setup input and output files
        # ----------------------------       
        comgen_CAN_cog_input_file = gen_path / input_file
        #remove "cog_" prefix to output file names
        comgen_CAN_cog_output_file = out_dir / (input_file[4:])
        #substitute network name if found (NWNAME)
        if network_name is not None:
            comgen_CAN_cog_output_file = \
                str(comgen_CAN_cog_output_file).replace('NWNAME',network_name)
        else:
            comgen_CAN_cog_output_file = \
                str(comgen_CAN_cog_output_file).replace('NWNAME_','')
        #substitute node name if found (NODENAME)
        if node_name is not None:
            comgen_CAN_cog_output_file = \
                str(comgen_CAN_cog_output_file).replace('NODENAME','node')
        else:
            comgen_CAN_cog_output_file = \
                str(comgen_CAN_cog_output_file).replace('NODENAME_','')
                
        # Invoke code generation
        # ----------------------           
        cog_arguments = ['dummy_argument', \
                   '-d', \
                   '-D', 'input_worksheet=' + self.metadata_gen_source_file, \
                   '-D', 'project_working_dir=' + str(work_dir), \
                   '-D', 'cog_pickle_file=' + str(pickle), \
                   '-D', 'node_name=' + str(node_name), \
                   '-o', str(comgen_CAN_cog_output_file), \
                   str(comgen_CAN_cog_input_file) ]
        
        # Append additional variables if required
        if variables is not None:
            for variable in variables:
                cog_arguments.append('-D')
                cog_arguments.append(str(variable))
                
        # Call cogapp engine
        cog_return = cog.Cog().main(cog_arguments)
        if cog_return == 0:
            print("INFO: code generation successful: ",comgen_CAN_cog_output_file)
        else:
            print("INFO: code generation return value: ",cog_return)
        
    #===============================================================================================   
    def gen_code(self, out_dir, work_dir, gen_path, node_names = None, \
                 single_network = False):
        """ Generates C code for the given network.
        """

        if single_network is True:
            network_name = None
        else:
            network_name = self.name
        
        # Create temporal file with network object pickle.
        # ------------------------------------------------
        cog_pickle_file_name = "comgen_CAN_network_obj.pickle"
        cog_serialized_network_file = work_dir / cog_pickle_file_name
        
        try:
            with open(cog_serialized_network_file, 'wb') as f:
                pic.dump(self, f, pic.HIGHEST_PROTOCOL)
        except Exception as e:
                print('Failed to create pickle file %s. Reason: %s' \
                      % (cog_serialized_network_file, e))
        
        #----------------------------------------------------------------------
        # Generate signal file(s)
        #----------------------------------------------------------------------
        cog_file_name = "cog_comgen_CAN_NWNAME_network.h"
        self.cog_generator(cog_file_name, network_name, None, out_dir, \
                             work_dir, gen_path, cog_serialized_network_file)
        
        #----------------------------------------------------------------------
        # Generate Messages file(s)
        #----------------------------------------------------------------------
        cog_file_name = "cog_comgen_CAN_NWNAME_messages.h"
        self.cog_generator(cog_file_name, network_name, None, out_dir, \
                             work_dir, gen_path, cog_serialized_network_file)
        
        

        
        # Delete pickle file
        # ------------------
        print("INFO: deleting picke file")
        cg.delete_file(cog_serialized_network_file)
        
        #TODO: Logic for ensuring the current network is meaningful, e.g., is no emtpy, etc.
        if node_names == None:
            # C code for all nodes will be generated.
            pass
        elif len(node_names) > 0:
            # Generate C code for specified node each
            for node in node_names:
                #Check if node exists
                if node in self.nodes:
                    pass
                else:
                    pass
        else:
            warnings.warn("Invalid input. No C-Code will be generated for " 
                          + "network: " + self.name + ".", UserWarning)
        
    #===============================================================================================    
    def parse_spreadsheet_ods(self,input_file):
        """ parses an spreadsheet (ods format) to generate this network. """
        book = pe.get_book(file_name=str(input_file))
        self.metadata_gen_source_file = str(input_file)

        # -----------------------
        # Parse Network Nodes
        # -----------------------
        working_sheet = book["Nodes"]
        working_sheet.name_columns_by_row(0)
        
        for row in working_sheet:
            name = row[working_sheet.colnames.index("Node Name")]
            #Skip rows with no node defined
            if str(name) != "": 
                description = row[working_sheet.colnames.index("Description")]
                self.add_node(name, description)  
        
        # -----------------------
        # Parse Enum Types
        # -----------------------
        working_sheet = book["Data Types"]
        working_sheet.name_columns_by_row(0)
        
        for row in working_sheet:
            name = row[working_sheet.colnames.index("Type Name")]
            #Skip rows with no type defined
            if cg.is_valid_identifier(str(name)):
                enum_string = row[working_sheet.colnames.index("Enum Values")]
                self.add_enum_type(name, enum_string)
        
        # -----------------------
        # Parse Messages
        # -----------------------
        working_sheet = book["Messages"]
        working_sheet.name_columns_by_row(0)
        
        for row in working_sheet:
            message_name = row[working_sheet.colnames.index("Message Name")]
            #Skip rows with no message defined
            if cg.is_valid_identifier(str(message_name)):  
                message_id = row[working_sheet.colnames.index("Message ID")]
                message_extended = row[working_sheet.colnames.index("Extended Frame?")]
                message_lenght = row[working_sheet.colnames.index("Data Length (bytes)")]
                message_desc = row[working_sheet.colnames.index("Description")]
                message_publisher = row[working_sheet.colnames.index("Publisher")]
                message_subscribers = row[working_sheet.colnames.index("Subscribers")]
                message_tx_type = row[working_sheet.colnames.index("Tx Type")]
                message_period = row[working_sheet.colnames.index("Period (ms)")]
                message_repetitions = \
                    row[working_sheet.colnames.index("Repetitions (only for BAF)")]
                
                self.add_message(message_name, message_id, message_lenght, \
                                    message_extended, message_tx_type, message_period, \
                                    message_repetitions, message_desc)
                
                if str(message_publisher) != "":
                    self.add_tx_message_to_node(message_publisher,message_name)
                else:
                    warnings.warn("Message \"" + message_name 
                                  + "\" doesn't have a defined publisher.", SyntaxWarning)
                
                if str(message_subscribers) != "":
                    self.add_rx_message_to_subscribers(message_name, message_subscribers)
                else:
                    warnings.warn("Message \"" + message_name 
                                  + "\" doesn't have subscribers.", SyntaxWarning)
        
        # -----------------------
        # Parse Signals
        # -----------------------
        working_sheet = book["Signals"]
        working_sheet.name_columns_by_row(0)
        
        for row in working_sheet:
            signal_name = row[working_sheet.colnames.index("Signal Name")]   
            #Skip rows with no signal defined
            if  cg.is_valid_identifier(str(signal_name)):
                signal_len = row[working_sheet.colnames.index("Lenght (bits)")]
                signal_type = row[working_sheet.colnames.index("Data Type")]
                signal_desc = row[working_sheet.colnames.index("Description")]
                signal_conv_msg = row[working_sheet.colnames.index("Conveyor Message")]
                signal_start_byte = row[working_sheet.colnames.index("Start Byte")]
                signal_start_bit = row[working_sheet.colnames.index("Start Bit")]
                signal_init_val = row[working_sheet.colnames.index("Initial Value")]
                signal_fail_val = row[working_sheet.colnames.index("Fail Safe Value")]
                signal_offset = row[working_sheet.colnames.index("Offset")]
                signal_resolution = row[working_sheet.colnames.index("Resolution")]
                signal_unit = row[working_sheet.colnames.index("Unit")]
                #Add signal object to the network
                self.add_signal(signal_name, signal_len, signal_desc)
                #Add data type to the signal
                if signal_type != "":
                    self.set_signal_data_type(signal_name, signal_type)
                else:
                    warnings.warn("Signal \"" + signal_name 
                              + "\" doesn't have a defined data type.", SyntaxWarning)
                # Add signal to the specified message if any
                if signal_conv_msg != "":
                    if cg.is_valid_identifier(str(signal_conv_msg)):
                        #Set signal's conveyor message
                        self.add_signal_to_message(signal_conv_msg, signal_name, \
                                                signal_start_byte, signal_start_bit)
                    else:
                        warnings.warn("Wrong conveyor message \"" + signal_conv_msg \
                            + "\" for signal \"" + signal_name + "\".", SyntaxWarning)
                else:
                    warnings.warn("Signal \"" + signal_name 
                                  + "\" doesn't have a defined coveyor message.", SyntaxWarning)        
                # Add extra information paraemters of the signal (no warnings if empty)
                if str(signal_init_val) != "":
                    self.signals[signal_name].init_value = signal_init_val
                
                if str(signal_fail_val) != "":
                    self.signals[signal_name].fail_value = signal_fail_val
                
                if str(signal_offset) != "":
                    self.signals[signal_name].fail_value = signal_offset
                
                if str(signal_resolution) != "":
                    self.signals[signal_name].fail_value = signal_resolution
                
                if str(signal_unit) != "":
                    self.signals[signal_name].fail_value = signal_unit
    
    #===============================================================================================        
    class EnumType:
        """ Class to model an enumerated data type. """
        def __init__(self, name, enum_string, description = ""):
            self.name = str(name)
            self.enum_entries = {} # {enum symbol, enum value|None}
            self._temporal_entries = cg.parse_special_string( str(enum_string))
            if len(self._temporal_entries) > 0:
                self.enum_entries = self._temporal_entries

            self.description = description
        
        def add_enum_element(self,enum_symbol, enum_value = None):
            self.enum_entries.update(str(enum_symbol),enum_value)
            
        def remove_enum_element(self,enum_symbol):
            if enum_symbol in self.enum_entries:
                self.enum_entries.pop(str(enum_symbol))
            else:
                warnings.warn("enum symbol \"" + enum_symbol + \
                      "\" doesn't exist", UserWarning)
    
    #===============================================================================================
    class Node:
        """ Class to model a network node. """
        def __init__(self, name, description = ""):
            self.name = str(name)

            self.description = description
    
            self.subscribed_messages = {} # {message name, message timeout}
        
        def add_subscriber(self, message_name, timeout_ms):
            """ Adds an RX message for this node """
            if message_name not in self.subscribed_messages:
                self.subscribed_messages.update({message_name : timeout_ms}) 
            else:
                warnings.warn("Message \"" + message_name + \
                      "\" already subscribed to node." + self.name, UserWarning)
    
    #===============================================================================================      
    class Message:
    
        def __init__(self, name, msg_id, length, extended_id = False, \
                     tx_type = None, tx_period = None, tx_baf_repeats = None, \
                     description = ""):
            """ Constructor for the class """
            self.name = str(name)
            
            self.id = cg.get_valid_number(msg_id)
            if self.id is None:
                warnings.warn("Wrong ID value: \"" + msg_id \
                              + "\" of message \"" + self.name \
                              + "\". Assumed ID 'None'.", UserWarning)
            
            self.len = cg.get_valid_number(length)
            if self.len is None:
                #Assume None for message ID.
                self.len = 8
                warnings.warn("Wrong lenght value: \"" + length \
                              + "\" of message \"" + self.name \
                              + "\". Assumed lenght of 8 bytes.", UserWarning)

            if extended_id is False:
                self.extended_id = False
            else:
                self.extended_id = True
                
            if tx_type == None:
                #If no tx type was given, assume default one
                self.tx_type = tx_type_list[DEFAULT]
                #warn user    
                warnings.warn("No Tx type given for message: \"" + self.name \
                                + "\". Assumed default one: \"" \
                                + self.tx_type + "\"", UserWarning)
            else:
                if tx_type in tx_type_list:
                    self.tx_type = tx_type
                else:
                    #Form a string of the allowed Tx types to warn the user
                    allowed_tx_types = ""
                    for tx_type_element in tx_type_list:
                        allowed_tx_types = allowed_tx_types + tx_type_element + ", " 
                    #warn user    
                    warnings.warn("Wrong Tx type: \"" + tx_type + "\" of message: \"" \
                                    + self.name \
                                    + "\". \n Please use on the following: " \
                                    + allowed_tx_types
                                    + ".\n. Assummed default tx type: "
                                    + tx_type_list[DEFAULT], UserWarning)
                    #Sef default tx type
                    self.tx_type = tx_type_list[DEFAULT]
            
            #If message is defined as cyclic or BAF then validate period value
            if self.tx_type == tx_type_list[CYCLIC] \
                or self.tx_type == tx_type_list[BAF]:
                self.tx_period = cg.get_valid_number(tx_period)
                if self.tx_period is not None:
                    self.tx_period = tx_period
                else:
                    #Assume None for message ID.
                    warnings.warn("Wrong Tx period value: \"" + tx_period \
                                 + "\" of message \"" \
                                 + self.name + "\".", UserWarning)
            else:
                self.tx_period = tx_period
            
            #If message is defined as BAF then validate BAF repeats value
            if self.tx_type == tx_type_list[BAF]:
                self.tx_baf_repeats = cg.get_valid_number(tx_baf_repeats)
                if self.tx_baf_repeats is not None:
                    self.tx_baf_repeats = tx_baf_repeats
                else:
                    #Assume None for Baf repeats.
                    warnings.warn("Wrong BAF repeats value: \"" + tx_baf_repeats \
                                 + "\" of message \"" \
                                 + self.name + "\".", UserWarning)
            else:
                self.tx_baf_repeats = tx_baf_repeats

            
            self.description = description
            
            self.publisher = None
        
        def set_publisher(self, publisher_node):
            """ Sets the publisher node of this message """
            self.publisher = str(publisher_node)
                
        def get_publisher(self):
            """ Returns the publisher node of this message """
            return self.publisher
    
    #===============================================================================================    
    class MessageStructure:
        """ Class to model a message structure (signals layout). """
        def __init__(self, name, base_type_len, is_cannonical = None):
            self.name = str(name)
            self.base_type_len = base_type_len
            self.is_cannonical = is_cannonical #TODO: see if this is needed
            
            self.signals = []  # [ SignalStructure object1, 
                               # SignalStructure object2, ... ]
        
        #===========================================================================================
        class SignalStructure:
            """ """
            def __init__(self, name):
                self.name = str(name)
                
                self.fragments = [] # [fragment_name, fragment_size, is_reserved]
                
    #===============================================================================================            
    class Signal:
        """ Class to model a CAN signal """
        def __init__(self, name, lenght, description = ""):
            """ Constructor for the class """
            self.name = str(name)
            
            self.len = None
            self.set_data_len(lenght)
            
            self.data_type = None
            self.is_enum_type = False
                    
            self.description = description    
                
            self.init_value = None
            self.fail_value = None
            self.offset = None
            self.resolution = None
            self.unit = None
            self.message = None
            self.start_byte = None
            self.start_bit = None
        
        #===========================================================================================    
        def set_conveyor_message(self, message_name):
            """ Sets conveyor message for this signal """
            if cg.is_valid_identifier(message_name):
                self.message = message_name
            else:
                warnings.warn("Invalid message name: \"" + message_name + \
                  "\". Message names must comply with C-language identifier syntax.", \
                  UserWarning)
        
        #===========================================================================================
        def set_layout_info(self, start_byte, start_bit):
            """ Sets the layout information for the signal """   
            temporal_start_byte = cg.get_valid_number(start_byte)
            if temporal_start_byte is not None:
                self.start_byte = temporal_start_byte
            else:
                warnings.warn("Wrong Start Byte parameter: \"" + str(start_byte) \
                              + "\" for signal \"" + self.name \
                              + "\".", UserWarning)
                
            temporal_start_bit = cg.get_valid_number(start_bit)
            if temporal_start_bit is not None:
                self.start_bit = temporal_start_bit
            else:
                warnings.warn("Wrong Start Byte parameter: \"" + str(start_bit) \
                              + "\" for signal \"" + self.name \
                              + "\".", UserWarning)
        
        #===========================================================================================
        def set_data_len(self, length):
            temporal_len = cg.get_valid_number(length)
            if temporal_len != None and temporal_len > 0 and temporal_len <= 64:
                self.len = temporal_len
            else:
                warnings.warn("Wrong lenght parameter: \"" + str(length) \
                              + "\" for signal \"" + self.name \
                              + "\". No change performed.", UserWarning)
        
        #===========================================================================================
        def is_array(self):
            """ Returns True if signal is of array type.
            """
            return_value = None
            if self.data_type == data_types_list[ARRAY]:
                return_value = True
            else:
                return_value = False
                
            return return_value

#===================================================================================================
class CodeGen():
    """ Class to contain code generation parameters for CAN networks. """
    def __init__(self):
        p = {} # Dictionary of defined parameters for code generation
        
        p.update({"part_px", ["","part_", "ele_"]})
        p.update({"PRFX_DEFAULT", 0})
        pass
    
    # Prefixes to use for signal parts.
    part_px = ["","part_", "ele_"]
    PRFX_DEFAULT = 0


#===================================================================================================
def load_input(input_file, input_type, params):
    """ Loads input file and returns the corresponding object.
    
    This function is invoked from the project handler.
    
    Parameters
    ----------
        input_file : Path
            Path object pointing to the input file for the current project' module.
        input_type : int
            Type of input, can be either IN_TYPE_XML or IN_TYPE_ODS. Those constants are
            defined in the "project" module.
        params : dict
            Dictionary of parameters if required.
    
    Returns
    -------
        Object
        This function returns an object with the loaded data for the current module. For CAN
        Networks this object is of class Network_CAN.
    """
    del input_type, params # Unused parameters
    try:
        return_object = Network_CAN("TestNW") 
        return_object.parse_spreadsheet_ods(str(input_file)) 
    except Exception as e:
        return_object = None
        log_warn('Failed to process input file "%s". Reason: %s' % (input_file, e))
    finally:
        return return_object    
    
    return return_object

#===================================================================================================
def generate(input_object, out_path, working_path, calvos_path, params = {}):
    """ Generate C code for the given object.
    
    Parameters
    ----------
        input_object : Object
            Input object to use for the code generation of this module.
        out_path : Path
            Output path were the generated code is to be placed.
        working_path : Path
            Working path to be used for storage of temporary data used during code
            generation.
        calvos_path : Path
            Path of the calvos python package
        params : dict
            Dictionary of parameters (if required)
    
    Yields
    ------
        This function produced C-code for the corresponding module. This code
        is generated in the provided out_path.
    """
    del params # Unused parameter
    #TODO: Check for required parameters
    cog_files_path = calvos_path / "comgen" / "gen" / "CAN"
    input_object.gen_code(out_path, working_path, cog_files_path)
    
        