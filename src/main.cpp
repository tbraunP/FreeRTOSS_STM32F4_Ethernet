/*
 * main.c
 *
 *  Created on: 19.06.2012
 *      Author: braun
 */
#include "main.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "core_cm4.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_it.h"


GPIO_InitTypeDef GPIO_InitStructure;
volatile uint32_t TimingDelay;

void dDelay(uint32_t nTime);
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
//	dDelay(100);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay > 0)
  {
    TimingDelay--;
  }
}


void dDelay(uint32_t nTime)
{
  nTime*=100000;

  while(--nTime > 0);
}


// C++ test case
#ifdef __cplusplus

class A {
private:
	int i;
public:
	A():i(10) {

	};

	void setI(int i) {
		this->i=i;
	};

	int getI() {
		return i;
	};
};

class B {
public:
	B() {

	};

	virtual ~B() {

	};

	virtual void setF(int i) = 0;
	virtual int getF() = 0;
};

class C : public B {
private:
	int f;
public:
	virtual void setF(int i) {
		f=i;
	};

	virtual int getF() {
		return f;
	};
};

A a;
A aa;
#endif

int main(void) {
	// Enable UART
	Q_UART_init();

#ifdef __cplusplus

#endif
	//printf("Test");
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure PD12, 13, 14 and PD15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// activate systick (timebase 100 Hz -> 10 ms), using floating point else the
	// result will be 0 of the division!
	SysTick_Config(SystemCoreClock*(1/1000.0));
	//SysTick_Config(SystemCoreClock);

	while (1) {
		/* Set PD12 Green */
		GPIOD ->BSRRL = GPIO_Pin_12;
		/* Reset PD13 Orange, PD14 Red, PD15 Blue */GPIOD ->BSRRH = GPIO_Pin_13
				| GPIO_Pin_14 | GPIO_Pin_15;
		Delay(100L);

#ifdef __cplusplus
		if(a.getI()==10 && aa.getI()==10)
#endif
		{
			/* Set PD13 Orange */GPIOD ->BSRRL = GPIO_Pin_13;
			/* Reset PD12 Green, PD14 Red, PD15 Blue */GPIOD ->BSRRH =
					GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
			Delay(100L);
		}

#ifdef __cplusplus
		A* b = new A();
		if(b->getI()==10)
#endif
		{
			/* Set PD14 Red */
			GPIOD ->BSRRL = GPIO_Pin_14;
			/* Reset PD12 Green, PD13 Orange, PD15 Blue */GPIOD ->BSRRH =
					GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
			Delay(100L);
		}
#ifdef __cplusplus
		delete b;
#endif

#ifdef __cplusplus
		C* c = new C();
		c->setF(20);
		if(c->getF()==20)
#endif
		{
			/* Set PD15 Blue */
			GPIOD ->BSRRL = GPIO_Pin_15;
			/* Reset PD12 Green, PD13 Orange, PD14 Red */GPIOD ->BSRRH =
					GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
			Delay(100L);
		}
#ifdef __cplusplus
		delete c;
#endif
		Q_UART_DMAsendZTString("DingDong\n");
	}
}
