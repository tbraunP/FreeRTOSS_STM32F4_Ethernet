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

#include "periph/led.h"

#include <iostream>
#include <fstream>


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
	static A nnn;

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

A A::nnn{};


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

static A nnn{};


A a;
A aa;
#endif


int main(void) {
	// Enable UART
	Q_UART_init();

#ifdef __cplusplus

#endif
	LED_init();

	// activate systick (timebase 100 Hz -> 10 ms), using floating point else the
	// result will be 0 of the division!
	SysTick_Config(SystemCoreClock*(1/1000.0));
	//SysTick_Config(SystemCoreClock);

	while (1) {
		LED_On(LED_GREEN);
		LED_Off(LED_ORANGE | LED_BLUE | LED_RED);
		Delay(100L);


#ifdef __cplusplus
		if(a.getI()==10 && aa.getI()==10)
#endif
		{
			LED_On(LED_ORANGE);
			LED_Off(LED_GREEN | LED_BLUE | LED_RED);
			Delay(100L);
		}

#ifdef __cplusplus
		A* b = new A();
		if(b->getI()==10)
#endif
		{
			LED_On(LED_RED);
			LED_Off(LED_GREEN | LED_BLUE | LED_ORANGE);
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
			LED_On(LED_BLUE);
			LED_Off(LED_GREEN | LED_RED | LED_ORANGE);
			Delay(100L);
		}
#ifdef __cplusplus
		delete c;
#endif
		//Q_UART_DMAsendZTString("DingDong\n");
		//std::cout << "Yeah\n";
		printf("DingDong2\n");

	}
}
