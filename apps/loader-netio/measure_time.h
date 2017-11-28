#include <time.h>
#include <stdint.h>

#define FREQ 3300000000
#define NANO_SECONDS_IN_SEC  1000000000
#define ITER	10000

//rdtscp wrapper
static inline uint64_t RDTSCP()
{
  unsigned int hi,lo;
  __asm__ volatile("rdtscp" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi<<32) | lo;
}

/* returns a static buffer of struct timespec with the time difference of ts1 and ts2
   ts1 is assumed to be greater than ts2 */
static inline struct timespec TimeSpecDiff(struct timespec *ts1, struct timespec *ts2)
{
  struct timespec ts;
  ts.tv_sec = ts1->tv_sec - ts2->tv_sec;
  ts.tv_nsec = ts1->tv_nsec - ts2->tv_nsec;
  if (ts.tv_nsec < 0) {
    ts.tv_sec--;
    ts.tv_nsec += NANO_SECONDS_IN_SEC;
  }
  return ts;
}

