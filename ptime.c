#include "ptime.h"

#if defined(__APPLE__)
#include <mach/mach_time.h>
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#include <time.h>
#endif

// Conversion state
#if defined(__APPLE__)
static mach_timebase_info_data_t g_timebase = {0, 0};
static pthread_once_t g_ptime_once = PTHREAD_ONCE_INIT;

static void ptime_init_once_body(void) {
    (void)mach_timebase_info(&g_timebase);
    if (g_timebase.denom == 0) {
        // Fallback to 1:1 to avoid division by zero (should not happen)
        g_timebase.numer = 1;
        g_timebase.denom = 1;
    }
}

void ptime_init(void) {
    pthread_once(&g_ptime_once, ptime_init_once_body);
}

ptime_point ptime_now(void) {
    ptime_init();
    ptime_point p;
    p.ticks = mach_absolute_time();
    return p;
}

static inline uint64_t ticks_to_ns(uint64_t dticks) {
    // Use 128-bit to avoid overflow: ns = dticks * numer / denom
#if defined(__SIZEOF_INT128__)
    __uint128_t num = (__uint128_t)dticks * (uint64_t)g_timebase.numer;
    return (uint64_t)(num / g_timebase.denom);
#else
    // Fallback: do it in double; may lose precision for very large intervals
    long double v = (long double)dticks;
    v *= (long double)g_timebase.numer;
    v /= (long double)g_timebase.denom;
    if (v < 0) v = 0;
    if (v > 1.84e19L) v = 1.84e19L; // clamp to ~UINT64_MAX
    return (uint64_t)v;
#endif
}

uint64_t ptime_ns_between(ptime_point start, ptime_point end) {
    uint64_t dt = end.ticks - start.ticks;
    return ticks_to_ns(dt);
}

double ptime_s_between(ptime_point start, ptime_point end) {
    return (double)ptime_ns_between(start, end) / 1e9;
}

#elif defined(_WIN32)

// Windows high-performance timer using QueryPerformanceCounter
static INIT_ONCE g_ptime_once = INIT_ONCE_STATIC_INIT;
static LARGE_INTEGER g_qpc_freq = {0};
static double g_ns_per_tick = 0.0; // nanoseconds per performance counter tick

static BOOL CALLBACK ptime_init_once_body(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
    (void)InitOnce; (void)Parameter; (void)Context;
    if (!QueryPerformanceFrequency(&g_qpc_freq) || g_qpc_freq.QuadPart <= 0) {
        // Extremely unlikely; fall back to 1 to avoid division by zero
        g_qpc_freq.QuadPart = 1;
    }
    g_ns_per_tick = 1e9 / (double)g_qpc_freq.QuadPart;
    return TRUE;
}

void ptime_init(void) {
    InitOnceExecuteOnce(&g_ptime_once, ptime_init_once_body, NULL, NULL);
}

ptime_point ptime_now(void) {
    ptime_init();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    ptime_point p; p.ticks = (uint64_t)c.QuadPart; return p;
}

uint64_t ptime_ns_between(ptime_point start, ptime_point end) {
    uint64_t dt = end.ticks - start.ticks;
    // Use long double to reduce precision loss and avoid overflow
    long double v = (long double)dt * (long double)g_ns_per_tick;
    if (v < 0) v = 0;
    if (v > 1.84e19L) v = 1.84e19L; // clamp to ~UINT64_MAX
    return (uint64_t)v;
}

double ptime_s_between(ptime_point start, ptime_point end) {
    return (double)ptime_ns_between(start, end) / 1e9;
}

#else // non-Apple and non-Windows: use clock_gettime if available

static void ptime_init_once_body(void) {}
void ptime_init(void) { (void)ptime_init_once_body; }

ptime_point ptime_now(void) {
    ptime_point p;
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    p.ticks = (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
#else
    // Very poor fallback; not monotonic
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    p.ticks = (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
#endif
    return p;
}

uint64_t ptime_ns_between(ptime_point start, ptime_point end) {
    return end.ticks - start.ticks;
}

double ptime_s_between(ptime_point start, ptime_point end) {
    return (double)(end.ticks - start.ticks) / 1e9;
}

#endif
