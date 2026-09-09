/**
  ******************************************************************************
  * @file    Boot.c
  * @author  JHT Team
  * @version V1.0.0
  * @date    03/12/2010
  * @brief   
  ******************************************************************************
*/ 

//------------------------------------------------------------------------------
//#include <stdio.h>
//#include <string.h>

//------------------------------------------------------------------------------
// File System and usb host system
#include  "hcc_types.h"
#include  "usb_host.h"
#include  "pmgr.h"
#include  "usb_mst.h"
#include  "fat.h"
#include  "mst.h"
#include  "os.h"
#include  "Bootflash.h"
//#include "XmlFile.h"
#include  "Boot.h"
#include  "BoardInfo.h"
#include  "stm32_reg.h"
//#include "HT1381.h"
#include  "BootLCM.h"
#include  "Stm32GPIO.h"
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
#define _VolumeNumber_        1


//------------------------------------------------------------------------------
char TempFilename[256] ; 
const char *const FW_BIN = {"/*.bin"} ;
const unsigned char UpdateMsg[7][17] = {
 // 0123456789012345
  {"                "},
  {"     No USB     "},
  {" No update file "},
  {" Flash Erasing  "},
  {" Erasing Error  "},  
  {" Updating:    % "}
} ;
unsigned long CheckTime = 0 ;
//------------------------------------------------------------------------------
/* Declares a section */
char Boot_CheckUserProgram(void) ;
char Boot_UpdateUserProgram(char*filename,char DeleteFile)  ;
char Boot_CheckPenDriverPlunin(void) ;
char Boot_CheckBINFileInformation(char *filename,char *sAddr,long tAddr) ; // filename: "/xxx/xxx.bin, sAddr: source addres, tAddr: file data index address
char Boot_CheckJHTfile(char *filename) ;
char Boot_SearchBINfile(char*filename,char*retfilename) ;
char Boot_CheckUserInformation(const unsigned char *Src,const unsigned char *Dist) ;
void Boot_DisableIRQn(char All) ;
void Boot_UCBFirmwareUpdate(char*filename,char DeleteFile,char Source) ;
void Boot_MoveUCBvectorToRAM(void) ;
//

//------------------------------------------------------------------------------
FLASH_Status Boot_EarseUserProgram()
{
  FLASH_Status status;
  // Show Erase Memory
  BootLCMDisplay_show_string(0,1,(unsigned char *)&UpdateMsg[3][0]) ;
  //----------------------------------------------------------------
  /* Unlock the Flash Program Erase controller */
  BootFLASH_Unlock();
  // Earse Flash
  status = BootFLASH_EarseUserProgram(StartAddr,EndAddr) ;
  // 
  return status ;
}



//------------------------------------------------------------------------------
char Boot_CheckUserInformation(const unsigned char *Src,const unsigned char *Dist) 
{
  while( *Src != 0 )
      {
      if( *Src != *Dist )
          {
          return _DataNotMatch_ ;
          }
      Src += 1 ;
      Dist += 1 ;
      } 
  return _DataMatch_;
}


//------------------------------------------------------------------------------
void Boot_UCBFirmwareUpdate( char*filename, char DeleteFile, char Source)
{
  char Status ;
  //
  Boot_DisableIRQn(1) ; // Disable All IRQ except OTG 
  //
  Status = Boot_UpdateUserProgram(filename,DeleteFile) ;

  // if Error then Erase Flash Memory
  if( Status != _FirmwareUpdateOK_ )
      {
      Boot_EarseUserProgram() ;
      }
  //
  // SWReset
  Boot_DisableIRQn(1) ;
  Boot_CallApplication(BOOT_START_ADDRESS);
  //
  //
  return ;
}



