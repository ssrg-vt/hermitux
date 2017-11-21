#define CPU_FREQ_MHZ	3500

inline static uint64_t rdtsc(void)
{
        uint32_t lo, hi;

        asm volatile ("rdtsc" : "=a"(lo), "=d"(hi) :: "memory");

        return ((uint64_t)hi << 32ULL | (uint64_t)lo);
}

inline double ticks_to_sec(uint64_t ticks) {
	return (double)ticks * (1.0/((double)CPU_FREQ_MHZ*1000000.0));
}
