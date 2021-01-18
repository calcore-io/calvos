# Calvos Engine
This folder contains the source code for the calvos engine. Calvos engine is a python package called "calvos".

## Engine Modules
Following modules are currently defined within calvos engine.

Module Name | Python module | Description | prefix
------------|---------------|-------------|------------
CAN	|	comgen.CAN	| Implements a Controller Area Network (CAN) interaction layer. | can
project	|	common.project	| Implements all the handling of a calvos project. | proj
logsys	|	common.logsys	| Logging system to use across all calvos engine modules. Produces a .log file with info/warning/error messages during the processing of a project. | proj
general	|	common.general	| Defines general symbols to use across all calvos engine modules. | grl
codegen |	common.codegen	| General utilities for the C-code generation. | cg
time	| utils.time	| Implement utilities for time management. | tmr

_Note:_ Prefixes are used as namespaces for the XML/XSD and when importing the modules in python.
