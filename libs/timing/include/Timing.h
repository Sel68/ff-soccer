#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

typedef uint32_t (*OsTicksFunctionPtr)(void);

extern OsTicksFunctionPtr OsTicksFunction;

// Function pointer type for getting system tick count

// Setter for the tick function (e.g., osKernelGetTickCount or a mock)
void RegisterClock(OsTicksFunctionPtr funtion_name);

/**
 * Utilities for time conversion.
 */

inline uint32_t GetCurrentTimeUs() { return OsTicksFunction(); }

uint32_t GetCurrentTimeMs();

// Subtracts offset from current time (in master clock)
uint32_t GetCurrentCalibratedTimeUs();

// Next calibrated cycle time (heartbeat in ms)
uint32_t GetNextCalibratedCycleTime();

// Direct setting
void SetTimeOffset(int32_t new_offset);

// Updates the offset which is subtracted when time is calculated
void UpdateTimeOffset(int32_t new_offset);

// Gets the time offset
int32_t GetTimeOffset();

// Difference between two uint32_t times
int32_t TimeDiff(uint32_t later, uint32_t earlier);

// Gets time in the master clock
uint32_t ToHostTime(uint32_t device_time, int32_t offset);

// Gets time in the slave clock
uint32_t ToDeviceTime(uint32_t host_time, int32_t offset);

#endif  // TIMING_H