//------------------------------------------------------------------------------
char Boot_CheckUserProgram()
{
  unsigned long *pulApp;
  char ret_status ;
  ret_status = _UserAPIError_ ;
  //----------------------------------------------------------------------------
  if( Boot_CheckUserInformation(&Boot_Manufacture[0],(const unsigned char *)(0x08000000+_Info_ManufactureIndex_)) == _DataMatch_ )
      if( Boot_CheckUserInformation(&Boot_McuType[0],(const unsigned char *)(0x08000000+_Info_McuTypeIndex_)) == _DataMatch_ )
          if( Boot_CheckUserInformation(&Boot_ModuleName[0],(const unsigned char *)(0x08000000+_Info_ModuleNameIndex_)) == _DataMatch_ )
              if( Boot_CheckUserInformation(&Boot_ModuleNo[0],(const unsigned char *)(0x08000000+_Info_ModuleNoIndex_)) == _DataMatch_ ) 
                  //if( Boot_CheckUserInformation(&Boot_Product[0],(const unsigned char *)(0x08000000+_Info_ProductIndex_)) == _DataMatch_ )                            
                      ret_status = _UserAPIOk_ ;
       
  //----------------------------------------------------------------------------
  // Check User Main Function Address is OK
  // See if the first location is 0xfffffffff or something that does not
  // look like a stack pointer, or if the second location is 0xffffffff or
  // something that does not look like a reset vector.
  //
  if(  ret_status == _UserAPIOk_ )
      {
      pulApp = (unsigned long *)APP_START_ADDRESS;
      if((pulApp[0] == 0xffffffff) || ((pulApp[0] & 0xfff00000) != 0x20000000) ||
          (pulApp[1] == 0xffffffff) || ((pulApp[1] & 0xfffff000) < 0x0800D000) )  
          {
          //
          // App starting stack pointer or PC is not valid, so force an update.
          //
          ret_status = _UserAPIError_ ;
          }
      }
  //----------------------------------------------------------------------------
  return ret_status ;
}


//------------------------------------------------------------------------------
char Boot_CheckPenDriverPlunin(void)
{
  if(!f_checkvolume(_VolumeNumber_))
      {
      //
      if( f_getdrive() != _VolumeNumber_ )
            f_chdrive(_VolumeNumber_) ;
      //
      return(_USBPlugin_) ;
      }
  //
  return(_USBNotPlugin_) ;
}



//------------------------------------------------------------------------------
char Boot_CheckJHTfile(char *filename)
{
  F_FIND find;
#ifdef  _Debuf_API_
  if( Boot_CheckPenDriverPlunin() == _USBPlugin_ )
      {
#endif    
    
      if(!f_findfirst(filename, &find)) 
          return  _JHTFileExist_ ;
        
#ifdef  _Debuf_API_
      }
  else
      return _USBNotPlugin_ ;
#endif 
  return _JHTFileNotExist_ ;
}



//------------------------------------------------------------------------------
// 20230526 Fix USB unplugin bug
char Boot_CheckBINFileInformation(char *filename,char *sAddr,long tAddr )
{
  //
  F_FIND find;
  F_FILE *file; 
  char Buffer[200] ;
  char *ptr ;
  char *ptr1 ;
  //
#ifdef  _Debuf_API_
  if( Boot_CheckPenDriverPlunin() == _USBPlugin_ )
      {
#endif        
     
      if(!f_findfirst((const char*)filename, &find))
          {
          file = f_open(filename,"r"); 
          if( !file )
              {
              f_close(file) ;
              return _NoFileName_ ;
              }
          else
              {
              if(!f_seek(file,tAddr,FN_SEEK_SET))//==>file ����P�_�r���}
                  {
                  //Fix bug 20230526  
                  //f_read(&Buffer[0],1,0x80,file) ;//read 128 byte
                  if( f_read(&Buffer[0],1,0x80,file) == 0x80 ) 
                      {
                      //----------------------------------------------------------------
                      // check Manufacture
                      ptr = sAddr ;
                      ptr1 = &Buffer[0] ;
                      while( *ptr != 0 )
                          {
                          if( *ptr != *ptr1 )
                              {
                              f_close(file) ;
                              return _DataNotMatch_ ;
                              }
                          ptr += 1 ;
                          ptr1 += 1 ;
                          }
                      // 20110727  Check length
                      if( *ptr1 != 0 )
                          {
                          f_close(file) ;
                          return _DataNotMatch_ ;
                          }
                      }
                  //Fix bug 20230526
                  else
                      {
                      f_close(file) ;
                      return _USBNotPlugin_ ;
                      } 
                  //  
                  }
              // Fix bug 20230526
              else
                  {
                  f_close(file) ;
                  return _USBNotPlugin_ ;
                  }
              //
              }
          }
      // Fix bug 20230526
      else
          {
          return _USBNotPlugin_ ;
          }
      //----------------------------------------
#ifdef  _Debuf_API_      
      }
  else
      return _USBNotPlugin_ ;
#endif  
  //              
  f_close(file) ;
  return _DataMatch_ ;
}

//------------------------------------------------------------------------------
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */

