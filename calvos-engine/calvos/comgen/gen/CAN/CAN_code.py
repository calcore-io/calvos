'''
Created on 9 ene. 2021

@author: uidw7826
'''
import calvos.common.codegen as cg
 
def C_gen_info(worksheet, network_object):
    """ Returns a C-comment with network information. """
    # Print generation information
    curren_time = cg.get_local_time_formatted() # Gets current local time
    return_str = """/*-----------------------------------------------------------------------------
 * This file was generated on (mm.dd.yyyy::hh:mm:ss): """ + curren_time +""")
 * Generated from following source(s):
 *     Network file: \""""+ str(worksheet) +"""\"
 *     Network name: \""""+ str(network_object.name) +"""\"
 *     Network id: \""""+ str(network_object.id_string) +"""\"
 *     Network date: \""""+ str(network_object.date) + """\"
 *     Network version: \""""+ str(network_object.version) +"""\"
 -----------------------------------------------------------------------------*/"""
    return return_str