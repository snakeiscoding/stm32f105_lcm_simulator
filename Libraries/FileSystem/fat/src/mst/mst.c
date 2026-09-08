/****************************************************************************
 *
 *            Copyright (c) 2003-2008 by HCC Embedded
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
#include "mst.h"
#include "scsi.h"

static F_DRIVER mst_driver[SCSI_MAX_DEVICE*SCSI_MAX_LUN];

/****************************************************************************
 *
 * mst_readmultiplesector
 *
 * read multiple sectors from MST
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer where to store data
 * sector - where to read data from
 * cnt - number of sectors to read
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static int mst_readmultiplesector(F_DRIVER *driver,void *data, unsigned long sector, int cnt)
{
  int rc;
  if (scsi_read((unsigned char)driver->user_data,sector,(unsigned long)cnt,data))
    rc=MST_ERROR;
  else
    rc=MST_NO_ERROR;
  return rc;
}

/****************************************************************************
 * Read one sector
 ***************************************************************************/
static int mst_readsector(F_DRIVER *driver,void *data, unsigned long sector)
{
  int rc;
  if (scsi_read((unsigned char)driver->user_data,sector,1,data))
    rc=MST_ERROR;
  else
    rc=MST_NO_ERROR;
  return rc;
}

/****************************************************************************
 *
 * mst_writemultiplesector
 *
 * write multiple sectors into MST
 *
 * INPUTS
 *
 * driver - driver structure
 * data - data pointer
 * sector - where to write data
 * cnt - number of sectors to write
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static int mst_writemultiplesector(F_DRIVER *driver,void *data, unsigned long sector, int cnt)
{
  int rc;
  if (scsi_write((unsigned char)driver->user_data,sector,(unsigned long)cnt,data))
    rc=MST_ERROR;
  else
    rc=MST_NO_ERROR;
  return rc;
}

/****************************************************************************
 * Write one sector
 ***************************************************************************/
static int mst_writesector(F_DRIVER *driver,void *data, unsigned long sector)
{
  int rc;
  if (scsi_write((unsigned char)driver->user_data,sector,1,data))
    rc=MST_ERROR;
  else
    rc=MST_NO_ERROR;
  return rc;
}


/****************************************************************************
 *
 * mst_getphy
 *
 * determinate MST physicals
 *
 * INPUTS
 *
 * driver - driver structure
 * phy - this structure has to be filled with physical information
 *
 * RETURNS
 *
 * error code or zero if successful
 *
 ***************************************************************************/
static int mst_getphy(F_DRIVER *driver,F_PHY *phy)
{
  t_lun_info *info;
  info=scsi_get_lun_info((unsigned char)driver->user_data);
  if (info==0) return MST_ERROR;
  phy->number_of_sectors=info->no_of_sectors;
  phy->bytes_per_sector=(unsigned short)info->sector_size;
  return MST_NO_ERROR;
}


/****************************************************************************
 *
 * mst_release
 *
 * Releases a drive
 *
 * INPUTS
 *
 * driver_param - driver parameter
 *
 ***************************************************************************/
static void mst_release (F_DRIVER *driver)
{
  (void)driver;
}



/****************************************************************************
 *
 * mst_getstatus
 *
 * get status of card, missing or/and removed,changed,writeprotect
 *
 * INPUTS
 *
 * driver - driver structure
 *
 * RETURNS
 *
 * F_ST_xxx code for high level
 *
 ***************************************************************************/
static long mst_getstatus (F_DRIVER *driver)
{
   int st;
   long rc=0;

   st=scsi_get_unit_state((unsigned char)driver->user_data);
   if (st&SCSI_ST_CONNECTED)
   {
     if (st&SCSI_ST_CHANGED) rc|=F_ST_CHANGED;
     if (st&SCSI_ST_WRPROTECT) rc|=F_ST_WRPROTECT;
   }
   else
   {
     rc|=F_ST_MISSING;
   }

   return rc;
}



/****************************************************************************
 *
 * f_mstglueinit
 *
 * this init function has to be passed for highlevel to initiate the
 * driver functions
 *
 * INPUTS
 *
 * driver_param - driver parameter
 *
 * RETURNS
 *
 * driver structure pointer
 *
 ***************************************************************************/
F_DRIVER *mst_initfunc (unsigned long driver_param)
{
   F_DRIVER *driver;
   if (driver_param==F_AUTO_ASSIGN) driver_param=0;
   driver=mst_driver+driver_param;
   (void)memset (driver,0,sizeof(F_DRIVER));
   driver->readsector=mst_readsector;
   driver->writesector=mst_writesector;
   driver->readmultiplesector=mst_readmultiplesector;
   driver->writemultiplesector=mst_writemultiplesector;
   driver->getstatus=mst_getstatus;
   driver->getphy=mst_getphy;
   driver->release=mst_release;
   driver->user_data=driver_param;
   return driver;
}


