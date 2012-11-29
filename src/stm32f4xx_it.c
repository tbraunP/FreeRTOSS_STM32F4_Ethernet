/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"

// include needed since extern "C" must be considered by compiler during creating
// the object file
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"
#include "main.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
 * @{
 */

/** @addtogroup CAN_Networking
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler() {
	TimingDelay_Decrement();
}

void DMA1_Stream6_IRQHandler() {
	Q_UART_DMA_TXComplete();
}

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
