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
/*#define FAT_CONF_5272*/
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
#define CLKIN 66350000

#define IPSBAR 0x40000000
/****************************** SPI *************************************/
#define QMR (*(volatile unsigned short *)(IPSBAR+0x340))
#define QDLYR (*(volatile unsigned short *)(IPSBAR+0x344))
#define QWR (*(volatile unsigned short *)(IPSBAR+0x348))
#define QIR (*(volatile unsigned short *)(IPSBAR+0x34c))
#define QAR (*(volatile unsigned short *)(IPSBAR+0x350))
#define QDR (*(volatile unsigned short *)(IPSBAR+0x354))


/***************************** PORT *************************************/
#define PORTQ_DIR (*(volatile unsigned char *)(IPSBAR+0x100021))
#define PORTQ_ASSIGN (*(volatile unsigned char *)(IPSBAR+0x100059))
#define PORTQ_DATA ((volatile unsigned char *)(IPSBAR+0x100035))
#define PORTQ_SET ((volatile unsigned char *)(IPSBAR+0x10000d))

#define SPIPORT_DIR PORTQ_DIR
#define SPIPORT_ASSIGN PORTQ_ASSIGN
#define SPIPORT_WP PORTQ_DATA
#define SPIPORT_CD PORTQ_DATA
#define SPIPORT_CS PORTQ_SET

#define SPICS0_S 3
#define SPICS2_S 5
#define SPIWP_S 4
#define SPICD_S 6

#define SPICS0 (1<<SPICS0_S)
#define SPICS2 (1<<SPICS2_S)
#define SPIWP  (1<<SPIWP_S)	/* write protect */
#define SPICD  (1<<SPICD_S)	/* card detect */

#define SPICS	SPICS2		/* current chip select */

#define QWR_VAL	0x1000

#elif defined FAT_CONF_5272
/************************************************************************
 ***************************** 5272 *************************************
 ************************************************************************/
#define CLKIN 62500000

#define MBAR 0x10000000

/****************************** SPI *************************************/
#define QMR (*(volatile unsigned short *)(MBAR+0xa0))
#define QDLYR (*(volatile unsigned short *)(MBAR+0xa4))
#define QWR (*(volatile unsigned short *)(MBAR+0xa8))
#define QIR (*(volatile unsigned short *)(MBAR+0xac))
#define QAR (*(volatile unsigned short *)(MBAR+0xb0))
#define QDR (*(volatile unsigned short *)(MBAR+0xb4))

#define CPTQP ((QWR&0xf0)>>4)

/***************************** PORT *************************************/
#define PORTA_CTRL (*(volatile unsigned long *)(MBAR+0x80))
#define PORTD_CTRL (*(volatile unsigned long *)(MBAR+0x98))
#define PORTA_DIR (*(volatile unsigned short *)(MBAR+0x84))
#define PORTA_DATA_WP ((volatile unsigned char *)(MBAR+0x86))
#define PORTA_DATA_CD ((volatile unsigned char *)(MBAR+0x87))

#define SPIPORT_CTRL PORTA_CTRL
#define SPIPORT_CTRL2 PORTD_CTRL
#define SPIPORT_DIR PORTA_DIR
#define SPIPORT_WP PORTA_DATA_WP
#define SPIPORT_CD PORTA_DATA_CD

#define SPICS2_S 2
#define SPIWP_S 11
#define SPICD_S 7
#define SPIWP  (1<<SPIWP_S)  /* write protect */
#define SPICD  (1<<SPICD_S)  /* card detect */

#define SPICS2	0x4

#define SPICS	SPICS2

#define QWR_VAL	0x0000

#elif defined FAT_CONF_5270
/************************************************************************
 ***************************** 5270 *************************************
 ************************************************************************/
#define CLKIN 47995200

