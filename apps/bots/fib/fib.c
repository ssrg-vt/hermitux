/**********************************************************************************************/
/*  This program is part of the Barcelona OpenMP Tasks Suite                                  */
/*  Copyright (C) 2009 Barcelona Supercomputing Center - Centro Nacional de Supercomputacion  */
/*  Copyright (C) 2009 Universitat Politecnica de Catalunya                                   */
/*                                                                                            */
/*  This program is free software; you can redistribute it and/or modify                      */
/*  it under the terms of the GNU General Public License as published by                      */
/*  the Free Software Foundation; either version 2 of the License, or                         */
/*  (at your option) any later version.                                                       */
/*                                                                                            */
/*  This program is distributed in the hope that it will be useful,                           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of                            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                             */
/*  GNU General Public License for more details.                                              */
/*                                                                                            */
/*  You should have received a copy of the GNU General Public License                         */
/*  along with this program; if not, write to the Free Software                               */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA            */
/**********************************************************************************************/

#include "bots.h"
#include "fib.h"

#define FIB_RESULTS_PRE 41
long long fib_results[FIB_RESULTS_PRE] = {0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597,2584,4181,6765,10946,17711,28657,46368,75025,121393,196418,317811,514229,832040,1346269,2178309,3524578,5702887,9227465,14930352,24157817,39088169,63245986,102334155};

long long fib_seq (int n)
{
	int x, y;
	if (n < 2) return n;

	x = fib_seq(n - 1);
	y = fib_seq(n - 2);

	return x + y;
}

#if defined(IF_CUTOFF)

long long fib (int n,int d)
{
	long long x, y;
	if (n < 2) return n;

	#pragma omp task untied shared(x) firstprivate(n) if(d < bots_cutoff_value)
	x = fib(n - 1,d+1);

	#pragma omp task untied shared(y) firstprivate(n) if(d < bots_cutoff_value)
	y = fib(n - 2,d+1);

	#pragma omp taskwait
	return x + y;
}

#elif defined(FINAL_CUTOFF)

long long fib (int n,int d)
{
	long long x, y;
	if (n < 2) return n;

	#pragma omp task untied shared(x) firstprivate(n) final(d+1 >= bots_cutoff_value) mergeable
	x = fib(n - 1,d+1);

	#pragma omp task untied shared(y) firstprivate(n) final(d+1 >= bots_cutoff_value) mergeable
	y = fib(n - 2,d+1);

	#pragma omp taskwait
	return x + y;
}

#elif defined(MANUAL_CUTOFF)

long long fib (int n, int d)
{
	long long x, y;
	if (n < 2) return n;

	if ( d < bots_cutoff_value ) {
		#pragma omp task untied shared(x) firstprivate(n)
		x = fib(n - 1,d+1);

		#pragma omp task untied shared(y) firstprivate(n)
		y = fib(n - 2,d+1);

		#pragma omp taskwait
	} else {
		x = fib_seq(n-1);
		y = fib_seq(n-2);
	}

	return x + y;
}

#else

long long fib (int n)
{
	long long x, y;
	if (n < 2) return n;

	#pragma omp task untied shared(x) firstprivate(n)
	x = fib(n - 1);
	#pragma omp task untied shared(y) firstprivate(n)
	y = fib(n - 2);

	#pragma omp taskwait
	return x + y;
}

#endif

static long long par_res, seq_res;

void fib0 (int n)
{
	#pragma omp parallel
	#pragma omp single
#if defined(MANUAL_CUTOFF) || defined(IF_CUTOFF) || defined(FINAL_CUTOFF)
	par_res = fib(n,0);
#else
	par_res = fib(n);
#endif
	bots_message("Fibonacci result for %d is %lld\n",n,par_res);
}

void fib0_seq (int n)
{
	seq_res = fib_seq(n);
	bots_message("Fibonacci result for %d is %lld\n",n,seq_res);
}

long long fib_verify_value(int n)
{
	if (n < FIB_RESULTS_PRE) return fib_results[n];
	return ( fib_verify_value(n-1) + fib_verify_value(n-2));
}

int fib_verify (int n)
{
	int result;

	if (bots_sequential_flag)
	{
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_UNSUCCESSFUL;
	}
	else
	{
		seq_res = fib_verify_value(n);
		if (par_res == seq_res) result = BOTS_RESULT_SUCCESSFUL;
		else result = BOTS_RESULT_UNSUCCESSFUL;
	}

	return result;
}

