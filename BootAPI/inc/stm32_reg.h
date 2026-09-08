/****************************************************************************
 *
 *            Copyright (c) 2003-2009 by HCC Embedded
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
#ifndef _STM32_REG_H
#define _STM32_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hcc_types.h"



/********************************** RCC **********************************/
#define HCC_RCC_BASE			0x40021000

#define HCC_RCC_CFGR			*(hcc_reg32 *)(HCC_RCC_BASE+0x04)
	#define HCC_OTGFSPRE_m	1u
	#define HCC_OTGFSPRE_s	22

#define HCC_RCC_AHBENR			*(hcc_reg32 *)(HCC_RCC_BASE+0x14)
	#define HCC_OTGFSEN_m	1u
	#define HCC_OTGFSEN_s	12

#define HCC_RCC_APB2ENR			*(hcc_reg32 *)(HCC_RCC_BASE+0x18)
	#define HCC_IOPA_m		1u
	#define HCC_IOPA_s		2
	
	#define HCC_IOPB_m		1u
	#define HCC_IOPB_s		3
	
	#define HCC_IOPC_m		1u
	#define HCC_IOPC_s		4
	
	#define HCC_IOPD_m		1u
	#define HCC_IOPD_s		5
	
	#define HCC_IOPE_m		1u
	#define HCC_IOPE_s		6

#define HCC_RCC_APB1ENR			*(hcc_reg32 *)(HCC_RCC_BASE+0x1c)
	#define HCC_TIM6_EN_m	1u
	#define HCC_TIM6_EN_s	4

	#define HCC_SPI3_EN_m	1u
	#define HCC_SPI3_EN_s	15

#define HCC_RCC_AHBRSTR			*(hcc_reg32 *)(HCC_RCC_BASE+0x28)
	#define HCC_OTGFSRST_m	1u
	#define HCC_OTGFSRST_s	12


/********************************** TIMERS (BASIC) **********************************/
#define HCC_TIM_1_BASE			0x40012c00
#define HCC_TIM_2_BASE			0x40000000
#define HCC_TIM_3_BASE			0x40000400
#define HCC_TIM_4_BASE			0x40000800
#define HCC_TIM_5_BASE			0x40000c00
#define HCC_TIM_6_BASE			0x40001000
#define HCC_TIM_7_BASE			0x40001400
#define HCC_TIM_8_BASE			0x40013400

#define HCC_TIM_CR1(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x00)
#define HCC_TIM_CR2(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x04)
#define HCC_TIM_DIER(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x0c)
#define HCC_TIM_SR(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x10)
#define HCC_TIM_EGR(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x14)
#define HCC_TIM_CNT(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x24)
#define HCC_TIM_PSC(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x28)
#define HCC_TIM_ARR(n)			*(hcc_reg16 *)(HCC_TIM_##n##_BASE+0x2c)


/********************************** AFIO **********************************/
#define HCC_AFIO_BASE			0x40010000

#define HCC_AFIO_MAPR			*(hcc_reg32 *)(HCC_AFIO_BASE+0x04)
	#define HCC_SPI3_REMAP_m	1u
	#define HCC_SPI3_REMAP_s	28

/********************************** GPIO **********************************/
#define HCC_GPIO_PORT_A_BASE	0x40010800
#define HCC_GPIO_PORT_B_BASE	0x40010c00
#define HCC_GPIO_PORT_C_BASE	0x40011000
#define HCC_GPIO_PORT_D_BASE	0x40011400
#define HCC_GPIO_PORT_E_BASE	0x40011800
#define HCC_GPIO_PORT_F_BASE	0x40011c00
#define HCC_GPIO_PORT_G_BASE	0x40012000

#define HCC_GPIO_CRL(p)			*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x00)
#define HCC_GPIO_CRH(p)			*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x04)
#define HCC_GPIO_IDR(p)			*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x08)
#define HCC_GPIO_ODR(p)			*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x0c)
#define HCC_GPIO_BSRR(p)		*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x10)
#define HCC_GPIO_BRR(p)			*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x14)
#define HCC_GPIO_LCKR(p)		*(hcc_reg32 *)(HCC_GPIO_PORT_##p##_BASE+0x18)

/********************************** SPI **********************************/
#define HCC_SPI_1_BASE			0x40013000
#define HCC_SPI_2_BASE			0x40003800
#define HCC_SPI_3_BASE			0x40003c00

#define HCC_SPI_CR1(s)			*(hcc_reg32 *)(HCC_SPI_##s##_BASE+0x00)
	#define HCC_BR_m		0x7
	#define HCC_BR_s		3

#define HCC_SPI_CR2(s)			*(hcc_reg32 *)(HCC_SPI_##s##_BASE+0x04)

