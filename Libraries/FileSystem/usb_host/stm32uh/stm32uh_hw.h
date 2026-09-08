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
#ifndef _STM32UH_HW_H
#define _STM32UH_HW_H

#ifdef __cplusplus
extern "C" {
#endif

int stm32uh_hw_init (void);
int stm32uh_hw_start (void);
int stm32uh_hw_stop (void);
void stm32uh_hw_suspend (void);
void stm32uh_hw_resume (void);
int stm32uh_hw_state (void);

/*
#define ehci_hw_lock_buffer(ptr,psize)			imx27_lock_buffer(ptr,psize)
#define ehci_hw_unlock_buffer(ptr,size)			imx27_unlock_buffer(ptr,size)
*/

#ifdef __cplusplus
}
#endif

#endif

