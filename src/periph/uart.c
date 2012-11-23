/*
 * uart.c
 *
 *  Created on: 06.04.2012
 *      Author: pyro
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"

#include "../util/linked_list.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

// UART send lst
NODE_T volatile* volatile lst;

DMA_InitTypeDef USART2_DMA_InitStruct;

/**
 * Config DMA Channel Stream6 for reading
 */
void Q_UART_configDMA() {
	// DMA configuration
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Stream6 );
	DMA_StructInit(&USART2_DMA_InitStruct);

	USART2_DMA_InitStruct.DMA_Channel = DMA_Channel_4;

	/* Initialize the DMA_PeripheralBaseAddr member */
	USART2_DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) &(USART2 ->DR);

	/* Initialize the DMA_Memory0BaseAddr member */
	// set on transfer I2C_DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) &newValue;
	/* Initialize the DMA_DIR member */
	USART2_DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;

	/* Initialize the DMA_PeripheralInc member */
	USART2_DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	/* Initialize the DMA_MemoryInc member */
	USART2_DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;

	/* Priority */
	USART2_DMA_InitStruct.DMA_Priority = DMA_Priority_Low;

	/* Initialize the DMA_PeripheralDataSize member */
	USART2_DMA_InitStruct.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;

	/* Initialize the DMA_MemoryDataSize member */
	USART2_DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

	// allow fifo
	USART2_DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;

	// configure DMA interrupts
	NVIC_InitTypeDef UARTNVIC_InitStruct;
	UARTNVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	UARTNVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	UARTNVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_Init(&UARTNVIC_InitStruct);
}

static inline void Q_UART_startDMA() {
	DMA_Init(DMA1_Stream6, &USART2_DMA_InitStruct);

	// Enable and reset finish bit
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6 );

	DMA_Cmd(DMA1_Stream6, ENABLE);

	/* Enable USART2 DMAReq_Tx-Request */
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
}

static inline void Q_UART_stopDMA() {
	USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);

	DMA_Cmd(DMA1_Stream6, DISABLE);

	// Enable and reset finish bit
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, DISABLE);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6 );

	DMA_DeInit(DMA1_Stream6 );
}

void Q_UART_init() {
	// GPIO Enable + Clock + Pin settings
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// Pin config
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Connect Pins to UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2 );
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2 );

	// Set Alternate Pins
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Enable Clock for UART
	USART_DeInit(USART2 );
	USART_InitTypeDef USART_conf;
	USART_StructInit(&USART_conf);
	USART_conf.USART_BaudRate = 115200;
	USART_Init(USART2, &USART_conf);

	Q_UART_configDMA();
	USART_Cmd(USART2, ENABLE);
}

/**
 * Send string via USART2 using busy waiting
 *
 * @param buf must be '\0' terminated
 */
void Q_UART_sendZTString(const char* buf) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC ) != SET)
		;

	while (*buf) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE ) != SET)
			;
		USART_SendData(USART2, *buf);
		++buf;
	}

	while (USART_GetFlagStatus(USART2, USART_FLAG_TC ) != SET)
		;
}

/**
 * Send char via USART2 using busy waiting
 *
 * @param buf must be '\0' terminated
 */
void Q_UART_sendChar(const char c) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC ) != SET)
		;

	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE ) != SET)
		;
	USART_SendData(USART2, c);
}

/**
 * Send string via USART2 using DMA
 *
 * @param buf must be '\0' terminated
 */
void Q_UART_DMAsendZTString(const char* buf) {
	size_t len = strlen(buf);
	Q_UART_DMAsendString(buf, len);
}

/**
 * Send string via USART2 using DMA adding '0' to the end of the string
 *
 * \param buf - String
 * \param cnt - length of buf string
 */
void Q_UART_DMAsendString(const char* buf, size_t cnt) {
	if (cnt > 0) {
		//lock DMA irq
		NVIC_DisableIRQ(DMA1_Stream6_IRQn);

		// copy data to heap incl. \0 at the end
		char* data = (char*) malloc(sizeof(char) * (cnt + 1));
		memcpy(data, buf, sizeof(char) * (cnt));
		data[cnt] = '0';

		if (lst == NULL ) {
			lst = list_create(data);

			// configure and start DMA
			USART2_DMA_InitStruct.DMA_BufferSize = cnt;
			USART2_DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) lst->data;
			Q_UART_startDMA();
		} else {
			list_append(lst, data);
		}
		//unlock DMA
		NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	}
}

// callback for transfer finished interrupt
void Q_UART_DMA_TXComplete() {
	// wait for transfer finish
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE != SET))
		;
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC ) != SET)
		;

	// stop dma
	Q_UART_stopDMA();

	// free first node
	NODE_T volatile* volatile tmp = lst->next;
	//*((uint8_t*) lst->data) = '1';
	free(lst->data);
	free((void*) lst);
	lst = tmp;

	// wait till transfer is finished, before starting new one
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC ) != SET)
		;

	// check for next element and start DMA is needed
	if (lst != NULL ) {
		// configure and start DMA
		USART2_DMA_InitStruct.DMA_BufferSize = strlen((const char*) lst->data);
		USART2_DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t) lst->data;

		// re-Init DMA transfer
		Q_UART_startDMA();
	}
}
