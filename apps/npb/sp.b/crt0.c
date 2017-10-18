/*
 * Copyright (c) 2016, Stefan Lankes, RWTH Aachen University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the University nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

int libc_sd = -1;

extern int main(int argc, char** argv);
extern void __libc_init_array(void);
extern void __libc_fini_array (void);
extern int _init_signal(void);
extern char** environ;

/* Pierre: musl init functions */
extern void __init_libc(char **envp, char *pn);
extern void __libc_start_init(void);

/* Pierre: */
extern void gettimeofday_init(void);

#define PHYS	0x800000ULL

int libc_start(int argc, char** argv, char** env)
{
   int ret;
   char **envp = argv+argc+1;

   /* call init function */
   //__libc_init_array();

   /* register a function to be called at normal process termination */
   //atexit(__libc_fini_array);

   /* optind is the index of the next element to be processed in argv */
   optind = 0;

   if (env)
      environ = env;

   /* initialize simple signal handling */
   //_init_signal();


   /* Pierre: call musl init */
	__init_libc(envp, argv[0]);
	__libc_start_init();


	/* pierre: gettimeofday init */
	gettimeofday_init();

   ret = main(argc, argv);

   /* call exit from the C library so atexit gets called, and the
      C++ destructors get run. This calls our exit routine below    
      when it's done. */
   exit(ret);

   /* we should never reach this point */
   return 0;
}

int get_cpufreq(void)
{
   return *((int*) (PHYS + 0x18));
}

int isle_id(void)
{
   return *((int*) (PHYS + 0x34));
}

int get_num_cpus(void)
{
   return *((int*) (PHYS + 0x20));
}
