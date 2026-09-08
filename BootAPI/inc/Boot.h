

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOOT_H
#define __BOOT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define     _NoFileName_          10
#define     _DataNotMatch_        20
#define     _DataMatch_           0
#define     _JHTFileExist_        30
#define     _JHTFileNotExist_     35
#define     _JHTFileCreate_       40
#define     _JHTFileUser_         41
#define     _JHTFileWork_         42
#define     _JHTFileProg_         43


#define     _FirmwareUpdateOK_    100
#define     _FlashEarseError_     101
#define     _FirmwareUpdateError_ 102

#define     _HaveBinFile_         0xD0
#define     _NoBinFile_           0xDF
#define     _UserAPIError_        0xEF
#define     _UserAPIOk_           0xE0 
#define     _USBPlugin_           0xF0
#define     _USBNotPlugin_        0xFF

#define     _Debuf_API_
//#define     _Debug_JHTFile_   


#define StartAddr               ((uint32_t)0x0800D000)
#define InitialTableAddr        ((uint32_t)0x0803F000)
#define InfomationTableAddr     ((uint32_t)0x0803F800)
#define EndAddr                 ((uint32_t)0x0803FFFF)

#define BOOT_START_ADDRESS         0x08000000  //
#define APP_START_ADDRESS          0x0800D000  //
//#define APP_START_ADDRESS          0x2000C000  //


// Infomation 
#define   _Info_ManufactureIndex_           0x3FE00
#define   _Info_McuTypeIndex_                _Info_ManufactureIndex_ + 52
#define   _Info_ModuleNameIndex_             _Info_McuTypeIndex_ + 20
#define   _Info_ModuleNoIndex_               _Info_ModuleNameIndex_ + 20
#define   _Info_ProductIndex_                _Info_ModuleNoIndex_ + 20
#define   _Info_VersionIndex_                _Info_ProductIndex_ + 20

/* Exported functions ------------------------------------------------------- */

char Boot_CheckJHTfile(char *filename);
char Boot_SearchBINfile(char*filename,char*retfilename) ;
void Boot_UCBFirmwareUpdate(char*filename,char DeleteFile,char Source) ;
char Boot_CheckPenDriverPlunin(void) ;
FLASH_Status Boot_EarseUserProgram(void) ;
void Boot_CallApplication(unsigned long ulStartAddr);
#endif /* __BOOT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
