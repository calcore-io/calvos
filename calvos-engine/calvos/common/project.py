# -*- coding: utf-8 -*-
""" CalvOS Project Manager.

Module for managing a calvos project.

@author: Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.1.0'
from pickle import TRUE, NONE
from calvos.common.codegen import calvos_path
__date__ = '2020-11-12'
__updated__ = '2020-11-12'

import xml.etree.ElementTree as ET
import pathlib as plib
import importlib
import json

import calvos.common.codegen as cg
import calvos.common.logsys as lg
import calvos.common.general as grl


IN_TYPE_XML = 0
IN_TYPE_ODS = 1

# -----------------------------------------------------------------------------
# Definitions for the logging system
# -----------------------------------------------------------------------------
LOGGER_LABEL = "proj"

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

#===================================================================================================
class Project:
    """ Class to mode a calvos project. """

    #===============================================================================================
    def __init__(self, name, project_file, calvos_path):
        """ Class constructor.
        
        Parameters
        ----------
            name : str
                Name of the project
            project_file : str
                Path and file name of the project to be processed.
            calvos_path : str
                Path of the calvos python package.
        """
        self.name = name

        project_file = cg.string_to_path(project_file)
        self.project_file = project_file
        # Extract project path
        self.project_path = project_file.parent.resolve()
        
        self.calvos_path = cg.string_to_path(calvos_path)
        

        self.paths = {"project_path" : cg.string_to_path(self.project_path),
                      "project_inputs" : self.project_path / "usr_in",
                      "project_output" : self.project_path / "out",
                      "project_working_dir" : self.project_path / "out/working_dir",
                      "project_docs" : self.project_path / "out/doc",
                      "project_templates" : self.project_path / "out/doc/templates",
                      "calvos_path" : cg.string_to_path(str(self.calvos_path))
                      }

        self.components_definitions = {} #Expected dict {comp_type : CompDefinition object}
        self.components = [] #List of Component objects
        
        self.params = {} # param objects {param_id : param value}
        
        self.param_categories = {} # {category id : objects ParamCategory }
    
#     #===============================================================================================
#     def load_params_categories(self, XML_root):
#         """ Loads parameter categories definitions.
#         
#         Needs to be called prior to call to load_param_definitions function.
#         """
#         for category in XML_root.findall("./Params/Categories/Category"):
#             category_id = category.get("id")
#             if category_id not in self.param_categories:
#                 # Add new category
#                 category_name = category.findtext("Name")
#                 category_desc = category.findtext("Desc")
#                 
#                 self.param_categories.update({category_id : \
#                         g.ParamCategory(category_id, category_name, category_desc)})
#             else:
#                 log_warn('Category id "%s" duplicated. Only added first appearance' % category_id)
#             
#             
#     #===============================================================================================    
#     def load_params_definitions(self, XML_root):
#         """ Loads parameter definitions and their default values.
#         
#         Function load_params_categories needs to be called prior to this one.
#         """
#         for param in XML_root.findall("./Params/Param"):
#             
#             param_id = param.get("id")
#             if param_id not in self.params:
#                 param_name = param.get("Name")
#                 param_desc = param.get("Desc")
#                 param_default = param.findtext("Default")
#                 param_category = param.get("category")
#                 param_type = param.get("type")
#                 
#                 param_object = g.Parameter(param_id, param_default, \
#                                                param_default, param_name, param_desc)
#                 
#                 # Check if category is valid
#                 if param_category in self.param_categories:
#                     param_object.category = param_category
#                 else:
#                     log_warn('Invalid category "%s" for parameter id "%s".' \
#                              % (param_category,param_id))
#                 
#                 # Set rest of parameter attributes bases on its type
#                 if param_type == "scalar":
#                     param_min = param.findtext("Min")
#                     param_max = param.findtext("Max")
#                     param_offset = param.findtext("Offset")
#                     param_res = param.findtext("Resolution")
#                     param_unit = param.findtext("Unit")
#                     
#                     param_object.type = g.TYPE_SCALAR
#                     param_object.min = param_min
#                     param_object.max = param_max
#                     param_object.offset = param_offset
#                     param_object.resolution = param_res
#                     param_object.unit = param_unit
#                     
#                     param_object.default = param_object.default
#                     
#                 elif param_type == "enum":
#                     # Get enumerated values
#                     enum_values = {}
#                     for entry in param.findall("./Enumeration/Entry"):
#                         entry_id = entry.get("name")
#                         if entry_id not in enum_values:
#                             enum_values.update( {entry_id : entry.text} )
#                         else:
#                             log_warn(('Duplicated entry id %s for parameter id %s. ' \
#                                      +' Only first occurrence was taken.') \
#                                        % (entry_id, param_id))
#                     
#                     # Check if enum default is a valid entry
#                     if param_default != "" and param_default not in enum_values:
#                         log_warn(('Default value "%s" for parameter id "%s" is not ' \
#                                  + 'a valid value as per the enumerated values.') \
#                                  % (param_default, param_id))
#                     
#                     # Update Parameter Object with enumeration
#                     param_object.type = g.TYPE_ENUM
#                     param_object.enumeration = enum_values
#                     
#                 elif param_type == "string":
#                     param_min = param.findtext("Min")
#                     param_max = param.findtext("Max")
#                     
#                     param_object.type = g.TYPE_STRING
#                     param_object.min = param_min
#                     param_object.max = param_max
#                     
#                     param_object.default = str(param_object.default)
#                     
#                     # Check if default value complies with min/max characters
#                     if param_min is not None and param_max is not None:
#                         if int(param_max) < int(param_min):
#                             log_warn(( 'Max value for parameter id "%s" is not greater than or '\
#                                      + 'equal to Min.') % param_id)
#                         else:
#                             if len(param_object.default) < param_min \
#                             or len(param_object.default) > param_max:
#                                 log_warn(('Default value "%s" for parameter id "%s" is not a ' \
#                                          + 'valid value as per the min/max characters definition.')\
#                                          % (param_default, param_id))
# 
#                 else:
#                     log_warn('Invalid parameter type "%s" for parameter id "%s".' \
#                              % (param_type, param_id))  
#                 
#                 self.params.update({param_id : param_object}) 
#             else:
#                 log_warn('Parameter id "%s" is duplicated. Only considered first appearance' \
#                          % param_id)
#     
#     #===============================================================================================
#     def write_param(self, param_id, param_value):
#         pass
#     
#     #===============================================================================================
#     def load_project_params(self, XML_root):
#         pass
    
    #===============================================================================================
    def load_component_definitions(self, XML_root, comp_def_ojb):
        """ Loads the component definitions from the given xml file.
        comp_def_ojb: CompDefinition type """

        # Define XML namespace(s)
        nsmap = {"clv":"calvos"}
        
        component = comp_def_ojb.type
        
        log_debug("Loading definitions for component '%s'..." % component)

        for param in XML_root.findall("./clv:ParamsDefinitions/clv:ParamDefinition", nsmap):
            param_id = param.get("id", None)
            log_debug("Loading parameter '%s:%s'..." % (component, param_id))
            if param_id != None:
                param_type = param.get("type", None)
                if param_type is not None:
                    param_value = param.findtext("./clv:Default", None, nsmap)
                    if param_value is not None:
                        param_value = grl.process_simple_param(param_type, param_value)
                    else:
                        log_warn(("Missing default value for parameter '%s' of component '%s'." \
                                     + " Assumed 'None'.") % (param_id, component))
                    
                    param_read_only = param.get("is_read_only", None)
                    if param_read_only is None:
                        # If parameter is not found then it is not read-only
                        param_read_only = False
                    else:
                        param_read_only = grl.process_simple_param('boolean', param_read_only)
                        if param_read_only is None:
                            # If there is a problem converting read only parameter assume True
                            param_read_only = True
                            log_warn(("Wrong value for 'is_read_only' attribute of parameter '%s'" \
                                + " of component '%s'. Assumed read only.")%(param_id, component))
                    
                    param_validator = param.get("validator", None)
                    if param_validator is not None:
                        log_debug("Processing validator '%s'..." % param_validator)
                        param_validator = grl.process_simple_param('list', param_validator)
                        if param_validator is None:
                            # If there is a problem converting validator assume None
                            param_validator = None
                            log_warn(("Wrong value for 'validator' attribute of parameter '%s'" \
                                + " of component '%s'. Assumed 'None'.")%(param_id, component))
                            
                    param_ojb = grl.SimpleParam(param_id, param_type, param_value, \
                                                param_read_only, param_validator)
                    comp_def_ojb.simple_params.update({param_id : param_ojb})
                else:
                    log_warn(("Missing parameter type for parameter '%s' of component '%s'. " \
                              + "Component not added.") % (param_id, component))
            else:
                log_warn(("Id '%s' not found for a parameter of component '%s'. " \
                             + "Parameter not added.") % component)
        
        log_debug("Loading of definitions completed.")   
   
    #===============================================================================================    
    def add_component(self, type_name, \
                     input_file = None, input_type = IN_TYPE_XML, params = {}, \
                     name = None, desc = None):
        """ Adds a Component to this project.
        
        Function load_component_definitions needs to be called prior to this one.
        """
        
        if type_name in self.components_definitions:
            component = self.Component(type_name, \
                         input_file, input_type, params, \
                         name, desc)
            
            self.components.append(component)
        else:
            log_warn("Undefined component type '%s'. Component not added." % type_name)
    
    #===============================================================================================        
    def find_components(self, _calvos_path):
        """ Search for calvos components within calvos package.
        
        Parameters
        ----------
            _calvos_path : Path
                Path of the calvos package.
        
        Returns
        -------
            If at least one component is found returns a dictionary with following format:
            {component_name1 : component_xml_path1, component_name2 : component_xml_path2, ...}
            If no component is found then returns an empty dictionary.
        """
        
        calvos_components = {} # {name : xml_path}
        for path in sorted(calvos_path.rglob('*.py')):
            module_name = path.stem
            xml_path = path.with_suffix(".xml")
            # Check if corresponding xml file is found
            if xml_path.is_file() is True:
                # Python module is a calvos component, now gather its module string
                calvos_path_found = False
                module_name_list = []
                module_name_string = ""
                for parent in xml_path.parents:
                    if parent.name == "calvos":
                        calvos_path_found = True
                        break;
                    else:
                        module_name_list.append(str(parent.name))
                if calvos_path_found is True:
                    # Form module name string
                    for piece in module_name_list:
                        module_name_string = piece + "." + module_name_string
                    module_name_string += module_name
                    calvos_components.update({module_name_string : xml_path})
                    log_debug("Found component: '%s', XML: '%s'" \
                              % (module_name_string, calvos_components[module_name_string])) 
        
        return calvos_components
    
    #===============================================================================================  
    def load_project(self):
        """ Loads current project.
        
        Loading includes:
         - loading the parameter and component definitions (not project specific)
         - loading 
        """
        
        success = True
        
        # Set default project paths
        # -------------------------
        
        # Load project definitions
        # ------------------------
        #project_definitions_file = self.calvos_path / "common/project_definitions.xml"
        
        log_info('Loading project parameters and component definitions...')
        
        # Search for calvos components
        # ----------------------------
        # A calvos component shall have a module.py together with a module.xml at same path
        calvos_components = self.find_components(calvos_path)
        
        # Load parameters of found components
        # -----------------------------------
        for comp_name, comp_xml in calvos_components.items():
            if success == False:
                success = True
            # Create component definition object
            self.components_definitions.update({comp_name : self.CompDefinition(comp_name)})
            # Parse XML with component definitions
            XML_tree = ET.parse(comp_xml)
            XML_root = XML_tree.getroot()
            self.load_component_definitions(XML_root, self.components_definitions[comp_name])  
        
        if success is True:
            # Load project components
            log_info("============== Loading project components. ==============")
            
            project_file_path = self.project_file
            XML_tree = ET.parse(project_file_path)
            XML_root = XML_tree.getroot()
            
            for component in XML_root.findall("./Components/Component"):
                component_type = component.get("type")
                
                if component_type is not None \
                and component_type in self.components_definitions:
                
                    log_info("-------------- Loading component of type '%s'." \
                             % component_type)
                
                    component_name = str(component.findtext("Name"))
                    component_desc = str(component.findtext("Desc"))
                    component_input_type = str(component.find("Input").get("type"))
                    if component_input_type == "ods":
                        component_input_type = IN_TYPE_ODS
                    elif component_input_type == "xml":
                        component_input_type = IN_TYPE_XML
                    else:
                        component_input_type = None
                        log_warn('Invalid input type "%s" for component "%s"' \
                                  % (component_input_type, component_name))
                        
                    # Get input file, remove trailing/leading quotes
                    component_input = str(component.findtext("Input")).strip('"')
                    component_input = cg.string_to_path(component_input)
                    # Check if path is relavite and make it aboslute based on project path
                    if component_input.is_absolute() is False:
                        component_input = self.paths["project_path"] / component_input
                        
                    component_params = {}
                    for param in component.findall("./Params/Param"):
                        param_name = param.get("name")
                        param_value = str(param.text)
                        if param_name is not None:
                            component_params.update( \
                                { param_name : param_value })
                        else:
                            log_warn('Invalid parameter name for component "%s".' % param_name)
                    
                    project_component = self.Component(component_type, component_input, \
                                                       component_input_type, component_params, \
                                                       component_name, component_desc)
                    
                    self.components.append(project_component)

                else:
                    log_warn('Component type is invalid: "%s".' % component_type)
    
            log_info('Loading project components completed.')
    
    #===============================================================================================    
    def load_component_data(self, component):
        """ Loads the data related to the given component and returns and object. """
        # Get module corresponding to the component type
        if component.type in self.components_definitions:
            # Get processing calvos module
            module_name = component.type
            # Add prefix for calvos package
            module_name = "calvos." + module_name
            # Call load_input function
            module = importlib.import_module(module_name)
            return_object = module.load_input(component.input_file_path, \
                                              component.input_type, component.params, self)
            component.component_object = return_object
        else:
            log_warn("Invalid component type: '%s'" % component.type)
            

    #===============================================================================================
    def generate_component_code(self, component, params = {}):
        """ Generate C-code for the given component. """
        # Check if component data has been loaded in
        if component.component_object is not None:
            # Call the code generation function for the object.
            if component.type in self.components_definitions:
                module_name = component.type
                # Add prefix for calvos package
                module_name = "calvos." + module_name
                # Call generate function
                module = importlib.import_module(module_name)
                module.generate(component.component_object, \
                         self.paths["project_output"], \
                         self.paths["project_working_dir"], \
                         self.paths["calvos_path"], \
                         params)
            else:
                log_warn("Invalid component type: '%s'" % component.type)
        else:
            log_warn("Component data has not been loaded in.") 
    
    #===============================================================================================
    def process_project(self):
        """ Processes all defined components for this project. 
        load_project function shall be called before this one. """
        log_info("============== Processing project components. ==============")
        for component in self.components:
            try:
                log_info('-------------- Loading data for component "%s" of type "%s".' \
                         % (component.name, component.type))
                self.load_component_data(component)
                
                log_info('-------------- Generating code for component "%s" of type "%s".' \
                         % (component.name, component.type))
                self.generate_component_code(component, component.params)
                
            except Exception as e:
                log_error('Failed to process component "%s". Reason: %s' % (component.name, e) )
                
    #===============================================================================================
    def simple_param_exists(self, component, param_id):
        """ Check if a component parameter is defined. """
        
        return_value = False
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                return_value = True
                
        return return_value
    
    #===============================================================================================
    def simple_param_is_read_only(self, component, param_id):
        """ Check if a component parameter is defined. """
        
        return_value = None
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                return_value = \
                    self.components_definitions[component].simple_params[param_id].read_only
                
        return return_value
    
    #===============================================================================================
    def validate_simple_param(self, component, param_id, validator_comp, validator_id):
        return_value = None
        if component in self.components_definitions \
        and param_id in self.components_definitions[component].simple_params \
        and validator_comp in self.components_definitions \
        and validator_id in self.components_definitions[component].simple_params:
            # See if param_id is within validator allowed values. Validator shall be of 'list'
            # type.
            if self.get_simple_param_type(validator_comp, validator_id) == 'list':
                param_val = \
                    self.components_definitions[component].simple_params[param_id].param_value
                validator_list = \
                 self.components_definitions[validator_comp].simple_params[validator_id].param_value
                if param_val in validator_list:
                    return_value = True
                else:
                    return_value = False
            else:
                log_warn("Validator param '%s' of component '%s' is not of 'list' type." \
                         % (validator_id, validator_comp))
            
        else:
            log_warn("Either param to be compared '%s:%s' or validator '%s:%s' not Found." \
                     % (component, param_id, validator_comp, validator_id))
        
        return return_value
    
    #===============================================================================================
    def get_simple_param_default(self, component, param_id):
        """ Get component parameter default value. """

        # Value of parameters in project's component_definitions is always the default           
        return self.get_simple_param_val(component, param_id)
    
    #===============================================================================================
    def get_simple_param_type(self, component, param_id):
        """ Get component parameter type. """
        
        return_value = None
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                return_value = \
                    self.components_definitions[component].simple_params[param_id].param_type
                
        return return_value
    
    #===============================================================================================
    def get_simple_param_validator(self, component, param_id):
        """ Get component parameter type. """
        return_value = None
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                return_value = \
                    self.components_definitions[component].simple_params[param_id].validator
                
        return return_value
    
    #===============================================================================================
    def get_simple_param_val(self, component, param_id, default = None):
        """ Gets a component parameter.
        Returns the provided default value if parameter is not found. """
        return_value = default
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                return_value = \
                    self.components_definitions[component].simple_params[param_id].param_value
            else:
                log_warn("Parameter '%s' not found in component type '%'." % (component, component))
        else:
            log_warn("Component '%s' is not defined. Parameter '%s' can't be retrieved." \
                     % (component, param_id))
        
        return return_value

    #===============================================================================================
    def update_simple_param(self, component, param_id, write_value):
        """ Updates a component parameter if existing. """
        if component in self.components_definitions:
            if param_id in self.components_definitions[component].simple_params:
                self.components_definitions[component].simple_params[param_id].param_value \
                    = write_value
            else:
                log_warn("Parameter '%s' not found in component type '%'." % (component, component))
        else:
            log_warn("Component '%s' is not defined. Parameter '%s' can't be updated." \
                     % (component, param_id))
                
              
    #===============================================================================================        
    class CompDefinition:
        """ Class for modeling a calvos project component definition. """
        def __init__(self, type_name, **kwargs):
            self.type = type_name
            
            self.title = kwargs.get('title', None)
            self.desc = kwargs.get('desc', None)
            self.instances = kwargs.get('instances', None)
            self.params = kwargs.get('params', None)
            self.simple_params = {} # {param_id, SimpleParam object}
    
    #===============================================================================================
    class Component:
        """ Models a calvos project Component. """
        
        #===========================================================================================
        def __init__(self, type_name, \
                     input_file = None, input_type = IN_TYPE_XML, params = {}, \
                     name = None, desc = None):
            
            self.name = str(name)
            self.desc = str(desc)
            
            self.type = type_name
            
            self.input_type = input_type
            self.input_file = input_file
            self.input_file_path = None
            
            if input_file is not None:
                self.set_input(input_file, input_type)
                
            self.params = params
            
            self.component_object = None
            
            self.simple_params = {}
        
        #===========================================================================================    
        def load_param_defaults(self):
            """ Loads the default parameter values for this component type. """
            pass
        
        #===========================================================================================    
        def add_param(self, param_name, value):
            """ Adds/updates the given parameter to this component. """
            self.params.update({param_name : value})
        
        #===========================================================================================
        def remove_param(self, param_name):
            """ Removes a parameter from this component. """
            result = self.params.pop(param_name, None)
            if result is None:
                log_warn("Parameter \"%s\" to be removed didn't exist." % param_name)
        
        #===========================================================================================
        def set_input(self, input_file, input_type = IN_TYPE_XML):
            """ Sets the input file and input type for this component. """
            self.input_file_path = cg.string_to_path(str(input_file))
            
            if input_type == IN_TYPE_XML or input_type == IN_TYPE_ODS:
                self.input_type = input_type
            else:
                self.input_type = None
                log_error("Wrong input type parameter for project component: %s" % input_type)
        
        #===========================================================================================
        def set_object(self, component_object):
            self.component_object = component_object       