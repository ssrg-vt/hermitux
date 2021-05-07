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

/* Original code from the Application Kernel Matrix by Cray */
/* that was based on the ClustalW application */

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#define EOS '\0'
#define MAXLINE 512

#define NUMRES       32	  /* max size of comparison matrix */
#define MAXNAMES     30	  /* max chars read for seq. names */
#define FILENAMELEN 256   /* max file name length */

void del(int k, int *print_ptr, int *last_print, int *displ);
void add(int v, int *print_ptr, int *last_print, int *displ);
int calc_score(int iat, int jat, int v1, int v2, int seq1, int seq2);
int get_matrix(int *matptr, int *xref, int scale); 
void forward_pass(char *ia, char *ib, int n, int m, int *se1, int *se2, int *maxscore, int g, int gh);
void reverse_pass(char *ia, char *ib, int se1, int se2, int *sb1, int *sb2, int maxscore, int g, int gh);
int diff(int A, int B, int M, int N, int tb, int te, int *pr_ptr, int *last_print, int *displ, int seq1, int seq2, int g, int gh);
double tracepath(int tsb1, int tsb2, int *print_ptr, int *displ, int seq1, int seq2);

void init_matrix(void);
void pairalign_init(char *filename);
int pairalign();
int pairalign_seq();
void align_init();
void align();
void align_seq_init();
void align_seq();
void align_end();
int align_verify();
#endif

