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

#define BOTS_APP_NAME "N Queens"
#define BOTS_APP_PARAMETERS_DESC "N=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 14
#define BOTS_APP_DESC_ARG_SIZE "Board size"

int ok(int n, char *a);

#ifndef FORCE_TIED_TASKS
void nqueens(int n, int j, char *a, int *solutions, int depth);
#else
void nqueens(int n, int j, char *a, int depth);
#endif

#ifndef FORCE_TIED_TASKS
void nqueens_ser (int n, int j, char *a, int *solutions);
#else
void nqueens_ser (int n, int j, char *a);
#endif

int verify_queens(int);
void find_queens (int);

#define KERNEL_CALL find_queens(bots_arg_size)
#define KERNEL_CHECK verify_queens(bots_arg_size)

#define BOTS_CUTOFF_DEF_VALUE 3
