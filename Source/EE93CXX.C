/******************** (C) COPYRIGHT 2009 Johnson Fitness Inc. ******************
* File Name          : EE93cxx.c
* Author             : 
* Version            : V1.0.0
* Date               : 01/13/2009
* Description        : 
*******************************************************************************/

//##############################################################################
// EEPROM AT93Cxx Serial Program
// -----------------------------
// AT93C86 Command Define
// ----------------------------------------------------------------------------
// Command 		Format
//					  SB  OP		Address 				          Data
//					      CODE	x8			      x16		      x8			x16		    Comments
//-----------------------------------------------------------------------------
//	READ 			1 	10 	  A10-A0 	      A9-A0								          Reads data stored 
//																				                          in memory, at
//																				                          specified address.
//-----------------------------------------------------------------------------
//	EWEN 			1 	00 	  11XXXXXXXXX 	11XXXXXXXX							      Write enable must 
//																				                          precede all
//																				                          programming modes.
//-----------------------------------------------------------------------------
//	ERASE 		1 	11 	  A10-A0 	      A9-A0 							          Erase memory 
//																				                          location An - A0.
//-----------------------------------------------------------------------------
//  WRITE			1	  01	  A10-A0		    A9-A0		    D7-D0		D15-D0	  Writes memory 
//																				                          location An - A0. 
//-----------------------------------------------------------------------------
//	ERAL 			1 	00 	  10XXXXXXXXX 	10XXXXXXXX							      Erases all memory 
//																				                          locations. Valid
//																				                          only at VCC=4.5-5.5V
//-----------------------------------------------------------------------------
//	WRAL 			1 	00 	  01XXXXXXXXX 	01XXXXXXXX 	D7-D0 	D15-D0	  Writes all memory 
//																				                          locations. Valid
//																				                          only at VCC=4.5-5.5V
//-----------------------------------------------------------------------------
//	EWDS 			1 	00 	  00XXXXXXXXX 	00XXXXXXXX 							      Disables all 
//																				                          programming 
//																				                          instructions.
//-----------------------------------------------------------------------------
//
//<SYMBOL>                < 93Cxx Pin Assign>       	< MCU Pin Assign>
//oEEPROM_CS-->93Cxx Pin1:CS,Chip Select	      P3_0	// O
//oEEPROM_SK-->93Cxx Pin2:CLK,Clock			        P3_1	// O
//oEEPROM_DI-->93Cxx Pin3:DI,Data Input		      P6_6	// O
//iEEPROM_DO -->93Cxx Pin4:DO,Data Output	      P6_7	// I 
//##############################################################################
/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void EE93CXX_WriteBlockToEeprom(unsigned short,unsigned short,unsigned char*) ;
void EE93CXX_ReadBlockFromEeprom(unsigned short,unsigned short,unsigned char*) ;
void EE93CXX_WriteDataToEeprom(unsigned short,unsigned char,unsigned char*) ;
void EE93CXX_ReadDataFromEeprom(unsigned short,unsigned char , unsigned char*) ;

void EE93CXX_EARSE(unsigned short) ;
void EE93CXX_WriteByte(unsigned short,unsigned char) ;
unsigned char EE93CXX_ReadByte( unsigned short ) ;

void EE93CXX_EWEN(void) ;       
void EE93CXX_EWDS(void) ;  
void EE93CXX_EARSE(unsigned short) ;
void EE93CXX_WriteByte(unsigned short,unsigned char) ;
unsigned char EE93CXX_ReadByte( unsigned short ) ;
void EE93CXX_WriteAddr( unsigned short ) ;
void EE93CXX_WriteData( unsigned char ) ;
unsigned char EE93CXX_ReceiveByte(void) ;
void EE93CXX_SK(void) ;


//*****************************************************************************
//
//-----------------------------------------------------------------------------

//*****************************************************************************
void EE93CXX_WriteBlockToEeprom( unsigned short addr , unsigned short count, unsigned char *DataBuffer  ) 
{
  unsigned short DataCount ;
  //
  oEEPROM_CS(_High) ;
  EE93CXX_EWEN() ;
  //	 
  for( DataCount = 0 ; DataCount < count ; DataCount++ )
      {  
      EE93CXX_EARSE( (addr+DataCount) ) ;  
      EE93CXX_WriteByte( (addr+DataCount) , DataBuffer[DataCount] ) ;
      } 
  //
  EE93CXX_EWDS() ;            
  oEEPROM_CS(_Low) ;
  oEEPROM_SK(_Low) ;
  return ;              
}


//*****************************************************************************
// Read EEPROM ALL DATA to Memory
//*****************************************************************************
void EE93CXX_ReadBlockFromEeprom(unsigned short addr , unsigned short count, unsigned char *DataBuffer )
{
  unsigned short DataCount ;
  
  for( DataCount = addr ; DataCount < ( addr+count ) ; DataCount++ )
      {
      DataBuffer[DataCount] = EE93CXX_ReadByte( DataCount ) ;
      } 
  return ;                  
}