//
char Boot_UpdateUserProgram(char*filename,char DeleteFile)
{
  F_FIND find;
  F_FILE *file; 
  uint16_t Buffer[1024] ;
  uint16_t *ptr ;
  uint32_t FlashAddress ;
  unsigned int ProcessCount ;
  // Flash
  long SearchIndex ;
  uint32_t DataCounter = 0x00 ;
  ProcessCount = 0 ;
  //__IO uint32_t NbrOfPage = 0x00;
  volatile FLASH_Status FLASHStatus;
  //
#ifdef  _Debuf_API_
  if( Boot_CheckPenDriverPlunin() == _USBPlugin_ )
      {
#endif        
      if(!f_findfirst((const char*)filename, &find))
          {
          file = f_open(filename,"r"); 
          SearchIndex = (StartAddr & 0x000FFFFF) ;
          //f_close(file) ;
          //return 0 ;
          if( !file )
              {
              f_close(file) ;
              return _NoFileName_ ;
              }
          else
              {
              if(!f_seek(file,SearchIndex,FN_SEEK_SET))
                  {
                  //----------------------------------------------------------------
                  FLASHStatus = Boot_EarseUserProgram() ;
                  if( FLASHStatus != FLASH_COMPLETE )
                      {
                      return _FlashEarseError_ ;                      
                      }
                  //
                  // Show Updating
                  BootLCMDisplay_show_string(0,1,(unsigned char *)&UpdateMsg[5][0]) ;
                  //                  
                  FlashAddress = StartAddr ; 
                  while(1)
                      {
                      f_read(&Buffer[0],2,1024,file); /* read 2048 byte flash block size 2Kbyte*/
                      ptr = &Buffer[0] ;
                      for( DataCounter = 0 ; DataCounter < 1024 ; DataCounter++)
                          {
                          FLASHStatus = BootFLASH_ProgramHalfWord((FlashAddress+(DataCounter*2)),*ptr ) ;
                          if( FLASHStatus != FLASH_COMPLETE )
                              {
                              f_close(file) ;
                              return _FirmwareUpdateError_ ;
                              }
                          ptr++ ;
                          }
                      FlashAddress += 2048 ;
                      if( FlashAddress >= InitialTableAddr )
                          break ;
                      //--------------------------------------------------------
                      // Show Process %
                      ProcessCount += 1 ;
                      if( ProcessCount > 99 )
                          ProcessCount = 99 ;
                      BootLCMDisplay_show_word_bcd(11,1,ProcessCount,3) ;
                      //--------------------------------------------------------
                      }
                  //----------------------------------------------------------------
                  SearchIndex = 0xC800 ;
                  if(!f_seek(file,SearchIndex,FN_SEEK_SET))
                      {
                      f_read(&Buffer[0],2,1024,file); /* read 2048 byte flash block size 2Kbyte*/
                      ptr = &Buffer[0] ;
                      FlashAddress = InitialTableAddr ;
                      for( DataCounter = 0 ; DataCounter < 1024 ; DataCounter++)
                          {
                          FLASHStatus = BootFLASH_ProgramHalfWord((FlashAddress+(DataCounter*2)),*ptr ) ;
                          if( FLASHStatus != FLASH_COMPLETE )
                              {
                              f_close(file) ;
                              return _FirmwareUpdateError_ ;
                              }
                          ptr++ ;
                          }
                      //
                      SearchIndex = 0x3F800 ;
                      if(!f_seek(file,SearchIndex,FN_SEEK_SET))
                          {
                          f_read(&Buffer[0],2,1024,file); /* read 2048 byte flash block size 2Kbyte*/
                          ptr = &Buffer[0] ;
                          FlashAddress = InfomationTableAddr ;
                          for( DataCounter = 0 ; DataCounter < 1024 ; DataCounter++)
                              {
                              FLASHStatus = BootFLASH_ProgramHalfWord((FlashAddress+(DataCounter*2)),*ptr ) ;
                              if( FLASHStatus != FLASH_COMPLETE )
                                  {
                                  f_close(file) ;
                                  return _FirmwareUpdateError_ ;
                                  }
                              ptr++ ;
                              }
                          }
                      else
                          {
                          f_close(file) ;
                          return _FirmwareUpdateError_ ;
                          }
                      }
                  else
                      {
                      f_close(file) ;
                      return _FirmwareUpdateError_ ;
                      }
                  //----------------------------------------------------------------
                  }
              f_close(file) ;
              //
              if( DeleteFile != 0 )
                  f_delete((const char*)filename);
              //
              }
          }
      else
          return _FirmwareUpdateError_ ;
#ifdef  _Debuf_API_
      }
  else
      return _USBNotPlugin_ ;
#endif        
  return _FirmwareUpdateOK_ ;
}



