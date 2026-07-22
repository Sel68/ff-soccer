#include "Timing.h"

// Static pointer to the system tick function
// static OsTicksFunctionPtr OsTicksFunction = 0;
OsTicksFunctionPtr OsTicksFunction = 0;

// Time offset to master. slave_clock - time_offset = master_clock
static int32_t time_offset = 0;

void RegisterClock(OsTicksFunctionPtr function_name) { OsTicksFunction = function_name; }

// uint32_t GetCurrentTimeUs() { return OsTicksFunction(); }

uint32_t GetCurrentTimeMs() { return GetCurrentTimeUs() / 1000; }

uint32_t GetCurrentCalibratedTimeUs() {
  return OsTicksFunction ? (GetCurrentTimeUs() + time_offset) : 0;
}

// Direct setting
void SetTimeOffset(int32_t new_offset) {
  time_offset = new_offset;
}

// Updates the offset which is subtracted when time is calculated
void UpdateTimeOffset(int32_t new_offset, bool registering) {
  // if (registering) {
  SetTimeOffset(new_offset);
  // return;
  // }
  // time_offset = (int32_t)(alpha * (time_offset) + (1.f - alpha) * new_offset);
  // When offsets are large, above one causes problems. Claude suggested this:
  // time_offset = (time_offset + new_offset) >> 1;

  // if (new_offset % 2 == 0 && time_offset % 2 != 0) {
  //   time_offset += 1;
  // } else if (new_offset % 2 != 0 && time_offset % 2 == 0) {
  //   time_offset += 1;
  // }
}

int32_t GetTimeOffset() { return time_offset; }

int32_t TimeDiff(uint32_t later, uint32_t earlier) { return (int32_t)(later - earlier); }

uint32_t ToHostTime(uint32_t device_time, int32_t offset) { return device_time + offset; }

uint32_t ToDeviceTime(uint32_t host_time, int32_t offset) { return host_time - offset; }