#define IPSBAR 0x40000000
/****************************** SPI *************************************/
#define QMR (*(volatile unsigned short *)(IPSBAR+0x340))
#define QDLYR (*(volatile unsigned short *)(IPSBAR+0x344))
#define QWR (*(volatile unsigned short *)(IPSBAR+0x348))
#define QIR (*(volatile unsigned short *)(IPSBAR+0x34c))
#define QAR (*(volatile unsigned short *)(IPSBAR+0x350))
#define QDR (*(volatile unsigned short *)(IPSBAR+0x354))


/***************************** PORT *************************************/
#define PORTQ_DIR (*(volatile unsigned char *)(IPSBAR+0x10001a))
#define PORTQ_ASSIGN (*(volatile unsigned char *)(IPSBAR+0x10004a))
#define PORTQ_DATA ((volatile unsigned char *)(IPSBAR+0x10002a))
#define PORTQ_SET ((volatile unsigned char *)(IPSBAR+0x10000a))
#define PORTT_DIR (*(volatile unsigned char *)(IPSBAR+0x10001b))
#define PORTT_ASSIGN (*(volatile unsigned short *)(IPSBAR+0x10004c))
#define PORTT_DATA ((volatile unsigned char *)(IPSBAR+0x10002b))
#define PORTT_SET ((volatile unsigned char *)(IPSBAR+0x10000b))

#define SPIPORT_DIR PORTQ_DIR
#define SPIPORT_ASSIGN PORTQ_ASSIGN
#define SPIPORTCS23_DIR PORTT_DIR
#define SPIPORTCS23_ASSIGN PORTT_ASSIGN
#define SPIPORT_WP PORTQ_DATA
#define SPIPORT_CD PORTT_DATA

#define SPIWP_S 4
#define SPICD_S 6
#define SPIWP  (1<<SPIWP_S)	/* write protect */
#define SPICD  (1<<SPICD_S)	/* card detect */

#define SPICS0	(1<<3)		/* located on PORTQ */
#define SPICS2	(1<<5)		/* located on PORTT */

#define SPICS	SPICS2

#if (SPICS==SPICS0)
#define SPIPORT_CS PORTQ_SET
#else
#define SPIPORT_CS PORTT_SET
#endif

#define QWR_VAL	0x1000

#endif


/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
   QAR=0;
   QDR=data8;
   QWR=QWR_VAL+0x000;
   QIR|=1;
   QDLYR=0x8000;
   while((QIR&1)==0);
}

/* transmit 2 bytes */
void spi_tx2 (unsigned short data16)
{
   QAR=0;
   QDR=data16>>8;
   QDR=data16&0xff;
   QWR=QWR_VAL+0x100;
   QIR|=1;
   QDLYR=0x8000;
   while((QIR&1)==0);
}

/* transmit 4 bytes */
void spi_tx4 (unsigned long data32)
{
   QAR=0;
   QDR=data32>>24;
   QDR=(data32>>16)&0xff;
   QDR=(data32>>8)&0xff;
   QDR=data32&0xff;
   QWR=QWR_VAL+0x300;
   QIR|=1;
   QDLYR=0x8000;
   while((QIR&1)==0);
}

/* transmit 512 bytes */
void spi_tx512 (unsigned char *buf)
{
  unsigned short i,j;

  QAR=0;
  for (j=0;j<16;j++) QDR=*buf++;	/* fill SPI buffer */

  for (i=0;i<32;i++)
  {
    QWR=QWR_VAL+0xf00;
    QIR|=1;
    QDLYR=0x8000;			/* start SPI transactions */

    if (i<31)
    {
      QAR=0;
      j=0;
      while (j<0xf0)
      {
        if ((QWR&0xf0)>j)
        {
          QDR=*buf++;			/* put next byte to free entries */
          j+=0x10;
        }
      }
      QDR=*buf++;
    }

    while ((QIR&1)==0);			/* wait last byte to send */
  }
}

