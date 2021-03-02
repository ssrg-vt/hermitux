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
#include "fft.h"

#define BOTS_APP_NAME "FFT"
#define BOTS_APP_PARAMETERS_DESC "Size=%d"
#define BOTS_APP_PARAMETERS_LIST ,bots_arg_size

#define BOTS_APP_USES_ARG_SIZE
#define BOTS_APP_DEF_ARG_SIZE 32*1024*1024
#define BOTS_APP_DESC_ARG_SIZE "Matrix Size"

#define BOTS_APP_INIT int i;\
     COMPLEX *in, *out1=NULL, *out2=NULL;\
     in = (COMPLEX *)malloc(bots_arg_size * sizeof(COMPLEX));\

#define KERNEL_INIT\
     out1 = (COMPLEX *)malloc(bots_arg_size * sizeof(COMPLEX));\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_CALL fft(bots_arg_size, in, out1);
#define KERNEL_FINI 

#define KERNEL_SEQ_INIT\
     out2 = (COMPLEX *)malloc(bots_arg_size * sizeof(COMPLEX));\
     for (i = 0; i < bots_arg_size; ++i) {\
          c_re(in[i]) = 1.0;\
          c_im(in[i]) = 1.0;\
     }
#define KERNEL_SEQ_CALL fft_seq(bots_arg_size, in, out2);
#define KERNEL_SEQ_FINI

#define BOTS_APP_CHECK_USES_SEQ_RESULT
#define KERNEL_CHECK test_correctness(bots_arg_size, out1, out2)