#define HCC_SPI_SR(s)			*(hcc_reg32 *)(HCC_SPI_##s##_BASE+0x08)
	#define HCC_RXNE_m		1u
	#define HCC_RXNE_s		0

	#define HCC_TXNE_m		1u
	#define HCC_TXNE_s		1

#define HCC_SPI_DR(s)			*(hcc_reg32 *)(HCC_SPI_##s##_BASE+0x0c)


/******************************* USB OTG FS ******************************/
#define HCC_OTG_FS_BASE			0x50000000

/***** OTG *****/
#define HCC_OTG_OTGCTL			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x000)

#define HCC_OTG_GOTGINT			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x004)

#define HCC_OTG_GAHBCFG			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x008)
	#define HCC_GINT_m		1u
	#define HCC_GINT_s		0

	#define HCC_TXFELVL_m	1u
	#define HCC_TXFELVL_s	7

	#define HCC_PTXFELVL_m	1u
	#define HCC_PTXFELVL_s	8

#define HCC_OTG_GUSBCFG			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x00c)
	#define HCC_FHMOD_m		1u
	#define HCC_FHMOD_s		29

#define HCC_OTG_GRSTCTL			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x010)
	#define HCC_RXFFLSH_m	1u
	#define HCC_RXFFLSH_s	4

	#define HCC_TXFFLSH_m	1u
	#define HCC_TXFFLSH_s	5

	#define HCC_TXFNUM_m	0x1f
	#define HCC_TXFNUM_s	6

#define HCC_OTG_GINTSTS			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x014)
#define HCC_OTG_GINTMSK			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x018)
	#define HCC_HCINT_m		1u
	#define HCC_HCINT_s		25

	#define HCC_HPRTINT_m	1u
	#define HCC_HPRTINT_s	24

	#define HCC_RXFLVL_m	1u
	#define HCC_RXFLVL_s	4

	#define HCC_SOF_m		1u
	#define HCC_SOF_s		3

#define HCC_OTG_GRXSTSR			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x01c)
#define HCC_OTG_GRXSTSP			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x020)
	#define HCC_PKTSTS_m	0xf
	#define HCC_PKTSTS_s	17
		#define HCC_PKTSTS_IN_RECEIVED	2
	
	#define HCC_RDPID_m		0x3
	#define HCC_RDPID_s		15
	
	#define HCC_BCNT_m		0x7ff
	#define HCC_BCNT_s		4
	
	#define HCC_CHNUM_m		0xf
	#define HCC_CHNUM_s		0

#define HCC_OTG_GRXFSIZ			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x024)
    #define HCC_RXFD_m		0xffff
    #define HCC_RXFD_s		0

#define HCC_OTG_GNPTXFSIZ		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x028)
    #define HCC_NPTXFSA_m	0xffff
    #define HCC_NPTXFSA_s	0

    #define HCC_NPTXFD_m	0xffff
    #define HCC_NPTXFD_s	16

#define HCC_OTG_GNPTXSTS		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x02c)

#define HCC_OTG_GCCFG			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x038)
	#define HCC_PWRDWN_m	1u
	#define HCC_PWRDWN_s	16

#define HCC_OTG_CID				*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x03c)

#define HCC_OTG_HPTXFSIZ		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x100)
    #define HCC_PTXSA_m		0xffff
    #define HCC_PTXSA_s		0
    #define HCC_PTXFSIZ_m	0xffff
    #define HCC_PTXFSIZ_s	16


/***** HOST *****/
#define HCC_OTG_HCFG			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x400)
	#define HCC_FSLSS_m		1
	#define HCC_FSLSS_s		2

	#define HCC_FSLSPCS_m	0x3
	#define HCC_FSLSPCS_s	0
	
#define HCC_OTG_HFIR			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x404)
	#define HCC_FRIVL_m		0xffff
	#define HCC_FRIVL_s		0

#define HCC_OTG_HFNUM			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x408)
	#define HCC_FRNUM_m		0x3fff
	#define HCC_FRNUM_s		0

	#define HCC_FTREM_m		0xffff
	#define HCC_FTREM_s		16

#define HCC_OTG_HPTXSTS			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x410)
	#define HCC_PTXQTOP_m	0xff
	#define HCC_PTXQTOP_s	24
	
	#define HCC_PTXQSAV_m	0xff
	#define HCC_PTXQSAV_s	16

	#define HCC_PTXFSAVL_m	0xffff
	#define HCC_PTXFSAVL_s	0

#define HCC_OTG_HAINT			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x414)
#define HCC_OTG_HAINTMSK		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x418)

