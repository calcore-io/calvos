# CalvOS Project

Open Source SW Utilities for Embedded Systems

<img src="doc/templates/logo_full_bw_small.png">

## Overview

CalvOS system is an open-source (GPL v3 license) set of software utilities for the development of embedded systems in C programming language. CalvOS generates C-code based on user defined inputs for "middleware" SW layers sitting above Hardware Abstraction Layer (HAL) and below Application Layer. This in order to speed up the development of embedded SW applications.

## Motivation

Some level of HAL layers are typically provided by microcontroller manufacturers (e.g., ConfigTools by NXP) or via third party suppliers (e.g., Arduino via "wire" libraries) easing the configuration of MCU features/peripherals or board support packages for development kits, etc. However, is not common to find open source/free SW utilities sitting above such HAL layers.

Hence, the motivation for calvos is to provide a set of SW utilities that cover some aspects of those layers above HAL in order to ease the implementation of final embedded SW applications.

## Scope

The initial scope of calvos is an "interaction layer" (IL) generator for the Controller Area Network (CAN) communication protocol which is widely used in the automotive industry.

This interaction layer automatically generates C-code with APIs and functions to ease the reading/writting, reception/transmission of signals in an user defined CAN network (a template for the definition such CAN network is provided).

The generated code is aimed to be easily ported by providing a set of APIs where the user needs to instrument code for the integration of the interaction layer into the target's MCU HAL. Also, the generated code provides APIs for direct interaction with CAN signals of the defined network by the application layers.

Initial version of the CAN IL is available (beta version). 

Future developments will follow in this intended order:

- SW Timers
- De-bouncing for digital inputs/signals
- Basic filtering for analog inputs
- Local Interconnect (LIN) protocol interaction layer
- Generic UART interaction layer
- CAN Transport Protocol
- Basic set of UDS Diagnostics (ISO 14229)

## User Definitions

Currently, the user needs to provide its definitions to the calvos-engine (generator of C-code) using a set of spreadsheet templates (ODS format by default) and then create a calvos project (an XML file).

Calvos-engine is organized in components each in charge of determined feature. For example, the component in charge of the CAN interaction layer generation is named "comgen.CAN" and an specific ODS template is provided for the user to design the required CAN network.

Future plans include the development of an user interface (for example in the form of an Eclipse IDE plugin or an standalone GUI) for easing the creation of projects. 

## CalvOS System

CalvOS system will be composed by two main components: calvOS Engine and calvOS User Interface (UI). Now only calvOS Engine is implemented which is controlled via command-line arguments.

 - CalvOS Engine: is the main component processing the user inputs and auto-generating the corresponding C-code. It is mainly developed in python and as such it is a python package named “calvos”.
 - CalvOS UI: will provide an user friendly interface for creating and customizing projects that will, in turn, be processed by the calvOS Engine. CalvOS UI is planned to be an Eclipse IDE plug-in. 

## Usage

### Installing Calvos

#### From PyPi

Calvos engine python package is indexed at PyPi and, therefore, can be installed with a `pip` command from python:

`python -m pip install calvos`

Alternatively, it can be installed "manually" by downloading the code and installing it locally. See section below.

#### Manual Installation

Here we'll exemplify the installation of a calvos under MS Windows.

1. Ensure python v3.7 or greater is installed

2. Create a temporal folder *c:\\calvos* (you can use any drive letter and folder name as desired).

3. Get source code from https://github.com/calcore-io/calvos

4. Unzip (if zip was downloaded) and copy the downloaded files to the temporal folder *c:\calvos*.

   Folder structure should look as follows (not all contents are shown, just an example)...

   ```
   c:\calvos
   +-- calvos-engine
   |   +-- calvos
   |       +-- comgen
   |       +-- common
   |       +-- doc
   |       +-- __init__.py
   |       +-- __main__.py
   |   +-- .project
   |   +-- .pydevproject
   |   +-- README.md
   +-- doc
   +-- project_example
   +-- LICENSE
   +-- README.md
   ```

5. Get into the *calvos/calvos-engine* folder

   `c:\>cd calvos\calvos-engine`

6. Run python setup.py install command:

   `c:\calvos\calvos-engine>python setup.py install`

7. Verify calvos was installed properly by getting the calvos version.

   `c:\>python -m calvos -v`

   Version should like like: `calvos v0.0.5`

### Command Line Arguments

Calvos accepts the following command-line arguments:

| Argument                      | Usage                                                        |
| ----------------------------- | ------------------------------------------------------------ |
| -h, --help                    | show help message and exit.                                  |
| -d DEMO, --demo DEMO          | Will provide an example calvos project with user input templates in the given DEMO path and will process it. Argument -p is ignored if this argument is provided. |
| -p PROJECT, --project PROJECT | Required (if -d was not provided). Full path with file name of the calvos project to be processed. |
| -l LOG_LEVEL, --log LOG_LEVEL | Optional. LOG_LEVEL: 0 - Debug, 1 - Info, 2 - Warning, 3 - Error. Default is 1 - Info. |
| -e EXPORT, --export EXPORT    | Optional. Generated C-code will be exported (copied) into the provided EXPORT path. |
| -b BACKUP, --backup BACKUP    | Optional. Backups of the overwritten C-code during an export operation will be placed in the provided BACKUP path. This is only used if -e argument was provided. |
| -V, --version                 | show program's version number and exit                       |
| -v, --ver                     | show program's version number and exit                       |

### Demonstration Project

As a starting point, a demonstration project can be generated by the calvos system and exported to a given path. This is achieved by using the line argument `-d` as in the example below:

`python -m calvos -d c:\demo_project`

This will create the following files:

- **calvos_project.xml**: XML project file

  location: *c:\\demo_project\\calvos_project.xml*

- **template - CAN Network Definition.ods**: Template for CAN Network definition

  location: *c:\\demo_project\\usr_in\\template - CAN Network Definition.ods*

- **log.log**: Log file

  location: *c:\\demo_project\\log.log*

- Folder "**out**": Contains the generated C source code for the demo project  

It is useful to get this demo project since by doing so, also the template for the CAN network definition is exported.

In order to generate the code of the demo project, run the following command:

`python -m calvos -p c:\demo_project\calvos_project.xml`

If everything went Ok then the generated source code shall be located in the default output folder:

- *c:\\demo_project\\out*

Information about the project processing and possible warnings/errors found during the processing can be consulted in the generated log file located in the project's *root* folder:

- *c:\\demo_project\\log.log*

## CAN Interaction Layer User Guide

Refer to document "[comgen_CAN - Integration Manual.md](https://github.com/calcore-io/calvos/blob/main/calvos-engine/calvos/comgen/doc/comgen_CAN%20-%20Integration%20Manual.md)" for user guide on the integration of a CAN IL with Calvos.
