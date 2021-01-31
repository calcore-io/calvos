# -*- coding: utf-8 -*-
""" CalvOS Project Manager.

Module for managing a calvos project.

@author: Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.1.0'
__date__ = '2020-11-12'
__updated__ = '2020-11-12'

import xml.etree.ElementTree as ET
import pathlib
import importlib
import json
import re
import pickle

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
        
        self.simple_params = {}
        
        self.module = "common.project"
        
        
        
        self.test_dict = {"hola1" : 1, "hola2" : 2, "hola3" : 3, "hola4" : 4, "hola5" : 5}
        self.test_list = ["lst1", "lst2", "lst3", "lst4", "lst5"]
        self.test_var = 100
    
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
                        log_debug("Validating validator '%s'..." % param_validator)
                        param_validator = grl.process_simple_param('list', param_validator)
                        if param_validator is None:
                            # If there is a problem converting validator assume invalid
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
        
        log_info("Loading of definitions for component '%s' completed." % component)   
   
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
        for path in sorted(_calvos_path.rglob('*.py')):
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
        calvos_components = self.find_components(self.calvos_path)
        
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
            log_info("============== Validating parameters' default data ==============")
            valid = self.validate_simple_params_defaults()
            if valid is False:
                log_warn("Invalid data found during parameters validation.")
                #TODO. determine wheter or not to continue processing, success = True/False? 
        
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
            
            log_info("============== Resolving project paths. ==============")   
            self.resolve_paths()
            log_info("Paths expansion completed.") 
    
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
                # Loads user input and creates a component specific object. Such object will be
                # set in component.component_object
                self.load_component_data(component)
            except Exception as e:
                log_error('Failed to load user input for component "%s". Reason: %s' \
                          % (component.name, e) )

        log_info("============== Generating components code. ==============")
        
        
        test_str = ["rew ert wer\"$v:calvos_path$\" wert ewr", \
"wert er\"$d:test_dict,hola1$\"  erte rt \"$d:test_dict,hola1$\" tre w", \
" wert \"$func:function_name$\" tttt", \
"trey rt\"$d:test_dict,hola2$\" ttt", \
" rtyert \"$l:test_list,3$\"ttttt", \
"rrty \"$d:test_dict,hola1$\"asda\"$v:calvos_path$\"sdasd\"$l:test_list,4$\"asaaa asd  sa a dsa da\"$d:test_dict,hola4$\"", \
"hello \"$project_working$\""]
        
#        for string in test_str:
#            self.expand_all_tokens(string)
  
        # Create a pickle of all project object to be available for source generators.
        pickle_full_file_name = self.get_simple_param_val(self.module, "project_path_working") / \
            self.get_simple_param_val(self.module, "project_pickle")
        try:
            with open(pickle_full_file_name, 'wb') as f:
                pickle.dump(self, f, pickle.HIGHEST_PROTOCOL)
        except Exception as e:
                print('Failed to create pickle file %s. Reason: %s' \
                      % (pickle_full_file_name, e))
        
        
        for component in self.components:
            try:
                log_info('-------------- Generating code for component "%s" of type "%s".' \
                         % (component.name, component.type))
                self.generate_component_code(component, component.params)
                
            except Exception as e:
                log_error('Failed to generate code for component "%s". Reason: %s' \
                          % (component.name, e) )
    
    #===============================================================================================
    def validate_given_simple_params(self, component, simple_params_list):
        """ .
        
        simple_params_list shall be a list of SimpleParam objects
        """
        all_valid = True
        valid = True
        if len(simple_params_list) > 0:
            for param in simple_params_list:
                param_id = param.param_id
                validator = param.validator
                if validator is not None:
                    log_debug("Validating parameter '%s' against its validator '%s:%s'..." \
                              % (param_id, validator[0], validator[1]))
                    valid = self.validate_simple_param(component, param_id, \
                                                       validator[0], validator[1])
                    if valid is True:
                        log_debug("Valid data.")
                    else:
                        all_valid = False
        else:
            log_debug("List of parameters to validate is empty.")
        
        return all_valid
    
    #===============================================================================================
    def validate_simple_params_defaults(self):
        """. """
        # Validate local parameters
        valid = self.validate_given_simple_params(self.module, self.simple_params.values())
        if valid is True:
            # Check each component definition's parameters
            for component in self.components_definitions.values():
                valid = self.validate_given_simple_params(component.type, \
                                                         component.simple_params.values())
                if valid is False:
                    break
        
        return valid
    
    #===============================================================================================
    def validate_comp_params(self):
        """ Validates defined component's data. """
        
        valid = True
        for component in self.components.values():
            valid = self.validate_given_simple_params(component.type, \
                                                     component.simple_params.values())
            if valid is False:
                break
        
        return valid
        
                          
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
                    log_warn("Invalid data. Allowed values list: '%s'." % validator_list)
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
                log_warn("Parameter '%s' not found in component type '%s'." \
                         % (param_id, component))
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
    TOKEN_ERROR = 0
    TOKENS_NOT_FOUND = 1
    TOKENS_REPLACED = 2
    def expand_direct_tokens(self, input_str, module = None):
        """ Retrieves local data based on the input retrieve string.
        
        retrieve string can have combination of the following elements:
          - $v:variable_name$
          - $f:function_name$
          - $d:dict_name,key$
          - $l:list_name,idx$
          - $p:module,param_id$ --> get global param_id
          - $p:module,param_id$ --> get global param_id
          - $k:string_name$ --> get named string from self.simple_params["<module>_named_strings"]
          - $alias$ --> get a new retrieve string from self.simple_params["<module>_alias_data"]
        """
        # TODO: make function also be able to provide arguments, for example with **kwargs  
        if module is None:
            module = self.module
            
        token_error = False
        current_expansion = None
        
        # Resolve direct tokens
        # ---------------------
        # Search for direct tokens $token_type:token_value$
        regex = re.compile(r"(\$(v|f|d|l|p):(.+?)\$)")
        regex_matches = re.findall(regex,str(input_str))
        
        expanded_str = input_str
        
        for match in regex_matches:
            # Element TOKEN_TYPE corresponds to the token_type, element TOKEN_VALUE to token_value
            token_full = match[0]
            token_type = match[1]
            token_value = match[2]
            current_expansion = None
             
            if token_type == "v":
                log_debug("Processing variable token '$%s:%s$'..." % (token_type, token_value))
                if hasattr(self, token_value):
                    current_expansion = getattr(self, token_value)
                    log_debug("Token expanded to: '%s'" % current_expansion)
                else:
                    log_warn("Local variable '%s' not found in object" % (token_value))
            elif token_type == "f":
                # TODO: implement this
                pass
            elif token_type == "d":
                log_debug("Processing dictionary token '$%s:%s$'..." % (token_type, token_value))
                token_val_args = token_value.split(",")
                if len(token_val_args) == 2:
                    # Element zero of splitted string is the variable name, element one is the key
                    if hasattr(self, token_val_args[0]):
                        key_str = str(token_val_args[1])
                        dict_var = getattr(self, token_val_args[0])
                        if key_str in dict_var:
                            current_expansion = dict_var[key_str]
                            log_debug("Token expanded to: '%s'" % current_expansion)
                        else:
                            log_warn("In token '$%s:%s$', key '%s' doesn't exist." \
                                     % (token_type, token_value, str(key_str)))
                    else:
                        log_warn("Local variable '%s' not found. Returned 'None'." % (token_value))
                else:
                    log_warn(("In token '$%s:%s$' separator ',' is missing or has more than one." \
                             + " Returned 'None'.") % (token_type, token_value))
            elif token_type == "l":
                log_debug("Processing list token '$%s:%s$'..." % (token_type, token_value))
                token_val_args = token_value.split(",")
                if len(token_val_args) == 2:
                    # Element zero of splitted string is the variable name, element one is the index
                    if hasattr(self, token_val_args[0]):
                        index = int(token_val_args[1])
                        list_var = getattr(self, token_val_args[0])
                        if index < len(list_var):
                            current_expansion = list_var[index]
                            log_debug("Token expanded to: '%s'" % current_expansion)
                        else:
                            log_warn("In token '$%s:%s$', index '%s' is out of range." \
                                     % (token_type, token_value, str(index)))
                    else:
                        log_warn("Local variable '%s' not found. Returned 'None'." % (token_value))
                else:
                    log_warn(("In token '$%s:%s$' separator ',' is missing or has more than one." \
                             + " Returned 'None'.") % (token_type, token_value))
            elif token_type == "p":
                log_debug("Processing parameter token '$%s:%s$'..." % (token_type, token_value))
                token_val_args = token_value.split(",")
                if len(token_val_args) == 1:
                    # Element zero of splitted string is the local param_id.
                    current_expansion = self.get_simple_param_val(self.module, token_val_args[0])
                    log_debug("Token expanded to: '%s'" % current_expansion)
                elif len(token_val_args) == 2:
                    # Element zero of splitted string is the parameter module name, element one 
                    # is the param_id.
                    current_expansion = \
                        self.get_simple_param_val(token_val_args[0], token_val_args[1])
                    log_debug("Token expanded to: '%s'" % current_expansion)
                else:
                    log_warn(("In token '$%s:%s$' separator ',' appears more than once." \
                             + " Returned 'None'.") % (token_type, token_value))
            else:
                log_warn("In token '$%s:%s$', unknown token type '%s." \
                         % (token_type, token_value, token_type))
            
            # Substitute value in input string
            if current_expansion is None:
                token_error = True
                
            # If expansion has a single back slash \ need to use a temporal set of characters
            # like '#####' since it causes problems with regex 'sub' function.
            escaping_chars = '#####'
            escape_diag_regex = re.compile(r"\\(?!\\)")
            escaped_str = escape_diag_regex.sub(escaping_chars,str(current_expansion))
            # Perform replacements
            expanded_str = regex.sub(str(escaped_str),str(expanded_str), 1)
            # Un-scape back slashes if any was escaped.
            expanded_str = expanded_str.replace(escaping_chars, "\\")
        
        output_str = expanded_str
        
        if token_error == True:
            return_stat = self.TOKEN_ERROR
        elif current_expansion is None:
            return_stat = self.TOKENS_NOT_FOUND
        else:
            return_stat = self.TOKENS_REPLACED
            
        return {"status" : return_stat, "out_str" : output_str}
        
    #===============================================================================================
    def expand_alias_tokens(self, input_str, module = None):
        """ Expands alias tokens.
        
        input_str can have combination of the following elements:
          - $alias$ --> get a new retrieve string from self.simple_params["<module>_alias_data"]
        """
        
        if module is None:
            module = self.module
            
        component_name = module.split(".")
        component_name = component_name[-1]
        
        token_error = False
        retrieve_string = None
        
        expanded_str = input_str
        
        # Resolve aliased tokens
        # ----------------------
        # Search for aliased tokens $alias$. Alias name shall comply with C-identifier syntax.
        regex = re.compile(r"(\$([a-zA-Z_][a-zA-Z0-9_]*)\$)")
        regex_matches = re.findall(regex,str(input_str))
        
        for match in regex_matches:
            # new retrieve string will be taken from get_simple_param_val(<module>, alias_data)
            token_value = match[1]
            retrieve_string = None
            
            log_debug("Processing aliased token '$%s$'..." % token_value)
            
            # Get retrieve string
            if cg.is_valid_identifier(token_value):
                alias_dict_name = str(component_name)+"_alias_dict"
                if self.simple_param_exists(module, alias_dict_name):
                    alias_dict = self.get_simple_param_val(module, alias_dict_name)
                    if token_value in alias_dict:
                        retrieve_string = str(alias_dict[token_value])
                        log_debug("Token expanded to: '%s'" % retrieve_string)
                    else:
                        log_warn("Alias name '%s' not defined. Returned 'None'. " % token_value)
                else:
                    log_warn("Alias dictionary not found for component '%s'." % module)
            else:
                log_warn("Alias name '%s' shall comply with C-identifier syntax." % token_value)
            
            if retrieve_string is None:
                token_error = True

            # Substitute value in input string
            expanded_str = regex.sub(str(retrieve_string),expanded_str, 1)

        output_str = expanded_str
    
        if token_error == True:
            return_stat = self.TOKEN_ERROR
        elif retrieve_string is None:
            return_stat = self.TOKENS_NOT_FOUND
        else:
            return_stat = self.TOKENS_REPLACED
        
        return {"status" : return_stat, "out_str" : output_str}
    
    #===============================================================================================
    def expand_all_tokens(self, input_str, module = None):
        """ Expand all replacing tokens in the given string. """
        
        if module is None:
            module = self.module
        
        tokens_found = False
        token_error = False
        
        # First expand direct tokens then expand aliased tokens and finally expand again possible
        # direct tokens generated while expanding aliased tokens. The order is important since
        # one first replacement of direct tokens needs to occur before the aliased tokens to
        # avoid confusing 'non-token' text in between tokens as if they were aliased tokens.
        log_debug("Expanding string: '%s'..." % input_str)
        return_value = self.expand_direct_tokens(input_str, module)
        if return_value["status"] == self.TOKENS_REPLACED:
            tokens_found = True
            log_debug("String with direct tokens replaced: '%s'" % return_value["out_str"])
        elif return_value["status"] == self.TOKEN_ERROR:
            token_error = True
        
        # Keep expanding all aliased/direct tokens until no one is found
        while_guard = grl.LOOP_GUARD_MED
        last_loop = False
        while while_guard > 0:
            while_guard -= 1
            if while_guard == 0:
                log_error(("Error expanding tokens. Search took %s loops without success " \
                          + "and was canceled.") % str(grl.LOOP_GUARD_MED))
                
            alias_tokens_this_loop = False
            direct_tokens_this_loop = False
                
            return_value = self.expand_alias_tokens(return_value["out_str"], module)
            
            if return_value["status"] == self.TOKEN_ERROR:
                token_error = True
                break
            elif return_value["status"] == self.TOKENS_REPLACED:
                tokens_found = True
                alias_tokens_this_loop = True
                log_debug("Expanded string: '%s'" % return_value["out_str"])
            
            # Process direct tokens resulting from expansion of aliased ones
            return_value = self.expand_direct_tokens(return_value["out_str"], module)
            if return_value["status"] == self.TOKEN_ERROR:
                token_error = True
                break
            elif return_value["status"] == self.TOKENS_REPLACED:
                tokens_found = True
                direct_tokens_this_loop = True
                
            if alias_tokens_this_loop is True or direct_tokens_this_loop is True:
                last_loop = False
            elif alias_tokens_this_loop is False and direct_tokens_this_loop is False:
                if last_loop is True:
                    break
                else:
                    last_loop = True
            
            
        # Determine returning values
        if token_error == True:
            return_value["status"] = self.TOKEN_ERROR
            log_warn("Final expanded string with errors: '%s'" % return_value["out_str"])
        elif tokens_found == True:
            return_value["status"] = self.TOKENS_REPLACED
            log_debug("Final expanded string: '%s'" % return_value["out_str"]) 
        else:
            return_value["status"] = self.TOKENS_NOT_FOUND
            log_debug("No replacements performed.")
            
        return return_value
            
    #===============================================================================================
    def resolve_paths(self):
        """ Expands all parameters of type "path". """ 
        for param in self.simple_params.values():
            if param.param_type == "path":
                current_path = self.expand_all_tokens(param.param_value)["out_str"]
                current_path = cg.string_to_path(current_path)
                param.param_value = current_path                
                log_debug("Resolved path '%s': '%s'" % (param.param_id, param.param_value))
                
        for comp_def in self.components_definitions.values():
            for param in comp_def.simple_params.values():
                if param.param_type == "path":
                    current_path = self.expand_all_tokens(param.param_value)["out_str"]
                    current_path = cg.string_to_path(current_path)
                    param.param_value = current_path  
                    log_debug("Resolved component '%s' path '%s': '%s'" \
                              % (comp_def.type, param.param_id, param.param_value))
    
    #===============================================================================================
    def get_work_file_path(self, comp_id, param_id):
        """ Returns a Path to the working directory file from a parameter. 
        File name is extracted from a the parameter comp_id:param_id. """
        
        return_value = None
        work_dir = self.get_simple_param_val(self.module,"project_path_working")
        work_file = self.get_simple_param_val(comp_id,param_id)
        if work_dir is not None and work_file is not None:
            return_value = work_dir / work_file
        
        return return_value
    
    #===============================================================================================
    def get_component_gen_path(self, component_id):
        """ Returns the path with cog files for the given component. 
        
        Parameters
        ----------
            component_id, str
                String constituing the component name, for example: "utils.time" or
                "comgen.CAN", etc.
        """     
        gen_path_lst = component_id.split(".")
        module_name = gen_path_lst[-1]
        gen_path_val = cg.string_to_path(self.calvos_path)
        for i, element in enumerate(gen_path_lst):
            if i < len(gen_path_lst) - 1:
                gen_path_val = gen_path_val / element
        gen_path_val = gen_path_val / "gen" / module_name
        
        return gen_path_val
                          
    #===============================================================================================        
    class CompDefinition:
        """ Class for modeling a calvos project component definition. """
        def __init__(self, type_name, **kwargs):
            self.type = type_name
            
            self.title = kwargs.get('title', None)
            self.desc = kwargs.get('desc', None)
            self.instances = kwargs.get('instances', None)
            self.params = kwargs.get('params', None)
            self.simple_params = {} # {param_id : SimpleParam object}
    
    #===============================================================================================
    class Component:
        """ Models a calvos project Component. """
        
        component_idx = 0
        
        #===========================================================================================
        def __init__(self, type_name, \
                     input_file = None, input_type = IN_TYPE_XML, params = {}, \
                     name = None, desc = None):
            
            self.comp_idx = self.get_and_increase_comp_idx()
            
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
        @classmethod
        def get_and_increase_comp_idx(cls):
            """ Returns current class component index and then increase it by 1. """
            
            return_value = cls.component_idx
            cls.component_idx += 1
            return return_value
        
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