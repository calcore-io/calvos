'''
Created on 13 ene. 2021

@author: Carlos Calvillo
'''
import calvos.common.codegen as cg
import calvos.common.logsys as lg

# --------------------------------------------------------------------------------------------------
# Definitions for the logging system
# --------------------------------------------------------------------------------------------------
LOGGER_LABEL = "cog_CAN"

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
 
def C_gen_info(worksheet, network_object):
    """ Returns a C-comment with network information. """
    # Adjust file string "worksheet" to do not exceed 80 characters and to start at 
    # a given character
    file_str = cg.str_to_multiline(worksheet, (80-22), 22)   
    # Print generation information
    curren_time = cg.get_local_time_formatted() # Gets current local time
    return_str = """/*-----------------------------------------------------------------------------
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): """ + curren_time +"""
 * Generated from following source(s):
 *     Network file: \""""+ str(file_str) +"""\"
 *     Network name: \""""+ str(network_object.name) +"""\"
 *     Network id: \""""+ str(network_object.id_string) +"""\"
 *     Network date: \""""+ str(network_object.date) + """\"
 *     Network version: \""""+ str(network_object.version) +"""\"
 -----------------------------------------------------------------------------*/"""
    return return_str