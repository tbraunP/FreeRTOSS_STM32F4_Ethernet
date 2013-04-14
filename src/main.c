#include "stm32f4xx.h"
#include "uart.h"
#include "ansi.h"
#include "ustime.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define LED_RED		(GPIO_Pin_14)
#define LED_BLUE	(GPIO_Pin_15)
#define LED_ORANGE	(GPIO_Pin_13)
#define LED_GREEN	(GPIO_Pin_12)

#define LED_On(led)		GPIOD ->BSRRL |= led
#define LED_Off(led)	GPIOD ->BSRRH |= led

void LED_init(){
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12, 13, 14 and PD15 in output pushpull mode */
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void set_leds(int leds)
{
    GPIOD->ODR = (GPIOD->ODR & ~0xF000) | ((leds << 12) & 0xF000);
}

struct task_param {
    char *name;
    int   interval;
};

static void fpu_task(void *pvParameters)
{
    LED_On(LED_BLUE);
    // Force x to stay in a FPU reg.
    //
    register float x = 0;
    struct task_param *p = pvParameters;

    for(;;) {
        printf("%s: x = %f\n", p->name, x);
        x += 1;
        vTaskDelay(p->interval);
        LED_On(LED_BLUE);
        vTaskDelay(p->interval);
        LED_Off(LED_BLUE);
    }
   
}

static void init_task(void *pvParameters)
{
 LED_On(LED_ORANGE);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIO_Init(GPIOD, &(GPIO_InitTypeDef) {
        .GPIO_Pin   = 0xF000,
        .GPIO_Mode  = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd  = GPIO_PuPd_NOPULL,
        .GPIO_Speed = GPIO_Speed_50MHz,
    });

    uart_init(115200);
    printf(ANSI_FG_LTRED "STM32F407" ANSI_NORMAL " FreeRTOS Test\n" );

    vTaskDelay(100);

    for (int i=0; i<5; i++) {
        printf("Starting FPU task %d..\n", i);

        struct task_param *p;

        p = malloc(sizeof(struct task_param));
        p->name     = malloc(16);
        p->interval = (i+1) * 500;
        sprintf(p->name, "FPU_%d", i);
        LED_On(LED_GREEN);

        xTaskCreate(fpu_task, (int8_t*)p->name, 1024, p, tskIDLE_PRIORITY, NULL);
    }

    for(;;);
}


int main(void)
{
    // FreeRTOS assumes 4 preemption- and 0 subpriority-bits
    //
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    LED_init();      
    LED_On(LED_RED);
    // Create init task and start the scheduler
    //
    xTaskCreate(init_task, (signed char*)"init", 1024, NULL, tskIDLE_PRIORITY, NULL);
    vTaskStartScheduler();
}
