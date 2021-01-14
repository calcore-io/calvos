/* [[[cog
import cog
import math
import pickle as pic
import pathlib as plib


import calvos.common.codegen as cg

from cog_codegen import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = "calvos_types.h"
padding = 80 - (19 + len(file_name))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for calvos type definitions.
 *  \details   	Contains typedef to be used across all calvos related source
 *				code.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-01-13
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
cog.outl(C_gen_info("File Name"))
]]] */
// [[[end]]]
#ifndef "CALVOS_TYPES_H"
#define "CALVOS_TYPES_H"

/* [[[cog

# Definition of basic data types
cog.outl("/"+chr(42)+" Definition of basic data types "+chr(42)+"/")
for key, data_type in cg.dt_compiler.items():
	typedef_str = None
	if key in cg.dt:
		if data_type != cg.dt[key]:
			typedef_str = "typedef " + data_type + " " + cg.dt[key] + ";"
	else:
		typedef_str = "#error Data type string not found for '%s'" % data_type
		log_warn("Data type string not found for '%s'" % data_type)
	if typedef_str is not None:
		cog.outl(typedef_str)

 ]]] */
// [[[end]]]

#endif /* CALVOS_TYPES_H */
