//##############################################################################


//##############################################################################
/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#ifdef  _PRODUCT_BOARD
#include  "ADC.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_ConversionProcess       DMA1_Channel1_IRQHandler


#define _ADC1_DR_Address           ((uint32_t)0x4001244C)
#define _ADC1_NumberOfChnnel       2     
#define _ADC_FilterDataLength      10
#define _ADC_SampleTime            10




/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if __ICCARM__
__no_init unsigned short ADC1ConvertedValue[_ADC1_NumberOfChnnel] ;
__no_init ADCDataStruct ADCData[_ADC1_NumberOfChnnel] ;
__no_init unsigned short ADCValue[_ADC1_NumberOfChnnel] ;
#elif __GNUC__
__attribute__((section(".noinit"))) unsigned short ADC1ConvertedValue[_ADC1_NumberOfChnnel] ;
__attribute__((section(".noinit"))) ADCDataStruct ADCData[_ADC1_NumberOfChnnel] ;
__attribute__((section(".noinit"))) unsigned short ADCValue[_ADC1_NumberOfChnnel] ;
#endif
unsigned short ADCFilterData[_ADC1_NumberOfChnnel][_ADC_FilterDataLength] ;
unsigned char FilterDataCounter ;
volatile ADCStatusReg ADCStatus ;
unsigned char ADC_SampleTimeCount ;
unsigned short DC12VoltageValue ;
unsigned short TemperatureMCU ;
float DC12V_ADCoffset ;



