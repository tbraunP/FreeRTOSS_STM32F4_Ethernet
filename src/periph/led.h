/*
 * led.h
 *
 *  Created on: 25.11.2012
 *      Author: pyro
 */

#ifndef LED_H_
#define LED_H_
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_RED		(GPIO_Pin_14)
#define LED_BLUE	(GPIO_Pin_15)
#define LED_ORANGE	(GPIO_Pin_13)
#define LED_GREEN	(GPIO_Pin_12)

void LED_init();

#define LED_On(led)		GPIOD ->BSRRL |= led
#define LED_Off(led)	GPIOD ->BSRRH |= led


#ifdef __cplusplus
}
#endif

#endif /* LED_H_ */