//------------------------------------------------------------------------------
void Boot_CallApplication(unsigned long ulStartAddr)
{
  //
  // Set the vector table to the beginning of the app in flash.
  //
  SCB->VTOR = ulStartAddr ;
  //
  // Load the stack pointer from the application's vector table.
  //
  __asm("    ldr     r1, [r0]\n"
        "    mov     sp, r1");

  //
  // Load the initial PC from the application's vector table and branch to
  // the application's entry point.
  //
  __asm("    ldr     r0, [r0, #4]\n"
        "    bx      r0\n");
  
  return ;
}


//------------------------------------------------------------------------------

char Boot_SearchBINfile(char*filename,char*retfilename) 
{
  F_FIND find;
  char *pch ;
  char len ;
  //
  if(!f_findfirst(filename,&find))
      {
      // Copies src to the beginning of dest 
      strcpy(retfilename,filename) ;
      pch = strstr(retfilename,"*.bin") ;
      len = strlen(find.filename)+1 ;//==>�p�����
      strncpy(pch,find.filename,len);
      do
          {
          if( Boot_CheckBINFileInformation(retfilename,(char*)&Boot_Manufacture[0],_Info_ManufactureIndex_) == _DataMatch_ ) 
              {
              if( Boot_CheckBINFileInformation(retfilename,(char*)&Boot_McuType[0],_Info_McuTypeIndex_) == _DataMatch_ )
                  {
                  if( Boot_CheckBINFileInformation(retfilename,(char*)&Boot_ModuleName[0],_Info_ModuleNameIndex_) == _DataMatch_ )
                      {
                      return _HaveBinFile_ ;
                      /*  
                      //if( Boot_CheckBINFileInformation(retfilename,(char*)&User_ModuleNo[0],_Info_ModuleNoIndex_) == _DataMatch_ )
                          {
                              //if( Boot_CheckBINFileInformation(retfilename,(char*)&User_Product[0],_Info_ProductIndex_) == _DataMatch_ )
                              {
                                  //if( Boot_CheckBINFileInformation(retfilename,(char*)&User_Version[0],_Info_VersionIndex_) != _DataMatch_ )
                                  {//==>���������ŦX��
                                      return _HaveBinFile_ ;
                                  }
                                  //else return  _NoBinFile_  ; 
                              }
                          }
                      */
                      }
                  }
              }
          } while (!f_findnext(&find));
      }
  //
  return  _NoBinFile_  ; 
}

//------------------------------------------------------------------------------
void Boot_DisableIRQn(char All)
{
  SCB->SHCSR &= ~SCB_SHCSR_SYSTICKACT ;
  SysTick->VAL   = 0 ;                      /* Load the SysTick Counter Value */
  SysTick->CTRL  = 0 ;
  if( All != 0 )
      {
      NVIC->ICER[0] = 0xFFFFFFFF ;
      NVIC->ICER[1] = 0xFFFFFFFF ;
      NVIC->ICER[2] = 0xFFFFFFFF ;
      }
  else
      {
      NVIC->ICER[0] = 0xFFFFFFFF ; //  0~31
      NVIC->ICER[1] = 0xFFFFFFFF ; // 32~63
      NVIC->ICER[2] = 0xFFFFFFF7 ; // 64~67 //OTG = 67
      }
  
  return ;
}


//------------------------------------------------------------------------------
int Boot_InitialUSB() 
{
  int rc=0;
  // Add RTC Hardware GPIO define
  //HT1381_HWInitial() ; 
  //
  rc=os_init();
  if(rc==0) rc=usbh_init();			    /* initialize USB host */
  if(rc==0) rc=usb_mst_init();			    /* initalize mass storage */
  if(rc==0) rc=usbh_start();			    /* start USB host */
  if(rc==0)
      {
      f_init();
      f_enterFS();
      (void)f_initvolume(_VolumeNumber_,mst_initfunc,F_AUTO_ASSIGN);
      (void)f_chdrive(_VolumeNumber_);					  /* initialize file system */
      }
  return rc;  
}


