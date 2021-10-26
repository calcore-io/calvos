# -*- coding: utf-8 -*-
""" Communication Generator for CAN Network.

calvos.comgen.CAN models a network for Controller Area Network (CAN)
protocol.

@author:     Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.0.1'
__date__ = '2020-08-03'
__updated__ = '2020-12-18'
    
import pyexcel as pe
import math
from lxml import etree as ET
import xml.dom.minidom
import pickle
import cogapp as cog
import pathlib as pl
import importlib
import json
import copy
import canmatrix

import calvos.common.codegen as cg
import calvos.common.logsys as lg
import calvos.common.general as grl

# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "CAN"

log = lg.log_system
log.add_logger(LOGGER_LABEL)

def log_debug(message):
    """ Wrapper for logging a debug message. """
    
    log.debug(LOGGER_LABEL, message)
    
def log_info(message):
    """ Wrapper for logging an info message. """
    
    log.info(LOGGER_LABEL, message)
    
def log_warn(message):
    """ Wrapper for logging a warning message. """
    
    log.warning(LOGGER_LABEL, message)
    
def log_error(message):
    """ Wrapper for logging an error message. """
    
    log.error(LOGGER_LABEL, message)
    
def log_critical(message):
    """ Wrapper for logging a critical error message. """
    
    log.critical(LOGGER_LABEL, message)
# --------------------------------------------------------------------------------------------------

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
# Constants for indicating read or write. Used by function get_signal_abstract_read
READ = 0
WRITE = 1
# Constants for indicating message direction (transmission or reception)
CAN_TX = 0
CAN_RX = 1

# Cog sources to use for code generation of this module
cog_sources = cg.CogSources("comgen.CAN")
    
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
    metadata = {"templateName" : "Network_CAN",
                "templateVersion" : __version__,
                "templateDesc": "Models a CAN Network"}
    
    #===============================================================================================    
    def __init__(self, project_obj, id_string = "", name = "", description = "", \
                 version = "", date = ""):
        """ Class constructor.
        
        Parameters
        ----------
            id : str
                Identifier for the network. Shall be short since it will be used
                as part of the name for various functions/macros in code generation
            name : str, optional
                Name of the network
            description : str, optional
                Description of the network
            version : str, optional
                Version of the network (user defined)
            date : str, optional
                Date corresponding to the version of the network (user defined)
         """
        self.id_string = id_string
        
        self.name = str(name)
        
        self.description = str(description)
        
        self.version = str(version)

        self.date = str(date)
        
        self.project_obj = project_obj
        
        self.module = "comgen.CAN"

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
        
        self.simple_params = {} # Non-default params for this object {param_id:SimpleParam obj, ...}
        
        # Last input file used to fill-out data for this object
        self.input_file = None
        
#        self.gen_params = {}
        
        self.subnetwork = None # Expect object of class Network_CAN
        
        self.gen_path = self.project_obj.get_component_gen_path(self.module)
           
    #===============================================================================================
    @staticmethod
    def split_signal(abs_start_bit, abs_end_bit, \
        nominal_type_len = 8, leading_type_len = None, trailing_type_len = None):
        """ Fragments a signal based on its layout information.
        
        Fragmentation of a signal may occur if signal crosses "type length" 
        boundaries.
        Signal could have a leading, nominal or tail sets of parts/fragments.
          - Leading fragments: fragments based on signal absolute bit position 
            and splitted according to the defined "trailing_type_len" length.
          - Nominal fragments: fragments based on signal absolute bit position
            and splitted according to the defined "nominal_type_len" length.
          - Tailing fragments: fragments based on signal absolute bit position
            and splitted according to the defined "trailing_type_len" length.
            
        Parameters
        ----------
            abs_start_bit : int
              The absolute start bit of a signal as per its layout definition
              of the signal's conveyor message.
            abs_end_bit : int
              The absolute end bit occupied by the signal as per its layout 
              definition of the signal's conveyor message.    
            nominal_type_len : int, optional
              Type length used to fragment the signal into its nominal parts.
              If no value is passed, will assume nominal_type_len as 8 bits.
            leading_type_len : int, optional
              Type length used to fragment the signal into its leading parts.
              If no value is passed, leading_type_len will assume same value
              as nominal_type_len.
            trailing_type_len : int, optional
              Type length used to fragment the signal into its leading parts.
              If no value is passed, trailing_type_len will assume same value
              as nominal_type_len.
       
        Returns
        -------
        list
            Will return a list (return_list) of lists with following format:
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
            
            Each element xxxx_part is a number conveying the length in bits 
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
            log_warn(('End bit shall be greater than or equal to the start bit.' \
                    + ' Current start bit: "%s", current end bit: "%s"') \
                    % (str(abs_start_bit), str(abs_end_bit)) )
        
        return_list.append(leading_list)
        return_list.append(nominal_list)
        return_list.append(trailing_list)
        
        return return_list

    #===============================================================================================
    def set_id(self, id_string):
        """ Sets the network identifier.
        Parameters
        ----------
            id_string : str
                String to be written to the Network id.
        
        Yields
        ------
            Throws a warning if the passed id_string argument doesn't comply to
            C-language identifier syntax.
        """
        if cg.is_valid_identifier(id) is True:
            self.id_string = id_string
        else:
            log_warn(("Network identifier '%s' is invalid. It shall comply with "
                      + "language identifiers syntax. Network id not modified.")
                      % id)
    
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
                    log_warn(("Enum type: \"" + name + \
                          "\" not well formed. Removed from the network."))    
            else:
                log_warn(("Duplicated Enum Type: \"" + name + \
                          "\" not added to the network."))
        else:
            #Warning, invalid node name
            log_warn(("Invalid Enum Type: \"" + name + \
                          "\". Enum Types must comply with C-language identifier syntax."))
        
    #===============================================================================================
    def remove_enum_type(self, name):
        """ Removes the specified node from the network.
        
        Parameters
        ----------
            name : str
                Name of the node to be removed.
        """
        if name not in self.nodes:
            #Node doesn't exist
            log_warn(("Enum Type \"" + name + "\" doesn't exist"))
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
                log_warn(("Duplicated Node: \"" + name + \
                    "\" not added to the network."))
        else:
            #Warning, invalid node name
            log_warn(("Invalid node name: \"" + name + \
                "\". Node names must comply with C-language identifier syntax."))
    
    #=============================================================================================== 
    def remove_node(self, name):
        """ Removes the specified node from the network. """
        if name in self.nodes:
            self.nodes.pop(name)
        else:
            #Node doesn't exist
            log_warn(("Node \"" + name + "\" doesn't exist"))
    
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
                        log_warn(("Duplicated Message Id: \"" + str(msg_id) \
                              + "\". Message \"" + name + "\" not added to the network."))
                else:
                    log_warn(("Invalid Message Id: \"" + str(msg_id) \
                          + "\". Message \"" + name + "\" not added to the network."))
            else:
                log_warn(("Duplicated Message: \"" + name + \
                          "\" not added to the network."))
        else:
            #Warning, invalid message name
            log_warn(("Invalid Message name: \"" + name + \
                          "\". Message names must comply with C-language identifier syntax."))
    
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
                    log_warn(("Message: \"" + message_name \
                                  + "\" already assigned for TX to node: \"" \
                                  + self.messages[message_name].get_publisher() \
                                  + "\". No change performed."))
            else:
                log_warn(("Message: \"" + message_name \
                          + "\" not defined. Define first the message in the network"))
        else:
            log_warn(("Node: \"" + node_name \
                          + "\" not defined. Define first the node in the network"))
    
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
                            log_warn(("Message \"" + message_name + \
                                      "\" already published by node \"" + node \
                                      + "\", message can't be published " \
                                      + "and subscribed by same node."))       
                    else:
                        log_warn(("Node \"" + node + "\" not defined."))
        else:
            log_warn(("Message \"" + message_name + "\" not defined."))
    
    #===============================================================================================
    def add_rx_messages_to_node(self, node_name, message_name, timeout_ms):
        """ . """
        #TODO: This function has not been tested
        if node_name in self.nodes:
            if message_name in self.messages:
                #Message exists, add subscriber to Node
                self.nodes[node_name].add_subscriber(message_name,timeout_ms)
            else:
                log_warn(("Message \"" + message_name + "\" not defined."))
        else:
            log_warn(("Node \"" + node_name + "\" not defined."))

    #===============================================================================================
    def get_message_direction(self, node_name, message_name):
        """ Gets the direction ('Tx', 'Rx' or None) for the given node. """
        return_value = None
        if node_name in self.nodes and message_name in self.messages:
            if self.messages[message_name].publisher == node_name:
                # This message is a TX message for the given node
                return_value = CAN_TX
            elif message_name in self.nodes[node_name].subscribed_messages:
                # This message is an RX message for the given node
                return_value = CAN_RX
        return return_value
    
    #===============================================================================================
    def get_message_timeout(self, node_name, message_name):
        """ Gets the timeout for the given message w.r.t. a given node. """
        return_value = None
        if node_name in self.nodes and message_name in self.messages:
            if self.get_message_direction(node_name, message_name) == CAN_RX:
                if self.nodes[node_name].subscribed_messages[message_name] is not None:
                    return_value = self.nodes[node_name].subscribed_messages[message_name]
        return return_value

    #===============================================================================================
    def get_messages_of_node(self, node_name):
        """ Gets the messages names for the given node. """
        return_value = [] # List of node names
        if node_name in self.nodes:
            for message in self.messages.values():
                if message.publisher == node_name:
                    return_value.append(message.name)
            for message_name in self.nodes[node_name].subscribed_messages.keys():
                return_value.append(message_name)  
        return return_value 
    
    #===============================================================================================
    def get_message_subscribers(self, message_name):
        """ Gets the subscribers names of the given message. """
        return_value = [] # List of node names
        if message_name in self.messages:
            for node in self.nodes.values():
                if message_name in node.subscribed_messages:
                    return_value.append(node.name) 
        return return_value          
    
    #===============================================================================================
    def get_signals_of_message(self, message_name):
        """ Returns a list of signals objects mapped to the given message.
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
        # TODO: Change definition? To add that cannonical also need signals to match
        # native data types, e.g., a 3 bytes signal is NOT cannonical
        # Message layout is considered cannonical if:
        # - All signals have a size which is multiple of a byte, and
        # - All signals absolute start bit is multiple of a byte
        if message_name in self.messages:
            signals = self.get_signals_of_message(message_name)
            if signals is not None:
                for signal in signals:
                    
                    if self.signal_layout_is_cannonical(signal.name) is False:
                        is_cannonical = False
                        break
            else:
                #TODO: warning, message has no mapped signals
                log_warn("Message '%s' has no mapped signals." % message_name)
                is_cannonical = None
        else:
            log_warn("Message '%s' is not defined in network id '%s'" 
                     % (message_name, self.id_string))
        return is_cannonical
            
    
    #===============================================================================================
    def message_has_init_values(self, message_name):
        """ Returns True if any signal of the given message has a defined initial value.
        """
        
        return_value = False 
        signals = self.get_signals_of_message(message_name)
        for signal in signals:
            if signal.init_value is not None:
                return_value = True
                break
        
        return return_value
        
    #===============================================================================================
    def message_has_fail_values(self, message_name):
        """ Returns True if any signal of the given message has a defined fail-safe value.
        """
        
        return_value = False 
        signals = self.get_signals_of_message(message_name)
        for signal in signals:
            if signal.fail_value is not None:
                return_value = True
                break
        
        return return_value
    
    #===============================================================================================
    def get_signals_with_init_values(self, message_name):
        """ Returns a list of signal objects for a given message that have defined initial values.
        """
        
        return_value = []
        signals = self.get_signals_of_message(message_name)
        for signal in signals:
            if signal.init_value is not None:
                return_value.append(signal)
        
        return return_value
    
    #===============================================================================================
    def get_signals_with_fail_values(self, message_name):
        """ Returns a list of signal objects for a given message that have defined fail-safe values.
        """
        
        return_value = []
        signals = self.get_signals_of_message(message_name)
        for signal in signals:
            if signal.fail_value is not None:
                return_value.append(signal)
        
        return return_value
        
    #===============================================================================================
    def get_messages_structures(self):
        """ Gets layout structure of all messages.
        
        Performs signal fragmentation as needed in order to produce a message layout structure
        ready to be used for corresponding C-code generation.
        
        return
        ------
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
                
                # Calculate base data type length to use for the bitfield of
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
                    #TODO: warning. Wrong length of message message_name
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
                        # Get start bit and length of next signal
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
                                    fragment_name = str(signal_name) + "_" \
                                        + prefix + str(array_byte)
                                    
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
                                                    fragment_name = str(signal_name) + "_" \
                                                        + prefix + str(fragment_counter)
                                                    
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
                log_warn(("Duplicated Signal: \"" + name + \
                          "\" not added to the network."))
        else:
            #Warning, invalid signal name
            log_warn(("Invalid signal name: \"" + name + \
                      "\". Signal names must comply with C-language identifier syntax."))
    
    #===============================================================================================        
    def add_signal_to_message(self, message_name, signal_name, start_byte, start_bit):
        """ Adds a signal to a message with the speficied layout information. 
        If soace in the target message is available, then the given signal is added with its
        layout information.
        """
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
                        log_warn(("Signal \"" + signal_name \
                                      + "\" doesn't fit in message \"" \
                                      + message_name + "\"."))
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
                                    log_warn(("Signal \"" + signal_name \
                                      + "\" overlaps with signal \"" \
                                      + signal.name + "\"."))
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
                                log_warn(("Signal \"" + signal_name \
                                      + "\" is of array type and its start bit " \
                                      + "is not byte aligned or its lenght " \
                                      + "is not a multiple of 8-bits (1 byte)." \
                                      + "Signal not added to the network."))
                        else:
                            log_warn(("Signal \"" + signal_name \
                                      + "\" Can't be added to message \"" \
                                      + message_name + "\" with current layout."))
                else:
                    log_warn(("Wrong paratemer(s) for start_byte/start_bit/signal_lenght :" \
                                 + str(start_byte) + "/" + str(start_bit) + "/" \
                                 + str(self.signals[signal_name].len) \
                                 + " for signal \"" + signal_name + "\".")) 
            else:
                log_warn(("Signal \"" + signal_name + "\" not defined."))
        else:
            log_warn(("Message \"" + message_name + "\" associated to signal \"" \
                      + signal_name + "\" is not defined."))
            
    #===============================================================================================    
    def set_signal_data_type(self, signal_name, data_type):
        """ Sets the data type of a signal. """
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
                    log_warn(("Undefined signal type: \"" + data_type + "\" for signal \"" \
                              + signal_name + "\". Type not assigned to the signal."))
    
    #===============================================================================================
    def get_valid_signal_value(self, signal_name, value):
        """ Returns the proper value if the provided signal value is valid. Returns None otherwise.
        
        """
        return_value = None
        
        value = str(value)
        sig_data_type = str(self.signals[signal_name].data_type)
        if self.signals[signal_name].is_enum_type is True:
            # Check if provided value belongs to the enum type
            if sig_data_type in self.enum_types:
                if value in self.enum_types[sig_data_type].enum_entries:
                    # Valid value
                    return_value = value
                else:
                    log_debug(("Invalid value '"+value+"' for signal '" +signal_name\
                              +"' of enum type '"+sig_data_type+"'."))
            else:
                log_debug(("Invalid data type '"+sig_data_type+"' for signal '"+signal_name+"'."))
        elif self.signals[signal_name].is_array():
            # For array signals, only a byte value is allowed for checking.
            # Such value will be assumed for all array signal bytes (init, fail-safe values)
            sig_data_value = cg.get_valid_number(value)
            if sig_data_value is not None:
                if sig_data_value <= 255:
                    # Valid value
                    return_value = sig_data_value
                else:
                    log_debug(("Invalid signal value '"+value \
                         + "' for signal '"+signal_name+"' of type '" \
                         + sig_data_type + "'. Value for array signals " \
                         + "shall not exceed 1 byte (255 dec or 0xFF hex). "))
            else:
                log_debug(("Invalid signal value '"+value \
                         + "' for signal '"+signal_name+"' of type '" \
                         + sig_data_type + "'. Value for array signals " \
                         + "shall be a decimal or hexadecimal number not " \
                         + "exceeding 1 byte (255 dec or 0xFF hex). "))          
        elif self.signals[signal_name].is_scalar():
            sig_data_value = cg.get_valid_number(value)
            if sig_data_value is not None:
                if sig_data_value > 0:
                    value_len = math.log(sig_data_value,2)
                else:
                    value_len = 0
                if self.signals[signal_name].len >= value_len:
                    # Valid value
                    return_value = sig_data_value
                else:
                    log_debug(("Invalid signal value '" + value + "' for signal '" + signal_name \
                             + "' of type '"+sig_data_type + "'. Value exceeds signal's length."))
            else:
                log_debug(("Invalid signal value '" + value + "' for signal '" + signal_name \
                          + "' of type '" + sig_data_type + "'. Value for scalar signals " \
                             + "shall be a decimal or hexadecimal number not " \
                             + "exceeding the signal's length."))
                
        return return_value
    
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
        """ Sorts the signals in this network according to their layout.
        
        Sorting order is as follows: sort by message name, then by start byte, then by start bit.
        """
        return_list = {}
        sorted_signals_list = []
        
        # If input signals has elements do the sorting, otherwise return empty output.
        if len(self.signals) > 0:
            # Create working list of signals
            for signal in self.signals:
                if self.signals[signal].message is not None:
                    sorted_signals_list.append({signal : self.signals[signal]})
                else:
                    log_warn(("Signal: \"" + signal \
                            + "\" doesn't have a publishing message."))
                
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
                    signals_of_a_message = \
                        sorted(signals_of_a_message, \
                               key= lambda signal_entry : \
                               list(signal_entry.values())[0].start_byte)
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
    def signal_layout_is_cannonical(self, signal_name):
        """ Returns true if signal layout is cannonical.
        """
        # TODO: Change input argument from "signal_name" to "signaL_object" same for other 
        # similar functions...
        is_cannonical = True
        
        if signal_name in self.signals:
            signal = self.signals[signal_name]
            abs_start_bit = (signal.start_byte * 8) + signal.start_bit
            if (signal.len % 8 != 0) or (abs_start_bit % 8 != 0):
                # Signal doesn't start at a bit position multiple of 8
                # or its size is not multiple of 8.
                is_cannonical = False
        else:
            log_warn("Signal '%s' not defined in network with id '%s'"
                     % (signal_name, self.id_string))
        
        return is_cannonical  

    #===============================================================================================                
    def get_signal_abstract_read(self, signal_name):
        """ Gets an abstract access to the signal for code generation. """
        return_value = None
        
        if signal_name in self.signals:
            signal = self.signals[signal_name]
            
            signal_access = Network_CAN.SignalAccess(signal_name, signal.len)
            
            base_len = cg.calculate_base_type_len(signal.len)
            
            if base_len == 8:
                # A single piece is enough for this signal
                signal_piece = Network_CAN.SignalAccess.PieceAccess()
                # Inner shifting (left shifting) is equal to the signal's start bit
                if signal.start_bit > 0:
                    signal_piece.shift_inner = signal.start_bit
                signal_piece.len = base_len
                # Generate mask if required for removing most-significant bits
                # not belonging to the signal.
                if signal.len < 8:
                    mask_msb = int(255 << signal.len).to_bytes(8,'big')
                    mask_msb = hex(mask_msb[-1] ^ 255)
                    signal_piece.mask_outer =  mask_msb
                # No outer shifting / inner masking is required in this case
                signal_piece.abs_byte = signal.start_byte
                signal_access.pieces.append(signal_piece)
                
            elif base_len <= cg.Compiler_max_size:
                abs_start_bit = (signal.start_byte*8)  + signal.start_bit
                abs_end_bit = abs_start_bit + signal.len - 1
    
                abs_start_byte = signal.start_byte
                abs_end_byte = int(math.floor(abs_end_bit / 8))
    
                bytes_remaining = abs_end_byte - abs_start_byte + 1
                bits_remaining = signal.len
    
                current_byte = abs_start_byte
                
                part_base_len = cg.calculate_base_type_len(bytes_remaining*8)
                shifting_bits = 0
                first_chunk = True

                while (part_base_len / 8) > bytes_remaining:
                    # Signal doesn't fit in a single data type so multiple
                    # parts for accessing it are needed.
                    signal_piece = Network_CAN.SignalAccess.PieceAccess()
                    # Get next signal piece with a smaller data size than previously
                    part_base_len = int(part_base_len / 2)
                    # TODO: Is it ensured that reduced_type_len is never
                    # less than 8?
                    signal_piece.abs_byte = current_byte
                    signal_piece.len = part_base_len

                    if first_chunk is True and signal.start_bit > 0:
                        # If signal doesn't start at bit zero then
                        # right shifting is required.
                        signal_piece.shift_inner = signal.start_bit

                    if shifting_bits > 0:
                        signal_piece.shift_outer = shifting_bits

                    signal_access.pieces.append(signal_piece)

                    current_byte += int(part_base_len/8)
                    bytes_remaining -= int(part_base_len/8)

                    bits_remaining -= part_base_len
                    if first_chunk is True:
                        # For first chunk, bits before start_bit don't
                        # contribute to the remaining bits so need to be
                        # added back.
                        bits_remaining += signal.start_bit

                    shifting_bits += (part_base_len - signal.start_bit)

                    part_base_len = \
                        cg.calculate_base_type_len(bytes_remaining * 8)

                    first_chunk = False
                else:
                    signal_piece = Network_CAN.SignalAccess.PieceAccess()
                    
                    signal_piece.abs_byte = current_byte
                    signal_piece.len = part_base_len

                    if first_chunk is True and signal.start_bit > 0:
                        # If signal doesn't start at bit zero then
                        # right shifting is required.
                        signal_piece.shift_inner = signal.start_bit

                    if shifting_bits > 0:
                        signal_piece.shift_outer = shifting_bits

                    mask_bits = bits_remaining
                    if mask_bits < part_base_len:
                        # Some most-significant bits need to be masked-out
                        mask_msb = 0
                        for i in range(mask_bits):
                            mask_msb = mask_msb | (1 << i)
                        signal_piece.mask_outer =  mask_msb
                    
                    signal_access.pieces.append(signal_piece)            
            else:
                # Signal length exceeds maximum compiler data type
                log_warn(("Length of scalar signal '" + signal_name \
                          + "' exceeds maximum compiler data size. " +
                         " Consider defining the signal as an array instead."))
                
            return_value = signal_access
            
        else:
            log_warn("Signal '%s' is not defined." % signal_name)
        
        return return_value
    
    #===============================================================================================                
    def get_signal_abstract_write(self, signal_name):
        """ Gets an abstract access to the signal for code generation. """
        return_value = None
        
        if signal_name in self.signals:
            signal = self.signals[signal_name]
            
            signal_access = Network_CAN.SignalAccess(signal_name, signal.len)
            
            base_len = cg.calculate_base_type_len(signal.len)
            
            if base_len <= cg.Compiler_max_size:
                abs_start_bit = (signal.start_byte*8)  + signal.start_bit
                abs_end_bit = abs_start_bit + signal.len - 1
    
                abs_start_byte = signal.start_byte
                abs_end_byte = int(math.floor(abs_end_bit / 8))
    
                bytes_remaining = abs_end_byte - abs_start_byte + 1
                bits_remaining = signal.len
    
                current_byte = abs_start_byte
                
                part_base_len = cg.calculate_base_type_len(bytes_remaining*8)
                first_chunk = True

                while (part_base_len / 8) > bytes_remaining:
                    # Signal doesn't fit in a single data type so multiple
                    # parts for accessing it are needed.
                    signal_piece = Network_CAN.SignalAccess.PieceAccess()
                    # Get next signal piece with a smaller data size than previously
                    part_base_len = int(part_base_len / 2)
                    # TODO: Is it ensured that reduced_type_len is never
                    # less than 8?
                    signal_piece.abs_byte = current_byte
                    signal_piece.len = part_base_len  
                    
                    if first_chunk is True:
                        # Calculate clearing mask and shifting bits for first chunk...
                        if signal.start_bit > 0:
                            # Shifting is not required if start_bit is zero.
                            signal_piece.shift_inner = signal.start_bit
                            # Masking is not required if start_bit is zero since all bits
                            # of the buffer need to be overwritten
                            signal_piece.mask_inner = \
                                cg.get_bit_mask(part_base_len - signal.start_bit, \
                                                signal.start_bit, True, part_base_len)
                        # Calculate data bits mask for first chunk
                        signal_piece.mask_outer = \
                            cg.get_bit_mask(part_base_len - signal.start_bit, \
                                            signal.start_bit, False, part_base_len)
                    else:
                        # For subsequent chunks (except last one that will be treated in the
                        # "else" statement of this while statement). Shifting will be right
                        # shifting and mask should be of part_base_len.
                        # shift_bits = signal.len - bits_remaining
                        # Consume the bits from the input data (right shifting of consumed bits).
                        signal_piece.shift_inner = signal.len - bits_remaining
                        # Mask out bits not belonging to this piece
                        signal_piece.mask_outer = \
                            cg.get_bit_mask(part_base_len, 0, False, part_base_len)

                    bits_remaining -= part_base_len
                    if first_chunk is True:
                        # For first chunk, bits before start_bit don't
                        # contribute to the remaining bits so need to be
                        # added back.
                        bits_remaining += signal.start_bit
                        

                    signal_access.pieces.append(signal_piece)

                    current_byte += int(part_base_len/8)
                    bytes_remaining -= int(part_base_len/8)

                    part_base_len = \
                        cg.calculate_base_type_len(bytes_remaining * 8)

                    first_chunk = False
                else:
                    # Flow goes here in two cases. If first_chunk is True in here it means that
                    # only one piece is required for this signal, if first_chunk is False here
                    # it means it is the last piece of a multi-piece signal.
                    signal_piece = Network_CAN.SignalAccess.PieceAccess()
                    
                    signal_piece.abs_byte = current_byte
                    signal_piece.len = part_base_len
                    
                    if first_chunk is True:
                        first_chunk = False
                        # Calculate shifting for first chunk (left shifting)
                        if signal.start_bit > 0:
                            signal_piece.shift_inner = signal.start_bit
                        # Calculate clearing bits mask for first chunk
                        if signal.len < part_base_len:
                            # Masks make sense if the signal len is smaller than the part_base_len
                            # otherwise mask of all zeros and all ones will be generated which is
                            # not optimal.
                            signal_piece.mask_inner = \
                                cg.get_bit_mask(signal.len, signal.start_bit, \
                                                True, part_base_len)
                            # Calculate data bits mask for first chunk
                            signal_piece.mask_outer = \
                                cg.get_bit_mask(signal.len, signal.start_bit, \
                                                False, part_base_len)
                    else:
                        # For last chunk (in a multi-chunk data) shifting will be right
                        # shifting and mask should be of bits_remaining.
                        # Consume the bits from the input data (right shifting of consumed bits).
                        if signal.len - bits_remaining > 0:
                            signal_piece.shift_inner = signal.len - bits_remaining
                        # Calculate mask for clearing bits in target location
                        if bits_remaining < part_base_len:
                            signal_piece.mask_inner = \
                                cg.get_bit_mask(bits_remaining, 0, True, part_base_len)
                        # Mask out bits not belonging to this piece
                        signal_piece.mask_outer = \
                            cg.get_bit_mask(bits_remaining, 0, False, part_base_len)         
                    
                    signal_access.pieces.append(signal_piece)            
            else:
                # Signal length exceeds maximum compiler data type
                log_warn(("Length of scalar signal '" + signal_name \
                          + "' exceeds maximum compiler data size '" + cg.Compiler_max_size \
                         + "'. Consider defining the signal as an array instead."))
            
            return_value = signal_access
            
        else:
            log_warn("Signal '%s' is not defined." % signal_name)
        
        return return_value
            
    #=============================================================================================== 
    #TODO: check usage and implementation of function "fragment_signal"
    def fragment_signal(self, signal_name, abs_start_bit, abs_end_bit, \
            nominal_type_len = 8, leading_type_len = None, trailing_type_len = None):
        """ Fragments a signal based on its size and layout information.
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
        
        XML_comment = ET.Comment('CAN Network description for calvOS system')
        XML_root.addprevious(XML_comment)
        XML_root.insert(1, XML_comment)
        
        XML_level_1 = ET.Element("Name")
        XML_level_1.text = self.name
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Id")
        XML_level_1.text = self.id_string
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Desc")
        XML_level_1.text = self.description
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Version")
        XML_level_1.text = str(self.version)
        XML_root.append(XML_level_1)
        XML_level_1 = ET.Element("Date")
        XML_level_1.text = str(self.date)
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
        
        for data_name, data_value in Network_CAN.metadata.items():
            XML_level_2 = ET.SubElement(XML_level_1,data_name)
            XML_level_2.text = data_value
            
        XML_root.append(XML_level_1)
        
        print("INFO: Generating CAN Network XML...")
        XML_string = ET.tostring(XML_root)
        
        XML_string = xml.dom.minidom.parseString(XML_string)
        XML_string = XML_string.toprettyxml()

        if output_file is None:
            print(XML_string)
        else:
            myfile = open(output_file, "w")
            myfile.write(XML_string)
            print("INFO: XML generation done")
        #TODO: save XML to file if output_file is different than None
    
    #===============================================================================================    
    def gen_DBC(self, output_file = None, gen_types = True, gen_nodes = True, \
                 gen_messages = True, gen_signals = True):
        """ Generates a DBC filedescribing the Network modeled by this object. """
        
        tmp_output_file = output_file.with_suffix(".dbctmp")
        
        can_dbc = canmatrix.CanMatrix()
        
        # Add attributes
        can_dbc.add_frame_defines("GenMsgSendType",'ENUM "cyclic","triggered","cyclicIfActive","cyclicAndTriggered","cyclicIfActiveAndTriggered","none"')
        can_dbc.frame_defines["GenMsgSendType"].set_default("none")
     
        if gen_nodes is True:
            #Generate nodes
            for node in self.nodes.values():
                ecu_dbc = canmatrix.Ecu(node.name, node.description)
                can_dbc.add_ecu(ecu_dbc)
    
        if gen_signals is True:
            
            local_dbc_signals = []
            
            for signal in self.signals.values():
                
                dbc_signal = canmatrix.Signal(name = signal.name, 
                                              start_bit = signal.get_abs_start_bit(),
                                              size = signal.len)
                if signal.description is not None:
                    dbc_signal.add_comment(signal.description)
                    
                # For now only supporting unsigned signals
                dbc_signal.is_signed = False
                
                # Add enumeration if defined
                if signal.is_enum_type is True:
                    if signal.data_type in self.enum_types:
                        signal_enum = self.enum_types[signal.data_type]
                        resolved_enum = signal_enum.get_resolved_values()
                        for entry in resolved_enum.enum_entries:
                            number = str(resolved_enum.enum_entries[entry])
                            val_name = entry
                            dbc_signal.add_values(number, val_name)
                        # Add initial value
                        if signal.init_value is not None:
                            dbc_signal.initial_value = \
                                int(resolved_enum.enum_entries[str(signal.init_value)])
                        
                        # Add min and max
#                         dbc_signal.set_min(signal_enum.get_min())
#                         dbc_signal.set_max(signal_enum.get_max()) 
                        dbc_signal.set_min()
                        dbc_signal.set_max()
                        
                elif signal.is_array() is True:
                    # Add initial value
                    if signal.init_value is not None:
                        init_val_array = 0
                        for i in range(int(signal.len / 8)):
                            factor = pow(2,i*8)
                            init_val_array += signal.init_value * factor
                        dbc_signal.initial_value = int(init_val_array)
                        
                    dbc_signal.set_min()
                    dbc_signal.set_max()
                else:
                    # Add initial value
                    if signal.init_value is not None:
                        dbc_signal.initial_value = int(signal.init_value)
                    
                    dbc_signal.set_min()
                    dbc_signal.set_max()
                    
                local_dbc_signals.append(dbc_signal)
        
        if gen_messages is True:
            for message in self.messages.values():
                
                dbc_message = canmatrix.Frame(name = message.name, arbitration_id = message.id,
                                              size = message.len)
                
                if message.get_publisher() is not None:
                    dbc_message.add_transmitter(message.get_publisher())
                
                if message.description is not None:
                    dbc_message.add_comment(message.description)
                
                # Get signals for this message
                dbc_signals = []
                signals = self.get_signals_of_message(message.name)
                # Get canmatrix signal object
                for signal in signals:
                    for signal_obj in local_dbc_signals:
                        if signal_obj.name == signal.name:
                            dbc_signals.append(signal_obj)
                            break
                        
                for dbc_signal in dbc_signals:
                    dbc_message.add_signal(dbc_signal)
                    
                # Get message subscribers
                msg_subscribers = self.get_message_subscribers(message.name)
                for subscriber in msg_subscribers:
                    dbc_message.add_receiver(subscriber)
                    # Add all signals to the same receiver.
                    # Currently, calvos doesn't support partial reception of signals of messages
                    for dbc_signal in dbc_message.signals:
                        dbc_signal.add_receiver(subscriber)
                    

                # Set tx type
                if message.tx_type == tx_type_list[SPONTAN]:
                    dbc_message.add_attribute("GenMsgSendType", "triggered")
                elif message.tx_type == tx_type_list[CYCLIC]:
                    dbc_message.add_attribute("GenMsgSendType", "cyclic")
                elif message.tx_type == tx_type_list[CYCLIC_SPONTAN]:
                    dbc_message.add_attribute("GenMsgSendType", "cyclicAndTriggered")
                elif message.tx_type == tx_type_list[BAF]:
                    dbc_message.add_attribute("GenMsgSendType", "cyclicIfActive")
                else:
                    dbc_message.add_attribute("GenMsgSendType", "none")
                
                # Set cycle time
                if message.tx_period is not None:
                    dbc_message.cycle_time = message.tx_period
                
                # Add frame to dbc
                can_dbc.add_frame(dbc_message)
        
        if gen_messages is True:
            #Generate nodes
            for message in self.messages.values():
                ecu_dbc = canmatrix.Ecu(node.name, node.description)
                can_dbc.add_ecu(ecu_dbc)     

         
        if output_file is not None:
            with open(tmp_output_file, 'wb') as fout:
                canmatrix.formats.dump(can_dbc, fout, "dbc")
            # Update "VERSION" field in the DBC
            try:
                version_str = "VERSION \"Network: '" + str(self.id_string) + "', version: '" \
                    + str(self.version) + "', Date: '" + str(self.date) +"'\""
            except Exception as e:
                print('Failed to create DBC VERSION string. Reason: %s' % e)
                version_str = ""

            with open(tmp_output_file, "r") as fin, open(output_file, "w") as fout:
                fout.write(version_str) 
                for i, line_str in enumerate(fin):
                    if i > 0:
                        fout.write(line_str)
                        
            # Remove temp file
            cg.delete_file(tmp_output_file)
            
            print("INFO: DBC generation done")
        
    #===============================================================================================        
    def update_cog_sources(self):
        """ Updates the list of cog file(s) for this module. """
        
        global cog_sources
        # Get location of the cog files      
        gen_path_lst = __name__.split(".")
        module_name = gen_path_lst[-1]
        #gen_path_val = calvos_path
        gen_path_val = pl.Path()
        for i, element in enumerate(gen_path_lst):
            if i > 0 and i < len(gen_path_lst) - 1:
                gen_path_val = gen_path_val / element
        gen_path_val = gen_path_val / "gen" / module_name
        
        cog_sources = cg.CogSources(module_name, gen_path = gen_path_val)
        
        self.add_cog_source("common_h", "cog_comgen_CAN_common.h", True, \
                            None, {"category" : "common"})
        self.add_cog_source("common_c", "cog_comgen_CAN_common.c", False, \
                            [["comgen.CAN", "common_h"]], \
                            {"category" : "common"})
        self.add_cog_source("network_h", "cog_comgen_CAN_NWID_network.h", True, \
                            None, {"category" : "network"})
        self.add_cog_source("node_net_h", "cog_comgen_CAN_NWID_NODEID_node_network.h", True, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "network_h"]], \
                            {"category" : "node"})
        self.add_cog_source("network_masks_h", "cog_comgen_CAN_NWID_network_masks.h", True, \
                            [["comgen.CAN", "network_h"]], \
                            {"category" : "network"})
        self.add_cog_source("nw_includes_h", "cog_comgen_CAN_NWID_includes.h", True, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "network_h"], \
                             ["comgen.CAN", "network_masks_h"], \
                             ["comgen.CAN", "can_node_hal_h"], \
                             ["comgen.CAN", "node_net_h"], \
                             ["comgen.CAN", "core_h"], \
                             ["comgen.CAN", "callbacks_h"]], \
                            {"category" : "network"})
        self.add_cog_source("can_node_hal_h", "cog_comgen_CAN_NWID_NODEID_hal.h", True, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "network_h"]], \
                            {"category" : "node"}, \
                            user_code=True)
        self.add_cog_source("can_node_hal_c", "cog_comgen_CAN_NWID_NODEID_hal.c", False, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "can_node_hal_h"], \
                             ["comgen.CAN", "core_h"]], \
                            {"category" : "node"}, \
                            user_code=True)
        self.add_cog_source("node_net_c", "cog_comgen_CAN_NWID_NODEID_node_network.c", False, \
                            [["comgen.CAN", "network_h"],
                             ["comgen.CAN", "node_net_h"],
                             ["comgen.CAN", "core_h"]], \
                            {"category" : "node"})
        self.add_cog_source("core_h", "cog_comgen_CAN_NWID_NODEID_core.h", True, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "network_h"], \
                             ["comgen.CAN", "node_net_h"]], \
                            {"category" : "node"})
        self.add_cog_source("core", "cog_comgen_CAN_NWID_NODEID_core.c", False, \
                            [["comgen.CAN", "core_h"], \
                             ["comgen.CAN", "network_h"], \
                             ["comgen.CAN", "network_masks_h"], \
                             ["comgen.CAN", "callbacks_h"], \
                             ["comgen.CAN","can_node_hal_h"]], \
                            {"category" : "node"})
        self.add_cog_source("callbacks_h", "cog_comgen_CAN_NWID_NODEID_callbacks.h", True, \
                            [["comgen.CAN", "common_h"], \
                             ["comgen.CAN", "node_net_h"]], \
                            {"category" : "node"}, \
                            user_code=True)
        self.add_cog_source("callbacks_c", "cog_comgen_CAN_NWID_NODEID_callbacks.c", False, \
                            [["comgen.CAN", "callbacks_h"]], \
                            {"category" : "node"}, \
                            user_code=True)

    #===============================================================================================    
    def get_cog_includes(self, source_id): 
        """ Returns a list of strings to be used with #include 'string'. """
        
        global cog_sources 
        return_str = []
        if source_id in cog_sources.sources:
            for relation in cog_sources.sources[source_id].relations:
                target_module = importlib.import_module("calvos."+relation.module)
                target_srcs = target_module.cog_sources
                if relation.source_id in target_srcs.sources:
                    return_str.append(target_srcs.sources[relation.source_id].cog_out_file)
                else:
                    log_warn("Include file id '%s' of module '%s' not found." \
                             % (source_id, "calvos."+relation.module))
        else:
            log_warn("Include file id '%s' for CAN module not defined.")
        
        return return_str
    
    #===============================================================================================    
    def get_cog_includes_objects(self, source_id): 
        """ Returns a list of CogSrc objects related to the given source_id code. """
        
        global cog_sources 
        return_obj = []
        if source_id in cog_sources.sources:
            for relation in cog_sources.sources[source_id].relations:
                target_module = importlib.import_module("calvos."+relation.module)
                target_srcs = target_module.cog_sources
                if relation.source_id in target_srcs.sources:
                    return_obj.append(target_srcs.sources[relation.source_id])
                else:
                    log_warn("Include file id '%s' of module '%s' not found." \
                             % (source_id, "calvos."+relation.module))
        else:
            log_warn("Include file id '%s' for CAN module not defined.")
        
        return return_obj

    #===============================================================================================    
    def add_cog_source(self, cog_id, cog_in_file, is_header = False, relations = None, \
                       dparams = {}, ** kwargs):
        """ Adds a cog source to this network object's cog files list. 
        
        Parameters
        ----------
            relations, list
                Should be a list of lists to model the relation of cog_id source file with other
                source files... expected -> [[module2, source_id1], [modul2, source_id2], ...]
        """ 
        
        global cog_sources
        
        _user_code = kwargs.get('user_code', False)
        # Create source object
        source_obj = cg.CogSources.CogSrc(\
                cog_id,cog_in_file, None, is_header, [], dparams, user_code=_user_code)
        
        # Determine cog output file name
        source_obj.cog_out_file = self.get_cog_out_name(source_obj)
        
        # Update sources dictionary
        cog_sources.sources.update({cog_id: source_obj})
        
        if relations is not None:
            for relation in relations:
                relation_obj = cg.CogSources.CogSrcRel(relation[0],relation[1])
                cog_sources.sources[cog_id].relations.append(relation_obj)
    
    #===============================================================================================
    def update_cog_out_sources_names(self, cog_sources, wildcards):
        """ Update cog sources names replacing found wildcards. 
        
        Parameters
        ----------
            cog_sources, CogSources
            wildcards, dict
                Named wildcards to be replaced, e.g., { "NWID" : "NetworkName", "NODEID" : nodeName}
        """
        
        #TODO: Optimize this function so that it only updates a selected list of source_ids
        for source_id, source_obj in cog_sources.sources.items():
            cog_out_file = self.get_cog_out_name(source_obj, wildcards)
            cog_sources.sources[source_id].cog_out_file = cog_out_file      
        
    #===============================================================================================    
    def get_cog_out_name(self, source_obj, wildcards = {}): 
        """ Generates cog output file name as per a given input file name. """
        #remove "cog_" prefix to output file names
        if source_obj.cog_in_file.find("cog_") == 0:  
            cog_output_file = source_obj.cog_in_file[4:]
  
        # Replace wildcards
        for wildcard, value in wildcards.items():
            if value is not None:
                cog_output_file = cog_output_file.replace(wildcard,value)
            else:
                cog_output_file = cog_output_file.replace(wildcard+'_','')
        
        return cog_output_file
        
    #===============================================================================================    
    def cog_generator(self, source_obj, project_pickle_file, \
                             comp_pickle_file = None, variables = None):
        """ Invoke cog generator for the specified file.
        """
        # Setup input and output files
        # ----------------------------
        out_dir = self.project_obj.get_simple_param_val("common.project", "project_path_out")
        work_dir = self.project_obj.get_simple_param_val("common.project", "project_path_working")
        gen_path = self.gen_path
        
        input_file = str(source_obj.cog_in_file)
        comgen_CAN_cog_input_file = gen_path / input_file
                
        # Set output file with path
        if source_obj.user_code is True:
            # Prepend string 'USER_'
            cog_output_file = "USER_" + source_obj.cog_out_file
        else:
            cog_output_file = source_obj.cog_out_file
            
        comgen_CAN_cog_output_file = out_dir / cog_output_file
                
        # Invoke code generation
        # ----------------------           
        cog_arguments = ['dummy_argument', \
                   '-d', \
                   '-D', 'input_worksheet=' + self.metadata_gen_source_file, \
                   '-D', 'project_working_dir=' + str(work_dir), \
                   '-D', 'cog_proj_pickle_file=' + str(project_pickle_file), \
                   '-D', 'cog_pickle_file=' + str(comp_pickle_file), \
                   '-D', 'cog_output_file=' + str(cog_output_file)]
        
        # Append additional variables if required
        if variables is not None:
            for variable, variable_val in variables.items():
                variable_str = str(variable) + "=" +str(variable_val)
                cog_arguments.append('-D')
                cog_arguments.append(variable_str)

        cog_arguments.append('-o')
        cog_arguments.append(str(comgen_CAN_cog_output_file))
        cog_arguments.append(str(comgen_CAN_cog_input_file))
        
                
        # Call cogapp engine
        cog_return = cog.Cog().main(cog_arguments)
        if cog_return == 0:
            log_info("Code generation successful: '%s'" % comgen_CAN_cog_output_file)
            print("INFO: code generation successful: ",comgen_CAN_cog_output_file)
        else:
            log_error("Code generation error for '%s'. Cogapp return code: '%s'" \
                 % (comgen_CAN_cog_output_file, str(cog_return)))
            print("INFO: code generation return value: ",cog_return)
        
    #===============================================================================================   
    def gen_code(self):
        """ Generates C code for the given network.
        """

        global cog_sources
        
        # Determine node list and full_names value 
        full_names = self.get_simple_param("CAN_gen_file_full_names")
        
        n_of_networks = 0
        for component in self.project_obj.components:
            if component.type == self.module:
                n_of_networks += 1
        if n_of_networks > 1 or full_names is True: 
            NWID_wildcard = self.id_string
        else:
            NWID_wildcard = None
    
        multiple_nodes = False
        node_lst = self.get_simple_param("CAN_gen_nodes")
        if len(node_lst) == 0:
            # assume all network nodes will be generated
            for node in self.nodes:
                node_lst.append(node)
            if len(node_lst) > 1:
                multiple_nodes = True
        elif len(node_lst) > 1:
            multiple_nodes = True
  
        # Get project pickle full file name  
        project_pickle = self.project_obj.get_work_file_path("common.project", \
                                                                         "project_pickle")
         
        # Create subnetwork will all involved nodes    
        subnetwork = self.get_subnetwork(node_lst)
        
        if subnetwork is not None and len(subnetwork.nodes) > 0 and len(subnetwork.messages) > 0:
            # Update cog output files names (replace network id wildcard)
            wildcards = {"NWID" : NWID_wildcard}
            self.update_cog_out_sources_names(cog_sources, wildcards)
            # Create temporal file with subnetwork object pickle.
            cog_pickle_file_name = "comgen_CAN_network_obj.pickle"
            work_dir = self.project_obj.get_simple_param_val("common.project", \
                                                             "project_path_working")
            cog_serialized_network_file = work_dir / cog_pickle_file_name 
            try:
                with open(cog_serialized_network_file, 'wb') as f:
                    pickle.dump(subnetwork, f, pickle.HIGHEST_PROTOCOL)
            except Exception as e:
                    print('Failed to create pickle file %s. Reason: %s' \
                          % (cog_serialized_network_file, e))
            
            # Generate common source file(s)
            # ------------------------------
            for cog_source in cog_sources.sources.values():
                if "category" in cog_source.dparams \
                and cog_source.dparams["category"] == "common" \
                and cog_source.generated == False:
                    # Generate includes variable if needed
                    log_debug("Generating common network file '%s'..." % cog_source.cog_out_file)
                    variables = {}
                    includes_lst = self.get_cog_includes(cog_source.source_id)
                    if len(includes_lst) > 0:
                        include_var = json.dumps(includes_lst)
                        variables.update({"include_var" : include_var})
                         
                    self.cog_generator(cog_source, project_pickle, cog_serialized_network_file, \
                                       variables)
            
            # Generate network source file(s)
            # -------------------------------
            for cog_source in cog_sources.sources.values():
                if "category" in cog_source.dparams and cog_source.dparams["category"] == "network":
                    # Generate includes variable if needed
                    log_debug("Generating network specific file '%s'..." % cog_source.cog_out_file)
                    variables = {}
                    includes_objs = self.get_cog_includes_objects(cog_source.source_id)
                    includes_lst = []
                    categories_lst = []
                    for src_object in includes_objs:
                        includes_lst.append(src_object.cog_out_file)
                        if "category" in src_object.dparams:
                            categories_lst.append(src_object.dparams["category"])
                        else:
                            categories_lst.append(None)
                    if len(includes_lst) > 0:
                        include_var = json.dumps(includes_lst)
                        variables.update({"include_var" : include_var})
                        
                        categories_var = json.dumps(categories_lst)
                        variables.update({"categories_var" : categories_var})
                    
                    # Set-up wildcard variables
                    # Add variable for NWID wildcard
                    variables.update({"NWID_wildcard" : str(NWID_wildcard)})
                    
                    # Add list of wildcards for nodes
                    NODEID_names = []
                    for node in subnetwork.nodes.values():
                        if len(self.get_messages_of_node(node.name)) > 0:
                            # Update cog output files names (replace node id wildcard)
                            if multiple_nodes is True or full_names is True:
                                NODEID_names.append(node.name) 
                    
                    NODEID_names_var = json.dumps(NODEID_names)
                    variables.update({"NODEID_names_var" : NODEID_names_var})
                            
                    self.cog_generator(cog_source, project_pickle, cog_serialized_network_file, \
                                       variables)
            
            # Generate Node specific source file(s)
            # -------------------------------------
            for node in subnetwork.nodes.values():
                if len(self.get_messages_of_node(node.name)) > 0:
                    # Update cog output files names (replace node id wildcard)
                    if multiple_nodes is True or full_names is True: 
                        NODEID_wildcard = node.name
                    else:
                        NODEID_wildcard = None
                    wildcards = {"NWID" : NWID_wildcard, "NODEID" : NODEID_wildcard}
                    self.update_cog_out_sources_names(cog_sources, wildcards)
                    for cog_source in cog_sources.sources.values():
                        if "category" in cog_source.dparams \
                        and cog_source.dparams["category"] == "node":
                            log_debug("Generating Node '%s' file '%s'..." \
                                      % (node.name, cog_source.cog_out_file))  
                            # Generate includes variable if needed
                            variables = {}
                            includes_lst = self.get_cog_includes(cog_source.source_id)
                            if len(includes_lst) > 0:
                                include_var = json.dumps(includes_lst)
                                variables.update({"include_var" : include_var})
                            # Add variable for NWID wildcard
                            variables.update({"NWID_wildcard" : str(NWID_wildcard)})
                            # Add variable for NODEID wildcard
                            variables.update({"NODEID_wildcard" : str(NODEID_wildcard)})
                            # Add variable containing current's node name
                            variables.update({"node_name" : str(node.name)})
                                 
                            self.cog_generator(cog_source, project_pickle, \
                                               cog_serialized_network_file, variables)
                else:
                    log_warn("No messages found for node '%s'. No C-code generated for the node." \
                             % node.name)
            
            # Delete pickle file
            cg.delete_file(cog_serialized_network_file)
        else:
            log_warn(("No nodes/messages found for the selected subnetwork '%s', node list: '%s'."
                     + " No C-code generated.") % (self.id_string, str(node_lst)))
        
    #===============================================================================================
    def get_simple_param(self, param_id):
        """ Returns local parameter if defined otherwise returns the default one. """
        if param_id in self.simple_params:
            return_value = self.simple_params[param_id].param_value
        else:
            return_value = self.project_obj.get_simple_param_val(self.module, param_id, None)
        
        return return_value
    
    #===============================================================================================
    def get_subnetwork(self, nodes_list):
        """ Returns a network only with data of the passed nodes. """
        
        if type(nodes_list) is list and len(nodes_list) > 0:
            subnetwork = copy.deepcopy(self)
    
            # Clean nodes
            subnetwork.nodes.clear()
            for node_str in nodes_list:
                # Only keep valid nodes
                if node_str in self.nodes:
                    subnetwork.nodes.update({node_str : self.nodes[node_str]})
                else:
                    log_warn("Node '%s' not found in Network '%s'." % (node_str, self.id_string))
                    
            # Clean messages
            subnetwork.messages.clear()
            for message in self.messages.values():
                # Only keep messages related to the specified nodes
                keep_message = False
                # See if message is subscribed or published by any of the specified nodes
                for subnet_node in subnetwork.nodes.values():
                    if message.name in subnet_node.subscribed_messages:
                        keep_message = True
                        break
                    elif message.publisher == subnet_node.name:
                        keep_message = True
                        break
                
                if keep_message is True:
                    subnetwork.messages.update({message.name : message})
    
            # Clean signals
            subnetwork.signals.clear()
            for signal in self.signals.values():
                if signal.message in subnetwork.messages:
                    subnetwork.signals.update({signal.name : signal})
            
            # Clean types
            subnetwork.enum_types.clear()
            for enum_type in self.enum_types.values():
                keep_type = False
                for signal in subnetwork.signals.values():
                    if enum_type.name == signal.data_type:
                        keep_type = True
                        break;
                if keep_type is True:
                    subnetwork.enum_types.update({enum_type.name : enum_type})
        else:
            log_warn("Provided nodes shall be a list greater than zero. Returned 'None'.")
            subnetwork = None
            
        return subnetwork
        
    #===============================================================================================    
    def parse_spreadsheet_ods(self,input_file):
        """ parses an spreadsheet (ods format) to generate this network. """
        book = pe.get_book(file_name=str(input_file))
        self.metadata_gen_source_file = str(input_file)
        
        self.input_file = cg.string_to_path(input_file)
        
        log_debug("Loading CAN information from file: '%s'..." % input_file)
        
        # -----------------------
        # Parse Parameters
        # -----------------------
        log_debug("Parsing parameters data.")
        CONFIG_TITLE_ROW = 1 # Row number with titles
        working_sheet = book["Config"]
        working_sheet.name_columns_by_row(CONFIG_TITLE_ROW)
        
        for idx, row in enumerate(working_sheet):
            # Ignore rows that are before the "config's" title row
            if idx >= CONFIG_TITLE_ROW:
                param_id = row[working_sheet.colnames.index("Parameter")]
                param_value = row[working_sheet.colnames.index("User Value")]
                if param_id != "" \
                and self.project_obj.simple_param_exists(self.module, param_id) is True:
                    read_only = self.project_obj.simple_param_is_read_only(self.module, param_id)
                    if read_only is False and param_value != "":
                        # Add parameter user's value to this object
                        param_type = self.project_obj.get_simple_param_type(self.module, param_id)
                        # Force read value from ODS to be a string and substitute quotes characters
                        # out of utf-8 if present.
                        param_value = str(param_value)
                        param_value = param_value.encode(encoding='utf-8')
                        param_value = param_value.replace(b'\xe2\x80\x9c', b'\"')
                        param_value = param_value.replace(b'\xe2\x80\x9d', b'\"')
                        param_value = param_value.decode('utf-8')
                        log_debug("Processing parameter '%s' with value '%s'..." \
                                  % (param_id, param_value))
                        param_value = grl.process_simple_param(param_type, param_value)
                        
                        # Check if validation is required
                        validator = \
                            self.project_obj.get_simple_param_validator(self.module, param_id)
                        if validator is not None:
                            # Validate parameter
                            valid = self.project_obj.validate_simple_param(self.module, param_id, \
                                                                           validator[0], \
                                                                           validator[1])
                        else:
                            # If no validator is defined assume data is valid.
                            valid = True
                        
                        if valid is True:
                            param_obj = grl.SimpleParam(param_id, param_type, param_value)
                            self.simple_params.update({param_id: param_obj})
                            log_debug("Added CAN user parameter '%s', type '%s' with value '%s'" \
                                      % (param_id, param_type, param_value))
                        else:
                            log_warn(("Parameter '%s:%s' is invalid as per its " \
                                     + "validator '%s:%s''. Parameter ignored.") \
                                     % (self.module, param_id, validator[0], validator[1]))
                    elif param_value != "":
                        log_warn("Parameter '%s' is read-only. Ignored user value." % param_id)
                    else:
                        pass
                elif param_id != "":
                    log_warn("Parameter '%s' is meaningless for component '%s'. Parameter ignored." \
                             % (param_id, self.module))
                else:
                    pass
                
        # -----------------------
        # Parse Network Data
        # -----------------------
        log_debug("Parsing network data.")
        working_sheet = book["Network_and_Nodes"]
                
        self.name = working_sheet[0,1]
        self.id_string = working_sheet[1,1]
        self.description = working_sheet[2,1]
        self.version = working_sheet[3,1]
        self.date = working_sheet[4,1]

        # -----------------------
        # Parse Network Nodes
        # -----------------------
        log_debug("Parsing nodes data.")
        NODES_TITLE_ROW = 6 # Row where the node's titles are located
        working_sheet.name_columns_by_row(NODES_TITLE_ROW)
        
        for idx, row in enumerate(working_sheet):
            # Ignore rows that are before the "node's" title row
            if idx >= NODES_TITLE_ROW:
                name = row[working_sheet.colnames.index("Node Name")]
                # Skip rows with no node defined
                if str(name) != "": 
                    description = row[working_sheet.colnames.index("Description")]
                    self.add_node(name, description)  
        
        # -----------------------
        # Parse Enum Types
        # -----------------------
        log_debug("Parsing enums type data.")
        working_sheet = book["Data_Types"]
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
        log_debug("Parsing messages data.")
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
                    log_warn(("Message \"" + message_name 
                                  + "\" doesn't have a defined publisher."))
                
                if str(message_subscribers) != "":
                    self.add_rx_message_to_subscribers(message_name, message_subscribers)
                else:
                    log_warn(("Message \"" + message_name 
                                  + "\" doesn't have subscribers."))
        
        # -----------------------
        # Parse Signals
        # -----------------------
        log_debug("Parsing signals data.")
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
                    log_info(("Signal \"" + signal_name 
                              + "\" doesn't have a defined data type."))
                # Add signal to the specified message if any
                if signal_conv_msg != "":
                    if cg.is_valid_identifier(str(signal_conv_msg)):
                        #Set signal's conveyor message
                        self.add_signal_to_message(signal_conv_msg, signal_name, \
                                                signal_start_byte, signal_start_bit)
                    else:
                        log_warn(("Wrong conveyor message \"" + signal_conv_msg \
                            + "\" for signal \"" + signal_name + "\"."))
                else:
                    log_warn(("Signal \"" + signal_name 
                                  + "\" doesn't have a defined coveyor message."))        
                # Add extra information parameters of the signal (no warnings if empty)
                
                optimize_init = self.get_simple_param("CAN_optimize_signals_init")
                default_init_value = \
                    cg.get_valid_number(self.get_simple_param("CAN_rx_data_init_val"))
                
                if str(signal_init_val) != "":
                    self.signals[signal_name].init_value = None
                    valid_init_value = \
                        self.get_valid_signal_value(signal_name, signal_init_val)
                    if valid_init_value is not None:
                        # If CAN_optimize_signals_init parameter is True, then set init value only 
                        # if it is different than the default one in CAN_rx_data_init_val
                        if optimize_init is True:
                            optimize = False
                            # If enum type, check if initial symbol numeric value is equal or not
                            # to the default init value.
                            if self.signals[signal_name].is_enum_type:
                                init_enum_type=self.enum_types[self.signals[signal_name].data_type]
                                init_enum_value = init_enum_type.enum_entries[valid_init_value]
                                if init_enum_value is not None:
                                    if int(init_enum_value) == int(default_init_value):
                                        optimize = True
                            else:
                                if cg.get_valid_number(valid_init_value) is not None \
                                and cg.get_valid_number(valid_init_value) == default_init_value:
                                    optimize = True
                                    
                            if optimize is False:
                                self.signals[signal_name].init_value = valid_init_value
                        else:
                            self.signals[signal_name].init_value = valid_init_value
                    else:
                        log_warn(("Invalid initial value '"+str(signal_init_val)+"' for signal '" \
                                  +signal_name+"'. Assumed initial value 'None'."))
                        
                if str(signal_fail_val) != "":
                    self.signals[signal_name].fail_value = None
                    valid_fail_value = \
                        self.get_valid_signal_value(signal_name, signal_fail_val)
                    if valid_fail_value is not None:
                        self.signals[signal_name].fail_value = valid_fail_value
                    else:
                        log_warn(("Invalid fail-safe value '"+str(signal_init_val) \
                                  +"' for signal '"+signal_name+"'. Assumed value 'None'."))
                
                if str(signal_offset) != "":
                    self.signals[signal_name].offset = signal_offset
                
                if str(signal_resolution) != "":
                    self.signals[signal_name].resolution = signal_resolution
                
                if str(signal_unit) != "":
                    self.signals[signal_name].uint = signal_unit

    #===============================================================================================        
    class EnumType:
        """ Class to model an enumerated data type. """
        def __init__(self, name, enum_string = None, description = ""):
            self.name = str(name)
            self.enum_entries = {} # {enum symbol, enum value|None}
            if enum_string is not None:
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
                log_warn(("enum symbol \"" + enum_symbol + \
                      "\" doesn't exist"))
        
        #===========================================================================================
        def get_resolved_values(self):
            """ Returns an EnumType will all symbols assigned to a numeric value.
            
            This mimics the numeric assignment in C if no explicit numeric value was given
            to a symbol.
            """
            return_value = Network_CAN.EnumType(self.name)
            
            check_duplicates = {}
            current_value = 0
            
            for entry in self.enum_entries:
                entry_key = entry
                entry_value = None
                if self.enum_entries[entry] is None:
                    entry_value = int(current_value)
                    if str(current_value) not in check_duplicates:
                        check_duplicates.update({str(current_value):int(current_value)})
                    else:
                        log_warn(("Duplicated resolved value(s) in enum type \"" + self.name + \
                                  "\". This may cause problems when compiling generated files."))
                    current_value += 1
                else:
                    entry_value = int(self.enum_entries[entry])
                    # If explicit value, then next empty symbol will continue from this number
                    current_value = int(self.enum_entries[entry]) + 1
                return_value.enum_entries.update({entry_key : entry_value})
            
            return return_value
            
        #===========================================================================================
        def get_min(self):
            """ return the minimum numeric value for the symbols """
            
            return_value = None
            
            resolved_enum = self.get_resolved_values();
            for i, entry in enumerate(resolved_enum.enum_entries):
                value = resolved_enum.enum_entries[entry]
                if i == 0:
                    return_value = value
                else:
                    if value < return_value:
                        return_value = value
            
            return return_value
            
        #===========================================================================================
        def get_max(self):
            """ return the maximum numeric value for the symbols """
            
            return_value = None
            
            resolved_enum = self.get_resolved_values();
            for i, entry in enumerate(resolved_enum.enum_entries):
                value = resolved_enum.enum_entries[entry]
                if i == 0:
                    return_value = value
                else:
                    if value > return_value:
                        return_value = value
            
            return return_value  
        
    #===============================================================================================
    class Node:
        """ Class to model a network node. """
        def __init__(self, name, description = ""):
            self.name = str(name)

            self.description = description
    
            self.subscribed_messages = {} # {message name : message timeout}
        
        def add_subscriber(self, message_name, timeout_ms):
            """ Adds an RX message for this node """
            if message_name not in self.subscribed_messages:
                self.subscribed_messages.update({message_name : timeout_ms}) 
            else:
                log_warn(("Message \"" + message_name + \
                      "\" already subscribed to node \"" + self.name + "\""))
    
    #===============================================================================================      
    class Message:
    
        def __init__(self, name, msg_id, length, extended_id = False, \
                     tx_type = None, tx_period = None, tx_baf_repeats = None, \
                     description = ""):
            """ Constructor for the class """
            self.name = str(name)
            
            self.id = cg.get_valid_number(msg_id)
            if self.id is None:
                log_warn(("Wrong ID value: \"" + msg_id \
                              + "\" of message \"" + self.name \
                              + "\". Assumed ID 'None'."))
            
            self.len = cg.get_valid_number(length)
            if self.len is None:
                #Assume None for message ID.
                self.len = 8
                log_warn(("Wrong lenght value: \"" + length \
                              + "\" of message \"" + self.name \
                              + "\". Assumed lenght of 8 bytes."))
            
            if extended_id is False or extended_id == "no" \
            or extended_id == "No" or extended_id == "NO" \
            or extended_id == "False" or extended_id == "false" \
            or extended_id == "FALSE":
                self.extended_id = False
            elif extended_id is True or extended_id == "yes" \
            or extended_id == "Yes" or extended_id == "YES" \
            or extended_id == "True" or extended_id == "true" \
            or extended_id == "TRUE":
                self.extended_id = True
            else:
                self.extended_id = True
                log_warn(("Wrong value '%s' for extended frame indication of message '%s'. " \
                          "Assumed frame IS extended.") % (str(extended_id),self.name))
                
            if tx_type == None:
                #If no tx type was given, assume default one
                self.tx_type = tx_type_list[DEFAULT]
                #warn user    
                log_warn(("No Tx type given for message: \"" + self.name \
                                + "\". Assumed default one: \"" \
                                + self.tx_type + "\""))
            else:
                if tx_type in tx_type_list:
                    self.tx_type = tx_type
                else:
                    #Form a string of the allowed Tx types to warn the user
                    allowed_tx_types = ""
                    for tx_type_element in tx_type_list:
                        allowed_tx_types = allowed_tx_types + tx_type_element + ", " 
                    #warn user    
                    log_warn(("Wrong Tx type: \"" + tx_type + "\" of message: \"" \
                                    + self.name \
                                    + "\". \n Please use on the following: " \
                                    + allowed_tx_types
                                    + ".\n. Assummed default tx type: "
                                    + tx_type_list[DEFAULT]))
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
                    log_warn(("Wrong Tx period value: \"" + tx_period \
                                 + "\" of message \"" \
                                 + self.name + "\"."))
            else:
                self.tx_period = tx_period
            
            #If message is defined as BAF then validate BAF repeats value
            if self.tx_type == tx_type_list[BAF]:
                self.tx_baf_repeats = cg.get_valid_number(tx_baf_repeats)
                if self.tx_baf_repeats is not None:
                    self.tx_baf_repeats = tx_baf_repeats
                else:
                    #Assume None for Baf repeats.
                    log_warn(("Wrong BAF repeats value: \"" + tx_baf_repeats \
                                 + "\" of message \"" \
                                 + self.name + "\"."))
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
                log_warn(("Invalid message name: \"" + message_name + \
                  "\". Message names must comply with C-language identifier syntax."))
        
        #===========================================================================================
        def set_layout_info(self, start_byte, start_bit):
            """ Sets the layout information for the signal """   
            temporal_start_byte = cg.get_valid_number(start_byte)
            if temporal_start_byte is not None:
                self.start_byte = temporal_start_byte
            else:
                log_warn(("Wrong Start Byte parameter: \"" + str(start_byte) \
                              + "\" for signal \"" + self.name + "\"."))
                
            temporal_start_bit = cg.get_valid_number(start_bit)
            if temporal_start_bit is not None:
                self.start_bit = temporal_start_bit
            else:
                log_warn(("Wrong Start Byte parameter: \"" + str(start_bit) \
                              + "\" for signal \"" + self.name + "\"."))
        
        #===========================================================================================
        def set_data_len(self, length):
            temporal_len = cg.get_valid_number(length)
            if temporal_len != None and temporal_len > 0 and temporal_len <= 64:
                self.len = temporal_len
            else:
                log_warn(("Wrong lenght parameter: \"" + str(length) \
                              + "\" for signal \"" + self.name \
                              + "\". No change performed."))
        
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
        
        #===========================================================================================
        def is_scalar(self):
            """ Returns True if signal is of scalar type.
            """
            return_value = None
            if self.data_type == data_types_list[SCALAR] and self.is_enum_type == False:
                return_value = True
            else:
                return_value = False
                
            return return_value
        
        #===========================================================================================
        def get_abs_start_bit(self):
            """ Returns the absolute start bit position of this signal (starting with bit 0)
            """
            return_value = None
            if self.start_byte is not None and self.start_bit is not None:
                return_value = (self.start_byte * 8) + (self.start_bit)
                
            return return_value

    #===============================================================================================
    class SignalAccess():
        """ Class to model the abstract access of a signal. """
        def __init__(self, name, length):
            self.signal_name = name
            self.signal_len = length
            self.signal_base_len = cg.calculate_base_type_len(self.signal_len)
                    
            self.pieces = [] # Array of PieceAccess objects
        
        class PieceAccess():
            """ Class to model a signal access piece. """
            def __init__(self, length = None, abs_byte = None, shift_inner = None, shift_outer = None, \
                         mask_inner = None, mask_outer = None, mask_3 = None):
                self.len = length
                self.abs_byte = abs_byte
                self.shift_inner = shift_inner 
                self.shift_outer = shift_outer
                self.mask_inner = mask_inner
                self.mask_outer = mask_outer
                self.mask_3 = mask_outer

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


#input_file_name = None # Used for determining name of output network xml file.

#===================================================================================================
def load_input(input_file, input_type, params, project_obj):
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
        return_object = Network_CAN(project_obj)
#        return_object.load_default_gen_params()
        return_object.parse_spreadsheet_ods(input_file)
        try:
            return_object.update_cog_sources()
        except Exception as e:
            log_error('Failed to update cog sources. Reason: %s' % (e))
        
#        input_file_name = input_file.stem
    except Exception as e:
        return_object = None
        log_error('Failed to process input file "%s". Reason: %s' % (input_file, e))
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
    input_object.gen_code()
    
    # Generate XML
    xml_output_file = out_path / (str(input_object.input_file.stem) + ".xml")
    input_object.gen_XML(xml_output_file)
    
    try:
        dbc_output_file = out_path / (str(input_object.input_file.stem) + ".dbc")
        input_object.gen_DBC(dbc_output_file)
    except Exception as e:
        log_error('Failed to generate DBC file "%s". Reason: %s' % (str(dbc_output_file), e))
    
#     except Exception as e:
#         log_error('Failed to generate code. Reason: %s' % e)
        