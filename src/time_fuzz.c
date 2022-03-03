#include <sbi/sbi_timer.h>
#include "time_fuzz.h"
#include "enclave.h"
#include "cpu.h"

void wait_until_epoch() {
  enclave_id eid = cpu_get_enclave_id();
  struct enclave* enclave = get_enclave(eid);
  if (enclave->fuzzy_status == FUZZ_ENABLED) {
    // fuzz
  }
  
  // TODO(chungmcl): put this inside if statement later -- force pause
  // on everything for now for debugging
  const struct sbi_timer_device* device = sbi_timer_get_device();
  unsigned long long msPassed = (sbi_timer_value() / device->timer_freq) * 1000;
  // pause until next 10 ms block
  sbi_timer_mdelay(10 - (msPassed % 10));
}

unsigned long get_time_ticks() {
  return sbi_timer_value();
}

unsigned long get_epoch_interval_len_ticks() {
  // For now, arbitrarily defining an interval as a 10 ms block
  const struct sbi_timer_device* device = sbi_timer_get_device();
  return 1000 * device->timer_freq;
}