/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
 *
 * This software is copyrighted by and is the sole property of
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1133
 * Vaci Ut 110
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef _PMGR_DEF_H
#define _PMGR_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PORTS				        5	/* max. no. of ports */
#define MAX_HUBS				        1	/* max. no. of hubs (must include root hub(s) of the host driver(s))*/
#define MAX_CLASS_DRIVERS		    1 //Janson 4	/* max. no. of class drivers */
#define MAX_INTERFACE_PER_PORT	6	/* max. no. of interfaces on a port */


/************ OS related settings. DO NOT CHANGE THIS ************/
#define PMGR_MUTEX_COUNT	0	/* number of mutexes required by the port manager */
#define PMGR_EVENT_COUNT	1	/* number of events required by the port manager */

#ifdef __cplusplus
}
#endif

#endif

