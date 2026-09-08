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

#define FAT_CONF_5270
/*#define FAT_CONF_5282*/


/****************************************************************************
 *
 * SPI port defintions
 *
 ***************************************************************************/
#ifdef FAT_CONF_5282
/************************************************************************
 ***************************** 5282 *************************************
 ************************************************************************/
#define IPSBAR 0x40000000

#define SPIPORT_O (*(volatile unsigned char *)(IPSBAR+0x0010000d))
#define SPIPORT_I (*(volatile unsigned char *)(IPSBAR+0x00100035))
#define SPIPORT_DIR (*(volatile unsigned char *)(IPSBAR+0x00100021))
#define SPIPORT_PAR (*(volatile unsigned char *)(IPSBAR+0x00100059))

/********************************
 * Control lines definitions
 *******************************/
#define SPI_CS_LO   SPIPORT_O&=~PBIT5
#define SPI_CS_HI   SPIPORT_O|=PBIT5

#define SPI_DATA_LO SPIPORT_O&=~PBIT0
#define SPI_DATA_HI SPIPORT_O|=PBIT0
#define SPI_CLK_LO SPIPORT_O&=~PBIT2
#define SPI_CLK_HI SPIPORT_O|=PBIT2

#define SPI_DATA_IN (SPIPORT_I&PBIT1)
#define SPI_CD_IN   (SPIPORT_I&PBIT6)
#define SPI_WP_IN   (SPIPORT_I&PBIT4)

#elif defined FAT_CONF_5270
/************************************************************************
 ***************************** 5270 *************************************
 ************************************************************************/
#define IPSBAR 0x40000000

/***************************** PORT *************************************/
#define SPIPORT_I (*(volatile unsigned char *)(IPSBAR+0x10002a))
#define SPIPORT_O (*(volatile unsigned char *)(IPSBAR+0x10000a))
#define SPIPORT_DIR (*(volatile unsigned char *)(IPSBAR+0x10001a))
#define SPIPORT_ASSIGN (*(volatile unsigned char *)(IPSBAR+0x10004a))

#define CSCD_I (*(volatile unsigned char *)(IPSBAR+0x10002b))
#define CSCD_O (*(volatile unsigned char *)(IPSBAR+0x10000b))
#define CSCD_DIR (*(volatile unsigned char *)(IPSBAR+0x10001b))
#define CSCD_ASSIGN (*(volatile unsigned short *)(IPSBAR+0x10004c))

/*#define SPICS	0*/
#define SPICS	2

/********************************
 * Control lines definitions
 *******************************/
#if SPICS==0
#define SPI_CS_LO   SPIPORT_O&=~PBIT3
#define SPI_CS_HI   SPIPORT_O|=PBIT3
#elif SPICS==2
#define SPI_CS_LO   CSCD_O&=~PBIT5
#define SPI_CS_HI   CSCD_O|=PBIT5
#endif

#define SPI_DATA_LO SPIPORT_O&=~PBIT0
#define SPI_DATA_HI SPIPORT_O|=PBIT0
#define SPI_CLK_LO SPIPORT_O&=~PBIT2
#define SPI_CLK_HI SPIPORT_O|=PBIT2

#define SPI_DATA_IN (SPIPORT_I&PBIT1)
#define SPI_CD_IN   (CSCD_I&PBIT6)
#define SPI_WP_IN   (SPIPORT_I&PBIT4)

#endif


#define PBIT0 0x01
#define PBIT1 0x02
#define PBIT2 0x04
#define PBIT3 0x08
#define PBIT4 0x10
#define PBIT5 0x20
#define PBIT6 0x40
#define PBIT7 0x80


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
#ifdef FAT_CONF_5282
  *SPIPORT_PAR=0;	/* let spi ports be port pins */
  *SPIPORT_DIR=0x2d;	/* cs3-I cs2-O cs1-I cs0-O clk-O di-I do-O */
#elif defined FAT_CONF_5270
  SPIPORT_ASSIGN=0;				/* CS1/CS0-GPIO / DI,DO,CLK-SPI */
  SPIPORT_DIR=0x0d;				/* CS1:I / CS0:O / CLK:O / DIN:I / DOUT:O */
  CSCD_ASSIGN&=~(0xc0c0);			/* CS3/CS2-GPIO */
  CSCD_DIR&=~(1<<6);
  CSCD_DIR|=(1<<5);
#endif
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
  if (SPI_CD_IN) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (SPI_WP_IN) return 1;
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

