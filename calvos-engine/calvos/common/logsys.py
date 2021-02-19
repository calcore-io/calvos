# -*- coding: utf-8 -*-
""" CalvOS Logging System.

Module for the logging system use across all calvos engine.

@author: Carlos Calvillo
@copyright:  2020 Carlos Calvillo. All rights reserved.
@license:    GPL v3
"""
__version__ = '0.1.0'
__date__ = '2020-12-06'
__updated__ = '2020-12-06'

import logging
import traceback
from lxml import etree as ET
import xml.dom.minidom

# #TODO: Solve how to pass the log file path dynamically to this module

# A new handler to store "raw" LogRecords instances
class LogRecordsHandler(logging.Handler):
    """
    A handler class which stores LogRecord entries in a list
    """
    def __init__(self, records):
        """
        Initiate the handler
        :param records_list: a list to store the LogRecords entries
        """
        self.records = records
        super().__init__()

    def emit(self, record):
        self.records.append(record)
        

class Logger:
    """ A class for modeling a logger.
    """
    def __init__(self, name, level = logging.DEBUG, \
             abstract_list = None, to_console = True, \
             file_handler = None, formatter_str = None):
        
        self.name = name
        self.level = level
        self.to_console = to_console
        self.abstract_list = abstract_list
        
        #Create logger
        self.logger = logging.getLogger(name)
        self.logger.setLevel(level)

        if formatter_str is not None:
            self.formatter = logging.Formatter(formatter_str)
        else:
            self.formatter = None
        
        #Add Handlers
        if to_console is True:
            pass
#            self.console_handler = logging.StreamHandler(sys.stdout)
#            if self.formatter is not None:
#                self.console_handler.setFormatter(formatter)
#            self.logger.addHandler(self.console_handler)
        if type(abstract_list) is list:
            self.logger.addHandler(LogRecordsHandler(abstract_list))
        if file_handler is not None:
            self.file_handler = file_handler
            if self.formatter is not None:
                self.file_handler.setFormatter(self.formatter)
            self.logger.addHandler(self.file_handler)
        

class Log:
    
    def __init__(self, level = logging.DEBUG, \
          log_file = None, formatter_str = None, \
          to_console = True, to_abstract = False):
    
        """ Class to model a log system for a calvOS project """
        self.level = level
        self.to_console = to_console
        
        self.loggers = {}   #{name, Logger object}
        
        self.abstract_logs = []
        
        self.default_formatter_str = \
            '[%(asctime)s] [%(name)s] [%(levelname)s] %(message)s'
        
        if formatter_str is None:
            self.formatter_str = self.default_formatter_str
        else:
            self.formatter_str = formatter_str
        self.formatter = logging.Formatter(formatter_str)
        
        self.file_handler = None 
        if log_file is not None:
            self.file_handler = logging.FileHandler(log_file)
            self.file_handler.setLevel(level)
            self.file_handler.setFormatter(self.formatter)
        
        self.counters = {"debug" : 0,
                         "info" : 0,
                         "warning" : 0,
                         "error" : 0,
                         "critical" : 0}
            
    def set_output_file(self, file_name):
        self.file_handler = logging.FileHandler(file_name)
    
    def set_formatter(self,formatter_str = None):
        if formatter_str is not None:
            self.formatter = logging.Formatter(formatter_str)
        else:
            self.formatter = self.default_formatter_str
            
    
    def set_abstract_logs(self, abstract_list):
        if type(abstract_list) is list:
            self.abstract_logs = abstract_list

    def add_logger(self, name, level = None, \
                   abstract_logs = None, \
                   to_console = None, file_handler = None, formatter_str = None):
        """ Adds a new logger to the logging system. """

        if level is None:
            level = self.level
        if abstract_logs is None:
            abstract_logs = self.abstract_logs
        if to_console is None:
            to_console = self.to_console
        if file_handler is None:
            file_handler = self.file_handler
        if formatter_str is None:
            formatter_str = self.formatter_str
                   
        self.loggers.update( {name : Logger(name, level, \
             abstract_logs, to_console, file_handler, formatter_str)} )
    
    def debug(self, name, message):
        """ """
        self.loggers[name].logger.debug(message)
        self.counters["debug"] += 1
    
    def info(self, name, message):
        """ """
        self.loggers[name].logger.info(message)
        self.counters["info"] += 1
    
    def warning(self, name, message):
        """ """
        self.loggers[name].logger.warning(message)
        self.counters["warning"] += 1
    
    def error(self, name, message):
        """ """
        self.counters["error"] += 1
        if traceback.format_exc().find("NoneType") == -1:
            trace = traceback.format_exc()
            print("ERROR: ", message)
            traceback.print_exc()
        else:
            trace = traceback.format_stack()
            print("ERROR: ", message)
            traceback.print_stack()
        message += "\nError traceback/callstack:\n"
        if type(trace) is list:
            for entry in trace:
                message += entry
        else:
            message += str(trace)
            
        self.loggers[name].logger.error(message)

    def critical(self, name, message):
        """ """
        self.loggers[name].logger.critical(message)
        self.counters["critical"] += 1
    
    def to_xml(self, output_file = None):
        #Generate Logs root node and its information
        XML_root = ET.Element("Logs")
        for record in self.abstract_logs:
            XML_level_1 = ET.Element("Logger")
            XML_level_1.text = record.name
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("Time")
            XML_level_1.text = record.asctime
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("Filename")
            XML_level_1.text = record.filename
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("lineNo")
            XML_level_1.text = record.lineno
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("functionName")
            XML_level_1.text = record.funcName
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("LevelName")
            XML_level_1.text = record.levelname
            XML_root.append(XML_level_1)
            XML_level_1 = ET.Element("Message")
            XML_level_1.text = record.getMessage()
            XML_root.append(XML_level_1)

        print("Generating XML...")
        XML_string = ET.tostring(XML_root)
        
        XML_string = xml.dom.minidom.parseString(XML_string)
        XML_string = XML_string.toprettyxml()

        if output_file is None:
            print(XML_string)
        else:
            myfile = open(output_file, "w")
            myfile.write(XML_string)


log_system = None

