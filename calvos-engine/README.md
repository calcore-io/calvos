# Calvos Engine
This folder contains the source code for the calvos engine. Calvos engine is a python package called "calvos".

## Usage
Command Line Arguments.

Argument |  Usage
---------|-------
  -h, --help            | show help message and exit
  -p PROJECT, --project PROJECT | Mandatory. PROJECT: Full path with file name of the calvos project to be processed
  -c CALVOS, --calvos CALVOS  | CALVOS: Path where the calvos python package is located. If not provided, will look from installed python packages.
  -l LOG_LEVEL, --log LOG_LEVEL | Logging level LOG_LEVEL: 0 - Debug, 1 - Info, 2 - Warning, 3 - Error. Default is 1 - Info.
  -V, --version           | show program's version number and exit
  -v, --ver               | show program's version number and exit

## Engine Modules
Following modules are currently defined within calvos engine.

Module Name | Python module | Description | prefix
------------|---------------|-------------|------------
CAN	|	comgen.CAN	| Implements a Controller Area Network (CAN) interaction layer. | can
project	|	common.project	| Implements all the handling of a calvos project. | prj
logsys	|	common.logsys	| Logging system to use across all calvos engine modules. Produces a .log file with info/warning/error messages generated during the processing of a project. | log
general	|	common.general	| Defines general symbols to use across all calvos engine modules. | grl
codegen |	common.codegen	| General utilities for the C-code generation. | cg
time	| utils.time	| Implement utilities for time management. | tmr

_Note:_ Prefixes are used as namespaces for the XML/XSD and when importing the modules in python.

## Development Tools
### Python
Calvos engine requires python >= 3.7.
Following python modules are also required:
- cogapp
- lxml
- pyexcel
- pyexcel-ods

### Eclipse
Suggested development environment is Eclipse. Will need at least following plugins:
- PyDev: Python IDE for Eclipse
- CDT: Eclipse Embedded C/C++
- Eclipse XML Editors and Tools
