#include "hw/uart.h"
#include "stm32f4xx.h"
#include "util/ringbuf.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "semphr.h"

#define RX_SIZE  (256)
#define TX_SIZE  (256)

#define USART_TX_DMA	(DMA2_Stream7)
#define OUT_USART	(USART1)

volatile static struct {
//	ringbuf_t rx_buf;
     ringbuf_t tx_buf;

     struct uart_stats {
          uint32_t rx_overrun;
          uint32_t tx_overrun;
     } uart_stats;

     DMA_InitTypeDef uartTXDMA;
     bool dmaRunning;
} uart_state;

static inline void startDMAFromBuffer(){
     // copy message
     int len = uart_state.tx_buf.len;
     uint8_t* str = ( uint8_t* ) malloc ( len * sizeof ( uint8_t ) );
     len = rb_read ( ( ringbuf_t* ) &uart_state.tx_buf, str, len );
     uart_state.uartTXDMA.DMA_Memory0BaseAddr = ( uint32_t ) str;
     uart_state.uartTXDMA.DMA_BufferSize = len;

     // start transfer
     // DMA Request: Always call DMA_Init and DMA_CMD
     DMA_Init ( USART_TX_DMA, ( DMA_InitTypeDef* ) &uart_state.uartTXDMA );
     DMA_Cmd ( USART_TX_DMA, ENABLE );
     uart_state.dmaRunning = true;
}

void DMA2_Stream7_IRQHandler ( void ){
     
     DMA_Cmd ( USART_TX_DMA, DISABLE );
     DMA_ClearITPendingBit ( USART_TX_DMA, DMA_IT_TCIF7 );

     // ready for next transmission
     USART_ClearITPendingBit ( OUT_USART, USART_IT_TC );

     vPortEnterCritical();
     {
	// free current memory of finished transmission
	free ( ( uint8_t* ) uart_state.uartTXDMA.DMA_Memory0BaseAddr );
	uart_state.uartTXDMA.DMA_Memory0BaseAddr = (uint32_t) NULL;
    
	// check if we should start a new transfer
	if ( uart_state.tx_buf.len > 0 ) {
	  startDMAFromBuffer();
	}else{
	  uart_state.dmaRunning = false;
	}
     }
     vPortExitCritical();
}

ssize_t UART_write_r ( struct _reent *r, int fd, const void *ptr, size_t len ){
     int ln = 0;
     vPortEnterCritical();
     {
          ln = rb_write ( ( ringbuf_t* ) &uart_state.tx_buf, ( const uint8_t* ) ptr, len );
          if ( ln != len ) {
               ++uart_state.uart_stats.tx_overrun;
          }

          
          // start a new transfer if none is running
          if ( !uart_state.dmaRunning && ln > 0 ) {
               startDMAFromBuffer();
          }
     }
     vPortExitCritical();
     return ln;
}

ssize_t UART_read_r ( struct _reent *r, int fd, void *ptr, size_t len ){
     /*
      while (!rx_buf.len)
      ;

      if (len > rx_buf.len)
      len = rx_buf.len;

      char *c = (char*) ptr;
      for (int i = 0; i < len; i++)
      rb_getc(&rx_buf, c++);
      */
     return len;
}

void UART_poll_send ( const char *ch ){
     while ( *ch ) {
          OUT_USART->DR = *ch++ & 0xff;
          while ( ! ( OUT_USART->SR & USART_FLAG_TXE ) )
               ;
     }
}

/**
 * Initialize UART.
 *
 * \param  baudrate  Baudrate
 *
 *  PB6   USART1_TXD
 *  PB7   USART1_RXD
 *
 */
void UART_init ( int baudrate ){
     // allocate memory
     rb_alloc ( ( ringbuf_t* ) &uart_state.tx_buf, TX_SIZE );
     uart_state.dmaRunning = false;

     // Enable peripheral clocks
     RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB;
     RCC->APB2ENR |= RCC_APB2Periph_USART1;

     // Initialize Serial Port
     GPIO_Init ( GPIOB, & ( GPIO_InitTypeDef ) {
       .GPIO_Pin = GPIO_Pin_6,
       .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF,
       .GPIO_OType = GPIO_OType_PP
     } );

     GPIO_Init ( GPIOB, & ( GPIO_InitTypeDef ) {
       .GPIO_Pin = GPIO_Pin_7, 
       .GPIO_Mode =GPIO_Mode_AF, 
       .GPIO_PuPd = GPIO_PuPd_UP
     } );

     GPIO_PinAFConfig ( GPIOB, GPIO_PinSource6, GPIO_AF_USART1 );
     GPIO_PinAFConfig ( GPIOB, GPIO_PinSource7, GPIO_AF_USART1 );

     USART_Init ( OUT_USART, & ( USART_InitTypeDef ) {
          .USART_BaudRate = baudrate,
          .USART_WordLength = USART_WordLength_8b, 
	  .USART_StopBits = USART_StopBits_1, 
	  .USART_Parity = USART_Parity_No,
          .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
          .USART_Mode = USART_Mode_Tx
     } );

     // enable DMA requests and USART
     USART_DMACmd ( OUT_USART, USART_DMAReq_Tx, ENABLE );
     USART_Cmd ( OUT_USART, ENABLE );

     // Enable DMA
     RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_DMA2, ENABLE );
     DMA_DeInit ( USART_TX_DMA );
     DMA_StructInit ( ( DMA_InitTypeDef* ) &uart_state.uartTXDMA );

     // configure dma
     uart_state.uartTXDMA.DMA_Channel = DMA_Channel_4;
     uart_state.uartTXDMA.DMA_DIR = DMA_DIR_MemoryToPeripheral;
     uart_state.uartTXDMA.DMA_PeripheralBaseAddr = ( uint32_t ) & ( USART1->DR );
     uart_state.uartTXDMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
     uart_state.uartTXDMA.DMA_Memory0BaseAddr = (uint32_t) 0;

     // Enable DMA finished interrupt
     DMA_ITConfig ( USART_TX_DMA, DMA_IT_TC, ENABLE );
     DMA_ClearITPendingBit ( USART_TX_DMA, DMA_IT_TCIF7 );

     // Enable DMA Transfer finished interrupt for DMA
     NVIC_Init ( & ( NVIC_InitTypeDef ) {
          .NVIC_IRQChannel = DMA2_Stream7_IRQn,
           .NVIC_IRQChannelPreemptionPriority =
                configLIBRARY_KERNEL_INTERRUPT_PRIORITY,
                .NVIC_IRQChannelSubPriority = 1, .NVIC_IRQChannelCmd =
                          ENABLE
     } );

     // start transfer
     //uart_state.uartTXDMA.DMA_BufferSize = 20;
     //uart_state.uartTXDMA.DMA_Memory0BaseAddr = startAdresse;
     //DMA_Init(DMA2_Stream7, &uart_state.uartTXDMA);
     //DMA_Cmd(DMA2_Stream7, ENABLE);
}
