#ifndef US_TIME_H
#define US_TIME_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

void     delay_us(unsigned long us);
void     delay_ms(unsigned long ms);
uint64_t get_us_time(void);
void     init_us_timer(void);


#ifdef __cplusplus
 }
#endif

#endif
