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
#ifndef _OS_NRTOS_H
#define _OS_NRTOS_H

#ifdef __cplusplus
extern "C" {
#endif


#include "hcc_types.h"

#define OS_ISR_FN(fn)		void fn (void)
#define OS_TASK_FN(fn)		void fn (void)

#define OS_MUTEX_TYPE		hcc_u8

#define OS_EVENT_BIT_TYPE	hcc_u32
#define OS_EVENT_TYPE		OS_EVENT_BIT_TYPE


#ifdef __cplusplus
}
#endif

#endif

