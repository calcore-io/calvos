#!/usr/local/bin/python2.7
# encoding: utf-8
'''
calvos.main -- shortdesc

calvos.main is a description

It defines classes_and_methods

@author:     Carlos Calvillo

@copyright:  2020 Carlos Calvillo. All rights reserved.

@license:    GPL v3

@contact:    calcore.io@gmail.com
@deffield    updated: Updated
'''

import sys
import os
import pathlib as pl
import traceback

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter

__all__ = []
__version__ = 0.1
__date__ = '2020-12-18'
__updated__ = '2020-12-18'

DEBUG = 0
TESTRUN = 0
PROFILE = 0

class CLIError(Exception):
    '''Generic exception to raise and log different fatal errors.'''
    def __init__(self, msg):
        super(CLIError).__init__(type(self))
        self.msg = "E: %s" % msg
    def __str__(self):
        return self.msg
    def __unicode__(self):
        return self.msg

def string_to_path(path_string):
    ''' Returns a "path" object for the given string. '''
    # pathlib requires regular diagonal, not reverse diagonal
    path_string = path_string.replace("\\", "/")
    
    return pl.Path(path_string)
    
    

def folder_exists(path):
    ''' Returns "True" if the given path is an existing folder. '''
    return_value = False
    
    if type(path) is str:
        path = string_to_path(path)
     
    if path.exists() and path.is_dir():
        return_value = True
    
    return return_value

def file_exists(path):
    ''' Returns "True" if the given path is an existing file. '''
    return_value = False
    
    if type(path) is str:
        path = string_to_path(path)
     
    if path.exists() and path.is_file():
        return_value = True
    
    return return_value  
    

