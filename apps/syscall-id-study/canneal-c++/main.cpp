// main.cpp
//
// Created by Daniel Schwartz-Narbonne on 13/04/07.
// Modified by Christian Bienia
//
// Copyright 2007-2008 Princeton University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.


#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#ifdef ENABLE_THREADS
#include <pthread.h>
#endif

#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

#include "annealer_types.h"
#include "annealer_thread.h"
#include "netlist.h"
#include "rng.h"

using namespace std;

void* entry_pt(void*);



int main (int argc, char * const argv[]) {
#ifdef PARSEC_VERSION
#define __PARSEC_STRING(x) #x
#define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
        cout << "PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION) << endl << flush;
#else
        cout << "PARSEC Benchmark Suite" << endl << flush;
#endif //PARSEC_VERSION
#ifdef ENABLE_PARSEC_HOOKS
	__parsec_bench_begin(__parsec_canneal);
#endif

	srandom(3);

	if(argc != 5 && argc != 6) {
		cout << "Usage: " << argv[0] << " NTHREADS NSWAPS TEMP NETLIST [NSTEPS]" << endl;
		exit(1);
	}	
	
	//argument 1 is numthreads
	int num_threads = atoi(argv[1]);
	cout << "Threadcount: " << num_threads << endl;
#ifndef ENABLE_THREADS
	if (num_threads != 1){
		cout << "NTHREADS must be 1 (serial version)" <<endl;
		exit(1);
	}
#endif
		
	//argument 2 is the num moves / temp
	int swaps_per_temp = atoi(argv[2]);
	cout << swaps_per_temp << " swaps per temperature step" << endl;

	//argument 3 is the start temp
	int start_temp =  atoi(argv[3]);
	cout << "start temperature: " << start_temp << endl;
	
	//argument 4 is the netlist filename
	string filename(argv[4]);
	cout << "netlist filename: " << filename << endl;
	
	//argument 5 (optional) is the number of temperature steps before termination
	int number_temp_steps = -1;
        if(argc == 6) {
		number_temp_steps = atoi(argv[5]);
		cout << "number of temperature steps: " << number_temp_steps << endl;
        }

	//now that we've read in the commandline, run the program
	netlist my_netlist(filename);
	
	annealer_thread a_thread(&my_netlist,num_threads,swaps_per_temp,start_temp,number_temp_steps);
	
#ifdef ENABLE_PARSEC_HOOKS
	__parsec_roi_begin();
#endif
#ifdef ENABLE_THREADS
	std::vector<pthread_t> threads(num_threads);
	void* thread_in = static_cast<void*>(&a_thread);
	for(int i=0; i<num_threads; i++){
		pthread_create(&threads[i], NULL, entry_pt,thread_in);
	}
	for (int i=0; i<num_threads; i++){
		pthread_join(threads[i], NULL);
	}
#else
	a_thread.Run();
#endif
#ifdef ENABLE_PARSEC_HOOKS
	__parsec_roi_end();
#endif
	
	cout << "Final routing is: " << my_netlist.total_routing_cost() << endl;

#ifdef ENABLE_PARSEC_HOOKS
	__parsec_bench_end();
#endif

	return 0;
}

void* entry_pt(void* data)
{
	annealer_thread* ptr = static_cast<annealer_thread*>(data);
	ptr->Run();
	return NULL;
}
