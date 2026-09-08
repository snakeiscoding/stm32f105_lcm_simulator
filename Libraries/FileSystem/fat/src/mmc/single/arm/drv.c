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
#include <sysClockApi.h>
#include "../drv.h"


/*#define SERIAL_PORT_A*/
#define SERIAL_PORT_C

#ifdef SERIAL_PORT_A
#define SER_BASE	0xffd00000
#define PORT_BASE	0xffb00020
#elif defined SERIAL_PORT_C
#define SER_BASE	0xffd00040
#define PORT_BASE	0xffb00028
#endif

#define SER_CTRLA	(*(volatile unsigned long *)(SER_BASE+0x00))
#define SER_CTRLB	(*(volatile unsigned long *)(SER_BASE+0x04))
#define SER_STATUS	(*(volatile unsigned long *)(SER_BASE+0x08))
#define SER_BITRATE	(*(volatile unsigned long *)(SER_BASE+0x0c))
#define SER_FIFO32	(*(volatile unsigned long *)(SER_BASE+0x10))
#define SER_FIFO8	(*(volatile unsigned char *)(SER_BASE+0x10))

#define SPI_PORT	(*(volatile unsigned long *)PORT_BASE)

#define TEMPTY	(1UL<<0)
#define TBC		(1UL<<1)
#define TRDY	(1UL<<3)
#define RFULL	(1UL<<8)
#define RBC		(1UL<<9)
#define RRDY	(1UL<<11)


#define SPICS  (1UL<<0)  /*chip select*/
#define SPICD  (1UL<<1)  /*card detect*/
#define SPIWP  (1UL<<2)  /*write protect*/
#define SPIDI  (1UL<<3)  /*host data in*/
#define SPICLK (1UL<<4)  /*clock*/
#define SPIDO  (1UL<<7)  /*host data out*/



#define SPI_CS_LO	SPI_PORT&=~SPICS
#define SPI_CS_HI	SPI_PORT|=SPICS
#define SPI_CD		SPI_PORT&SPICD
#define SPI_WP		SPI_PORT&SPIWP


/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
  SER_FIFO8=data8;
  SER_STATUS|=RBC;
  while ((SER_STATUS&RRDY)==0);
  (void)SER_FIFO32;
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
  int i;
  for (i=0;i<512;i++) spi_tx1(*buf++);
}

/* receive 1 byte */
unsigned char spi_rx1 (void)
{
  SER_FIFO8=0xff;
  SER_STATUS|=RBC;
  while ((SER_STATUS&RRDY)==0);
  return SER_FIFO32>>24;
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  int i;
  for (i=0;i<512;i++) *buf++=spi_rx1();
}

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
  SER_CTRLA=0;
  SER_CTRLB=0x00280000;
  SER_BITRATE=0xc10003ff;
  SER_STATUS=0;
  SER_CTRLA=0x83000000;
  SPI_PORT=((SPIDI|SPICLK|SPIDO)<<24)|((SPICS|SPICLK|SPIDO)<<16)|((SPIDI|SPICLK|SPIDO)<<8);
  while ((SER_STATUS&TEMPTY)==0);
  spi_cs_hi();
  return 0;
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  unsigned int sysclk=NAgetSysClkFreq();
  if (br>sysclk/6) br=sysclk/6;
  SER_CTRLA=0;
  SER_BITRATE=0xc1000000+(((sysclk/(2*br)-1)&0x3ff));
  SER_CTRLA=0x83000000;
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  unsigned int sysclk=NAgetSysClkFreq();
  return (sysclk/(2*((SER_BITRATE&0x3ff)+1)));
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  if (SPI_PORT & SPICD) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (SPI_PORT & SPIWP) return 1;
  return 0;
}


