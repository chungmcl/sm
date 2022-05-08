#ifndef _TIME_FUZZ_H_
#define _TIME_FUZZ_H_

void fix_time_interval();
void wait_until_epoch();
void wait_for_ms(unsigned long ms);
unsigned long get_time_ticks();
unsigned long get_granularity_ticks();

#endif