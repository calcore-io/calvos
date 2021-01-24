'''
Created on Dec 20, 2020

@author: Carlos Calvillo
'''
import json
import re

import calvos.common.codegen as cg
import calvos.common.logsys as lg


# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "grl"

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

TYPE_SCALAR = 0
TYPE_ENUM = 1
TYPE_STRING = 2

# # remove annoying characters
# chars_replacement = {
#     '\xc2\x82' : ',',        # High code comma
#     '\xc2\x84' : ',,',       # High code double comma
#     '\xc2\x85' : '...',      # Tripple dot
#     '\xc2\x88' : '^',        # High carat
#     '\xc2\x91' : '"',     # Forward single quote
#     '\xc2\x92' : '"',     # Reverse single quote
#     '\xc2\x93' : '"',     # Forward double quote
#     '\xc2\x94' : '"',     # Reverse double quote
#     '\xc2\x95' : ' ',
#     '\xc2\x96' : '-',        # High hyphen
#     '\xc2\x97' : '--',       # Double hyphen
#     '\xc2\x99' : ' ',
#     '\xc2\xa0' : ' ',
#     '\xc2\xa6' : '|',        # Split vertical bar
#     '\xc2\xab' : '<<',       # Double less than
#     '\xc2\xbb' : '>>',       # Double greater than
#     '\xc2\xbc' : '1/4',      # one quarter
#     '\xc2\xbd' : '1/2',      # one half
#     '\xc2\xbe' : '3/4',      # three quarters
#     '\xca\xbf' : '\x27',     # c-single quote
#     '\xcc\xa8' : '',         # modifier - under curve
#     '\xcc\xb1' : ''          # modifier - under line
# }
# 
# def replace_chars(match):
#     char = match.group(0)
#     return chars_replacement[char]
# 
#     return re.sub('(' + '|'.join(chars.keys()) + ')', replace_chars, text)


def process_simple_param(param_type, param_value):
    """ Returns a variable corresponding to the XML parameter string according to its type. """
    
    error_msg = ""
    if param_type == "int":
        try:
            param_value = int(json.loads(param_value))
        except Exception as e:
            param_value = None
            error_msg = "Expected int value for given parameter. Assumed 'None'.\n" \
                + "JSON error: " + str(e)
    elif param_type == "float":
        try:
            param_value = float(json.loads(param_value))
        except Exception as e:
            param_value = None
            error_msg = "Expected float value for given parameter. Assumed 'None'.\n" \
                + "JSON error: " + str(e)
    elif param_type == "str":
        param_value = str(param_value)
    elif param_type == "list":
        try:
            param_value = list(json.loads(param_value))
        except Exception as e:
            param_value = None
            error_msg = "Expected list type value for given parameter. Assumed 'None'.\n" \
                + "JSON error: " + str(e)
    elif param_type == "dict":
        try:
            param_value = dict(json.loads(param_value))
        except Exception as e:
            param_value = None
            error_msg = "Expected dictionary value for given parameter. Assumed 'None'.\n" \
                + "JSON error: " + str(e)
    elif param_type == "boolean":
        if param_value == "True" or param_value == "true" or param_value == "TRUE":
            param_value = True
        elif param_value == "False" or param_value == "false" or param_value == "FALSE":
            param_value = False
        else:
            param_value = None
            error_msg = "Expected boolean value for given parameter. Assumed 'None'."
    elif param_type == "cid":
        param_value = str(param_value)
        if cg.is_valid_identifier(param_value) is False:
            param_value = None
            error_msg = "Expected C-identifier value for given parameter. Assumed 'None'."
    elif param_type == "enum":
        param_value = cg.parse_special_string(param_value)
        if len(param_value) == 0:
            param_value = None
            error_msg ="Expected special string value for given parameter. Assumed 'None'."
    else:
        param_value = None
        error_msg = "Unknown parameter type '%s' for given parameter. Assumed 'None'." % param_type
    
    # Throw warning if any
    if error_msg != "":
        log_warn(error_msg)
        
    return param_value

class SimpleParam():
    def __init__(self, param_id, param_type, param_value = None, read_only = True, \
                 validator = None):
        self.param_id = param_id
        self.param_type = param_type
        self.param_value = param_value
        self.read_only = read_only
        self.validator = validator

class Parameter():
    
    def __init__(self, id, value = None, default = None, \
                 name = "", description = ""):
        self.id = id
        self.value = value
        self.default = default
        self.name = name
        self.desc = description
        
        self.category = None
        
        self.enumeration = {}
        self.type = TYPE_SCALAR
        self.multiple = False
        
        
        self.min = None
        self.max = None
        self.offset = None
        self.resolution = None
        
    def is_valid_value(self, value = None):
        """ validate if the given value is within the defined enum. """
        return_value = False
        
        if value is None:
            # Validate current parameter value
            if self.value in self.enumeration:
                return_value = True
        else:
            # Validate passed value
            if value in self.enumeration:
                return_value = True 
        
        return return_value

class ParamCategory():
    
    def __init__(self, id, name = "", description = ""):
        self.id = id
        self.name = name
        self.desc = description