#ifndef __UART_H
#define __UART_H

#include "../hcc_types.h"
int uart_init (hcc_u32 Baudrate);
hcc_u16 uart_getkey (void);
void uart_send (hcc_u8 *BufferPtr, hcc_u32 Length);
void uart_putchar (hcc_u8 ch);

#endif

