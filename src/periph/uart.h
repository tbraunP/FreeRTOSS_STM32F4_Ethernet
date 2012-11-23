/*
 * uart.h
 *
 *  Created on: 06.04.2012
 *      Author: pyro
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include "stm32f4xx.h"

#include "../util/linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Init Usart2 on GPIO_Pin_5 and  on Port GPIOD
 *
 */
void Q_UART_init();

/**
 * Print a string (using busy waiting)
 * \param buf - null terminated string
 */
void Q_UART_sendZTString(const char* buf);

/**
 * Send string via USART2 using DMA
 *
 * @param buf must be '\0' terminated
 */
void Q_UART_DMAsendZTString(const char* buf);

/**
 * DMA Transfer finished (runs within interrupt context)
 */
void Q_UART_DMA_TXComplete();

#ifdef __cplusplus
}
#endif
#endif /* UART_H_ */
