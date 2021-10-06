/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		general_defs.h                                                */
/** \brief     	Header file for general types/macros definitions.
 *  \details   	Contains macros to be used across all calvos related source
 *				code and that need some user instrumentation.
 *  \author    	Carlos Calvillo
 *  \version   	1.0
 *  \date      	2021-02-23
 *  \copyright 	2021 Carlos Calvillo.
 */
/*============================================================================*/
/*  This file is part of calvOS project <https://github.com/calcore-io/calvos>.
 *
 *  calvOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  calvOS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with calvOS.  If not, see <https://www.gnu.org/licenses/>. */
/*============================================================================*/
/*-----------------------------------------------------------------------------
 * This file was generated on (yyyy.mm.dd::hh:mm:ss): 2021.10.6::0:10:26
 * Generated from following source(s):
 *     Config file: "File Name"
 -----------------------------------------------------------------------------*/
#ifndef GENERAL_DEFS_H
#define GENERAL_DEFS_H

#include <stdint.h>
#include <stddef.h>

/* Macros for critical sections */
#warning "Insert user code to enter and exit critical section. Remove this directive when done."
/* IMPORTANT: macro CALVOS_CRITICAL_ENTER() shall store the current state of
 * "global" interrupt state and then disable them. CALVOS_CRITICAL_EXIT() shall
 * restore the "global" interrupt state that was stored before entering the
 * critical section. CALVOS_CRITICAL_EXIT() shall NOT always just enable
 * all the "global" interrupts, but rather restore previous state. */
#define CALVOS_CRITICAL_ENTER()
#define CALVOS_CRITICAL_EXIT()

#endif /* GENERAL_DEFS_H */