#define HCC_OTG_HPRT			*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x440)
	#define	HCC_PSPD_m		0x3
	#define	HCC_PSPD_s		17
		#define HCC_PSPD_FULL	1
		#define HCC_PSPD_LOW	2

	#define HCC_PTCTL_m		0xf
	#define HCC_PTCTL_s		13

	#define HCC_PPWR_m		1u
	#define HCC_PPWR_s		12

	#define HCC_PLSTS_m		0x3
	#define HCC_PLSTS_s		10

	#define HCC_PRST_m		1u
	#define HCC_PRST_s		8
	
	#define HCC_PSUSP_m		1u
	#define HCC_PSUSP_s		7

	#define HCC_PRES_m		1u
	#define HCC_PRES_s		6

	#define HCC_POCCHNG_m	1u
	#define HCC_POCCHNG_s	5

	#define HCC_POCA_m		1u
	#define HCC_POCA_s		4

	#define HCC_PENCHNG_m	1u
	#define HCC_PENCHNG_s	3

	#define HCC_PENA_m		1u
	#define HCC_PENA_s		2

	#define HCC_PCDET_m		1u
	#define HCC_PCDET_s		1

	#define HCC_PCSTS_m		1u
	#define HCC_PCSTS_s		0

#define HCC_OTG_HCCHAR(x)		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x500+(x<<5))
	#define HCC_CHENA_m		1u
	#define HCC_CHENA_s		31
	
	#define HCC_CHDIS_m		1u
	#define HCC_CHDIS_s		30
	
	#define HCC_ODDFRM_m	1u	
	#define HCC_ODDFRM_s	29	
	
	#define HCC_DAD_m		0x7f
	#define HCC_DAD_s		22
	
	#define HCC_EPTYP_m		0x3
	#define HCC_EPTYP_s		18
		#define HCC_EPTYP_CONTROL	0
		#define HCC_EPTYP_ISO		1
		#define HCC_EPTYP_BULK		2
		#define HCC_EPTYP_INTERRUPT	3
	
	#define HCC_LSDEV_m		1u
	#define HCC_LSDEV_s		17
	
	#define HCC_EPDIR_m		1u
	#define HCC_EPDIR_s		15
	
	#define HCC_EPNUM_m		0xf
	#define HCC_EPNUM_s		11
	
	#define HCC_MPSIZ_m		0x7ff
	#define HCC_MPSIZ_s		0


#define HCC_OTG_HCINT(x)		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x508+(x<<5))
#define HCC_OTG_HCINTMSK(x)		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x50c+(x<<5))
	#define HCC_DTERR_m		1u
	#define HCC_DTERR_s		10
	
	#define HCC_FRMOR_m		1u
	#define HCC_FRMOR_s		9
	
	#define HCC_BBERR_m		1u
	#define HCC_BBERR_s		8
	
	#define HCC_TXERR_m		1u
	#define HCC_TXERR_s		7
	
	#define HCC_ACK_m		1u
	#define HCC_ACK_s		5
	
	#define HCC_NAK_m		1u
	#define HCC_NAK_s		4
	
	#define HCC_STALL_m		1u
	#define HCC_STALL_s		3
	
	#define HCC_CHH_m		1u
	#define HCC_CHH_s		1
	
	#define HCC_XFRC_m		1u
	#define HCC_XFRC_s		0
	

#define HCC_OTG_HCTSIZ(x)		*(hcc_reg32 *)(HCC_OTG_FS_BASE+0x510+(x<<5))
	#define HCC_DPID_m		0x3
	#define HCC_DPID_s		29
		#define HCC_DPID_DATA0	0
		#define HCC_DPID_DATA2	1
		#define HCC_DPID_DATA1	2
		#define HCC_DPID_SETUP	3
	
	#define HCC_PKTCNT_m	0x3ff	
	#define HCC_PKTCNT_s	19
	
	#define HCC_XFRSIZ_m	0x7ffff	
	#define HCC_XFRSIZ_s	0	

/***** BUFFER ACCESS *****/
#define HCC_OTG_DFIFO(x)		(hcc_reg32 *)(HCC_OTG_FS_BASE+0x1000+(x<<12))



#define HCC_M(f)				(HCC_##f##_m<<HCC_##f##_s)		/* mask */
#define HCC_V(f,v)				(v<<HCC_##f##_s)				/* value */
#define HCC_R(r,f)				((r>>HCC_##f##_s)&HCC_##f##_m)	/* read */
#define HCC_W(r,f,v)			r=((r&~HCC_M(f))|HCC_V(f,v))	/* write */
#define HCC_S(r,f)				r|=HCC_M(f)						/* set */
#define HCC_C(r,f)				r&=~HCC_M(f)					/* clear */
#define HCC_T(r,f)				r^=HCC_M(f)						/* toggle */
#define HCC_Sb(r,b)				r|=(1u<<b)						/* set bit */
#define HCC_Cb(r,b)				r&=~(1u<<b)						/* clear bit */

#ifdef __cplusplus
}
#endif


#endif