/* receive 1 byte */
unsigned char spi_rx1 (void)
{
   QAR=0;
   QDR=0xff;
   QWR=QWR_VAL+0x000;
   QIR|=1;
   QDLYR=0x8000;
   while((QIR&1)==0);
   QAR=0x10;
   return QDR&0xff;
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  unsigned short i,j;

  QAR=0;
  for (j=0;j<16;j++) QDR=0xff;

  for (i=0;i<32;i++)
  {
    QWR=QWR_VAL+0xf00;
    QIR|=1;
    QDLYR=0x8000;		/* start SPI transactions */
    {
      QAR=0x10;
      j=0;
      while (j<0xf0)
      {
        if ((QWR&0xf0)>j)
        {
          *buf++=QDR&0xff;	/* get received bytes, while processing SPI buffer */
          j+=0x10;
        }
      }
      *buf++=QDR&0xff;
    }

    QAR=0x10;
    *(buf-16)=QDR&0xff;		/* MCF bug, need to reread 1st byte */
  }
}

void spi_cs_lo (void)
{
#ifndef FAT_CONF_5272
  *SPIPORT_CS&=~SPICS;
#else
  SPIPORT_CTRL2|=(3<<(SPICS2_S<<1));
#endif
}

void spi_cs_hi (void)
{
#ifndef FAT_CONF_5272
  *SPIPORT_CS|=SPICS;
#else
  SPIPORT_CTRL2&=~(3<<(SPICS2_S<<1));
#endif
}


/*
** Init SPI port
*/
int spi_init (void)
{
  int i;
#ifdef FAT_CONF_5282
  SPIPORT_ASSIGN=0x7f;	/* enables QSPI DIN/DOUT/CLK/CS0-CS3 pins */
  			/* D6-3:CS3-0 / D2:CLK / D1:DIN / D0:DOUT */
  			/* CS3->card detect / CS1->write protect */
  SPIPORT_ASSIGN&=~(SPICS0|SPICS2|SPICD|SPIWP);	/* set WP+CD as general port on the proper port */
  SPIPORT_DIR&=~(SPICD|SPIWP);		/* set WP+CD as inputs on the proper port */
  SPIPORT_DIR|=(SPICS0|SPICS2);		/* set CS0+CS2 to be output pins */

  QWR=0x9000;		/* stop + active low */
  QMR=0xC0FF;		/* Master mode */
  QDLYR=0;
#elif defined FAT_CONF_5272
  SPIPORT_CTRL&=~((3<<(SPICD_S<<1))+(3<<(SPIWP_S<<1)));	/* set WP+CD as general port on the proper port */
  SPIPORT_CTRL2|=(3<<(SPICS2_S<<1));
  SPIPORT_DIR&=~(SPICD+SPIWP);		/* set WP+CD as inputs on the proper port */

  QWR=0x9000;		/* stop + active low */
  QMR=0xC0FF;		/* Master mode */
  QDLYR=0;
#elif defined FAT_CONF_5270
  SPIPORT_ASSIGN=0x1f;				/* CS1/CS0-GPIO / DI,DO,CLK-SPI */
  SPIPORT_DIR=0x05|SPICS0;			/* CS1:I / CS0:O / CLK:O / DIN:I / DOUT:O */
  SPIPORTCS23_ASSIGN&=~(0xc0c0);		/* CS3/CS2-GPIO */
  SPIPORTCS23_DIR&=~SPICD;
  SPIPORTCS23_DIR|=SPICS2;

  QWR=0x9000;		/* stop + active low */
  QMR=0xC0FF;		/* Master mode */
  QDLYR=0;
#endif

  QAR=0x20;
#ifndef FAT_CONF_5272
  for (i=0;i<16;QDR=0x8f00,i++);
#else
  for (i=0;i<16;QDR=0x8000+((0xf-SPICS)<<8),i++);
#endif
  spi_cs_hi();
  return 0;
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  br<<=1;
  br=((CLKIN)+(br-1))/br;
  if (br<2) br=2;
  if (br>0xff) br=0xff;
  QMR=0xC000+br;
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  return CLKIN/(2*(QMR&0xff));
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  if (*SPIPORT_CD & SPICD) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (*SPIPORT_WP & SPIWP) return 1;
  return 0;
}


