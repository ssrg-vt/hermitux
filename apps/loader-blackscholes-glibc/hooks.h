#ifndef HOOKS_H
#define HOOKS_H

#include <stdint.h>

enum __parsec_benchmark {
	  __parsec_blackscholes,
};

uint64_t start, stop, res;

inline double ticks_to_sec(uint64_t ticks);
inline uint64_t rdtsc(void);

inline void __parsec_bench_begin(enum __parsec_benchmark __bench) {};
inline void __parsec_bench_end() {};

inline void __parsec_roi_begin() {
	start = rdtsc();
}

inline void __parsec_roi_end() {
	stop = rdtsc();
	printf("Time spent in ROI: %f\n", ticks_to_sec(stop - start));
}

#define CPU_FREQ_MHZ	3500

inline uint64_t rdtsc(void)
{
        uint32_t lo, hi;

        asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");

        return ((uint64_t)hi << 32ULL | (uint64_t)lo);
}

inline double ticks_to_sec(uint64_t ticks) {
	return (double)ticks * (1.0/((double)CPU_FREQ_MHZ*1000000.0));
}

#endif /* HOOKS_H */
