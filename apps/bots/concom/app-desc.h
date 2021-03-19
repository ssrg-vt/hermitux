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

#define BOTS_APP_NAME "Connected Components"
#define BOTS_APP_PARAMETERS_DESC "N=%d,L=%d,M=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size,bots_arg_size_2,bots_arg_size_1

#define BOTS_APP_CHECK_USES_SEQ_RESULT
//#define BOTS_APP_SELF_TIMING

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 100000
#define BOTS_APP_DESC_ARG_SIZE "Number of nodes"

#define BOTS_APP_USES_ARG_SIZE_1
#define BOTS_APP_DEF_ARG_SIZE_1 20
#define BOTS_APP_DESC_ARG_SIZE_1 "Maximum number of neighbors per node"

#define BOTS_APP_USES_ARG_SIZE_2
#define BOTS_APP_DEF_ARG_SIZE_2 100000
#define BOTS_APP_DESC_ARG_SIZE_2 "Number of links in the entire graph"

typedef struct node {
    int   n;
    int  *neighbor;
}node;


int linkable(int N1, int N2);
void initialize();
void write_outputs(int n, int cc);
void CC_par (int i, int cc);
void CC_seq (int i, int cc);
void cc_init();
void cc_par(int *cc);
void cc_seq(int *cc);
int cc_check(int ccs, int ccb);

#define BOTS_APP_INIT int ccs, ccp;\
   initialize();

#define KERNEL_INIT cc_init();
#define KERNEL_CALL cc_par(&ccp);
#define KERNEL_FINI

#define KERNEL_SEQ_INIT cc_init();
#define KERNEL_SEQ_CALL cc_seq(&ccs);
#define KERNEL_SEQ_FINI

#define KERNEL_CHECK cc_check(ccs,ccp);

