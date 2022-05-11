#include <sbi/sbi_timer.h>
#include "time_fuzz.h"
#include "enclave.h"
#include "cpu.h"
#include <sbi/sbi_console.h> // for sbi_printf() for debugging

#define GRANULARITY_MS 10

unsigned long t_end_ticks = 0;
unsigned long fuzz_clock_ticks;
unsigned long granularity_ticks;
unsigned long ticks_per_ms;

void fuzzy_time_init() {
  // (ticks / second) * (seconds / milliseconds) = (ticks / milliseconds)
  ticks_per_ms = sbi_timer_get_device()->timer_freq / 1000;
  // milliseconds * (ticks / milliseconds) = ticks
  granularity_ticks = GRANULARITY_MS * ticks_per_ms;
  unsigned long t = sbi_timer_value();
  t_end_ticks = t - (t % granularity_ticks);
}

void fix_time_interval(unsigned long t) {
  // back compute
  //
  // t_end_ticks == when the current tick ends
  // fuzz_clock_ticks == current fuzzed time
  // g == granularity/interval max
  //
  // keep going until t_end is in the future

  // may need to have safety mechanism to check for verrrry long gaps
  // between real_time and t_end_ticks? and skip this process
  // print with sbi_printf("");
  while (t_end_ticks < t) {
    fuzz_clock_ticks = t_end_ticks - (t_end_ticks % granularity_ticks);
    // TODO: ENSURE UNIFORMLY RANDOM! IT ISN'T RIGHT NOW!
    unsigned long rand = sbi_sm_random();
    // sbi_printf("sbi_sm_random: %lu\n", rand);
    unsigned long to_add = rand % (granularity_ticks + 1);
    t_end_ticks += to_add;
    // sbi_printf("t_end_ticks += %lu\n", to_add);
  }
}

void wait_until_epoch() {
  unsigned long start_fuzz_ticks = fuzz_clock_ticks;
  while (start_fuzz_ticks == fuzz_clock_ticks) {
    // sbi_printf("start_fuzz_ticks: %lu\n", start_fuzz_ticks);
    // sbi_printf("fuzz_clock_ticks: %lu\n", fuzz_clock_ticks);
    unsigned long t = sbi_timer_value();
    unsigned long delta_ticks = t_end_ticks - t;
    // ticks / (ticks / milliseconds) = milliseconds
    unsigned long delay_ms = delta_ticks / ticks_per_ms;
    if (t < t_end_ticks) {
      // sbi_printf("ms delay: %lu\n", delay_ms);
      sbi_timer_mdelay(delay_ms);
    }
    fix_time_interval(t);
  }
}

void wait_for_ms(unsigned long ms) {
  sbi_timer_mdelay(ms);
}

unsigned long get_time_ticks() {
  /*enclave_id eid = cpu_get_enclave_id();
  struct enclave* enclave = get_enclave(eid);
  if (enclave->fuzzy_status == FUZZ_ENABLED)*/ {
    fix_time_interval(sbi_timer_value());
    return fuzz_clock_ticks;
  }
}

unsigned long get_granularity_ticks() {
  return granularity_ticks;
}