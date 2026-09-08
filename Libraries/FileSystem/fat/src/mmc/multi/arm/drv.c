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



#define SERA_BASE	0xffd00000
#define PORTA_BASE	0xffb00020

#define SERC_BASE	0xffd00040
#define PORTC_BASE	0xffb00028


#define SER_CTRLA	0x00
#define SER_CTRLB	0x04
#define SER_STATUS	0x08
#define SER_BITRATE	0x0c
#define SER_FIFO	0x10

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

#define PORT_VAL	(*(volatile unsigned long *)(curDev->spi_port))
#define SER_VAL32(x)	(*(volatile unsigned long *)((curDev->ser_base)+x))
#define SER_VAL8(x)	(*(volatile unsigned char *)((curDev->ser_base)+x))


/* Structure definition for multiple device support */
typedef struct {
  int initok;				/* initialized */
  int use;				/* actual entry in use? */
  int active;				/* actual entry is active? */

  unsigned long ser_base;		/* serial port base */
  unsigned long spi_port;		/* SPI port */

  unsigned long cs_bit;			/* card detect bit */
  unsigned long cd_bit;			/* card detect bit */
  unsigned long wp_bit;			/* write protect bit */

  t_mmc_dsc *mmc_dsc;			/* current card descriptor */
  F_DRIVER *driver;
} t_MmcDrv;

#define MMCDRV_CNT 2

static F_DRIVER mmc_drivers[MMCDRV_CNT];
static t_mmc_dsc mmc_dsc[MMCDRV_CNT];

static t_MmcDrv MmcDrv[MMCDRV_CNT] = {
  { 0, 0, 0,  SERC_BASE, PORTC_BASE,  SPICS, SPICD, SPIWP, &mmc_dsc[0],&mmc_drivers[0] },	/* Card on CS2 */
  { 0, 0, 0,  SERA_BASE, PORTA_BASE,  SPICS, SPICD, SPIWP, &mmc_dsc[1],&mmc_drivers[1] }	/* Card on CS0 */
};

static t_MmcDrv *curDev;

/************************************ SPI routines *****************************************/
/* transmit 1 byte */
void spi_tx1 (unsigned char data8)
{
  SER_VAL8(SER_FIFO)=data8;
  SER_VAL32(SER_STATUS)|=RBC;
  while ((SER_VAL32(SER_STATUS)&RRDY)==0);
  (void)SER_VAL32(SER_FIFO);
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
  SER_VAL8(SER_FIFO)=0xff;
  SER_VAL32(SER_STATUS)|=RBC;
  while ((SER_VAL32(SER_STATUS)&RRDY)==0);
  return SER_VAL32(SER_FIFO)>>24;
}

/* receive 512 bytes */
void spi_rx512 (unsigned char *buf)
{
  int i;
  for (i=0;i<512;i++) *buf++=spi_rx1();
}

void spi_cs_lo (void)
{
  PORT_VAL&=~(curDev->cs_bit);
}

void spi_cs_hi (void)
{
  PORT_VAL|=(curDev->cs_bit);
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
  unsigned int sysclk=NAgetSysClkFreq();
  if (br>sysclk/6) br=sysclk/6;
  SER_VAL32(SER_CTRLA)=0;
  SER_VAL32(SER_BITRATE)=0xc1000000+(((sysclk/(2*br)-1)&0x3ff));
  SER_VAL32(SER_CTRLA)=0x83000000;
}


/*
** Get baudrate
** RETURN: baudrate
*/
unsigned long spi_get_baudrate (void)
{
  unsigned int sysclk=NAgetSysClkFreq();
  return (sysclk/(2*((SER_VAL32(SER_BITRATE)&0x3ff)+1)));
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
		SER_VAL32(SER_CTRLA)=0;
		SER_VAL32(SER_CTRLB)=0x00280000;
		SER_VAL32(SER_BITRATE)=0xc10003ff;
		SER_VAL32(SER_STATUS)=0;
		SER_VAL32(SER_CTRLA)=0x83000000;
		PORT_VAL=((SPIDI|SPICLK|SPIDO)<<24)|((SPICS|SPICLK|SPIDO)<<16)|((SPIDI|SPICLK|SPIDO)<<8);
		while ((SER_VAL32(SER_STATUS)&TEMPTY)==0);
		spi_cs_hi();
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

  if (p<&MmcDrv[0] || p>&MmcDrv[MMCDRV_CNT-1]) return 1;
  if (p->active==0)
  {
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
  if (PORT_VAL & (curDev->cd_bit)) return 0;
  return 1;
}


/*
** Get Write Protect state
** RETURN: 0 - not protected
**         1 - write protected
*/
int get_wp (void)
{
  if (PORT_VAL & (curDev->wp_bit)) return 1;
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

