/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded
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
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/
#ifndef __CMD_H
#define __CMD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../fat/src/common/fat.h"
#include "sos_conf.h"
#include "arg.h"
//#include "xmodem.h"
#include "UserDefine.h"

void cmd_exec (char *);


extern unsigned char buf2[];

#ifdef Debug_UART
void __printbuf (char *, int);

#define __printf0(b) { sprintf((char *)buf2,b); __printbuf((char *)buf2,strlen((const char *)buf2)); }
#define __printf1(b,a0) { sprintf((char *)buf2,b,a0); __printbuf((char *)buf2,strlen((const char *)buf2)); }
#define __printf2(b,a0,a1) { sprintf((char *)buf2,b,a0,a1); __printbuf((char *)buf2,strlen((const char *)buf2)); }
#define __printf5(b,a0,a1,a2,a3,a4) { sprintf((char *)buf2,b,a0,a1,a2,a3,a4); __printbuf((char *)buf2,strlen((const char *)buf2)); }
#endif


#endif

