/*
 * fault_handler.c
 *
 *  Created on: 29.11.2012
 *      Author: pyro
 */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "fault_handler.h"
#include "../periph/uart.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Fault Handlers                   */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	int *faultStack = 0;

	// extract the top of stack location
	asm volatile (
			"		TST LR, #4" "\n\t" // determine which stackpointer has to be used (single or dual stack)
			"		ITE EQ" "\n\t"
			"		MRSEQ R0, MSP		" "\n\t"// Main Stackpointer is relevant
			"		MRSNE R0, PSP		" "\n\t"// Process Stackpointer is relevant
			: "=r"(faultStack) : : );

	unsigned int stacked_r0 = ((unsigned int) faultStack[0]);
	unsigned int stacked_r1 = ((unsigned int) faultStack[1]);
	unsigned int stacked_r2 = ((unsigned int) faultStack[2]);
	unsigned int stacked_r3 = ((unsigned int) faultStack[3]);

	unsigned int stacked_r12 = ((unsigned int) faultStack[4]);
	unsigned int stacked_lr = ((unsigned int) faultStack[5]);
	unsigned int stacked_pc = ((unsigned int) faultStack[6]);
	unsigned int stacked_psr = ((unsigned int) faultStack[7]);

	// using a save output method without further interrupts (busy waiting)
	char str[200];
	snprintf(str, 200, "\n\n[Hard fault handler - all numbers in hex]\n");
	Q_UART_sendZTString(str);
	snprintf(str, 200, "R0 = %x\n", stacked_r0);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "R1 = %x\n", stacked_r1);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "R2 = %x\n", stacked_r2);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "R3 = %x\n", stacked_r3);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "R12 = %x\n", stacked_r12);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "LR [R14] = %x  subroutine call return address\n",
			stacked_lr);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "PC [R15] = %x  program counter\n", stacked_pc);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "PSR = %x\n", stacked_psr);
	Q_UART_sendZTString(str);
	snprintf(str, 200, "BFAR = %x\n",
			(*((volatile unsigned int *) (0xE000ED38))));
	Q_UART_sendZTString(str);
	snprintf(str, 200, "CFSR = %x\n",
			(*((volatile unsigned int *) (0xE000ED28))));
	Q_UART_sendZTString(str);
	snprintf(str, 200, "HFSR = %x\n",
			(*((volatile unsigned int *) (0xE000ED2C))));
	Q_UART_sendZTString(str);
	snprintf(str, 200, "DFSR = %x\n",
			(*((volatile unsigned int *) (0xE000ED30))));
	Q_UART_sendZTString(str);
	snprintf(str, 200, "AFSR = %x\n",
			(*((volatile unsigned int *) (0xE000ED3C))));
	Q_UART_sendZTString(str);
	snprintf(str, 200, "SCB_SHCSR = %x\n", (unsigned int) SCB ->SHCSR);
	Q_UART_sendZTString(str);

	while (1)
		;

}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}
