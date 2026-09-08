/****************************************************************************
 *
 *            Copyright (c) 2009 by HCC Embedded
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
#include "../../../../../target/stm32_reg.h"
#include "../drv.h"


#define PCLK	36000000


/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
  HCC_SPI_DR(3)=data8;
  while ((HCC_SPI_SR(3)&HCC_M(RXNE))==0)
    ;
  HCC_SPI_DR(3);
}

/* transmit 2 bytes */
void spi_tx2 (unsigned short data16)
{
  spi_tx1(data16>>8);
  spi_tx1(data16&0xff);
}

/* transmit 4 bytes */
void spi_tx4 (unsigned long data32)
{
  spi_tx1(data32>>24);
  spi_tx1((data32>>16)&0xff);
  spi_tx1((data32>>8)&0xff);
  spi_tx1(data32&0xff);
}

/* transmit 512 bytes */
void spi_tx512 (unsigned char *buf)
{
  unsigned int i;
  for (i=0;i<512;i++) spi_tx1(*buf++);
}

/* receive 1 byte */
unsigned char spi_rx1 (void)
{
  HCC_SPI_DR(3)=0xff;
  while ((HCC_SPI_SR(3)&HCC_M(RXNE))==0)
    ;
  return (unsigned char)HCC_SPI_DR(3);
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  unsigned int i;
  for (i=0;i<512;i++) *buf++=spi_rx1();
}


void spi_cs_lo (void)
{
  HCC_GPIO_BSRR(A)=((1u<<4)<<16);
}

void spi_cs_hi (void)
{
  HCC_GPIO_BSRR(A)=(1u<<4);
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  unsigned long d;
  for (d=0;d<7;d++)
  {
    if (br>(PCLK/(1<<(d+1)))) break;
  }
  HCC_W(HCC_SPI_CR1(3),BR,d);
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  unsigned long d=HCC_R(HCC_SPI_CR1(3),BR);
  return PCLK/(1<<(d+1));
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  return 0;
}


/*
** Init SPI port
*/
int spi_init (void)
{
  HCC_RCC_APB1ENR|=HCC_V(SPI3_EN,1);	/* enable SPI3 */
  HCC_RCC_APB2ENR|=HCC_V(IOPA,1);	/* enable PORTA */

  HCC_GPIO_CRL(A)&=~(0xf<<16);
  HCC_GPIO_CRL(A)|=(0x3<<16);		/* PA4 push-pull output 50MHz */
  HCC_GPIO_ODR(A)|=(1u<<4);

  HCC_S(HCC_AFIO_MAPR,SPI3_REMAP);	/* remap SPI3 to PC10,PC11,PC12 */
  HCC_GPIO_CRH(C)&=~(0xfff<<8);
  HCC_GPIO_CRH(C)|=(0xb4b<<8);		/* PC10-SCK (Ifloat), PC11-MISO (OPP), PC12-MOSI (OPP) */

  HCC_SPI_CR2(3)=4;
  HCC_SPI_CR1(3)=0x0044;


  spi_cs_hi();

  return 0;
}