/* Private function prototypes -----------------------------------------------*/
void ADC_DataFIFO( unsigned short *ptr, unsigned short NewData,unsigned char Length ) ;
void ADC_FilterBubbleSort( unsigned short* iSource, unsigned char Length ) ;
unsigned short ADC_FilterAverage( unsigned short* iSource, unsigned char Length ) ;


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Initial(void)
{
  ADC_InitTypeDef           ADC_InitStructure;
  DMA_InitTypeDef           DMA_InitStructure;
  GPIO_InitTypeDef 	    GPIO_InitStructure;
  NVIC_InitTypeDef 	    NVIC_InitStructure;
  
  //----------------------------------------------------------------------------
  // PB0 : Analog Input 8, 12V
  // Analog Intput
  GPIO_InitStructure.GPIO_Pin = _V12_PIN  ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(_V12_GPIO_GROUP, &GPIO_InitStructure);  

  //----------------------------------------------------------------------------

  unsigned char i ;
  for( i = 0; i < _ADC1_NumberOfChnnel ;i++)
      {
      ADCData[i].max = 0 ;
      ADCData[i].min = 0 ;
      ADCData[i].count = 0 ;
      ADCData[i].sum = 0 ;
      ADCData[i].adverage = 0 ;
      ADCData[i].amp = 999 ; // 1.000
      ADCValue[i] = 0 ;
      ADC1ConvertedValue[i] = 0 ;
      }

  ADC_SampleTimeCount = 0 ;
  FilterDataCounter = 0 ;
  ADCStatus.All = 0 ;
  DC12V_ADCoffset = 0.8 ;//1.0013706 ;
  /* DMA1 channel1 configuration ---------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = _ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = _ADC1_NumberOfChnnel ;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  /* Enable DMA1_Channel1 Interrupt */
  DMA_ITConfig( DMA1_Channel1,DMA_IT_TC,ENABLE ) ;

  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = _ADC1_NumberOfChnnel ;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, _V12_ADC_Channel,1, ADC_SampleTime_239Cycles5); 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16,2, ADC_SampleTime_239Cycles5);
  
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable Vrefint channel17 */
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  //
  ADCStatus.B.ADCSampleComplete = 1 ;
  
  //
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //
  return ;
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ConversionProcess(void)
{
  unsigned char i ;
  unsigned short Temp ;
  // Calculator
  for( i = 0 ; i < _ADC1_NumberOfChnnel ; i++ )
      {
      //  
      if( ADCData[i].count == 0 )
          {
          ADCData[i].min = ADC1ConvertedValue[i] ;
          ADCData[i].max = ADC1ConvertedValue[i] ;
          }
      else
          {
          if( ADCData[i].max < ADC1ConvertedValue[i] )
              ADCData[i].max = ADC1ConvertedValue[i] ;
          else
              {
              if( ADCData[i].min > ADC1ConvertedValue[i] )
                  ADCData[i].min = ADC1ConvertedValue[i] ;  
              }
          }
      //
      ADCData[i].count += 1 ;
      ADCData[i].sum += ADC1ConvertedValue[i] ;
      if( ADCData[i].count > 10 )
          {
          ADCData[i].adverage = ( ADCData[i].sum - ADCData[i].max - ADCData[i].min ) / (ADCData[i].count-2)*ADCData[i].amp/1000 ;
          ADCData[i].max = 0 ;
          ADCData[i].min = 0 ;
          ADCData[i].count = 0 ;
          ADCData[i].sum = 0 ;
          //
          ADC_DataFIFO(&ADCFilterData[i][0],ADCData[i].adverage,_ADC_FilterDataLength) ;
          if( i == (_ADC1_NumberOfChnnel-1) )
              {
              FilterDataCounter += 1 ;    
              }
          //
          }
      }    
  //----------------------------------------------------------------------------
  if( FilterDataCounter > (_ADC_FilterDataLength-1) )
      {
      FilterDataCounter = 0 ;
      ADCStatus.B.ADCDataComplete = 1 ;
      for( i = 0 ; i < _ADC1_NumberOfChnnel ; i++ )
          {
          ADC_FilterBubbleSort(&ADCFilterData[i][0],_ADC_FilterDataLength) ;
          if( ADCValue[i] == 0 )
              ADCValue[i] = ADC_FilterAverage(&ADCFilterData[i][3],4) ;
          else
              {
              Temp = (ADCValue[i]+ ADC_FilterAverage(&ADCFilterData[i][3],4)) /2 ;
              ADCValue[i] = Temp ;  
              }
          }
      }
  //----------------------------------------------------------------------------  
  /* Clear Channel 1 DMA1_FLAG_TC flag */
  DMA_ClearFlag(DMA1_FLAG_TC1);	
  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  ADCStatus.B.ADCSampleComplete = 1 ;
  return ;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_Process(void)
{

  //
  if( ADCStatus.B.ADCDataComplete == 1 )
      {
      if( ADCStatus.B.ADCEMDataComplete == 1 )
          {
          ADCStatus.B.ADCEMDataComplete = 0 ;
          }  
      
      DC12VoltageValue = (unsigned short)((float)ADCValue[_DC12Voltage_] / 4096 * 16.5 * 1000 * DC12V_ADCoffset ) ; //0.001v
      TemperatureMCU = (unsigned short)((float)((1.43-ADCValue[_TempSensor_]*3.3/4096)/4.3*1000)+25) ;
      ADCStatus.B.ADCDataComplete = 0 ;
      }
  //----------------------------------------------------------------------------
  if( ADCStatus.B.ADCSampleComplete == 1 )
      {
      ADC_SampleTimeCount += 1 ;
      if( ADC_SampleTimeCount >= _ADC_SampleTime )
          {
          // Start ADC1 Software Conversion 
          ADC_SoftwareStartConvCmd(ADC1, ENABLE);
          ADCStatus.B.ADCSampleComplete = 0 ;
          ADC_SampleTimeCount = 0 ;
          //
          }
      }
  //----------------------------------------------------------------------------    
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_DataFIFO( unsigned short *ptr, unsigned short NewData,unsigned char Length )
{
  unsigned short *ptr_bottom ;
  unsigned char	i ;
  unsigned short TmpData ;
  //
  ptr_bottom = ptr ;
  for( i = (Length-1)  ; i > 0 ; i-- )
      {
      TmpData = ptr_bottom[i-1] ;
      ptr_bottom[i] = TmpData ;
      }
  ptr_bottom[0] = NewData ;	
  //
  return  ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_FilterBubbleSort( unsigned short* iSource, unsigned char Length ) 
{
  unsigned char x , y ;
  unsigned short Temp ;
  //
  if( Length <= 1 )
      return ;
  //
  for( x = 0 ; x < Length ; x++ )
      {
      for( y = (x+1) ; y < Length ; y++ )
          {
          if( iSource[x] > iSource[y] )
              {
              Temp = iSource[x] ;
              iSource[x] = iSource[y] ;
              iSource[y] = Temp ;
              }
          }
      }		
  //	
  return ;
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADC_FilterAverage( unsigned short* iSource, unsigned char Length ) 
{
  unsigned long iAverage = 0;
  unsigned char i = 0;
  //---------------------------------------------------------------------------- 
  for( i = 0 ; i < Length ; i++ )
      {
      iAverage = iAverage + (unsigned long)iSource[i];
      }
  //----------------------------------------------------------------------------
  return ((unsigned short)(iAverage/Length));
}




/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned short ADC_GetDC12Voltage(void)
{
  return DC12VoltageValue ;
}

#endif










