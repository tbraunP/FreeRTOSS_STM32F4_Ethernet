/*
 * fault_handler.h
 *
 *  Created on: 29.11.2012
 *      Author: pyro
 */

#ifndef FAULT_HANDLER_H_
#define FAULT_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_HANDLER_H_ */
