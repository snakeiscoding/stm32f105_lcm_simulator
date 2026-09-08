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
#include "stm32_reg.h"
#include "port_os.h"
#include "os.h"

#define _OS_MUTEX_COUNT		    OS_MUTEX_COUNT
#define _OS_EVENT_BIT_COUNT	  OS_EVENT_BIT_COUNT


#if _OS_EVENT_BIT_COUNT
OS_EVENT_BIT_TYPE event;
OS_EVENT_BIT_TYPE event_used;
#endif
#if _OS_MUTEX_COUNT
OS_MUTEX_TYPE mutex[_OS_MUTEX_COUNT];
hcc_u8 mutex_used[_OS_MUTEX_COUNT];
#endif


#if OS_INTERRUPT_ENABLE

  #include "stm32f10x.h"

  #define os_idle()

  void (*usbh_isr) (void)=(void(*)(void))NULL;

  void OTG_FS_IRQHandler(void)
  {
    if (usbh_isr) usbh_isr();
  }


  int os_isr_init (hcc_u32 pos, void(*ih)(void))
  {
    if (pos==STM32UH_HOST_ISR)
    {
      usbh_isr=ih;
      return 0;
    }
    return 1;
  }

  int os_isr_enable (hcc_u32 pos)
  {
    if (pos==STM32UH_HOST_ISR)
    {
      NVIC_InitTypeDef NVIC_InitStructure;
      NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      return 0;
    }
    return 1;
  }

  int os_isr_disable (hcc_u32 pos)
  {
    if (pos==STM32UH_HOST_ISR)
    {
      NVIC_InitTypeDef NVIC_InitStructure;
      NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
      NVIC_Init(&NVIC_InitStructure);
      return 0;
    }
    return 1;
  }

#else

  static void(*isr_handler)(void);
  void os_idle (void);

  void OTG_FS_IRQHandler(void)
  {
  }

  void os_idle (void)
  {
    isr_handler();
  }

  int os_isr_init (hcc_u32 pos, void(*ih)(void))
  {
    if (pos==STM32UH_HOST_ISR)
    {
      isr_handler=ih;
      return 0;
    }
    return 1;
  }

  int os_isr_enable (hcc_u32 pos)
  {
    if (pos==STM32UH_HOST_ISR)
    {
      return 0;
    }
    return 1;
  }

  int os_isr_disable (hcc_u32 pos)
  {
    if (pos==STM32UH_HOST_ISR)
    {
      return 0;
    }
    return 1;
  }

#endif


#if _OS_MUTEX_COUNT
int os_mutex_create(OS_MUTEX_TYPE **pmutex)
{
  unsigned int i;
  for (i=0;i<_OS_MUTEX_COUNT;i++)
  {
    if (mutex_used[i]==0)
    {
      mutex_used[i]=1;
      *pmutex=&mutex[i];
      mutex[i]=0;
      return OS_SUCCESS;
    }
  }
  return OS_ERR;
}

int os_mutex_get(OS_MUTEX_TYPE *pmutex)
{
  if (*pmutex==(OS_MUTEX_TYPE)-1) return OS_ERR_MUTEX;
  while(*pmutex)  os_idle();
  *pmutex=1;
  return OS_SUCCESS;
}

int os_mutex_put(OS_MUTEX_TYPE *pmutex)
{
  if (*pmutex==(OS_MUTEX_TYPE)-1) return OS_ERR_MUTEX;
  *pmutex=0;
  return OS_SUCCESS;
}
#endif

#if _OS_EVENT_BIT_COUNT
int os_event_create(OS_EVENT_BIT_TYPE *event_bit)
{
  unsigned int j;
  if (event_used==(OS_EVENT_BIT_TYPE)-1) return OS_ERR;
  for (j=0;j<_OS_EVENT_BIT_COUNT;j++)
  {
    if ((event_used&(1<<j))==0)
    {
      event_used|=(1<<j);
      *event_bit=(OS_EVENT_BIT_TYPE)(1<<j);
      return OS_SUCCESS;
    }
  }
  return OS_ERR;
}

int os_event_get(OS_EVENT_BIT_TYPE event_bit)
{
  os_idle();
  if ((event&event_bit)==0) return OS_ERR;
  event&=~event_bit;
  return OS_SUCCESS;
}

int os_event_set(OS_EVENT_BIT_TYPE event_bit)
{
  //20110117 By Jason--event|=event_bit;
  event=event_bit;
  return OS_SUCCESS;
}

int os_event_set_int(OS_EVENT_BIT_TYPE event_bit)
{

//20110118 By Jason--{
if (event<2)
//20110118 By Jason--}
  	event|=event_bit;
  return OS_SUCCESS;
}
#endif


void os_delay (hcc_u32 ms)
{
  /* 2000 ticks / second (72MHz/36000) -> 2ticks/ms */
  HCC_TIM_ARR(6)=2*ms;
  HCC_TIM_CNT(6)=0;
  HCC_TIM_SR(6)=0;
  HCC_TIM_CR1(6)=1;
  while (HCC_TIM_SR(6)==0)
    ;
  HCC_TIM_CR1(6)=0;
}

int os_init (void)
{
#if _OS_MUTEX_COUNT
  unsigned int i;
#endif
#if _OS_EVENT_BIT_COUNT
  event_used=0;
#endif
#if _OS_MUTEX_COUNT
  for (i=0;i<_OS_MUTEX_COUNT;i++) mutex[i]=(OS_MUTEX_TYPE)-1;
  for (i=0;i<_OS_MUTEX_COUNT;i++) mutex_used[i]=0;
#endif

  HCC_S(HCC_RCC_APB1ENR,TIM6_EN);
  HCC_TIM_PSC(6)=36000;				        /* assuming 72 MHz */
  HCC_TIM_CR1(6)=0;

  return 0;
}





