//##############################################################################
//
//##############################################################################


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EE93CXX_H
#define __EE93CXX_H

/* Includes ------------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/

extern void EE93CXX_WriteBlockToEeprom(unsigned short,unsigned short,unsigned char*) ;
extern void EE93CXX_ReadBlockFromEeprom(unsigned short,unsigned short,unsigned char*) ;
extern void EE93CXX_WriteDataToEeprom(unsigned short,unsigned char,unsigned char*) ;
extern void EE93CXX_ReadDataFromEeprom(unsigned short,unsigned char,unsigned char*) ;
extern void EE93CXX_EARSE(unsigned short) ;


#define     EE93CXX_ReadByteData(a,c)             EE93CXX_ReadDataFromEeprom(a,1,(unsigned char*)&c) 
#define     EE93CXX_ReadWordData(a,c)             EE93CXX_ReadDataFromEeprom(a,2,(unsigned char*)&c) 
#define     EE93CXX_ReadDWordData(a,c)            EE93CXX_ReadDataFromEeprom(a,4,(unsigned char*)&c) 

#define     EE93CXX_WriteByteData(a,c)            EE93CXX_WriteDataToEeprom(a,1,(unsigned char*)&c)
#define     EE93CXX_WriteWordData(a,c)            EE93CXX_WriteDataToEeprom(a,2,(unsigned char*)&c)
#define     EE93CXX_WriteDWordData(a,c)           EE93CXX_WriteDataToEeprom(a,4,(unsigned char*)&c)


//------------------------------------------------------------------------------

#endif /* __EE93CXX_H */






//===========================================================================
// End of file.
//===========================================================================


