# -*- coding: utf-8 -*-
""" Time related utilities.

calvos.utils.time models utilities for time related function.

@author:     Carlos Calvillo
@copyright:  2021 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.1.0'
__date__ = '2021-01-17'
__updated__ = '2021-01-17'
    
import pyexcel as pe
import math
from lxml import etree as ET
import xml.dom.minidom
import pickle as pic
import cogapp as cog
import pathlib as pl
import importlib
import json

import calvos.common.codegen as cg
import calvos.common.logsys as lg

# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "time"

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
# Cog sources to use for code generation of this module
cog_sources = cg.CogSources("comgen.CAN")

input_file_name = None

class Timers():
    """ Models timers for the calvos projects. """
    def __init__(self, **kwargs):
        self.name = kwargs.get('name', None)
        self.input_file = kwargs.get('input_file', None)
        self.base_tick_ms = kwargs.get('base_tick_ms', None)
        
        self.timers = {} # Expects { timer_name : Timer object}
        self.tasks_options = kwargs.get('tasks_options', None)
        if self.tasks_options is None:
            # Use default ones.
            # TODO: get defaults from XML
            self.tasks_options = [5, 10, 20, 100, 1000]
             
        self.tasks = []
        
    def update_timer_tasks(self):
        """ Define which timer tasks to use for timers handling. """
        self.tasks = []
        if self.base_tick_ms is not None:
            for task_option in self.tasks_options:
                # Task option shall be equal to or greater than the base tick.
                if task_option >= self.base_tick_ms:
                    # Get an approximation to the option value
                    aprox_value = int(round(task_option / self.base_tick_ms) * self.base_tick_ms)
                    self.tasks.append(aprox_value)
                    if task_option % self.base_tick_ms != 0:
                        log_warn(("Timer task cycle time was approximated." \
                                  + "Target: %fms, actual: %fms, base tick: %fms.") \
                                  % (task_option, aprox_value, self.base_tick_ms))
        else:
            log_warn("Can't define timer tasks since there is no base tick defined.")
            
        
    class Timer():
        """ Models a timer. """
        def __init__(self, name, **kwargs):
            self.name = name
            
            self.desc = kwargs.get('desc', None)
            
            self.time_ticks = kwargs.get('time_ticks', None)
            if self.time_ticks is None:
                self.time_ms = kwargs.get('time_ms', None)
                if self.time_ms is not None:
                    self.set_time_ticks_from_ms(self.time_ms)
                    
            callback_str = kwargs.get('callback', None)
            if callback_str is not None and cg.is_valid_identifier(callback_str):
                self.callback = callback_str
        
        def get_task_tick_time(self, timer_time_ms):
            """ Returns the cycle time of the closes task for processing the given timer value. """
            pass
                    
        def set_time_ticks_from_ms(self, time_ms):
            """ Sets the time in ticks from an input in milliseconds. """
             
            if self.base_tick_ms is not None:
                if time_ms & self.base_tick_ms != 0:
                    self.time_ticks = int(round(time_ms / self.base_tick_ms))
                    log_warn(("Timer '%s' rounded. Requested time %fms, rounded time %dms." \
                              + " Current base tick value: %f. ") \
                              % (self.name, time_ms, self.base_tick_ms))
                else:
                    self.time_ticks = int(time_ms / self.base_tick_ms)
            else:
                log_warn(("Can't set value of timer '%s' because there is no base tick defined." \
                          + " Define first the value of the timers base tick.") % self.name)
                

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
        return_object = 0
        try:
            return_object.update_cog_sources()
        except Exception as e:
            log_error('Failed to update cog sources. Reason: %s' % (e))
        input_file_name = input_file.stem
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
#     try:
    cog_files_path = calvos_path / "comgen" / "gen" / "CAN"
    input_object.gen_code(out_path, working_path, cog_files_path)
    # Generate XML
    xml_output_file = out_path / (str(input_object.input_file.stem) + ".xml")
    input_object.gen_XML(xml_output_file)