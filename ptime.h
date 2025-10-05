#pragma once

// Simple performance timing utilities for macOS.
//
// API overview:
//   - ptime_init(): optional; initializes the timing subsystem (called lazily).
//   - ptime_now(): capture a high‑resolution timestamp.
//   - ptime_ns_between(a,b): elapsed nanoseconds from a to b.
//   - ptime_s_between(a,b): elapsed seconds (double) from a to b.
//   - ptime_ns_since(t): elapsed nanoseconds from t to now.
//   - ptime_s_since(t): elapsed seconds (double) from t to now.
//
// Implementation uses mach_absolute_time() on Apple platforms for monotonic,
// high‑resolution timing that is not subject to wall‑clock changes.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t ticks; // raw mach_absolute_time() ticks
} ptime_point;

// Initialize the timing system. This is optional because the module performs
// lazy initialization on first use. It is provided for explicit control.
void ptime_init(void);

// Capture current high‑resolution monotonic timestamp.
ptime_point ptime_now(void);

// Elapsed time helpers
uint64_t ptime_ns_between(ptime_point start, ptime_point end);
double    ptime_s_between(ptime_point start, ptime_point end);

static inline uint64_t ptime_ns_since(ptime_point start) {
    return ptime_ns_between(start, ptime_now());
}

static inline double ptime_s_since(ptime_point start) {
    return ptime_s_between(start, ptime_now());
}

#ifdef __cplusplus
}
#endif