//------------------------------------------------------------------------------
void Boot_MoveUCBvectorToRAM()
{
  uint32_t size = 0x200 ;
  uint32_t const * src = (uint32_t const *)0x0800D000 ;
  uint32_t * dest = (uint32_t *)0x2000C000 ;
  do
    {
      *dest++ = *src++;
      size -= 4;
    } while (size != 0); 
  
  return ;
}


#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
void Boot_NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  /* Check the parameters */
  assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));
  
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}


void Boot_NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  /* Check the parameters */
  assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
  assert_param(IS_NVIC_OFFSET(Offset));  
   
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}




void Boot_usbmain(void)
{
  unsigned char ShowMsg ;
  unsigned char MsgNo ;
  //
  Boot_DisableIRQn(1) ;
  Boot_NVIC_SetVectorTable(NVIC_VectTab_FLASH,0) ;
  /* Configure one bit for preemption priority */
  Boot_NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  // 
  Boot_InitialUSB() ; 
  // Initial LCM Port
  HCC_RCC_APB2ENR|=HCC_V(IOPA,1) ;	/* enable PORTA */
  HCC_RCC_APB2ENR|=HCC_V(IOPC,1) ;	/* enable PORTC */
  HCC_RCC_APB2ENR|=HCC_V(IOPD,1) ;	/* enable PORTD */
  
  HCC_GPIO_BSRR(A) = 0x03000000 ; // PA8 PA9 Reset
  HCC_GPIO_BSRR(C) = 0x01800200 ; // PC8 PC7 Reset PC9 Set
  // 20181101 Add LCM Control I/O 
  // Port configuration register Low
  // CHL = GPIO 0 ~ 7
  //      CNF7,MODE7,CNF6,MODE6,CNF5,MODE5,CNF4,MODE4,CNF3,MODE3,CNF2,MODE2,CNF1,MODE1,CNF0,MODE0
  // Port configuration register high
  // CHR = GPIO 8~15
  //      CNF15,MODE15,CNF14,MODE14,CNF13,MODE13,CNF12,MODE12,CNF11,MODE11,CNF10,MODE10,CNF9,MODE9,CNF8,MODE8
  // MODE[1:0]
  // 00: Input mode (reset state)
  //      CNF[1:0] 
  //      00: Analog mode
  //      01: Floating input (reset state)
  //      10: Input with pull-up / pull-down
  //      11: Reserved  
  // 01: Output mode, max speed 10 MHz.
  // 10: Output mode, max speed 2 MHz.
  // 11: Output mode, max speed 50 MHz.
  //      CNF[1:0] 
  //      00: General purpose output push-pull
  //      01: General purpose output Open-drain
  //      10: Alternate function output Push-pull
  //      11: Alternate function output Open-drain  
  //
  //
  // LCM Power Control        : PA9   Output push-pull
  // LCM Back Ligth Contorl   : PA8   Output push-pull
  HCC_GPIO_CRH(A) |=0x000000ff ;
  HCC_GPIO_CRH(A) &=0xffffff33 ;
  // LCM Enable               : PC7   Output push-pull
  // LCM Read or Write        : PC8   Output push-pull
  // LCM Resister or Command  : PC9   Output push-pull
  HCC_GPIO_CRH(C) |=0x000000ff ; // PC8,PC9
  HCC_GPIO_CRH(C) &=0xffffff33 ;
  HCC_GPIO_CRL(C) |=0xf0000000 ; // PC7
  HCC_GPIO_CRL(C) &=0x3fffff00 ;   
  // Data bus 8 Bit
  // LCM Data Bit 0           : PC6   Output Open-Drain
  // LCM Data Bit 1           : PD15  Output Open-Drain
  // LCM Data Bit 2           : PD14  Output Open-Drain
  // LCM Data Bit 3           : PD13  Output Open-Drain
  // LCM Data Bit 4           : PD12  Output Open-Drain
  // LCM Data Bit 5           : PD11  Output Open-Drain
  // LCM Data Bit 6           : PD10  Output Open-Drain
  // LCM Data Bit 7           : PD9   Output Open-Drain
  // Data bus 4 Bit
  // LCM Data Bit 4           : PD12  Output push-pull
  // LCM Data Bit 5           : PD11  Output push-pull
  // LCM Data Bit 6           : PD10  Output push-pull
  // LCM Data Bit 7           : PD9   Output push-pull 
  HCC_GPIO_CRH(D) |=0x000ffff0 ; // PD9 ~ PD12
  HCC_GPIO_CRH(D) &=0xfff3333f ;  
  //
  BootLCM_Initial() ;
  //                              12345678901234561234567890123456
  BootLCMDisplay_show_string(0,0,"USB Update Mode                 ") ;
  ShowMsg = 1 ;
  MsgNo = 0 ;
  // 
  for(;;)
      {
      //------------------------------------------------------------------------
      // Delay time
      CheckTime += 1 ;
      while( CheckTime > 0x0500 )
          {
          pmgr_pcd_task();//==> USB������
          usbh_transfer_task();//==> USB������
          CheckTime = 0 ;
          }
      //  
      //------------------------------------------------------------------------
      if( Boot_CheckPenDriverPlunin() == _USBPlugin_ )
          {
          if( Boot_SearchBINfile((char*)FW_BIN,&TempFilename[0]) == _HaveBinFile_ )
              {
              if( TempFilename[0] != 0 )
                  {
                  Boot_UCBFirmwareUpdate( &TempFilename[0],0 ,0 );//==>��s�����R��
                  }
              }
          else
              {
              if( MsgNo != 2 )
                  {
                  ShowMsg = 1 ;
                  MsgNo = 2 ;
                  }  
              }          
          } 
      else
          {
          if( MsgNo != 1 )
              {
              ShowMsg = 1 ;
              MsgNo = 1 ;
              }
          }
      //------------------------------------------------------------------------
      if( ShowMsg != 0 )
          {
          ShowMsg = 0 ;
          BootLCMDisplay_show_string(0,1,(unsigned char *)&UpdateMsg[MsgNo][0]) ;
          }
      //------------------------------------------------------------------------
      }
}

