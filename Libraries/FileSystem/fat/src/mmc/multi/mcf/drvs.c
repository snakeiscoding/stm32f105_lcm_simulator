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

#define SPIPORT_O ((volatile unsigned char *)(IPSBAR+0x0010000d))
#define SPIPORT_I ((volatile unsigned char *)(IPSBAR+0x00100035))
#define SPIPORT_DIR ((volatile unsigned char *)(IPSBAR+0x00100021))
#define SPIPORT_PAR ((volatile unsigned char *)(IPSBAR+0x00100059))

/********************************
 * Control lines definitions
 *******************************/
#define SPI_CS_LO   *spiport_o &= ~spics
#define SPI_CS_HI   *spiport_o |= spics

#define SPI_DATA_LO *spiport_o &= ~spido
#define SPI_DATA_HI *spiport_o |= spido
#define SPI_CLK_LO  *spiport_o &= ~spiclk
#define SPI_CLK_HI  *spiport_o |= spiclk

#define SPI_DATA_IN ((*spiport_i)&spidi)
#define SPI_CD_IN   ((*spiport_i)&spicd)
#define SPI_WP_IN   ((*spiport_i)&spiwp)

#elif defined FAT_CONF_5270
/************************************************************************
 ***************************** 5270 *************************************
 ************************************************************************/
#define IPSBAR 0x40000000

/***************************** PORT *************************************/
#define SPIPORT_I ((volatile unsigned char *)(IPSBAR+0x10002a))
#define SPIPORT_O ((volatile unsigned char *)(IPSBAR+0x10000a))
#define SPIPORT_DIR (*(volatile unsigned char *)(IPSBAR+0x10001a))
#define SPIPORT_ASSIGN (*(volatile unsigned char *)(IPSBAR+0x10004a))

#define CSCD_I ((volatile unsigned char *)(IPSBAR+0x10002b))
#define CSCD_O ((volatile unsigned char *)(IPSBAR+0x10000b))
#define CSCD_DIR (*(volatile unsigned char *)(IPSBAR+0x10001b))
#define CSCD_ASSIGN (*(volatile unsigned short *)(IPSBAR+0x10004c))

/********************************
 * Control lines definitions
 *******************************/
#define SPI_CS_LO   *spiport_cs &= ~spics
#define SPI_CS_HI   *spiport_cs |= spics

#define SPI_DATA_LO *spiport_o &= ~spido
#define SPI_DATA_HI *spiport_o |= spido
#define SPI_CLK_LO  *spiport_o &= ~spiclk
#define SPI_CLK_HI  *spiport_o |= spiclk

#define SPI_DATA_IN ((*spiport_i)&spidi)
#define SPI_CD_IN   ((*spiport_cd)&spicd)
#define SPI_WP_IN   ((*spiport_i)&spiwp)

#endif


#define PBIT0 0x01
#define PBIT1 0x02
#define PBIT2 0x04
#define PBIT3 0x08
#define PBIT4 0x10
#define PBIT5 0x20
#define PBIT6 0x40
#define PBIT7 0x80


/****************************************************************************
 *
 * Control lines definitions
 *
 ***************************************************************************/
/* active driver values */
static volatile unsigned char *spiport_i;
static volatile unsigned char *spiport_o;
static unsigned char spics;
static unsigned char spicd;
static unsigned char spiwp;
static unsigned char spidi;
static unsigned char spiclk;
static unsigned char spido;
#ifdef FAT_CONF_5270
static volatile unsigned char *spiport_cs;
static volatile unsigned char *spiport_cd;
#endif


/* Structure definition for multiple device support */
typedef struct {
  unsigned long br;	/* baudrate */

  int use;	/* actual entry in use? */
  int active;	/* actual entry is active? */

  volatile unsigned char *spiport_i;	/* input port */
  volatile unsigned char *spiport_o;	/* output port */
#if defined FAT_CONF_5270
  volatile unsigned char *spiport_cs;	/* cs output port */
  volatile unsigned char *spiport_cd;	/* cd input port */
#endif
  unsigned char spics;
  unsigned char spicd;
  unsigned char spiwp;
  unsigned char spidi;
  unsigned char spiclk;
  unsigned char spido;

  t_mmc_dsc *mmc_dsc;			/* current card descriptor */
  F_DRIVER *driver;
} t_MmcDrv;


#define MMCDRV_CNT 2

static t_mmc_dsc mmc_dsc[MMCDRV_CNT];
static F_DRIVER mmc_drivers[MMCDRV_CNT];

#ifdef FAT_CONF_5282
t_MmcDrv MmcDrv[MMCDRV_CNT] = {
  { 0,  0, 0,  SPIPORT_I, SPIPORT_O, PBIT5, PBIT6, PBIT4, PBIT1, PBIT2, PBIT0, &mmc_dsc[0],&mmc_drivers[0]  },	/* Card on CS2 */
  { 0,  0, 0,  SPIPORT_I, SPIPORT_O, PBIT3, PBIT6, PBIT4, PBIT1, PBIT2, PBIT0, &mmc_dsc[1],&mmc_drivers[1]  }	/* Card on CS0 */
};
#elif defined FAT_CONF_5270
t_MmcDrv MmcDrv[MMCDRV_CNT] = {
  { 0,  0, 0,  SPIPORT_I, SPIPORT_O, CSCD_O, CSCD_I, PBIT5, PBIT6, PBIT4, PBIT1, PBIT2, PBIT0, &mmc_dsc[0],&mmc_drivers[0] },	/* Card on CS2 */
  { 0,  0, 0,  SPIPORT_I, SPIPORT_O, SPIPORT_O, CSCD_I , PBIT3, PBIT6, PBIT4, PBIT1, PBIT2, PBIT0, &mmc_dsc[1],&mmc_drivers[1]  }	/* Card on CS0 */
};
#endif


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
    spiport_i=p->spiport_i;
    spiport_o=p->spiport_o;
#ifdef FAT_CONF_5270
    spiport_cs=p->spiport_cs;
    spiport_cd=p->spiport_cd;
#endif
    spics=p->spics;
    spicd=p->spicd;
    spiwp=p->spiwp;
    spidi=p->spidi;
    spiclk=p->spiclk;
    spido=p->spido;
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

