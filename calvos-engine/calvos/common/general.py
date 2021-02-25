# -*- coding: utf-8 -*-
""" CalvOS General Module.

Module for miscelaneous general features of the calvos engine.

@author: Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.0.1'
__date__ = '2020-12-20'
__updated__ = '2020-12-20'

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

LOOP_GUARD_MED = 500

TYPE_SCALAR = 0
TYPE_ENUM = 1
TYPE_STRING = 2

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
    elif param_type == "str" or param_type == "path":
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
        if param_value == "True" or param_value == "true" \
        or param_value == "TRUE" or param_value == True:
            param_value = True
        elif param_value == "False" or param_value == "false" \
        or param_value == "FALSE"  or param_value == False:
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