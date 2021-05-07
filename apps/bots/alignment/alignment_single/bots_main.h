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

#define BOTS_PARAM_TYPE_NONE 0
#define BOTS_PARAM_TYPE_INT 1
#define BOTS_PARAM_TYPE_BOOL 2
#define BOTS_PARAM_TYPE_STR 3

#ifdef _OPENMP
# include <omp.h>
#else
# define omp_get_max_threads()  1
# define omp_get_thread_num()   0
# define omp_set_num_threads(x)
#endif

void bots_print_usage(void);
void bots_print_usage_option(char opt, int type, char* description, char *val, int subc, char **subv);

/***********************************************************************
 * BENCHMARK HEADERS 
 *********************************************************************/
void bots_initialize();
void bots_finalize();
void bots_sequential_ini();
long bots_sequential();
void bots_sequential_fini();
int bots_check_result();
void bots_print_usage_specific();
void bots_get_params_specific(int argc, char **argv);
void bots_set_info();

void bots_get_params_common(int argc, char **argv);
void bots_get_params(int argc, char **argv);
