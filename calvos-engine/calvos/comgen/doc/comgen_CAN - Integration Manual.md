# Overview

This document presents a set of instructions to follow in order to integrate and use the generated code for CAN interaction layer in the target project.

# Generated Source Code

The generated source code is categorized in common code (common to all CAN entities), network specific code and node specific code.

## Common Code

Code common to all CAN generated source code. These set of files will be generated once regardless of the number of networks or nodes defined in the project.

| File                  | File contents                                                                                                                                            |
| --------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `comgen_CAN_common.h` | Contains definitions used across all CAN generated code. For example, common data structures, symbol definitions, internally used common functions, etc. |
| `comgen_CAN_common.c` | Implements common functionality for CAN. For example, logic for queuing transmission messages, binary search tree for received messages, etc.            |
|                       |                                                                                                                                                          |

## Network-specific Code

Code that needs to be generated per each defined network in the project. E.g., if two networks are defined, then two sets of these source code files will be generated, one per each network.

When referring to a file in this category or symbols within those files, the wildcard NWID (network ID) will be used in this documentation to represent the corresponding network ID. For example, if the defined network has an ID equal to 'C', then a flie referred as `comgen_CAN_NWID_network.h` corresponds to a generated file with name `comgen_CAN_C_network.h`. 

## Node-specific Code

Code that needs to be generated per each defined node within a network that has been selected for code generation (selected in parameter `CAN_gen_nodes`). A set of these files will be generated for each selected node in each defined network. These source code files will contain the corresponding node's name in the file names and symbols within them.

When referring to a file in this category, the wildcard NODEID (node name) will be used in this documentation to represent the corresponding node. For example, if the generated node name is 'NODE_1', then a flie referred as `comgen_CAN_NWID_NODEID_node_network.h` corresponds to a generated file with name `comgen_CAN_C_NODE_1_node_network.h`.

## Source Code Names Optimization

In order to avoid generating source code with long (and probably cumbersome) names (file names plus functions/data names inside them). A name "optimization" is implemented. This optimization is only active if parameter `CAN_gen_file_full_names` is set to FALSE.

If only one network is defined in the project then the NWID wildcard will be replaced by an empty string rather than the network's ID (if only one network is defined then there is no need to generate multiple network specific files and hence there is no need to disntinguish between the files and symbol names like in the case of having multiple networks).

If only one node is generated in a given network, the corresponding NODEID wildcard will be replaced by an empty string rather than the node's name.

For example, if only one network (network id: "C") is defined in the project and only a single node ("NODE_1") is to be generated, the file `comgen_CAN_NWID_NODEID_node_network.h`will be generated with the name `comgen_CAN_network.h`. However, if parameter `CAN_gen_file_full_names` is set to TRUE, the optmization won't take place and the generated file will have its full name: `comgen_CAN_C_NODE_1_node_network.h`.

**Note:** Optimization is also applied to the defined symbols within the generated files when applicable.

# Integration in Target Project

The integration is based in a per node basis. This means, each generated node needs to be integrated in the target's project. Hence, the hardware abstraction layer needs to be defined per each node.

It is assumed that a single node will own/use a single CAN peripheral in the target MCU. For example, if two nodes are generated for a single network then two CAN peripherals are required in the MCU, one per each node. Even though, this is the intention, user may decide to use one CAN peripheral for multiple nodes (not recommended).

## Initialization

Function `can_NWID_NODEID_coreInit()` needs to be called during initialization sequence of the SW. Header `comgen_CAN_NWID_NODEID_core.h` needs to be included in order to import this function.

## HAL Integration

For each node, a HAL integration is required for the reception and also for the transmission part of the code.

### HAL integration for reception

Following tasks are required in order to integrate the reception of CAN messages with the MCU's HAL (these steps need to be performed for each generated node):

1. Function `can_NWID_NODEID_HALreceiveMsg()` needs to be invoked within the HAL function that signals the reception of a CAN message for the associated CAN peripheral (typically within ISR context). If multiple nodes are generated, each of the generated functions need to be invoked from the corresponding CAN peripheral.
   
   Following arguments need to be provided:
   
   - `msg_id`: an unsigned integer containing the received message id.
   
   - `data_in`: a byte-pointer pointing to the place where the received data resides from the HAL layer. Data will be copied from this pointer into the generated reception data buffer `can_NWID_NODEID_RxDataBuffer`.
   
   - `data_len`: Length (number of bytes) of the received data. The data copy operation from the HAL to the reception buffer will be done for this amount of bytes in the case that the message is a valid rx message for the node.
   
   This function will perform a search in order to determine if the received message id belongs to the node (its a suscribed message for the node). If it is then the received data will be copied to the reception buffer, corresponding message available flags will be set and the message reception callback will be invoked.  

2. Write user-defined code within the reception message callback `can_NWID_NODEID_MESSAGEID_rx_callback()` if needed. MESSAGEID correspond to the received message name. These callbacks are defined in file `comgen_CAN_NWID_NODEID_callbacks.c`.
   
   If function `can_NWID_NODEID_HALreceiveMsg()`is called wihin ISR context, then the callbacks will be also called within ISR context so the user code shall be as small as possible.
   
   Is also possible to poll for the reception of messages based on their available flags (in case user doesn't need to use the callbacks directly).

3. Consume message's available flags (polling of received messages) if required. 

### HAL integration for transmission

Following tasks are required in order to integrate the transmission of CAN messages with the MCU's HAL:

1. Implement CAN HAL transmission. This means instrumenting code within function `can_CT_NODE_1_HALtransmitMsg`.
   
   HAL code to trigger a CAN message transmission by the associated CAN peripheral shall be put inside the body of this function. Data regarding the message to be transmitted can be taken from the provided argument `msg_info` as follows.
   
   - Message id to be transmitted can be extracted from `msg_info->id`.
   
   - Message length to be transmitted can be extracted from `msg_info->fields.len`.
   
   - Message data to be transmitted can be taken from `msg_info->data`. `msg_info->data` is a pointer to a bytes array. Hence, individual bytes can be accessed using  `msg_info->data[i]` (where `i` is the desired byte index) or simply use `msg_info->data` to point to the beginning of the array containing the transmission data.
   
   - Indication of extended id can be taken from `msg_info->fields.is_extended_id`. This is a boolean value with `1` indicating that the message has an extended id and `0` if it has an standard id.

2. Invoke callback for message transmission confirmation from HAL. Function `can_CT_NODE_1_HALconfirmTxMsg()` needs to be called within the HAL function that signals the confirmation of a CAN message transmission. This function does't require any argument.

# Application Usage

## Data Structures

### Message's data structures

### Accesing signals within a message

## Receiving Messages

### Using the reception callbacks

### Using the available flags (polling for received messages)

### Using the received data

## Transmitting Messages

### Setting data to be transmitted

### Transmission of periodic messages

### Transmission of spontaneous messages

# Functions References
