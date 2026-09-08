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
#include "../drv.h"

/*#define SERIAL_PORT_A*/
#define SERIAL_PORT_C

#ifdef SERIAL_PORT_A
#define PORT_BASE	0xffb00020
#elif defined SERIAL_PORT_C
#define PORT_BASE	0xffb00028
#endif

#define SPI_PORT	(*(volatile unsigned long *)PORT_BASE)

#define TEMPTY	(1<<0)
#define TBC	(1<<1)
#define TRDY	(1<<3)
#define RFULL	(1<<8)
#define RBC	(1<<9)
#define RRDY	(1<<11)


#define SPICS  (1<<0)  /*chip select*/
#define SPICD  (1<<1)  /*card detect*/
#define SPIWP  (1<<2)  /*write protect*/
#define SPIDI  (1<<3)  /*host data in*/
#define SPICLK (1<<4)  /*clock*/
#define SPIDO  (1<<7)  /*host data out*/



#define SPI_CS_LO	SPI_PORT&=~SPICS
#define SPI_CS_HI	SPI_PORT|=SPICS
#define SPI_CD		SPI_PORT&SPICD
#define SPI_WP		SPI_PORT&SPIWP

#define SPI_DATA_LO SPI_PORT&=~SPIDO
#define SPI_DATA_HI SPI_PORT|=SPIDO
#define SPI_CLK_LO SPI_PORT&=~SPICLK
#define SPI_CLK_HI SPI_PORT|=SPICLK
#define SPI_DATA_IN (SPI_PORT&SPIDI)


static unsigned long baudrate;

/************************************ SPI routines *****************************************/
void spi_cs_lo (void)
{
  SPI_CS_LO;
}

void spi_cs_hi (void)
{
  SPI_CS_HI;
}


/*
** Init SPI port
*/
int spi_init (void)
{
  SPI_PORT=((SPICS|SPICLK|SPIDO)<<16);
  SPI_CS_HI;
  SPI_CLK_LO;
  return 0;
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  baudrate=br/10;	/* approximation */
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  return baudrate;
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  if (SPI_CD) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (SPI_WP) return 1;
  return 0;
}


/*********************************************************************************
*********************** DO NOT CHANGE THESE FUNCTIONS ****************************
*********************************************************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
  int i;
  for (i=0;i<8;i++)
  {
    if (data8&0x80) SPI_DATA_HI;
               else SPI_DATA_LO;
    SPI_CLK_HI;
    SPI_CLK_LO;
    data8<<=1;
  }
}


/* transmit 2 bytes */
void spi_tx2 (unsigned short data16)
{
  int i;
  for (i=0;i<16;i++)
  {
    if (data16&0x8000) SPI_DATA_HI;
                  else SPI_DATA_LO;
    SPI_CLK_HI;
    SPI_CLK_LO;
    data16<<=1;
  }
}

/* transmit 4 bytes */
void spi_tx4 (unsigned long data32)
{
  int i;
  for (i=0;i<32;i++)
  {
    if (data32&0x80000000) SPI_DATA_HI;
                      else SPI_DATA_LO;
    SPI_CLK_HI;
    SPI_CLK_LO;
    data32<<=1;
  }
}

void spi_tx512 (unsigned char *buf)
{
  int i,j;
  unsigned char data8;

  for (i=0;i<512;i++)
  {
    data8=*buf++;
    for (j=0;j<8;j++)
    {
      if (data8&0x80) SPI_DATA_HI;
                 else SPI_DATA_LO;
      SPI_CLK_HI;
      SPI_CLK_LO;
      data8<<=1;
    }
  }
}


/* receive 1 byte */
unsigned char spi_rx1 (void)
{
  unsigned char ret;
  int i;

  ret=0;
  for (i=0;i<8;i++)
  {
    ret<<=1;
    SPI_CLK_HI;
    if (SPI_DATA_IN) ret|=1;
    SPI_CLK_LO;
  }

  return ret;
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  int i,j;
  unsigned char data8;

  for (i=0;i<512;i++)
  {
    for (j=0,data8=0;j<8;j++)
    {
      data8<<=1;
      SPI_CLK_HI;
      if (SPI_DATA_IN) data8|=1;
      SPI_CLK_LO;
    }
    *buf++=data8;
  }
}

