/*============================================================================*/
/*                           calvOS Project                                   */
/*============================================================================*/
/** \file		USER_comgen_CAN_DCM_RL_callbacks.c                            */
/** \brief     	Source file for CAN user callbacks.
 *  \details   	Definitions fo CAN callbacks to be filled-up with user code as
 *  			required.
 *  \author    	Carlos Calvillo
 *  \version   	0.1
 *  \date      	2021-02-01
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
 *     Network file: "G:\devproj\github\calvos_0_4\calvos\calvos-engine\calvos\..
                      demo\usr_in\template - CAN Network Definition.ods"
 *     Network name: "CAN-B"
 *     Network id: "B"
 *     Network date: "12/Mar/2021"
 *     Network version: "2"
 -----------------------------------------------------------------------------*/
#include "calvos.h"
#include "comgen_CAN_DCM_RL_callbacks.h"


/* =============================================================================
 * 	Function definitions
 * ===========================================================================*/

/* --------------------------- */
/* Message reception callbacks */
/* --------------------------- */
/* ===========================================================================*/
/** Callback for BCM_5 reception.
 *
 * Invoked within ISR context whenever BCM_5 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_BCM_5_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for BCM_2 reception.
 *
 * Invoked within ISR context whenever BCM_2 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_BCM_2_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for BRAKE_2 reception.
 *
 * Invoked within ISR context whenever BRAKE_2 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_BRAKE_2_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for TRANSM_2 reception.
 *
 * Invoked within ISR context whenever TRANSM_2 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_TRANSM_2_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for TRANSM_4 reception.
 *
 * Invoked within ISR context whenever TRANSM_4 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_TRANSM_4_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for DCU_1 reception.
 *
 * Invoked within ISR context whenever DCU_1 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_DCU_1_rx_callback(void){
	/* User code goes here... */
}

/* ===========================================================================*/
/** Callback for DCU_2 reception.
 *
 * Invoked within ISR context whenever DCU_2 is received by node DCM_RL
 *  of network B.
 * ===========================================================================*/
void can_DCM_RL_DCU_2_rx_callback(void){
	/* User code goes here... */
}


/* ------------------------- */
/* Message timeout callbacks */
/* ------------------------- */
/* ===========================================================================*/
/** Callback for BCM_5 timeout notification.
 *
 * Invoked when BCM_5 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_BCM_5_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for BCM_2 timeout notification.
 *
 * Invoked when BCM_2 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_BCM_2_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for BRAKE_2 timeout notification.
 *
 * Invoked when BRAKE_2 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_BRAKE_2_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for TRANSM_2 timeout notification.
 *
 * Invoked when TRANSM_2 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_TRANSM_2_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for TRANSM_4 timeout notification.
 *
 * Invoked when TRANSM_4 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_TRANSM_4_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for DCU_1 timeout notification.
 *
 * Invoked when DCU_1 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_DCU_1_timeout_callback(void){
	/* User code goes here... */
}
/* ===========================================================================*/
/** Callback for DCU_2 timeout notification.
 *
 * Invoked when DCU_2 times out as per suscriber node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_DCU_2_timeout_callback(void){
	/* User code goes here... */
}

/* ------------------------------ */
/* Message transmission callbacks */
/* ------------------------------ */
/* ===========================================================================*/
/** Callback for DCU_3 transmission notification.
 *
 * Invoked when DCU_3 is transmitted as per publisher node DCM_RL
 * of network B.
 * ===========================================================================*/
void can_DCM_RL_DCU_3_tx_callback(void){
	/* User code goes here... */
}
