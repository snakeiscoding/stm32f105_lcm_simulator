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

/*#define FAT_CONF_5270*/
#define FAT_CONF_5282


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
#define SPIWP  (1<<SPIWP_S)  /* write protect */
#define SPICD  (1<<SPICD_S)  /* card detect */

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

#endif


/* Structure definition for multiple device support */
typedef struct {
  int use;	/* actual entry in use? */
  int active;	/* actual entry is active? */

  unsigned long speed;	/* speed setting */

  volatile unsigned char *cs_port;	/* card detect port */
  unsigned long cs_bit;			/* card detect bit */
  volatile unsigned char *cd_port;	/* card detect port */
  unsigned long cd_bit;			/* card detect bit */
  volatile unsigned char *wp_port;	/* write protect port */
  unsigned long wp_bit;			/* write protect bit */

  t_mmc_dsc *mmc_dsc;			/* current card descriptor */
  F_DRIVER *driver;
} t_MmcDrv;

#define MMCDRV_CNT 2

static F_DRIVER mmc_drivers[MMCDRV_CNT];
static t_mmc_dsc mmc_dsc[MMCDRV_CNT];

static t_MmcDrv MmcDrv[MMCDRV_CNT] = {
  { 0, 0,  0,  SPIPORT_CS, SPICS2, SPIPORT_CD, SPICD, SPIPORT_WP, SPIWP,  &mmc_dsc[0],&mmc_drivers[0] },	/* Card on CS2 */
  { 0, 0,  0,  SPIPORT_CS, SPICS0, SPIPORT_CD, SPICD, SPIPORT_WP, SPIWP,  &mmc_dsc[1],&mmc_drivers[1] }	/* Card on CS0 */
};


static t_MmcDrv *curDev;

/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
   QAR=0;
   QDR=data8;
   QWR=0x1000;
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
   QWR=0x1100;
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
   QWR=0x1300;
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
    QWR=0x1f00;
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
   QWR=0x1000;
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
    QWR=0x1f00;
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
  *(curDev->cs_port)&=~(curDev->cs_bit);
}

void spi_cs_hi (void)
{
  *(curDev->cs_port)|=(curDev->cs_bit);
}


/*
** Init SPI port
*/
int spi_init (void)
{
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
  curDev->speed=0xC000+br;
  QMR=curDev->speed;
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  return CLKIN/(2*(curDev->speed&0xff));
}


/*
** Add subdevice to main driver
** INPUT: user_ptr -
**             F_AUTO_ASSIGN: auto assign device
**             F_MMC_DRIVEx: device number
*/
void *spi_add_device (unsigned long driver_param)
{
	t_MmcDrv *mmc=0;

	if (driver_param==F_AUTO_ASSIGN)
	{
		int i;
	    for (i=0;i<MMCDRV_CNT;i++)
		{
			if (!MmcDrv[i].use)
			{
				mmc=&MmcDrv[i];
				break;
			}
		}

	}
	else
	{
		if (driver_param<MMCDRV_CNT)
		{
			mmc=&MmcDrv[driver_param];
		}
	}

	if (!mmc) return 0;
	if (mmc->use) return 0;

	mmc->use=1;
	mmc->active=0;
	mmc->speed=0xc0ff;

	(void)memset(mmc->driver,0,sizeof(F_DRIVER));

	return mmc;
}



/*
** Delete subdevice from driver
*/
int spi_del_device (void *user_ptr)
{
  t_MmcDrv *p=(t_MmcDrv *)user_ptr;
  p->use=0;
  p->active=0;
  return 0;
}


/*
** Check whether the currently setted SPI interface is the active
** if not switch to it.
** This is needed for a driver with multi-device support.
*/
int spi_check_device (void *user_ptr)
{
  t_MmcDrv *p=(t_MmcDrv *)user_ptr;
  unsigned short qdr_val;
  unsigned int i;

  if (p<&MmcDrv[0] || p>&MmcDrv[MMCDRV_CNT-1]) return 1;
  if (p->active==0)
  {
    for (i=0;i<MMCDRV_CNT;MmcDrv[i++].active=0);
    qdr_val=0x8f00;
    QAR=0x20;
    QDR=qdr_val;
    QMR=p->speed;
    p->active=1;
    curDev=p;
  }
  return 0;
}


/*
** Get Card Detect state
** RETURN: 0 - card is removed
**         1 - card present
*/
int get_cd (void)
{
  if (*(curDev->cd_port) & (curDev->cd_bit)) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (*(curDev->wp_port) & (curDev->wp_bit)) return 1;
  return 0;
}


/*
** Get currently active device MMC descriptor structure
** DO NOT CHANGE THIS!!!
*/
t_mmc_dsc *get_mmc_dsc (void)
{
  return curDev->mmc_dsc;
}

/*
** Get driver structure of active MMC.
** DO NOT CHANGE THIS!!!
*/
F_DRIVER *get_mmc_driver (void)
{
  return curDev->driver;
}

