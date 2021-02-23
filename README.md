<img src="doc/templates/logo_full_bw_small.png">

Open Source SW Utilities for Embedded Systems

# Overview
CalvOS system is an open-source (GPL v3 license) set of software utilities for the development of embedded systems in C programming language. CalvOS generates C-code based on user defined inputs for "middleware" SW layers sitting above Hardware Abstraction Layer (HAL) and below Application Layer. This in order to speed up the development of embedded SW applications.

## Motivation

Some level of HAL layers are typically provided by microcontroller manufacturers (e.g., ConfigTools by NXP) or via third party suppliers (e.g., Arduino via "wire" libraries) easing the configuration of MCU features/peripherals or board support packages for development kits, etc. However, is not common to find open source/free SW utilities sitting above such HAL layers.

Hence, the motivation for calvos is to provide a set of SW utilities that cover some aspects of those layers above HAL in order to ease the implementation of final embedded SW applications.

## Scope

The initial scope of calvos is an "interaction layer" generator for the Controller Area Network (CAN) communication protocol which is widely used in the automotive industry.

This interaction layer automatically generates C-code with APIs and functions to ease the reading/writting, reception/transmission of signals in an user defined CAN network (a template for the definition such CAN network is provided).

The generated code is aimed to be easily ported by providing a clear set of APIs where the user needs to instrument code for the integration of the interaction layer into the target's MCU HAL. Also, the generated code provides APIs for direct interaction with CAN signals of the defined network by the application layers.

Current development progress of the CAN interaction layer is 90%. 

Future developments will follow in this intended order:

- SW Timers
- De-bouncing for digital inputs/signals
- Basic filtering for analog inputs
- Local Interconnect (LIN) protocol interaction layer
- Generic UART interaction layer
- CAN Transport Protocol
- Basic set of UDS Diagnostics (ISO 14229)

## User Definitions

Currently, the user needs to provide its definitions to the calvos-engine (generator of C-code) using a set of provided spreadsheet templates (ODS format by default) and by the creation of a calvos project (an XML file). Calvos-engine is organized in components each in charge of determined feature. For example, the component in charge of the CAN interaction layer generation is named "comgen.CAN" and an specific ODS template is provided for the user to design the required CAN network.

Future plans include the development of an user interface (for example in the form of an Eclipse IDE plugin or an standalone GUI). 

## CalvOS System

CalvOS system will be composed by two main components: calvOS Engine and calvOS User Interface (UI). Now only calvOS Engine is implemented which is controlled via command-line arguments.

 - CalvOS Engine: is the main component processing the user inputs and auto-generating the corresponding C-code. It is mainly developed in python and as such it is a python package named “calvos”.
 - CalvOS UI: will provide an user friendly interface for creating and customizing projects that will, in turn, be processed by the calvOS Engine. CalvOS UI is planned can be presented in two forms: an stand-alone graphical user interface (GUI) and/or an Eclipse IDE plug-in. 

# Usage

Refer to the following document for the usage of calvos for the generation of a CAN Interaction Later: [CAN Interaction Layer Generator.]([https://github.com/calcore-io/calvos/blob/engine-CAN-core/calvos-engine/calvos/comgen/doc/comgen_CAN%20-%20Integration%20Manual.md](https://github.com/calcore-io/calvos/blob/engine-CAN-core/calvos-engine/calvos/comgen/doc/comgen_CAN - Integration Manual.md))

An overview of the command-line arguments is listed below:

| Argument                      | Usage                                                                                                               |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| -h, --help                    | show help message and exit                                                                                          |
| -p PROJECT, --project PROJECT | Mandatory. PROJECT: Full path with file name of the calvos project to be processed                                  |
| -c CALVOS, --calvos CALVOS    | Optional. CALVOS: Path where the calvos python package is located. If not provided, will look from installed python packages. |
| -l LOG_LEVEL, --log LOG_LEVEL | Optional. Logging level LOG_LEVEL: 0 - Debug, 1 - Info, 2 - Warning, 3 -Error. Default is 1 - Info. |
| -e PATH, --export PATH | Optional. PATH: path where to export (copy) the generated C-code. |
| -b PATH, --backup PATH | Optional. PATH: path where backups of the overwritten C-code during an export C-code will be placed. This is only used if -e argument is provided. |
| -V, --version                 | show program's version number and exit |
| -v, --ver                     | show program's version number and exit |

