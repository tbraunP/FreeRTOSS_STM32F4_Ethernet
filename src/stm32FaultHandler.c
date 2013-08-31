#include <stdio.h>
#include <stdint.h>

// handler include
#include "stm32f4xx_it.h"
#include "hw/uart.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
static char tmpOut[300];

#define POLL_PRINT(buffer, len, format, value)  {           \
                       snprintf(buffer, len, format, value); \
                       UART_poll_send(buffer); \
                   }

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress,
		uint8_t sourceFault) {
	/* These are volatile to try and prevent the compiler/linker optimising them
	 away as the variables never actually get used.  If the debugger won't show the
	 values of the variables, make them global my moving their declaration outside
	 of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	r0 = pulFaultStackAddress[0];
	r1 = pulFaultStackAddress[1];
	r2 = pulFaultStackAddress[2];
	r3 = pulFaultStackAddress[3];

	r12 = pulFaultStackAddress[4];
	lr = pulFaultStackAddress[5];
	pc = pulFaultStackAddress[6];
	psr = pulFaultStackAddress[7];

	// print handler
	switch (sourceFault) {
	case 0:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"NMI_Handler")
		break;
	case 1:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"HardFault_Handler")
		break;
	case 2:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"MemManage_Handler")
		break;
	case 3:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"BusFault_Handler")
		break;
	case 4:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"UsageFault_Handler")
		break;

	case 5:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"DebugMon_Handler")
		break;
	default:
		POLL_PRINT(tmpOut, 300, "\n\n[%s - all numbers in hex]\n",
				"Unknown source Handler")
		break;
	}

	// print register values
	printf("R0 = %x\n", (unsigned int) r0);
	printf("R1 = %x\n", (unsigned int) r1);
	printf("R2 = %x\n", (unsigned int) r2);
	printf("R3 = %x\n", (unsigned int) r3);
	printf("R12 = %x\n", (unsigned int) r12);
	printf("LR [R14] = %x  subroutine call return address\n",
			(unsigned int) lr);
	printf("PC [R15] = %x  program counter (caused the handler call) \n",
			(unsigned int) pc);
	printf("PSR = %x\n", (unsigned int) psr);
	printf("BFAR = %x\n", (*((volatile unsigned int *) (0xE000ED38))));
	printf("CFSR = %x\n", (*((volatile unsigned int *) (0xE000ED28))));
	printf("HFSR = %x\n", (*((volatile unsigned int *) (0xE000ED2C))));
	printf("DFSR = %x\n", (*((volatile unsigned int *) (0xE000ED30))));
	printf("AFSR = %x\n", (*((volatile unsigned int *) (0xE000ED3C))));
	printf("SCB_SHCSR = %x\n", (unsigned int) SCB->SHCSR);

	/* When the following line is hit, the variables contain the register values. */
	for (;;)
		;
}

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =0					                            \n"
			" ldr r3, handler0_address_const 			                \n"
			" bx r3                                                     \n"
			" handler0_address_const: .word prvGetRegistersFromStack    \n"
	);
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =1					                            \n"
			" ldr r3, handler1_address_const 			                \n"
			" bx r3                                                     \n"
			" handler1_address_const: .word prvGetRegistersFromStack    \n"
	);
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =2					                            \n"
			" ldr r3, handler2_address_const 			                \n"
			" bx r3                                                     \n"
			" handler2_address_const: .word prvGetRegistersFromStack    \n"
	);
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =3					                            \n"
			" ldr r3, handler3_address_const 			                \n"
			" bx r3                                                     \n"
			" handler3_address_const: .word prvGetRegistersFromStack    \n"
	);
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =4					                            \n"
			" ldr r3, handler4_address_const 			                \n"
			" bx r3                                                     \n"
			" handler4_address_const: .word prvGetRegistersFromStack    \n"
	);
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
	__asm volatile
	(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, =5					                            \n"
			" ldr r3, handler5_address_const 			                \n"
			" bx r3                                                     \n"
			" handler5_address_const: .word prvGetRegistersFromStack    \n"
	);
}
