/******************************************************************************
for IAR 7.xx-->



*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include  <stdio.h>
#include  <stdarg.h>
#include  "stm32f10x_conf.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


#define INIT_TABLE                      0x0800C800
#define UPDATE_INIT_TABLE               0x0803F000
//
// The type of a pointer into the init table.
typedef void const * table_ptr_t;
#if __ICCARM__
__no_init uint32_t __iar_SB @ r9;
#elif __GNUC__
__attribute__((section(".noinit"))) uint32_t __iar_SB ;
#endif
	
void __manual_data_init(void)
{
  uint32_t const * p ;
  uint32_t const * Oldpi ; 
  uint32_t size;
  uint32_t d ;
  uint32_t const * src;               //source data point
  uint32_t * dest;		                //destination data point
  //----------------------------------------------------------------------------
  p = (uint32_t const *) UPDATE_INIT_TABLE;
  
  // Check Data can't null
  if(*p == 0x00 || *p == 0xffffffff)
      return;
  
  //----------------------------------------------------------------------------
  p++;
  //_zero_init
  while ( ( size = *p++ ) != 0 )        //get clean size
  {
      d = *p++;		                //get clean memory start address

      if ( d & 1 )
      {
          d -= 1;
          d += __iar_SB;
      }
      
      dest = (uint32_t*) d;
      
      do
      {
          *dest++ = 0;
          size -= 4;
      } while ( size != 0 );

  }
  
  //----------------------------------------------------------------------------
  // _copy_init
  while( *p != 0 && *p != 0xffffffff )
  {
      //copy_init arrary second location
      p++ ;
      //copy_init
      if( ( size = *p++ ) != 0 )//get clean size
      {
          //get data memory address
          Oldpi = (uint32_t const *)((uint32_t)p - (UPDATE_INIT_TABLE-INIT_TABLE)) ; 
          src = (uint32_t *) ( (char const *) Oldpi + *(int32_t *) p ); 
          p++;
          //get destination memory address
          d = *p++;				

          if ( d & 1 )
          {
              d -= 1;
              d += __iar_SB;
          }

          dest = (uint32_t *) d;

          do
          {
              *dest++ = *src++;
              size -= 4;
          } while ( size != 0 );

          // check null
          if( *p == 0 )
              p++ ;
      }
  }
  //--------------------------------------------------------------------------------
}


//--------------------------------------------------------------------------------






