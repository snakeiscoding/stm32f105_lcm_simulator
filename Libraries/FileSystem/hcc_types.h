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
#ifndef _HCC_TYPES_H_
#define _HCC_TYPES_H_

#ifndef NULL
#define NULL (void *)0
#endif

/* Type definitions */
typedef unsigned char hcc_u8;
typedef signed char hcc_s8;
typedef unsigned short hcc_u16;
typedef signed short hcc_s16;
typedef unsigned long hcc_u32;
typedef signed long hcc_s32;
typedef unsigned int hcc_uint;

typedef volatile hcc_u8 hcc_reg8;
typedef volatile hcc_u16 hcc_reg16;
typedef volatile hcc_u32 hcc_reg32;

typedef hcc_u32 hcc_imask;

#define RD_LE16(p)		(hcc_u16)(((hcc_u16)((p)[1])<<8)+(hcc_u16)((p)[0]))
#define WR_LE16(p,v)	{ (p)[0]=(hcc_u8)(v&0xff); (p)[1]=(hcc_u8)((v>>8)&0xff); }
#define RD_LE32(p)		(hcc_u32)(((hcc_u32)((p)[3])<<24)+((hcc_u32)((p)[2])<<16)+((hcc_u32)((p)[1])<<8)+(hcc_u32)((p)[0]))
#define WR_LE32(p,v)	{ (p)[0]=(hcc_u8)(v&0xff); (p)[1]=(hcc_u8)((v>>8)&0xff); (p)[2]=(hcc_u8)((v>>16)&0xff); (p)[3]=(hcc_u8)((v>>24)&0xff); }
#define RD_BE16(p)		(hcc_u16)(((hcc_u16)((p)[0])<<8)+(hcc_u16)((p)[1]))
#define WR_BE16(p,v)	{ (p)[1]=(hcc_u8)(v&0xff); (p)[0]=(hcc_u8)((v>>8)&0xff); }
#define RD_BE32(p)		(hcc_u32)(((hcc_u32)((p)[0])<<24)+((hcc_u32)((p)[1])<<16)+((hcc_u32)((p)[2])<<8)+(hcc_u32)((p)[3]))
#define WR_BE32(p,v)	{ (p)[3]=(hcc_u8)(v&0xff); (p)[2]=(hcc_u8)((v>>8)&0xff); (p)[1]=(hcc_u8)((v>>16)&0xff); (p)[0]=(hcc_u8)((v>>24)&0xff); }

#define RD_LE24(p)		(hcc_u32)(((hcc_u32)((p)[2])<<16)+((hcc_u32)((p)[1])<<8)+(hcc_u32)((p)[0]))
#define WR_LE24(p,v)	{ (p)[0]=(hcc_u8)(v&0xff); (p)[1]=(hcc_u8)((v>>8)&0xff); (p)[2]=(hcc_u8)((v>>16)&0xff); }

#include <string.h>
#ifndef _memcpy
#define _memcpy(d,s,l) memcpy(d,s,l)
#endif
#ifndef _memset
#define _memset(d,c,l) memset(d,c,l)
#endif
#ifndef _memcmp
#define _memcmp(d,s,l) memcmp(d,s,l)
#endif

#endif 

