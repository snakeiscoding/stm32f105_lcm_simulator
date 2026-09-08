/***************************************************************************

 ***************************************************************************/

#ifndef _VA_DEVICE_H_
#define _VA_DEVICE_H_

#include "hcc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VA_MAX_DEVICE         1
#define VA_RETRY_COUNT				3
#define VA_RETRY_WAIT_MS			10


/* Error codes returned by the lower layer communication driver. */
#define ERR_IO_NONE     0
#define ERR_IO_CHECK    1
#define ERR_IO_REMOVED  2
 
/* Transfer directions. */
#define VA_DIR_NONE 2
#define VA_DIR_IN   1
#define VA_DIR_OUT  0  


typedef int va_rw_func( void *dev_hdl ,hcc_u8 dir , void *buf , hcc_u32 buf_length ,hcc_u32 *rlength );
typedef hcc_u8 va_st_func(void *dev_hdl);

int VAD_init(void);
int VAD_open(hcc_u8 dev, va_rw_func *f, va_st_func *st, void *dev_hdl);
int VA_close(hcc_u8 dev);
int VAD_SendPacket(void);



hcc_u32 VAD_GetResponseResult(void) ;
hcc_u32 VAD_GetResponseCommand(void) ;
hcc_u32 VAD_GetResponseParameter(void) ;
hcc_u32 VAD_GetResponseDataLength(void) ;
hcc_u8* VAD_GetResponseDataAddress(void) ;
void VAD_SendPacketReset(void) ;
void VAD_ReceiverPacketReset(void) ;


void VAD_SetCommand(long command,long parameter1,long parameter2,long length,hcc_u8 *addr ) ;




#ifdef __cplusplus
}
#endif

#endif


