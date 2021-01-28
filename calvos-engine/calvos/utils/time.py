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
import calvos.common.general as grl

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
cog_sources = cg.CogSources("utils.time")

class Timers():
    """ Models Utilities for the calvos projects. """
    def __init__(self, project_obj, **kwargs):
        
        self.project_obj = project_obj
        self.name = kwargs.get('name', None)
        self.input_file = kwargs.get('input_file', None)
        
        self.timers = {} # Expects { timer_name : Timer object}
        self.tasks = []
        
        self.module = "utils.time"
        
        self.gen_path = self.project_obj.get_component_gen_path(self.module)
        
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
            
    #===============================================================================================    
    def parse_spreadsheet_ods(self,input_file):
        """ parses an spreadsheet (ods format) to generate the timers. """
        book = pe.get_book(file_name=str(input_file))
        self.metadata_gen_source_file = str(input_file)
        
        self.input_file = cg.string_to_path(input_file)
        
        
        # -----------------------
        # Parse Parameters
        # -----------------------
        log_debug("Parsing parameters data...")
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
        
        log_debug("Loading Timers information from file: '%s'..." % input_file)
        # -----------------------
        # Parse Timers Data
        # -----------------------
        working_sheet = book["Timers"]
        TIMERS_TITLES_ROW = 0 # Row where the node's titles are located
        working_sheet.name_columns_by_row(TIMERS_TITLES_ROW)
        
        for idx, row in enumerate(working_sheet):
            # Ignore rows that are before the "node's" title row
            if idx >= TIMERS_TITLES_ROW:
                timer_obj = None
                timer_def_error = False
                timer_id = row[working_sheet.colnames.index("Timer Id")]
                # Skip rows with no timer id
                
                if str(timer_id) != "": 
                    log_debug("Adding Timer '%s'..." % str(timer_id))
                    if cg.is_valid_identifier(timer_id) is True:
                        timer_obj = self.Timer(timer_id)
                    else:
                        log_warn(("Timer id '%s' is not valid. It shall comply with C identifier " \
                                  + "syntax. Timer ignored.") % timer_id)
                
                if timer_obj is not None:
                    # Gather rest of timer parameters
                    timer_obj.desc = str(row[working_sheet.colnames.index("Description")])
                    # Get timer tick base time (ms)
                    try:
                        base_time = int(row[working_sheet.colnames.index("Base tick time (ms)")])
                        # Confirm that selected base tick is a valid value 
                        validator = self.project_obj.get_simple_param_val("utils.time","time_tasks")
                        if base_time in validator:
                            timer_obj.base_time = base_time
                        else:
                            timer_def_error = True
                            log_warn(("Invalid timer '%s' base time. Allowed values are: '%s'. " \
                                      + "Timer not added.") % (timer_id,validator))
                    except Exception as e:
                        timer_def_error = True
                        log_warn(("Timer '%s' base time shall be an integer. Timer not added." \
                                 + ". Raised exception: \n%s") % (timer_id,e))
                    if timer_def_error is False:
                        # Get timer type if specified.
                        timer_type = str(row[working_sheet.colnames.index("Type")])
                        validator = self.project_obj.get_simple_param_val("utils.time","time_types")
                        default = self.project_obj.get_simple_param_val("utils.time", \
                                                                        "time_types_default")
                        if timer_type == "":
                            log_warn("Assumed type for timer '%s': '%s'." \
                                     % (timer_id, default))
                        else:
                            if timer_type in validator:
                                timer_obj.type = timer_type
                            else:
                                timer_def_error = True
                                log_warn(("Invalid timer '%s' type. Allowed values are: '%s'. " \
                                          + "Timer not added.") % (timer_id,validator))
                    if timer_def_error is False:
                        auto_reload = str(row[working_sheet.colnames.index("Auto reload")])
                        if auto_reload == "True" or auto_reload == "true" or auto_reload == "TRUE":
                            timer_obj.auto_reload = True
                        elif auto_reload == "False" or auto_reload == "false" \
                        or auto_reload == "FALSE":
                            timer_obj.auto_reload = False
                        else:
                            timer_obj.auto_reload = False
                            log_warn("Timer '%s' Auto reload shall be boolean. Assumed 'False'." \
                                     % timer_id)
                    if timer_def_error is False:
                        # Get timer type if specified, otherwise assume automatic.
                        reloads = str(row[working_sheet.colnames.index("Reloads")])
                        if reloads == "":
                            if timer_obj.auto_reload is False:
                                # If auto reload is False we need a defined number of reloads
                                # including zero.
                                timer_obj.reloads = reloads = 0
                                log_warn("Assumed zero reloads for timer '%s'" % timer_id)
                        else:
                            try:
                                reloads = \
                                    int(row[working_sheet.colnames.index("Reloads")])
                                timer_obj.reloads = reloads 
                            except Exception as e:
                                timer_def_error = True
                                log_warn(("Timer '%s' reloads shall be an integer. Timer not " \
                                         + "added. Raised exception: \n%s") % (timer_id,e))
                    if timer_def_error is False:
                        period = str(row[working_sheet.colnames.index("Period (ms)")])
                        if period != "" and timer_obj.auto_reload is True \
                        or timer_obj.reloads > 0:
                            period_mandatory = True
                        else:
                            period_mandatory = False
                        try:
                            period = int(row[working_sheet.colnames.index("Period (ms)")])
                            timer_obj.period = period
                        except Exception as e:
                            if period_mandatory:
                                log_warn(("Timer '%s' period shall be an integer if defined as " \
                                          + "auto-reload or has reloads > 0. Timer not " \
                                          + "added. Raised exception: \n%s") % (timer_id,e))
                            else:
                                log_info(("Timer '%s' period shall be an integer. " \
                                          + "Timer may be added with no period.") % timer_id)
                    if timer_def_error is False:
                        auto_start = str(row[working_sheet.colnames.index("Auto start")])
                        if auto_start == "True" or auto_start == "true" or auto_start == "TRUE":
                            timer_obj.auto_start = True
                        elif auto_start == "False" or auto_start == "false" \
                        or auto_start == "FALSE":
                            timer_obj.auto_start = False
                        else:
                            timer_obj.auto_start = False
                            log_warn("Timer '%s' Auto start shall be boolean. Assumed 'False'." \
                                     % timer_id)
                    
                    # If no critical error found and timer is not duplicated, 
                    # then add Timer to the Timers object.
                    if timer_id not in self.timers:
                        self.timers.update({timer_id : timer_obj})
                    else:
                        log_warn("Timer '%s' is duplicated. Duplications not added." % timer_id)
            
        
    class Timer():
        """ Models a timer. """
        def __init__(self, id_str, **kwargs):
            self.id_str = id_str
            
            self.desc = kwargs.get('desc', None)
            
            self.time_ticks = kwargs.get('time_ticks', None)
            if self.time_ticks is None:
                self.time_ms = kwargs.get('time_ms', None)
                if self.time_ms is not None:
                    self.set_time_ticks_from_ms(self.time_ms)
                    
            callback_str = kwargs.get('callback', None)
            if callback_str is not None and cg.is_valid_identifier(callback_str):
                self.callback = callback_str
            
            # callback_cycle triggers when a cycle finishes, for example if timer auto-reloads
            # 10 times, it will be called once the 10 times elapsed.
            # TODO: This can be a flag instead of a callback.
            callback_str = kwargs.get('callback_cycle', None)
            if callback_str is not None and cg.is_valid_identifier(callback_str):
                self.callback_cycle = callback_str
                
            self.reloads = kwargs.get('reloads', 0) # 0...n or "inf"
            self.auto_reload = kwargs.get('auto_reload', False)
            
            self.type = kwargs.get('type', None)
            
            self.base_time = kwargs.get('base_time', None) 
            
            self.auto_start = kwargs.get('auto_start', None)
        
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
        
        def determine_timer_type(self):
            pass
        
        #===============================================================================================        
    def update_cog_sources(self):
        """ Updates the list of cog file(s) for this module. """
        
        global cog_sources
        cog_sources = cg.CogSources(self.module, gen_path = self.gen_path)
        
        self.add_cog_source("time_h", "cog_utils_time.h", True)
        self.add_cog_source("time_core_h", "cog_utils_time_core.h", True, \
                            [[self.module, "time_h"]])
        self.add_cog_source("time_core", "cog_utils_time_core.c", False, \
                            [[self.module, "time_core_h"]])

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
    def add_cog_source(self, cog_id, cog_in_file, is_header = False, relations = None):
        """ Adds a cog source to this network object's cog files list. 
        
        Parameters
        ----------
            relations, list
                Should be a list of lists to model the relation of cog_id source file with other
                source files... expected -> [[module2, source_id1], [modul2, source_id2], ...]
        """ 
        
        global cog_sources

        ##### cog_out_file = self.get_cog_out_name(cog_in_file, self.id_string)
        cog_out_file = self.get_cog_out_name(cog_in_file)
        cog_sources.sources.update({cog_id: cg.CogSources.CogSrc(\
                cog_id,cog_in_file, cog_out_file, is_header)})
        
        if relations is not None:
            for relation in relations:
                relation_obj = cg.CogSources.CogSrcRel(relation[0],relation[1])
                cog_sources.sources[cog_id].relations.append(relation_obj)
    
    #===============================================================================================    
    def get_cog_out_name(self, cog_in_file, component_name = None): 
        """ Generates cog output file name as per a given input file name. """
        #remove "cog_" prefix to output file names
        if cog_in_file.find("cog_") == 0:  
            cog_output_file = cog_in_file[4:]  
        #substitute component name if found (COMP)
        if component_name is not None:
            cog_output_file = cog_output_file.replace('COMP',component_name)
        else:
            cog_output_file = cog_output_file.replace('COMP','')
        
        return cog_output_file
        
    #===============================================================================================     
    def cog_generator(self, input_file, cog_output_file, project_pickle_file, variables = None):
        """ Invoke cog generator for the specified file.
        """
        # Setup input and output files
        # ----------------------------
        input_file
        out_dir
        work_dir
        gen_path
        
        input_file = str(input_file) 
        comgen_CAN_cog_input_file = gen_path / input_file
                
        # Set output file with path
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
                
        # Get project pickle full file name 
        project_pickle = self.project_obj.get_work_file_path("common.project","project_pickle")
         
        #----------------------------------------------------------------------
        # Generate source file(s)
        #----------------------------------------------------------------------
        for cog_source in cog_sources.sources.values():
            # Generate includes variable if needed
            variables = None
            includes_lst = self.get_cog_includes(cog_source.source_id)
            if len(includes_lst) > 0:
                variables = {}
                include_var = json.dumps(includes_lst)
                variables.update({"include_var" : include_var})
                  
            self.cog_generator(cog_source.cog_in_file, cog_source.cog_out_file, project_pickle, \
                              variables)
#         
#         # Delete pickle file
#         # ------------------
#         print("INFO: deleting pickle file")
#         cg.delete_file(cog_serialized_network_file)
#         
#         #TODO: Logic for ensuring the current network is meaningful, e.g., is no emtpy, etc.
#         if node_names == None:
#             # C code for all nodes will be generated.
#             pass
#         elif len(node_names) > 0:
#             # Generate C code for specified node each
#             for node in node_names:
#                 #Check if node exists
#                 if node in self.nodes:
#                     pass
#                 else:
#                     pass
#         else:
#             log_warn(("Invalid input. No C-Code will be generated for " 
#                           + "network: " + self.name + "."))

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
        return_object = Timers(project_obj)
        return_object.parse_spreadsheet_ods(input_file)
        try:
            return_object.update_cog_sources()
        except Exception as e:
            log_error('Failed to update cog sources. Reason: %s' % (e))
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
#     #TODO: Check for required parameters
# #     try:
#     cog_files_path = calvos_path / "comgen" / "gen" / "CAN"
    input_object.gen_code()
#     # Generate XML
#     xml_output_file = out_path / (str(input_object.input_file.stem) + ".xml")
#     input_object.gen_XML(xml_output_file)