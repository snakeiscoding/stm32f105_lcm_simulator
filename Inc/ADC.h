#ifndef __ADC_H__
#define __ADC_H__


typedef struct {
  unsigned short max ;
  unsigned short min ;
  unsigned char  count ;
  unsigned long  sum ;
  unsigned short adverage ;
  unsigned short amp ;
} ADCDataStruct ;


typedef union {
  struct {
    unsigned char SetOffset:1 ;
    unsigned char ADCComplete:1 ;
    unsigned char ADCSampleComplete:1 ;
    unsigned char ADCDataComplete:1 ;
    unsigned char ADCEMDataComplete:1 ;
    unsigned char rev:3 ;
  } B ;
  unsigned char All ;
} ADCStatusReg ;


//------------------------------------------------------------------------------
#define   _DC12Voltage_                   0
#define   _TempSensor_                    1


//------------------------------------------------------------------------------
void ADC_Initial(void) ;
void ADC_ConversionProcess(void) ;
void ADC_Process(void) ;
unsigned short ADC_GetDC12Voltage(void) ; 

//------------------------------------------------------------------------------
extern volatile ADCStatusReg ADCStatus ;






#endif /* __ADC_H__ */


