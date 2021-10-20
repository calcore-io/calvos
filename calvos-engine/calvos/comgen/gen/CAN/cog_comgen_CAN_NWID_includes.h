/* [[[cog
import cog
import math
import warnings
import time
import sys
import pickle as pic
import pathlib as plib
import json

import calvos.comgen.CAN as nw
import calvos.common.codegen as cg

from cog_CAN import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_pickle_file, 'rb') as f:
		network = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_proj_pickle_file, e))
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = cog_output_file
padding = 80 - (19 + len(cog_output_file))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for including all the network headers.
 *  \details   	Includes al headers related to the network to ease the
 *  			integration of the network in the target project.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-10-08
 *  \copyright 	2021 Carlos Calvillo.
 */
/*============================================================================*/
/* [[[cog
cog.outl(cg.C_license())
]]] */
// [[[end]]]
/*============================================================================*/
/* [[[cog

# Print generation information
cog.outl(C_gen_info(input_worksheet,network))
]]] */
// [[[end]]]
/* [[[cog
# Print include guards
guard_symbol = cog_output_file.replace(".","_")
guard_symbol = guard_symbol.upper()

cog.outl("#ifndef "+guard_symbol)
cog.outl("#define "+guard_symbol)
]]] */
// [[[end]]]
/* [[[cog

# Definition of Functions
 ]]] */
// [[[end]]]

#include "calvos.h"

/* [[[cog
# Generate includes for all the network
if 'NODEID_names_var' in locals():
	node_names = json.loads(NODEID_names_var)
else:
	node_names = ["NODEID"]

if 'include_var' in locals() and 'categories_var' in locals():
	includes = json.loads(include_var)
	categories = json.loads(categories_var)
	for idx, include in enumerate(includes):
		#cog.outl("#include Test \"" + include + "\"" + categories[idx])
		if(categories[idx] != "node"):
			cog.outl("#include \"" + include + "\"")
		else:
			if len(node_names) == 0:
				cog.outl("#include \"" + include.replace("NODEID_","") + "\"")
			else:
				for node_name in node_names:
					cog.outl("#include \"" + include.replace("NODEID",node_name) + "\"")
 ]]] */
// [[[end]]]

/* [[[cog
# Print include guards
cog.outl("#endif /"+chr(42)+" "+ guard_symbol + " "+chr(42) + "/")
]]] */
// [[[end]]]
