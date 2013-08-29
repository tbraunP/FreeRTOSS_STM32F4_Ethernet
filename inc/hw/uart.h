#ifndef UART_H
#define UART_H

#include <sys/types.h>
#include <reent.h>

#ifdef __cplusplus
 extern "C" {
#endif

void UART_init(int baudrate);
void DMA2_Channel7_IRQHandler();
void UART_poll_send(const char *ch);
ssize_t UART_write_r(struct _reent *r, int fd, const void *ptr, size_t len);
ssize_t UART_read_r(struct _reent *r, int fd, void *ptr, size_t len);

#ifdef __cplusplus
}
#endif

#endif
