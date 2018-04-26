// rng.h
//
// Created by Daniel Schwartz-Narbonne on 25/04/07.
//
// Copyright 2007 Princeton University
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


#ifndef RNG_H
#define RNG_H

#include <vector>

#ifdef ENABLE_THREADS
#include <pthread.h>
#endif

#include "MersenneTwister.h"

class Rng
{
public:
	Rng() {
#ifdef ENABLE_THREADS
		pthread_mutex_lock(&seed_lock);
		_rng = new MTRand(seed++);
		pthread_mutex_unlock(&seed_lock);
#else
		_rng = new MTRand(seed++);
#endif //ENABLE_THREADS
	}
	~Rng() {
		delete _rng;
	}
	long rand();
	long rand(int max);
	double drand();
protected:
	//use same random seed for each run
	static unsigned int seed;
	static pthread_mutex_t seed_lock;
	MTRand *_rng;
};

#endif

