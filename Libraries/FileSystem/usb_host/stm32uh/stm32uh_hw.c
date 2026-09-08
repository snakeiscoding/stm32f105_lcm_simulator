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
#include "usb_host.h"
#include "stm32uh_hw.h"
#include "stm32_reg.h"

//#define   EVM

static int _suspend=USBH_SUCCESS;

int stm32uh_hw_init (void)
{
  HCC_C(HCC_RCC_AHBENR,OTGFSEN);		  /* stop OTG FS clock */
  HCC_C(HCC_RCC_CFGR,OTGFSPRE);			  /* set OTG prescaler to /3 -> 144MHz PLL1VCO */
  HCC_S(HCC_RCC_AHBENR,OTGFSEN);		  /* start OTG FS clock */
  HCC_S(HCC_RCC_AHBRSTR,OTGFSRST);		  /* reset OTG module */
  HCC_C(HCC_RCC_AHBRSTR,OTGFSRST);		  /* get OTG out of reset */
#ifdef  EVM
  HCC_RCC_APB2ENR|=HCC_V(IOPA,1)|HCC_V(IOPC,1);	/* enable PORTA, PORTC */
  HCC_GPIO_CRH(C)&=~(0xf<<4);
  HCC_GPIO_CRH(C)|=(0x2<<4);			    /* PC9 push-pull output */
#else  
  HCC_S(HCC_OTG_GUSBCFG,FHMOD) ;                  /* Set Force host mode */
  //HCC_RCC_APB2ENR|=HCC_V(IOPA,1)|HCC_V(IOPD,1)|HCC_V(IOPC,1) ;	/* enable PORTA,PORTC ,PORTD */
  //HCC_GPIO_CRL(D)&=(0xf0ffffff);
  //HCC_GPIO_CRL(D)|=(0x02000000);			        /* PD6 push-pull output */
  //HCC_GPIO_CRL(D)|=(0x06000000);			          /* PD6 Open-drain output */
  //HCC_GPIO_CRH(C)&=~(0xffffffff);
  //HCC_GPIO_CRH(C)|=(0x02000000);			        /* PC14 push-pull output */
  //
  //HCC_GPIO_BSRR(D)=((1u<<6)<<16);		            /* Disable PD6 - Reset VBUS on */
  //HCC_GPIO_BSRR(C)=((1u<<14)<<16);		        /* Disable PC14 - set +5V on */  
  //
#endif  
  return 0;
}


int stm32uh_hw_start (void)
{
  HCC_S(HCC_OTG_GCCFG,PWRDWN);			    /* deactivate power down */
#ifdef  EVM
  HCC_GPIO_BSRR(C)=((1u<<9)<<16);		    /* reset PC9 - set VBUS on */
#else
  //HCC_GPIO_BSRR(D)=((1u<<6));	          /* Enable PD6 - set VBUS on */
  //HCC_GPIO_BSRR(C)=(1u<<14);		      /* Enable PC14 - set +5V on */
#endif  
  return 0;
}

int stm32uh_hw_stop (void)
{
  return 0;
}

void stm32uh_hw_resume (void)
{
  _suspend=USBH_SUCCESS;
}

void stm32uh_hw_suspend (void)
{
  _suspend=USBH_ERR_SUSPENDED;
}

int stm32uh_hw_state (void)
{
  return _suspend;
}

