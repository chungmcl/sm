#include <sbi/sbi_timer.h>
#include "time_fuzz.h"
#include "enclave.h"
#include "cpu.h"

#define GRANULARITY_MS 10

unsigned long t_end_ticks = 0;
unsigned long fuzz_clock_ticks;

void fix_time_interval() {
  // back compute
  //
  // t_end_ticks == when the current tick ends
  // fuzz_clock_ticks == current fuzzed time
  // g == granularity/interval max
  //
  // keep going until t_end is in the future
  while (t_end_ticks < sbi_timer_value()) {
    fuzz_clock_ticks = t_end_ticks - (t_end_ticks % get_granularity_ticks());
    // TODO: ENSURE UNIFORMLY RANDOM! IT ISN'T RIGHT NOW!
    t_end_ticks += sbi_sm_random() % (get_granularity_ticks() + 1);
  }
}

void wait_until_epoch() {
  fix_time_interval();
  // ticks / (ticks / second) = seconds
  // seconds * 1000 = milliseconds
  unsigned long delta_ticks = t_end_ticks - fuzz_clock_ticks;
  sbi_timer_mdelay((delta_ticks / sbi_timer_get_device()->timer_freq) * 1000);
}

void wait_for_ms(unsigned long ms) {
  sbi_timer_mdelay(ms);
}

unsigned long get_time_ticks() {
  /*enclave_id eid = cpu_get_enclave_id();
  struct enclave* enclave = get_enclave(eid);
  if (enclave->fuzzy_status == FUZZ_ENABLED)*/ {
    fix_time_interval();
    return fuzz_clock_ticks;
  }
}

unsigned long get_granularity_ticks() {
  // (ticks / second) * (seconds / milliseconds) = (ticks / milliseconds)
  // milliseconds * (ticks / milliseconds) = ticks
  return GRANULARITY_MS * (sbi_timer_get_device()->timer_freq / 1000);
}