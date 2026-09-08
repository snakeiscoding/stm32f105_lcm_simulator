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


#define PORTA_BASE	0xffb00020
#define PORTC_BASE	0xffb00028

#define SPI_PORT	(*spiport)

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


/****************************************************************************
 *
 * Control lines definitions
 *
 ***************************************************************************/
/* active driver values */
static volatile unsigned long *spiport;

/* Structure definition for multiple device support */
typedef struct {
  int initok;		/* initialized */

  unsigned long br;	/* baudrate */

  int use;	/* actual entry in use? */
  int active;	/* actual entry is active? */

  unsigned long spiport;	/* spi port */

  t_mmc_dsc *mmc_dsc;			/* current card descriptor */
  F_DRIVER *driver;
} t_MmcDrv;


#define MMCDRV_CNT 2

static F_DRIVER mmc_drivers[MMCDRV_CNT];
static t_mmc_dsc mmc_dsc[MMCDRV_CNT];

static t_MmcDrv MmcDrv[MMCDRV_CNT] = {
  { 0,  0,  0, 0,  PORTC_BASE,  &mmc_dsc[0],&mmc_drivers[0]  },	/* Card on CS2 */
  { 0,  0,  0, 0,  PORTA_BASE,  &mmc_dsc[1],&mmc_drivers[1]  }	/* Card on CS0 */
};

static t_MmcDrv *curDev;



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
  return 0;
}


/*
** Set baudrate
** INPUT: br - baudrate
*/
void spi_set_baudrate (unsigned long br)
{
  curDev->br=br/10;	/* approximation */
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  return curDev->br;
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

	(void)memset(mmc->driver,0,sizeof(F_DRIVER));

	if (!mmc->initok)
	{
		curDev=mmc;
		spiport=(volatile unsigned long *)curDev->spiport;
		SPI_PORT=((SPICS|SPICLK|SPIDO)<<16);
		SPI_CS_HI;
		SPI_CLK_LO;
		mmc->initok=1;
	}

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
  unsigned int i;

  if (p<&MmcDrv[0] || p>&MmcDrv[MMCDRV_CNT-1]) return 1;
  if (p->active==0)
  {
    for (i=0;i<MMCDRV_CNT;MmcDrv[i++].active=0);
    spiport=(volatile unsigned long *)curDev->spiport;
    p->active=1;
    curDev=p;
    SPI_CS_HI;
    SPI_CLK_LO;
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

/*
** Get currently active device MMC descriptor structure
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

