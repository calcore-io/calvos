# calvos
<img src="doc/templates/logo_full_bw_small.png">

Open Source SW Utilities for Embedded Systems

## Overview
CalvOS system is an open-source (GPL v3 license) software development framework for embedded systems in C programming language. CalvOS auto-generates C code based on user defined inputs for aspects like network interaction layers, input processing, output generation, filtering, MCU configurations, operating system integration, etc.
This generated source code can be incorporated into the rest of the user’s application code to form the final desired application.
Main goal for the calvOS system is to provide a set of tools for speeding-up the development of embedded systems.
CalvOS system will be composed by three main components: calvOS Engine, calvOS User Interface (UI) and calvOS Deployment System.
 - CalvOS Engine: is the main component processing the user inputs and auto-generating the corresponding C-code. It is mainly developed in python and as such it is a python package named “calvos”.
 - CalvOS UI: provides an user friendly interface for creating and customizing projects that will, in turn, be processed by the calvOS Engine. CalvOS UI is planned to be presented in two forms: an stand-alone graphical user interface (GUI) and an Eclipse IDE plug-in. Both options will offer the same capabilitites for interacting with the calvOS Engine so it is up-to the user to select wich one better integrates its development flow.
 - CalvOS Deployment System: This component is in charge of generating the calvos python package and UI project so that it is easily distributed.
