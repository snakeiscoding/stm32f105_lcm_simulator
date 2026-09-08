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
#include <71x_map.h>
#include <bsp.h>
#include "../drv.h"




/********** SPI module port definitions **********/
#define SPI_PORT	BSPI1
#define TFNE		(1<<9)
#define TFF		(1<<8)
#define TFE		(1<<6)
#define RFF		(1<<4)
#define RFNE		(1<<3)

/********** SPI interface definitions **********/
#define SPI_MISO_PORT	GPIO0
#define SPI_CLK_PORT	GPIO0
#define SPI_MOSI_PORT	GPIO0
#define SPI_CS_PORT	GPIO0
#define SPI_WP_PORT	GPIO1
#define SPI_CD_PORT	GPIO2

#define SPI_MISO	(1<<4)
#define SPI_CLK		(1<<5)
#define SPI_MOSI	(1<<6)
#define SPI_CS		(1<<12)
#define SPI_WP		(1<<9)
#define SPI_CD		(1<<11)




/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
  SPI_PORT->TXR=data8<<8;
  while (((SPI_PORT->CSR2)&RFNE)==0);
  SPI_PORT->RXR;
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
  while ((SPI_PORT->CSR2)&RFNE) SPI_PORT->RXR;
  SPI_PORT->TXR=0xff00;
  while (((SPI_PORT->CSR2)&RFNE)==0);
  return (SPI_PORT->RXR)>>8;

}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  unsigned int i;
  for (i=0;i<512;i++) *buf++=spi_rx1();
}


void spi_cs_lo (void)
{
  SPI_CS_PORT->PD&=~SPI_CS;
}

void spi_cs_hi (void)
{
  SPI_CS_PORT->PD|=SPI_CS;
}


/*
** Init SPI port
*/
int spi_init (void)
{
  SPI_MISO_PORT->PC0|=SPI_MISO;
  SPI_MISO_PORT->PC1|=SPI_MISO;
  SPI_MISO_PORT->PC2|=SPI_MISO;	/* set MISO as AF */

  SPI_MOSI_PORT->PC0|=SPI_MOSI;
  SPI_MOSI_PORT->PC1|=SPI_MOSI;
  SPI_MOSI_PORT->PC2|=SPI_MOSI;	/* set MOSI as AF */

  SPI_CLK_PORT->PC0|=SPI_CLK;
  SPI_CLK_PORT->PC1|=SPI_CLK;
  SPI_CLK_PORT->PC2|=SPI_CLK;	/* set CLK as AF */

  SPI_CS_PORT->PC0|=SPI_CS;
  SPI_CS_PORT->PC1&=~SPI_CS;
  SPI_CS_PORT->PC2|=SPI_CS;	/* set CS as output */

  SPI_CD_PORT->PC0|=SPI_CD;
  SPI_CD_PORT->PC1&=~SPI_CD;
  SPI_CD_PORT->PC2&=~SPI_CD;	/* set CD as input */

  SPI_WP_PORT->PC0|=SPI_WP;
  SPI_WP_PORT->PC1&=~SPI_WP;
  SPI_WP_PORT->PC2&=~SPI_WP;	/* set WP as input */

  SPI_PORT->CSR1=0;
  SPI_PORT->CSR2=0x0000;
  SPI_PORT->CLK=254;		/* setup minimum clock speed */
  SPI_PORT->CSR1=0x0003;		/* setup SPI port */

  spi_cs_hi();

  return 0;
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  br=F_PCLK1/br;
  if (br<6) br=6;
  if (br&1) ++br;
  if (br>254) br=254;
  SPI_PORT->CSR1=0x0;
  { volatile int i=2000; while (i--); }
  SPI_PORT->CLK=br;
  SPI_PORT->CSR1=0x0003;
  { volatile int i=2000; while (i--); }
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  return F_PCLK1/(SPI_PORT->CLK);
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  if (SPI_CD_PORT->PD & SPI_CD) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (SPI_WP_PORT->PD & SPI_WP) return 1;
  return 0;
}


