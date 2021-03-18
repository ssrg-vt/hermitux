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

#include "omp-tasks-app.h"

#define BOTS_APP_NAME "Sort"
#define BOTS_APP_PARAMETERS_DESC "N=%d:Q=%d:I=%d:M=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_app_cutoff_value_1,bots_app_cutoff_value_2,bots_app_cutoff_value

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE (32*1024*1024)
#define BOTS_APP_DESC_ARG_SIZE "Array size"

#define BOTS_APP_USES_ARG_CUTOFF
#define BOTS_APP_DEF_ARG_CUTOFF (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF "Sequential Merge cutoff value"

#define BOTS_APP_USES_ARG_CUTOFF_1
#define BOTS_APP_DEF_ARG_CUTOFF_1 (2*1024)
#define BOTS_APP_DESC_ARG_CUTOFF_1 "Sequential Quicksort cutoff value"

#define BOTS_APP_USES_ARG_CUTOFF_2
#define BOTS_APP_DEF_ARG_CUTOFF_2 (20)
#define BOTS_APP_DESC_ARG_CUTOFF_2 "Sequential Insertion cutoff value"

typedef long ELM;

void seqquick(ELM *low, ELM *high); 
void seqmerge(ELM *low1, ELM *high1, ELM *low2, ELM *high2, ELM *lowdest);
ELM *binsplit(ELM val, ELM *low, ELM *high); 
void cilkmerge(ELM *low1, ELM *high1, ELM *low2, ELM *high2, ELM *lowdest);
void cilkmerge_par(ELM *low1, ELM *high1, ELM *low2, ELM *high2, ELM *lowdest);
void cilksort(ELM *low, ELM *tmp, long size);
void cilksort_par(ELM *low, ELM *tmp, long size);
void scramble_array( ELM *array ); 
void fill_array( ELM *array ); 
void sort ( void ); 

void sort_par (void);
void sort_init (void);
int sort_verify (void);

#define BOTS_APP_INIT sort_init()

#define KERNEL_INIT
#define KERNEL_CALL sort_par()
#define KERNEL_CHECK sort_verify()


