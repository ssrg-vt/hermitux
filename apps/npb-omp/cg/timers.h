#ifndef __TIMERS_H__
#define __TIMERS_H__

void timer_clear( int n );
void timer_start( int n );
void timer_stop( int n );
double timer_read( int n );
unsigned timer_count( int n );

#endif

