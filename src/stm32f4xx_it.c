/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f4x7_eth.h"

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* lwip includes */
#include "lwip/sys.h"

// Semaphore for packet reception
extern xSemaphoreHandle s_xSemaphore;

/**
 * @brief  This function handles External line 10 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET) {
		Eth_Link_ITHandler(DP83848_PHY_ADDRESS);
		/* Clear interrupt pending bit */
		EXTI_ClearITPendingBit(ETH_LINK_EXTI_LINE);
	}
}

/**
 * @brief  This function handles ethernet DMA interrupt request.
 * @param  None
 * @retval None
 */
void ETH_IRQHandler(void) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Frame received */
	if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) {
		/* Give the semaphore to wakeup LwIP task */
		xSemaphoreGiveFromISR(s_xSemaphore, &xHigherPriorityTaskWoken);
	}

	/* Clear the interrupt flags. */
	/* Clear the Eth DMA Rx IT pending bits */
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);

	/* Switch tasks if necessary. */
	if (xHigherPriorityTaskWoken != pdFALSE) {
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
