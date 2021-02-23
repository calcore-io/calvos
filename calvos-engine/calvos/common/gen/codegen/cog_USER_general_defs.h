/* [[[cog
import cog
import math
import pickle as pic
import pathlib as plib


import calvos.common.codegen as cg

from cog_codegen import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info

try:
	with open(cog_proj_pickle_file, 'rb') as f:
		project = pic.load(f)
except Exception as e:
        print('Failed to access pickle file %s. Reason: %s' % (cog_pickle_file, e))
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = "general_defs.h"
padding = 80 - (19 + len(file_name))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for general types/macros definitions.
 *  \details   	Contains macros to be used across all calvos related source
 *				code and that need some user instrumentation.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-02-23
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
#ifndef GENERAL_DEFS_H
#define GENERAL_DEFS_H

/* Macros for critical sections */
#warning "Insert user code to enter critical section. Remove this directive when done."
#define CALVOS_CRITICAL_ENTER()
#warning "Insert user code to exit critical section. Remove this directive when done."
#define CALVOS_CRITICAL_EXIT()

#endif /* GENERAL_DEFS_H */