def main(argv=None): # IGNORE:C0111
    '''Command line options.'''

    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "v%s" % __version__
    program_build_date = str(__updated__)
    program_version_message = '%%(prog)s %s (%s)' % (program_version, program_build_date)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_license = '''%s
    
  CalvOS. Open Source SW Utilitites for Embedded Systems.
  Copyright (C) 2020  Carlos Calvillo
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.  

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

USAGE
''' % program_shortdesc

    try:
        # Setup argument parser
        parser = ArgumentParser(description=program_license, formatter_class=RawDescriptionHelpFormatter)
        parser.add_argument("-r", "--recursive", dest="recurse", action="store_true", help="recurse into subfolders [default: %(default)s]")
        parser.add_argument("-v", "--verbose", dest="verbose", action="count", help="set verbosity level [default: %(default)s]")
        parser.add_argument("-i", "--include", dest="include", help="only include paths matching this regex pattern. Note: exclude is given preference over include. [default: %(default)s]", metavar="RE" )
        parser.add_argument("-e", "--exclude", dest="exclude", help="exclude paths matching this regex pattern. [default: %(default)s]", metavar="RE" )
        parser.add_argument('-V', '--version', action='version', version=program_version_message)
        #parser.add_argument(dest="paths", help="paths to folder(s) with source file(s) [default: %(default)s]", metavar="path", nargs='+')
        
        parser.add_argument("-p","--project", dest="project", help="Full path with file name of the calvos project to be processed")
        parser.add_argument("-calvos","--calvos", dest="calvos", help="Path where the calvos python package is located")

        # Process arguments
        args = parser.parse_args()

        #paths = args.paths
        verbose = args.verbose
        recurse = args.recurse
        inpat = args.include
        expat = args.exclude
        
        project = args.project
        calvos = args.calvos
        
        arguments_OK = True
        
        # Add calvos python package
        if calvos is not None:
            calvos_path = string_to_path(calvos)
            if folder_exists(calvos_path):
                calvos_path_str = str(calvos_path)
                print("INFO: Adding calvos to python sys.path...")
                if calvos_path_str not in sys.path:
                    sys.path.append(str(calvos_path))
                    print("INFO: calvos path added")
                else:
                    
                    print("INFO: calvos path already added")
            else:
                arguments_OK = False
                print("Error: folder: ", calvos_path, " doesn't exists.")
        else:
            # Gather calvos package path. It will be needed later on for finding
            # cog files, etc.
            calvos_entry_found = False
            for entry in sys.path:
                if entry.find("calvos"):
                    calvos_entry_found = True
                    calvos_path = string_to_path(entry)
                    break
                    #TODO: Test that statements above work as expected to extract calvos path
            if calvos_entry_found is False:
                arguments_OK = False
                print("Error: not finding calvos package path")
        
        print(calvos_path)
        


        if project is not None:
            project_file = string_to_path(project)
            if file_exists(project_file):
                # Extract project path
                project_path = project_file.parent.resolve()
                print("Project path: ", project_path)
            else:
                arguments_OK = False
                print("Error: file: ", project_file, " doesn't exists.")
         
        if arguments_OK is True:
            #==============================================================================
            # Setup logging system
            #==============================================================================
            import calvos.common.logsys as lg
            import logging
             
            log_output_file = project_path / "log_test.log"
            if file_exists(log_output_file):
                log_output_file.unlink()
             
            lg.log_system = lg.Log(logging.DEBUG, log_output_file)
            log = lg.log_system
            log.add_logger("main")
            log.info("main","======= Started calvOS project processing. =======")
             
            #==============================================================================
            # calvOS project folders
            #==============================================================================           
            project_path_user_inputs = project_path / "usr_in"
            project_path_output = project_path / "out"
            project_path_working_dir = project_path / "out/working_dir"
            project_path_docs = project_path / "out/doc"
            project_path_templates = project_path / "out/doc/templates"
            
            
            import calvos.common.codegen as cg
            #==============================================================================
            # Setup needed calvOS project folders
            #==============================================================================
            log.info("main","Setting up project folders.")
            #Delete output and working directories
            cg.delete_folder_contents(project_path_output)
              
            # Recreate output folder structure
            cg.create_folder(project_path_output)
            cg.create_folder(project_path_working_dir) 
            cg.create_folder(project_path_docs) 
            cg.create_folder(project_path_templates)  

            
            #==============================================================================
            # Process project
            #==============================================================================
            import calvos.common.project as pj

            log.info("main","======= Loading project infrastructure. =======")
            
            log.info("main",'Create instance for project in "%s"' % project_file)
            calvos_project = pj.Project("Project Name", project_file, calvos_path)
            
            calvos_project.load_project()
            
            calvos_project.process_project()
                   
                     
            log.info("main","======= Finished calvOS project processing. =======")
            for counter_name, counts in log.counters.items():
                log.info("main", "\"" + counter_name + "\" messages : " + str(counts))
              
              
            logging.shutdown()
   
                    
#         if verbose > 0:
#             print("Verbose mode on")
#             if recurse:
#                 print("Recursive mode on")
#             else:
#                 print("Recursive mode off")

        if inpat and expat and inpat == expat:
            raise CLIError("include and exclude pattern are equal! Nothing will be processed.")

#         for inpath in paths:
#             ### do something with inpath ###
#             print(inpath)
        return 0
    except KeyboardInterrupt:
        ### handle keyboard interrupt ###
        return 0
    except Exception as e:
        if DEBUG or TESTRUN:
            raise(e)
        indent = len(program_name) * " "
        
        message = program_name + ": " + repr(e) + "\n"
        message = message + traceback.format_exc()
        sys.stderr.write(program_name + ": " + repr(e) + "\n")
        sys.stderr.write(indent + "  for help use --help")
        
        log.critical("main",message)
        
        string_ms = traceback.print_exc()
        
        return 2

if __name__ == "__main__":
    if DEBUG:
        sys.argv.append("-h")
        sys.argv.append("-v")
        sys.argv.append("-r")
    if TESTRUN:
        import doctest
        doctest.testmod()
    if PROFILE:
        import cProfile
        import pstats
        profile_filename = 'calvos.main_profile.txt'
        cProfile.run('main()', profile_filename)
        statsfile = open("profile_stats.txt", "wb")
        p = pstats.Stats(profile_filename, stream=statsfile)
        stats = p.strip_dirs().sort_stats('cumulative')
        stats.print_stats()
        statsfile.close()
        sys.exit(0)
    sys.exit(main())