#ifdef __GNUC__
// added for GNUC compiler, otherwise it will cause a hard fault when call SystemInit() in startup file
void SystemInit(void)
{
}
#endif


//------------------------------------------------------------------------------
#define _FullFunction_
void main(void) 
{
  //
  unsigned short BtnCounter ;
  //
  Boot_NVIC_SetVectorTable(NVIC_VectTab_FLASH,0) ;
  /* Configure one bit for preemption priority */
  Boot_NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  //----------------------------------------------------------------------------  
  // Initial GPIO for Check Into Bootloader button
  HCC_RCC_APB2ENR|=HCC_V(IOPB,1) ;	/* enable PORTB */
  // Port configuration register Low
  // CHL = GPIO 0 ~ 7
  //      CNF7,MODE7,CNF6,MODE6,CNF5,MODE5,CNF4,MODE4,CNF3,MODE3,CNF2,MODE2,CNF1,MODE1,CNF0,MODE0
  // Port configuration register high
  // CHR = GPIO 8~15
  //      CNF15,MODE15,CNF14,MODE14,CNF13,MODE13,CNF12,MODE12,CNF11,MODE11,CNF10,MODE10,CNF9,MODE9,CNF8,MODE8
  // MODE[1:0]
  // 00: Input mode (reset state)
  //      CNF[1:0] 
  //      00: Analog mode
  //      01: Floating input (reset state)
  //      10: Input with pull-up / pull-down
  //      11: Reserved  
  // 01: Output mode, max speed 10 MHz.
  // 10: Output mode, max speed 2 MHz.
  // 11: Output mode, max speed 50 MHz.
  //      CNF[1:0] 
  //      00: General purpose output push-pull
  //      01: General purpose output Open-drain
  //      10: Alternate function output Push-pull
  //      11: Alternate function output Open-drain    
  HCC_GPIO_CRL(B) |=0x00f00000 ;        // PB5 Input
  HCC_GPIO_CRL(B) &=0xff4fffff ; 
  //
  BtnCounter = 0 ;
  while( (HCC_GPIO_IDR(B) & PIN5) == 0 && (BtnCounter < 0xF000 ) )
      {
      BtnCounter++ ;
      if( BtnCounter > 0xF000 )
          BtnCounter = 0xF000 ;
      }
  //----------------------------------------------------------------------------
#ifdef  _FullFunction_
  if( BtnCounter < 0xF000 )
      {
      //----------------------------------------------------------------------------
      if( Boot_CheckUserProgram() == _UserAPIOk_ )
          {
          Boot_DisableIRQn(1) ;
          Boot_CallApplication(APP_START_ADDRESS) ;
          }
      //----------------------------------------------------------------------------
      }
#endif
  Boot_usbmain() ;
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


