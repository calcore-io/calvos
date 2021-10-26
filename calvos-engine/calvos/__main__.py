#!/usr/local/bin/python2.7
# encoding: utf-8
'''
CalvOS - Open Source SW Utilitites for Embedded Systems.

Calvos provides a set of SW utilitites for the development
of embedded systems in C-language.

@author:     Carlos Calvillo

@copyright:  2020 Carlos Calvillo. All rights reserved.

@license:    GPL v3

@contact:    calcore.io@gmail.com
@deffield    updated: Updated
'''           
import sys
import os
import pathlib as pl
import shutil
import logging
import traceback
import csv

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter

__all__ = []
__date__ = '2021-10-08'
__updated__ = '2020-10-08'

DEBUG = 1
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

def create_folder(folder):
    """ Creates the specified folder.
    """
    if folder_exists(folder) is False:
        try:
            folder.mkdir()
        except Exception as e:
            print('Failed to create folder %s. Reason: %s' % (folder, e))
    else:
        print("Folder to be created '%s' already exists." % folder)
    

def main(argv=None): # IGNORE:C0111
    '''Command line options.'''
    # Import version from __version__.py file
    # Set calvos path which will be used for accessing package resources
    calvos_path = pl.Path(__file__).parent.absolute()
    version_path = calvos_path / "__version__.py"
        
    about = {}
    with open(version_path) as f:
        exec(f.read(), about)
    __version__ = about["__version__"]

    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "v%s" % __version__
    program_build_date = str(__updated__)
    program_version_message = 'calvos %s' % (program_version)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_license = '''%s
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
        parser = ArgumentParser(description=program_license, \
                                formatter_class=RawDescriptionHelpFormatter)
        parser.add_argument("-d","--demo", dest="demo", required=False, \
            help=("Will provide an example calvos project with user input templates in " \
                +"the given DEMO path and will process it. " \
                +"Argument -p is ignored if this argument is provided."))
        parser.add_argument('-V', '--version', action='version', version=program_version_message)
        parser.add_argument('-v', '--ver', action='version', version=program_version_message)
        parser.add_argument("-p","--project", dest="project", \
            required=(('--demo' not in sys.argv) and ('-d' not in sys.argv)), \
            help=("Required (if -d was not provided). Full path with file name of the calvos" \
                  + "project to be processed."))
        parser.add_argument("-l","--log", dest="log_level", required=False, \
            help=("Optional. LOG_LEVEL: 0 - Debug, 1 - Info, "\
                  + "2 - Warning, 3 - Error. Default is 1 - Info."))
        parser.add_argument("-e","--export", dest="export", required=False, \
            help="Optional. Generated C-code will be exported (copied) into the provided EXPORT path.")
        parser.add_argument("-b","--backup", dest="backup", required=False, \
            help=("Optional. Backups of the overwritten C-code during an export operation " \
                  + "will be placed in the provided BACKUP path. " \
                  + "This is only used if -e argument was provided."))
        parser.add_argument("--dont-export-user", dest="dont_export_user", action='store_true', \
                            required=False, \
            help=("Optional. If argument -e is provided and also this one, then the export " \
                  + "operation won't export 'USER_' files. Use with care."))
        

        # Process arguments
        args = parser.parse_args()
        project = args.project
        log_level = args.log_level
        
        arguments_OK = True
        
        # Check if calvos package is installed by trying to import the logging system
        try:
            import calvos.common.logsys as lg
        except:
            print("INFO: Setting up package...")
            # Add package path to sys.path and attempt the importing again
            calvos_pkg_path_str = str(calvos_path.parent)
            if calvos_pkg_path_str not in sys.path:
                sys.path.append(calvos_pkg_path_str)
                print("INFO: calvos path added to sys.path.")
            # Attempt the import
            try:
                import calvos.common.logsys as lg
            except Exception as e:
                print("ERROR: Couldn't load calvos package.")
                print("Generated exception:\n")
                print(str(e))
                return 2
        
        if log_level is not None:
            if int(log_level) == 0:
                log_level = logging.DEBUG
            elif int(log_level) == 1:
                log_level = logging.INFO
            elif int(log_level) == 2:
                log_level = logging.WARNING
            elif int(log_level) == 3:
                log_level = logging.ERROR
            else:
                arguments_OK = False
                print('Error: argument -l shall be from 0 to 3. Provided value: "%s"',log_level)
                return 2
        else:
            log_level = logging.INFO
            
        #==============================================================================
        # Generate demo project if argument -d is provided.
        #==============================================================================
        if args.demo is not None:
            # Check if path exists
            demo_path = string_to_path(args.demo)
            if folder_exists(demo_path):
                
                # Setting up logging system
                log_output_file = demo_path / "log.log"
                if file_exists(log_output_file):
                    log_output_file.unlink()
                lg.log_system = lg.Log(log_level, log_output_file)
                log = lg.log_system
                log.add_logger("main")
                
                # Create project object
                import calvos.common.project as pj
                
                log.info("main", \
                         "============== Generating Demo Project ==============")
                print("INFO: ============== Generating Demo Project ==============")
                calvos_project = pj.Project("Project Name", None, calvos_path)
                calvos_project.gen_demo_project(demo_path)
                print("INFO: Done. Demo project generated. ")
                log.info("main", "Done. Demo project generated. ")
            else:
                print("ERROR: Provided folder '%s' doesn't exist." % demo_path)
                return 2
                
            print("INFO: Don't provide argument -d if a project needs to be processed.")
            
            project = str(demo_path / "calvos_project.xml")
            #return 0    
        
        #==============================================================================
        # Start project processing if argument -d was not provided
        #==============================================================================
        if project is not None:
            project_file = string_to_path(project)
            if file_exists(project_file):
                # Extract project path
                project_path = project_file.parent.resolve()
                print("INFO: Project path: ", project_path)
            else:
                arguments_OK = False
                print("Error: file: ", project_file, " doesn't exists.")

        if arguments_OK is True:
            #==============================================================================
            # Setup logging system
            #==============================================================================
            # Setup logging system here only if argument -d was not provided, otherwise
            # logging system should've been already initialized during demo generation.
            if args.demo is None:
                log_output_file = project_path / "log.log"
                if file_exists(log_output_file):
                    log_output_file.unlink()
                 
                lg.log_system = lg.Log(log_level, log_output_file)
                log = lg.log_system
                log.add_logger("main")
            log.info("main","============== Started calvOS project processing. ==============")
            print("INFO: ============== Started calvOS project processing. ==============")
             
            #==============================================================================
            # calvOS project folders
            #==============================================================================           
            project_path_user_inputs = project_path / "usr_in"
            project_path_output = project_path / "out"
            project_path_working_dir = project_path / "out/working_dir"
            project_path_docs = project_path / "out/doc"
            project_path_templates = project_path / "out/doc/templates"
            
            import calvos.common.codegen as cg
 
            # Set global variables for code generation
            cg.calvos_path = calvos_path
            cg.calvos_project_path = project_path
            
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

            log.info("main","============== Loading project infrastructure. ==============")
            
            log.info("main",'Create instance for project in "%s"' % project_file)
            calvos_project = pj.Project("Project Name", project_file, calvos_path)
            
            calvos_project.load_project()
            
            calvos_project.process_project()
            
            #==============================================================================
            # Process export argument
            #==============================================================================
            if args.export is not None:
                log.info("main","============== Exporting generated code. ==============")
                print("INFO: Exporting generated C-code...")
                export_path = string_to_path(args.export)
                if cg.folder_exists(export_path):
                    # Get files to export
                    c_files = list(project_path_output.glob('*.c'))
                    h_files = list(project_path_output.glob('*.h'))
                    
                    # Check if argument --dont-export-user was provided
                    if args.dont_export_user is True:
                        # Remove user files, i.e., files starting with "USER_" so that they
                        # are not exported.
                        c_files_no_user = []
                        h_files_no_user = []
                        for c_file in c_files:
                            if str(c_file.name).startswith("USER_") is False:
                                c_files_no_user.append(c_file)
                        for h_file in h_files:
                            if str(h_file.name).startswith("USER_") is False:
                                h_files_no_user.append(h_file)
                        # Overwrite lists of files
                        c_files = c_files_no_user
                        h_files = h_files_no_user
                    
                    # If backup location is defined then do the backup
                    if args.backup is not None:
                        MAX_BKUP_COPIES = 10
                        current_bkup_copy = 0
                        create_header = False

                        log.info("main","-------------- Backing up files. --------------")
                        print("INFO: Backing up C-code to be overwritten during export...")
                        backup_path =  string_to_path(args.backup)
                        if export_path != backup_path:
                            if cg.folder_exists(backup_path) is False:
                                log.info("main", " Backup path '%s' doesn't exist, creating it..." \
                                         % backup_path)
                                cg.create_folder(backup_path)
                            # Check if history file exists
                            try:
                                history_file_name = "backup_history.csv"
                                history_fields = ['bkpu_set', 'store_date']
                                history_file = backup_path / history_file_name
                                if cg.file_exists(history_file):
                                    # Get last stored backup set
                                    with open(history_file, newline='') as csvfile:
                                        reader = csv.DictReader(csvfile, fieldnames=history_fields)
                                        for row in reader:
                                            current_bkup_copy = row['bkpu_set']
                                            
                                        #current_bkup_copy = int(writer[-1][history_fields[0]])
                                        current_bkup_copy = int(current_bkup_copy)
                                        # increment bkup copy
                                        if current_bkup_copy < MAX_BKUP_COPIES - 1:
                                            current_bkup_copy += 1
                                        else:
                                            current_bkup_copy = 0
                                else:
                                    # Create history file header
                                    create_header = True
                                    current_bkup_copy = 0
                            except Exception as e:
                                current_bkup_copy = MAX_BKUP_COPIES
                                print(("WARNING: Error accessing bkup history file (backup_history.csv). " \
                                      + "Backed up in set number %s.") % MAX_BKUP_COPIES)
                                log.warning(("Error accessing bkup history file (backup_history.csv). " \
                                      + "Backed up in set number %s.") % MAX_BKUP_COPIES)
                            
                            # Backup files to be overwritten
                            file_counter = 0
                            for file in c_files:
                                file_name = str(file.name)
                                file_name_dest = file_name
                                if current_bkup_copy > 0:
                                    file_name_dest = str(current_bkup_copy) + "_" +  file_name_dest
                                orig_file = export_path / file_name
                                dest_file = backup_path / file_name_dest
                                if cg.file_exists(orig_file):
                                    # Copy file to backup location
                                    shutil.copy(orig_file, dest_file)
                                    file_counter += 1
                            for file in h_files:
                                file_name = str(file.name)
                                file_name_dest = file_name
                                file_name_dest = str(current_bkup_copy) + "_" + file_name_dest 
                                orig_file = export_path / file_name
                                dest_file = backup_path / file_name_dest
                                if cg.file_exists(orig_file):
                                    # Copy file to backup location
                                    shutil.copy(orig_file, dest_file)
                                    file_counter += 1
                            
                            # Write history file with latest information
                            with open(history_file, 'a', newline='') as csvfile:
                                writer = csv.DictWriter(csvfile, fieldnames=history_fields)
                                if create_header:
                                    writer.writeheader()
                                writer.writerow({history_fields[0] : str(current_bkup_copy), 
                                                history_fields[1] : "\tCreated on " \
                                                + cg.get_local_time_formatted() \
                                                + ". " + str(file_counter) + " file(s) backed-up."})
                            
                            log.info("main", " Backup completed. '%s' file(s) backed-up." \
                                         % file_counter)
                            print("INFO: Backup completed. '%s' file(s) backed-up." % file_counter)
                        else:
                            arguments_OK = False
                            log.warning("main", "Provided Export and Backup folders are same. " \
                                + "Export operation cancelled.")
                    
                    # Export files
                    if arguments_OK is True:
                        log.info("main","-------------- Exporting files. --------------")
                        
                        file_counter = 0
                        for file in c_files:
                            file_name = file.name
                            orig_file = project_path_output / file_name
                            dest_file = export_path / file_name
                            if cg.file_exists(orig_file):
                                # Copy file to backup location
                                shutil.copy(orig_file, dest_file)
                                file_counter += 1
                        for file in h_files:
                            file_name = file.name
                            orig_file = project_path_output / file_name
                            dest_file = export_path / file_name
                            if cg.file_exists(orig_file):
                                # Copy file to backup location
                                shutil.copy(orig_file, dest_file)
                                file_counter += 1
                        
                        log.info("main", "Export completed. '%s' file(s) exported." % file_counter)
                        print("INFO: Export completed. '%s' file(s) exported." % file_counter)
                else:
                    print("ERROR: Export folder '%s' doesn't exist. No export performed." \
                             % export_path)
                    log.warning("main", "Export folder '%s' doesn't exist. No export performed." \
                             % export_path)
            
            log.info("main","============== Finished calvOS project processing. ==============")
            print("INFO: ============== Finished calvOS project processing. ==============")
            for counter_name, counts in log.counters.items():
                log.info("main", "\"" + counter_name + "\" messages : " + str(counts))
                print("INFO: \"" + counter_name + "\" messages : " + str(counts))
            print("INFO: See log file '%s' for details." % str(log_output_file))
              
            logging.shutdown()
   
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
        
        if 'log' in locals():
            log.critical("main",message)
        
        string_ms = traceback.print_exc()
        
        return 2

if __name__ == "__main__":
    if DEBUG:
        
        debug_calvos_path = pl.Path(__file__).parent.absolute()
        debug_demo_path = debug_calvos_path / "demo"
        debug_demo_project_path = debug_calvos_path / "demo" / "calvos_project.xml"
        create_folder(debug_demo_path)
        
        print(str(debug_demo_project_path))
        
        sys.argv.append("-p")
        sys.argv.append(str(debug_demo_project_path))
        
        sys.argv.append("-l")
        sys.argv.append("0")
  
#         sys.argv.append("-d")
#         sys.argv.append(str(debug_demo_path))
        
#         sys.argv.append("-e")
#         sys.argv.append("C:\\diplomado_udg\\s32ds_workspace\\DCU_2020B_EquipoX_3_testEngine0_5\\src\\DCU\\Communication\\Calvos")
# #          
#         sys.argv.append("-b")
#         sys.argv.append("C:\\diplomado_udg\\s32ds_workspace\\bkup")
# #          
#         sys.argv.append("--dont-export-user")
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