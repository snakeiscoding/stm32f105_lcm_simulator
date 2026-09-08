/***************************************************************************
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

#ifndef _VA_DRIVER_H_
#define _VA_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hcc_types.h"

hcc_u8 VA_state (void *dev_hdl);
//int VA_transfer( void *dev_hdl ,void *cmd, hcc_u32 cmd_length, void *buf ,hcc_u32* buf_length ) ;
int VA_transfer( void *dev_hdl ,hcc_u8 dir , void *buf , hcc_u32 buf_length ,hcc_u32 *rlength ) ;

int VA_init (void);
int VA_suspend (hcc_u8 dev);
int VA_resume (hcc_u8 dev);

#ifdef __cplusplus
}
#endif

#endif
/****************************** END OF FILE **********************************/

