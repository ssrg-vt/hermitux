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

#define BOTS_APP_NAME "Protein alignment (Single version)"
#define BOTS_APP_PARAMETERS_DESC "%s"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_file

#define BOTS_APP_USES_ARG_FILE
#define BOTS_APP_DESC_ARG_FILE "Protein sequences file (mandatory)"

void pairalign_init(char *);
void align_init ();
void align ();
void align_end ();
void align_seq_init ();
void align_seq ();
int align_verify ();


#define BOTS_APP_INIT pairalign_init(bots_arg_file)

#define KERNEL_INIT align_init()
#define KERNEL_CALL align()
#define KERNEL_FINI align_end()

#define KERNEL_SEQ_INIT align_seq_init()
#define KERNEL_SEQ_CALL align_seq()
//#define KERNEL_SEQ_FINI

#define KERNEL_CHECK align_verify()
#define BOTS_APP_CHECK_USES_SEQ_RESULT