//*****************************************************************************
// EEPROM Write Data
//
//*****************************************************************************
void EE93CXX_WriteDataToEeprom(unsigned short addr , unsigned char count , unsigned char *DataBuffer) 
{
  unsigned short DataCount ;
  //
  oEEPROM_CS(_High) ;
  EE93CXX_EWEN() ;
  //
  for( DataCount = addr ; DataCount < (addr+count) ; DataCount++ )
      {
      EE93CXX_EARSE( DataCount ) ;  
      EE93CXX_WriteByte( DataCount , DataBuffer[(DataCount-addr)] ) ;
      } 
  //
  EE93CXX_EWDS() ;            
  oEEPROM_CS(_Low) ;
  oEEPROM_SK(_Low) ;
  //
  return ;
}       


//*****************************************************************************
// EEPROM Read Data
//
//*****************************************************************************
void EE93CXX_ReadDataFromEeprom(unsigned short adr ,unsigned char count , unsigned char *ReturnDataBuffer ) 
{
  unsigned short DataCount ;

  oEEPROM_CS(_High) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_Low) ;
  EE93CXX_SK( ) ;
  
  EE93CXX_WriteAddr(adr) ;
  oEEPROM_DI(_Low) ;
  
  for( DataCount = 0 ; DataCount < count ; DataCount++ )
      ReturnDataBuffer[DataCount] = EE93CXX_ReceiveByte() ;
  
  oEEPROM_CS(_Low) ;
  oEEPROM_SK(_Low) ;	
}

//*****************************************************************************
// Write Enable
// ============
//
// Command 		Format
//					SB OP		Address 				Data
//						CODE	x8			x16		x8			x16		Comments
//-----------------------------------------------------------------------------
//	EWEN 			1 	00 	11XXXXX 	11XXXX							Write enable must 
//																				precede all
//																				programming modes.
//-----------------------------------------------------------------------------
void EE93CXX_EWEN( )
{
  oEEPROM_CS(_High) ;
  oEEPROM_DI(_High); 
  EE93CXX_SK( ) ;
  oEEPROM_DI(_Low) ; 
  EE93CXX_SK( ) ;
  oEEPROM_DI(_Low) ;
  EE93CXX_SK( ) ;                 
  EE93CXX_WriteAddr( 0x600 ) ;    
  oEEPROM_CS(_Low) ;
  oEEPROM_DI(_Low) ;   
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  oEEPROM_CS(_High) ;
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  oEEPROM_CS(_Low) ;
  return ;
}       



//*****************************************************************************
//	Erase EEPROM 
// ============
//
// Command 		Format
//					SB OP		Address 				Data
//						CODE	x8			x16		x8			x16		Comments
//-----------------------------------------------------------------------------
//	ERASE 		1 	11 	A6-A0 	A5-A0 							Erase memory 
//																				location An - A0.
//
// <Time chart>
//
//				+------------------------------------------
//				|	
// 		---+
//-----------------------------------------------------------------------------
void EE93CXX_EARSE(unsigned short adr)
{
  unsigned long DelayTimeCount = 0 ;
  oEEPROM_CS(_High) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  
  EE93CXX_WriteAddr( adr ) ;
  oEEPROM_CS(_Low) ;
  oEEPROM_SK(_Low) ;
  oEEPROM_DI(_Low) ;
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  //--------------------------------------------------------------------------
  // Check Status
  oEEPROM_CS(_High) ;
  while( iEEPROM_DO == 0 && DelayTimeCount < 20000 )
      {
      DelayTimeCount += 1 ;
      }
  //
  oEEPROM_CS(_Low) ;
  //--------------------------------------------------------------------------    
  return ;
}


//*****************************************************************************
// Write Data To EEPROM
// ====================
//
// Command 		Format
//					SB OP		Address 				Data
//						CODE	x8			x16		x8			x16		Comments
//-----------------------------------------------------------------------------
// WRITE			1	01		A6-A0		A5-A0		D7-D0		D15-D0	Writes memory 
//																				location An - A0. 
//-----------------------------------------------------------------------------
void EE93CXX_WriteByte(unsigned short adr,unsigned char value)
{  
  unsigned long DelayTimeCount = 0 ;
  oEEPROM_CS(_High) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_Low) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  EE93CXX_WriteAddr( adr ) ;
  EE93CXX_WriteData( value ) ;    
  oEEPROM_CS(_Low) ;
  oEEPROM_DI(_Low) ;
  oEEPROM_SK(_Low) ;
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  //--------------------------------------------------------------------------
  // Check Status
  oEEPROM_CS(_High) ;
  while( iEEPROM_DO == 0 && DelayTimeCount < 20000 )
      {
      DelayTimeCount += 1 ;
      }
  oEEPROM_CS(_Low) ;
  //--------------------------------------------------------------------------  
  return ;     
}               



