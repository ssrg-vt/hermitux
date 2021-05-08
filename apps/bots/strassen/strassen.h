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
#ifndef _STRASSEN_H
#define _STRASSEN_H
/* ******************************************************************* */
/* STRASSEN APPLICATION CUT OFF's                                      */
/* ******************************************************************* */
/* Strassen uses three different functions to compute Matrix Multiply. */
/* Each of them is related to an application cut off value:            */
/*  - Initial algorithm: OptimizedStrassenMultiply()                   */
/*  - bots_app_cutoff_value: MultiplyByDivideAndConquer()              */
/*  - SizeAtWhichNaiveAlgorithmIsMoreEfficient: FastAdditiveNaiveMatrixMultiply() */
/* ******************************************************************* */

/*FIXME: at the moment we use a constant value, change to parameter ???*/
/* Below this cut off  strassen uses FastAdditiveNaiveMatrixMultiply algorithm */
#define SizeAtWhichNaiveAlgorithmIsMoreEfficient 16

/***********************************************************************
 * maximum tolerable relative error (for the checking routine)
 **********************************************************************/
#define EPSILON (1.0E-6)
/***********************************************************************
 * Matrices are stored in row-major order; A is a pointer to
 * the first element of the matrix, and an is the number of elements
 * between two rows. This macro produces the element A[i,j]
 * given A, an, i and j
 **********************************************************************/
#define ELEM(A, an, i, j) (A[(i)*(an)+(j)])

void matrixmul(int n, REAL *A, int an, REAL *B, int bn, REAL *C, int cn);
void FastNaiveMatrixMultiply(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB);
void FastAdditiveNaiveMatrixMultiply(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB);
void MultiplyByDivideAndConquer(REAL *C, REAL *A, REAL *B,
				     unsigned MatrixSize,
				     unsigned RowWidthC,
				     unsigned RowWidthA,
				     unsigned RowWidthB,
				     int AdditiveMode
				    );
void OptimizedStrassenMultiply_par(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB, int Depth);
void OptimizedStrassenMultiply_seq(REAL *C, REAL *A, REAL *B, unsigned MatrixSize,
     unsigned RowWidthC, unsigned RowWidthA, unsigned RowWidthB, int Depth);
REAL *alloc_matrix(int n);
#endif

