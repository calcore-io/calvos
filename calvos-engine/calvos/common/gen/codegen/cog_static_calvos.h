/* [[[cog
import cog

import calvos.common.codegen as cg
from cog_codegen import log_debug, log_info, log_warn, log_error, log_critical, C_gen_info
]]] */
// [[[end]]]
/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/* [[[cog
file_name = "calvos.h"
padding = 80 - (19 + len(file_name))
padding_str = " "
if padding > 1:
	for i in range(padding-1):
		padding_str += " "
cog.outl("/"+chr(42)+chr(42)+" \\file\t\t"+file_name+" "+padding_str+chr(42)+"/")
]]] */
// [[[end]]]
/** \brief     	Header file for calvos-wide includes.
 *  \details   	Contains include statements to be used across whole calvos
 *  			project
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
#ifndef CALVOS_H
#define CALVOS_H

#include "calvos_types.h"
#include "general_defs.h"

#endif /* CALVOS_H */