//*****************************************************************************
// Disable All Program Instructions
// ================================
// Command 		Format
//					SB OP		Address 				Data
//						CODE	x8			x16		x8			x16		Comments
//-----------------------------------------------------------------------------
//	EWDS 			1 	00 	00XXXXX 	00XXXX 							Disables all 
//																				programming 
//																				instructions.
//-----------------------------------------------------------------------------
void EE93CXX_EWDS( )
{
  oEEPROM_CS(_High) ;
  
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  
  oEEPROM_DI(_Low) ;
  EE93CXX_SK( ) ;
  
  oEEPROM_DI(_Low) ; 
  EE93CXX_SK( ) ;
  
  EE93CXX_WriteAddr( 0 ) ;
  oEEPROM_CS(_Low) ;
  oEEPROM_DI(_Low) ;
  oEEPROM_SK(_Low) ;
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  oEEPROM_CS(_High) ;
#if __ICCARM__
  asm( " Nop " ) ;
#elif __GNUC__
  __asm( " Nop " ) ;
#endif
  oEEPROM_CS(_Low) ;
  return ;
}


//*****************************************************************************
// Write Address Sub-Program
// =========================
//
// AT93C46 		<A5-A0>
// AT93C56/66	<A7-A0>
// AT93C86    <A10-A0>
//*****************************************************************************
void EE93CXX_WriteAddr( unsigned short value )
{ 
  unsigned char DataCount ;
  unsigned short OutputData ;
  
  OutputData = value << 5 ;
  for( DataCount = 0 ; DataCount < 11 ; DataCount++ )
      {
      //
      oEEPROM_DI(_Low);  
      if( (OutputData & 0x8000) != 0 )
          oEEPROM_DI(_High);
      //
      OutputData = OutputData << 1 ;
      EE93CXX_SK( ) ;
      //
      }
  
  return ;
}       


//*****************************************************************************
// Write Data Sub-Program
//*****************************************************************************
void EE93CXX_WriteData( unsigned char value )
{ 
  unsigned char DataCount ;
  unsigned char OutputData ;
  
  OutputData = value ;
  for( DataCount = 0 ; DataCount < 8 ; DataCount++ )
      {
      //
      oEEPROM_DI(_Low);  
      if( (OutputData & 0x80) != 0 )
          oEEPROM_DI(_High);
      //
      OutputData = OutputData << 1 ;
      EE93CXX_SK( ) ;
      //
      }
  return ;
} 



//*****************************************************************************
// Read EEPROM Data All
//
// Command 		Format
//					SB OP		Address 				Data
//						CODE	x8			x16		x8			x16		Comments
//-----------------------------------------------------------------------------
//	READ 			1 	10 	A6-A0 	A5-A0								Reads data stored 
//																				in memory, at
//																				specified address.
//
// Timer
//***************************************************************************** 
unsigned char EE93CXX_ReadByte( unsigned short adr )
{
  unsigned char value ;
  
  oEEPROM_CS(_High) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_High) ;
  EE93CXX_SK( ) ;
  oEEPROM_DI(_Low) ;
  EE93CXX_SK( ) ;
  
  EE93CXX_WriteAddr(adr) ;
  oEEPROM_DI(_Low) ;
  
  value = EE93CXX_ReceiveByte() ;
  
  oEEPROM_CS(_Low) ;
  oEEPROM_SK(_Low) ;	
  
  return value ;
}


//*****************************************************************************
// Recevice Data Byte
//*****************************************************************************
unsigned char EE93CXX_ReceiveByte( )
{ 
  unsigned char i;
  unsigned char value=0;
  
  for( i = 0;  i < 8 ;  i++ )
      {  
      EE93CXX_SK( ) ;
      value = value << 1 ;
      value = value | iEEPROM_DO ;
      }
  
  return value;  
}



//***************************************************************************
// EEPROM SK CLOCK Sub-Program
// 
// AT93Cxx Specification
// Frequency	High		Low		voltage
// 	2MHz		250ns		250ns		5V
//		1MHz		500ns		500ns		3.3V
// 
// 
//			+--------+			+--------+
//			|			|			|
// ------+			+--------+
//			^						^
//			|	500ns   500ns	|
//
//***************************************************************************
void EE93CXX_SK( )
{
  //
  oEEPROM_SK(_Low) ;
  //asm( " Nop " ) ; 
  oEEPROM_SK(_High) ;
  //asm( " Nop " ) ; 
  //
  return ;
}


/******************* (C) COPYRIGHT 2009 Johnson Fitness Inc. ***END OF FILE****/

