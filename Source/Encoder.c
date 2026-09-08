/**************************************************************************//**
 * @file     Encoder.c
 * @brief    
 * @version  
 * @date     
 *
 *
 ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include  "PinDefine.h"
#include  "Encoder.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef  _PRODUCT_V2
#define   Encoder_IRQFunction           TIM4_IRQHandler
#define   Encoder_TIM                   TIM4
#define   Encoder_TIM_IRQn              TIM4_IRQn
#else
#define   Encoder_IRQFunction           TIM3_IRQHandler
#define   Encoder_TIM                   TIM3
#define   Encoder_TIM_IRQn              TIM3_IRQn
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned short NewCounter ;
unsigned short OldCounter ;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function Encoder initial 
  * @param  None
  * @retval None
  */
void Encoder_Initial(void)
{
  NVIC_InitTypeDef 	    NVIC_InitStructure;
  GPIO_InitTypeDef          GPIO_InitStructure; 
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  TIM_ICInitTypeDef         TIM_ICInitStructure;  
  /*-----Timer 3 Encoder, PA6, PA7 , Button PA5*/
  // Encoder A/B Button
  GPIO_InitStructure.GPIO_Pin = _EN_Button_PIN | _EN_A_PIN | _EN_B_PIN ;  // Encoder used
#ifdef _PRODUCT_BOARD  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
#else
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
#endif    
  GPIO_Init(_EN_GPIO_GROUP, &GPIO_InitStructure);	
      
  TIM_DeInit(Encoder_TIM);  
  TIM_TimeBaseInit(Encoder_TIM, &TIM_TimeBaseStructure);         
  TIM_TimeBaseStructure.TIM_Period =0xffff; 
  TIM_TimeBaseStructure.TIM_Prescaler =0;
  TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     
  TIM_TimeBaseInit(Encoder_TIM, &TIM_TimeBaseStructure);  
  
  /*-----------------------------------------------------------------*/  
  #ifdef _PRODUCT_BOARD
  TIM_EncoderInterfaceConfig(Encoder_TIM, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising );
  #else
  TIM_EncoderInterfaceConfig(Encoder_TIM, TIM_EncoderMode_TI12,TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);  
  #endif
  TIM_ICStructInit(&TIM_ICInitStructure);  
  TIM_ICInitStructure.TIM_ICFilter = 8;         
  TIM_ICInit(Encoder_TIM, &TIM_ICInitStructure);  
 
  // Clear all pending interrupts  
  TIM_ClearFlag(Encoder_TIM, TIM_FLAG_Update);  
  TIM_ITConfig(Encoder_TIM, TIM_IT_Update, ENABLE);   
  //Reset counter  
  Encoder_TIM->CNT = 0;     
  TIM_Cmd(Encoder_TIM, ENABLE);  
  
  /* Enable the Encoder_TIM global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = Encoder_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //  
}


/**
  * @brief  This function handles Encoder_TIM interrupt request.
  * @param  None
  * @retval None
  */
void Encoder_IRQFunction(void)
{
  TIM_ClearFlag(Encoder_TIM,(TIM_FLAG_Update|TIM_FLAG_CC1OF));   
  TIM_Cmd(Encoder_TIM, ENABLE); 
}




void Encoder_GetValue( unsigned short Min, unsigned short Max, unsigned short *PtrOfData, unsigned char Mode, unsigned char Scale)
{
  unsigned short CounterValue ;
  unsigned char Minus ;
  //------------------------------------------------------------------------
  Minus = 0 ;
#ifdef _DIV4_
  NewCounter = TIM_GetCounter(Encoder_TIM) / 4 ;
#else
  NewCounter = TIM_GetCounter(Encoder_TIM);  
#endif
  if( OldCounter != NewCounter )
      {
      if( OldCounter > NewCounter )
          {
          CounterValue = (OldCounter - NewCounter)  ;
          if( CounterValue < 0x3F00 )
              {
              Minus = 1 ;
              }
          else
              {               
              #ifdef _DIV4_
              CounterValue = 0x3FFF - CounterValue ;
              #else
              CounterValue = 0xFFFF - CounterValue ;
              #endif              
              }
          }
      else
          {
          CounterValue = (NewCounter - OldCounter)  ;
          if( CounterValue > 0x3F00 )
              {
              Minus = 1 ; 
              #ifdef _DIV4_
              CounterValue = 0x3FFF - CounterValue ;
              #else
              CounterValue = 0xFFFF - CounterValue ;
              #endif
              if( CounterValue == 0 )
                  CounterValue = 1 ;
              }
          } 
      //------------------------------------------------------------------------
      // Add by Kunlung 20200528
      // Scale of adjustment
      if( Scale > 0 && Scale < 200 )
          CounterValue = CounterValue * Scale ;      
      //------------------------------------------------------------------------
      if( Minus == 1 )
          {
          if( *PtrOfData >= CounterValue )
              {
              *PtrOfData -= CounterValue ;
              //20210624 Modify Min Limit issue
              if( *PtrOfData < Min )
                  {
                  if( Mode == _RING )
                      *PtrOfData = Max ;
                  else
                      {
                      *PtrOfData = Min ; 
                      #ifdef _DIV4_
                      TIM_SetCounter(Encoder_TIM,65532) ;
                      NewCounter = TIM_GetCounter(Encoder_TIM) / 4 ;
                      #else
                      TIM_SetCounter(Encoder_TIM,65535) ;
                      NewCounter = TIM_GetCounter(Encoder_TIM);  
                      #endif
                      OldCounter = NewCounter ;
                      }
                  }
              }
          else
              {
              if( Mode == _RING )
                  *PtrOfData = Max ;
              else  
                  {
                  *PtrOfData = Min ;
                  
                  #ifdef _DIV4_
                  TIM_SetCounter(Encoder_TIM,65532) ;
                  NewCounter = TIM_GetCounter(Encoder_TIM) / 4 ;
                  #else
                  TIM_SetCounter(Encoder_TIM,65535) ;
                  NewCounter = TIM_GetCounter(Encoder_TIM);  
                  #endif
                  OldCounter = NewCounter ;
                  }
              }
          //------------------------------------------------------
          }
      else
          {
          *PtrOfData += CounterValue ;
          if( *PtrOfData > Max )
              {
              if( Mode == _RING )
                  *PtrOfData = Min ;
              else
                  {
                  *PtrOfData = Max ;
                  TIM_SetCounter(Encoder_TIM,0) ;
                  NewCounter = 0 ;
                  OldCounter = 0 ;
                  }
              }              
          }       
      //
      OldCounter = NewCounter ;
      }
 return ;  
}