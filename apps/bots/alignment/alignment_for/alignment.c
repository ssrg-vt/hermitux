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

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <libgen.h>
#include "sequence.h"
#include "param.h"
#include "alignment.h"
#include "bots.h"

int ktup, window, signif;
int prot_ktup, prot_window, prot_signif;

int gap_pos1, gap_pos2, mat_avscore;
int nseqs, max_aa;
#define MAX_ALN_LENGTH 5000

int *seqlen_array, def_aa_xref[NUMRES+1];

int *bench_output, *seq_output;

double gap_open,      gap_extend;
double prot_gap_open, prot_gap_extend;
double pw_go_penalty, pw_ge_penalty;
double prot_pw_go_penalty, prot_pw_ge_penalty;

char **args, **names, **seq_array;

int matrix[NUMRES][NUMRES];

double gap_open_scale;
double gap_extend_scale;

// dnaFlag default value is false
int dnaFlag = FALSE;

// clustalw default value is false
int clustalw = FALSE;

#define INT_SCALE 100

#define MIN(a,b) ((a)<(b)?(a):(b))
#define tbgap(k) ((k) <= 0 ? 0 : tb + gh * (k))
#define tegap(k) ((k) <= 0 ? 0 : te + gh * (k))

/***********************************************************************
 * : 
 **********************************************************************/
void del(int k, int *print_ptr, int *last_print, int *displ)
{
   if (*last_print<0) *last_print = displ[(*print_ptr)-1] -=  k;
   else               *last_print = displ[(*print_ptr)++]  = -k;
}

/***********************************************************************
 * : 
 **********************************************************************/
void add(int v, int *print_ptr, int *last_print, int *displ)
{
   if (*last_print < 0) {
      displ[(*print_ptr)-1] = v;
      displ[(*print_ptr)++] = *last_print;
   } else {
      *last_print = displ[(*print_ptr)++] = v;
   }
}

/***********************************************************************
 * : 
 **********************************************************************/
int calc_score(int iat, int jat, int v1, int v2, int seq1, int seq2)
{
   int i, j, ipos, jpos;

   ipos = v1 + iat;
   jpos = v2 + jat;
   i    = seq_array[seq1][ipos];
   j    = seq_array[seq2][jpos];
   
   return (matrix[i][j]);
}

/***********************************************************************
 * : 
 **********************************************************************/
int get_matrix(int *matptr, int *xref, int scale)
{
   int gg_score = 0;
   int gr_score = 0;
   int i, j, k, ti, tj, ix;
   int av1, av2, av3, min, max, maxres;

   for (i = 0; i <= max_aa; i++)
      for (j = 0; j <= max_aa; j++) matrix[i][j] = 0;

   ix     = 0;
   maxres = 0;

   for (i = 0; i <= max_aa; i++) {
      ti = xref[i];
      for (j = 0; j <= i; j++) {
         tj = xref[j];
         if ((ti != -1) && (tj != -1)) {
            k = matptr[ix];
            if (ti == tj) {
               matrix[ti][ti] = k * scale;
               maxres++;
            } else {
               matrix[ti][tj] = k * scale;
               matrix[tj][ti] = k * scale;
            }
            ix++;
         }
      }
   }

   maxres--;
   av1 = av2 = av3 = 0;

   for (i = 0; i <= max_aa; i++) {
      for (j = 0; j <= i;      j++) {
         av1 += matrix[i][j];
         if (i == j) av2 += matrix[i][j];
         else        av3 += matrix[i][j];
      }
   }

   av1 /= (maxres*maxres)/2;
   av2 /= maxres;
   av3 /= (int) (((double)(maxres*maxres-maxres))/2);
   mat_avscore = -av3;

   min = max = matrix[0][0];

   for (i = 0; i <= max_aa; i++)
      for (j = 1; j <= i;      j++) {
         if (matrix[i][j] < min) min = matrix[i][j];
            if (matrix[i][j] > max) max = matrix[i][j];
      }

   for (i = 0; i < gap_pos1; i++) {
      matrix[i][gap_pos1] = gr_score;
      matrix[gap_pos1][i] = gr_score;
      matrix[i][gap_pos2] = gr_score;
      matrix[gap_pos2][i] = gr_score;
   }

   matrix[gap_pos1][gap_pos1] = gg_score;
   matrix[gap_pos2][gap_pos2] = gg_score;
   matrix[gap_pos2][gap_pos1] = gg_score;
   matrix[gap_pos1][gap_pos2] = gg_score;

   maxres += 2;

   return(maxres);
}

/***********************************************************************
 * : 
 **********************************************************************/
void forward_pass(char *ia, char *ib, int n, int m, int *se1, int *se2, int *maxscore, int g, int gh)
{ 
   int i, j, f, p, t, hh;
   int HH[MAX_ALN_LENGTH];
        int DD[MAX_ALN_LENGTH];

   *maxscore  = 0;
   *se1 = *se2 = 0;

   for (i = 0; i <= m; i++) {HH[i] = 0; DD[i] = -g;}

   for (i = 1; i <= n; i++) {
      hh = p = 0;
      f  = -g;

      for (j = 1; j <= m; j++) {
         f -= gh;
         t  = hh - g - gh;

         if (f < t) f = t;

         DD[j] -= gh;
         t      = HH[j] - g - gh;

         if (DD[j] < t) DD[j] = t;

         hh = p + matrix[(int)ia[i]][(int)ib[j]];
         if (hh < f) hh = f;
         if (hh < DD[j]) hh = DD[j];
         if (hh < 0) hh = 0;

         p     = HH[j];
         HH[j] = hh;

         if (hh > *maxscore) {*maxscore = hh; *se1 = i; *se2 = j;}
      }
   }
}

/***********************************************************************
 * : 
 **********************************************************************/
void reverse_pass(char *ia, char *ib, int se1, int se2, int *sb1, int *sb2, int maxscore, int g, int gh)
{ 
   int i, j, f, p, t, hh, cost;
   int HH[MAX_ALN_LENGTH];
        int DD[MAX_ALN_LENGTH];

   cost = 0;
   *sb1  = *sb2 = 1;

   for (i = se2; i > 0; i--){ HH[i] = -1; DD[i] = -1;}

   for (i = se1; i > 0; i--) {
      hh = f = -1;
      if (i == se1) p = 0; else p = -1;

      for (j = se2; j > 0; j--) {

         f -= gh;
         t  = hh - g - gh;
         if (f < t) f = t;

         DD[j] -= gh;
         t      = HH[j] - g - gh;
         if (DD[j] < t) DD[j] = t;

         hh = p + matrix[(int)ia[i]][(int)ib[j]];
         if (hh < f) hh = f;
         if (hh < DD[j]) hh = DD[j];

         p     = HH[j];
         HH[j] = hh;

         if (hh > cost) {
            cost = hh; *sb1 = i; *sb2 = j;
            if (cost >= maxscore) break;
         }
      }

      if (cost >= maxscore) break;
   }
}

/***********************************************************************
 * : 
 **********************************************************************/
int diff (int A, int B, int M, int N, int tb, int te, int *print_ptr, int *last_print, int *displ, int seq1, int seq2, int g, int gh)
{
   int i, j, f, e, s, t, hh;
   int midi, midj, midh, type;
   int HH[MAX_ALN_LENGTH];
        int DD[MAX_ALN_LENGTH];
   int RR[MAX_ALN_LENGTH];
   int SS[MAX_ALN_LENGTH];

   if (N <= 0) {if (M > 0) del(M, print_ptr, last_print, displ); return( - (int) tbgap(M)); }

   if (M <= 1) {

      if (M <= 0) {add(N, print_ptr, last_print, displ); return( - (int)tbgap(N));}

      midh = -(tb+gh) - tegap(N);
      hh   = -(te+gh) - tbgap(N);

      if (hh > midh) midh = hh;
      midj = 0;

      for (j = 1; j <= N; j++) {
         hh = calc_score(1,j,A,B,seq1,seq2) - tegap(N-j) - tbgap(j-1);
         if (hh > midh) {midh = hh; midj = j;}
      }

      if (midj == 0) {
         del(1, print_ptr, last_print, displ);
         add(N, print_ptr, last_print, displ);
      } else {
         if (midj > 1) add(midj-1, print_ptr, last_print, displ);
         displ[(*print_ptr)++] = *last_print = 0;
         if (midj < N) add(N-midj, print_ptr, last_print, displ);
      }

      return midh;
   }

   midi  = M / 2;
   HH[0] = 0.0;
   t     = -tb;

   for (j = 1; j <= N; j++) {
      HH[j] = t = t - gh;
      DD[j] = t - g;
   }

   t = -tb;

   for (i = 1; i <= midi; i++) {
      s     = HH[0];
      HH[0] = hh = t = t - gh;
      f     = t - g;
      for (j = 1; j <= N; j++) {
         if ((hh = hh - g - gh)    > (f = f - gh))    f = hh;
         if ((hh = HH[j] - g - gh) > (e = DD[j]- gh)) e = hh;
         hh = s + calc_score(i,j,A,B,seq1,seq2);
         if (f > hh) hh = f;
         if (e > hh) hh = e;

         s = HH[j];
         HH[j] = hh;
         DD[j] = e;
      }
   }

   DD[0] = HH[0];
   RR[N] = 0;
   t     = -te;

   for (j = N-1; j >= 0; j--) {RR[j] = t = t - gh; SS[j] = t - g;}

   t = -te;

   for (i = M - 1; i >= midi; i--) {
      s     = RR[N];
      RR[N] = hh = t = t-gh;
      f     = t - g;
      for (j = N - 1; j >= 0; j--) {
         if ((hh = hh - g - gh)    > (f = f - gh))     f = hh;
         if ((hh = RR[j] - g - gh) > (e = SS[j] - gh)) e = hh;
         hh = s + calc_score(i+1,j+1,A,B,seq1,seq2);
         if (f > hh) hh = f;
         if (e > hh) hh = e;

         s     = RR[j];
         RR[j] = hh;
         SS[j] = e;
      }
   }

   SS[N] = RR[N];

   midh = HH[0] + RR[0];
   midj = 0;
   type = 1;

   for (j = 0; j <= N; j++) {
      hh = HH[j] + RR[j];
      if (hh >= midh)
      if (hh > midh || (HH[j] != DD[j] && RR[j] == SS[j]))
         {midh = hh; midj = j;}
   }

   for (j = N; j >= 0; j--) {
      hh = DD[j] + SS[j] + g;
      if (hh > midh) {midh = hh;midj = j;type = 2;}
   }


   if (type == 1) {
      diff(A, B, midi, midj, tb, g, print_ptr, last_print, displ, seq1, seq2, g, gh);
      diff(A+midi, B+midj, M-midi, N-midj, g, te, print_ptr, last_print, displ, seq1, seq2, g, gh);
   } else {
      diff(A, B, midi-1, midj, tb, 0.0, print_ptr, last_print, displ, seq1, seq2, g, gh);
      del(2, print_ptr, last_print, displ);
      diff(A+midi+1, B+midj, M-midi-1, N-midj, 0.0, te, print_ptr, last_print, displ, seq1, seq2, g, gh);
   }

   return midh;
}

/***********************************************************************
 * : 
 **********************************************************************/
double tracepath(int tsb1, int tsb2, int *print_ptr, int *displ, int seq1, int seq2)
{
   int  i, k;
   int i1    = tsb1;
   int i2    = tsb2;
   int pos   = 0;
   int count = 0;

   for (i = 1; i <= *print_ptr - 1; ++i) {
      if (displ[i]==0) {
         char c1 = seq_array[seq1][i1];
         char c2 = seq_array[seq2][i2];

         if ((c1!=gap_pos1) && (c1 != gap_pos2) && (c1 == c2)) count++;

         ++i1; ++i2; ++pos;

      } else if ((k = displ[i]) > 0) {
         i2  += k;
         pos += k;
      } else {
         i1  -= k;
         pos -= k;
      }
   }

   return (100.0 * (double) count);
}


int pairalign()
{
   int i, n, m, si, sj;
   int len1, len2, maxres;
   double gg, mm_score;
   int    *mat_xref, *matptr;

   matptr   = gon250mt;
   mat_xref = def_aa_xref;
   maxres = get_matrix(matptr, mat_xref, 10);
   if (maxres == 0) return(-1);

   bots_message("Start aligning ");

   #pragma omp parallel
   {
   #pragma omp for schedule(dynamic) private(i,n,si,sj,len1,m)
      for (si = 0; si < nseqs; si++) {
         n = seqlen_array[si+1];
         for (i = 1, len1 = 0; i <= n; i++) {
            char c = seq_array[si+1][i];
            if ((c != gap_pos1) && (c != gap_pos2)) len1++;
         }
         for (sj = si + 1; sj < nseqs; sj++) 
         {
            m = seqlen_array[sj+1];
            if ( n == 0 || m == 0 ) {
               bench_output[si*nseqs+sj] = (int) 1.0;
            } else {
               #pragma omp task untied \
               private(i,gg,len2,mm_score) firstprivate(m,n,si,sj,len1) \
               shared(nseqs, bench_output,seqlen_array,seq_array,gap_pos1,gap_pos2,pw_ge_penalty,pw_go_penalty,mat_avscore)
               {
                  int se1, se2, sb1, sb2, maxscore, seq1, seq2, g, gh;
                  int displ[2*MAX_ALN_LENGTH+1];
                  int print_ptr, last_print;

                  for (i = 1, len2 = 0; i <= m; i++) {
                     char c = seq_array[sj+1][i];
                     if ((c != gap_pos1) && (c != gap_pos2)) len2++;
                  }

                  if ( dnaFlag == TRUE ) {
                     g  = (int) ( 2 * INT_SCALE * pw_go_penalty * gap_open_scale ); // gapOpen
                     gh = (int) (INT_SCALE * pw_ge_penalty * gap_extend_scale); //gapExtend
                  } else {
                     gg = pw_go_penalty + log((double) MIN(n, m)); // temporary value
                     g  = (int) ((mat_avscore <= 0) ? (2 * INT_SCALE * gg) : (2 * mat_avscore * gg * gap_open_scale) ); // gapOpen
                     gh = (int) (INT_SCALE * pw_ge_penalty); //gapExtend
                  }

                  seq1 = si + 1;
                  seq2 = sj + 1;

                  forward_pass(&seq_array[seq1][0], &seq_array[seq2][0], n, m, &se1, &se2, &maxscore, g, gh);
                  reverse_pass(&seq_array[seq1][0], &seq_array[seq2][0], se1, se2, &sb1, &sb2, maxscore, g, gh);

                  print_ptr  = 1;
                  last_print = 0;

                  diff(sb1-1, sb2-1, se1-sb1+1, se2-sb2+1, 0, 0, &print_ptr, &last_print, displ, seq1, seq2, g, gh);
                  mm_score = tracepath(sb1, sb2, &print_ptr, displ, seq1, seq2);

                  if (len1 == 0 || len2 == 0) mm_score  = 0.0;
                  else                        mm_score /= (double) MIN(len1,len2);

                  bench_output[si*nseqs+sj] = (int) mm_score;
               } // end task
            } // end if (n == 0 || m == 0)
         } // for (j)
      } // end parallel for (i)
   } // end parallel
   bots_message(" completed!\n");
   return 0;
}

int pairalign_seq()
{
   int i, n, m, si, sj;
   int len1, len2, maxres;
   double gg, mm_score;
   int    *mat_xref, *matptr;

   matptr   = gon250mt;
   mat_xref = def_aa_xref;
   maxres = get_matrix(matptr, mat_xref, 10);
   if (maxres == 0) return(-1);

   for (si = 0; si < nseqs; si++) {
      n = seqlen_array[si+1];
      for (i = 1, len1 = 0; i <= n; i++) {
         char c = seq_array[si+1][i];
         if ((c != gap_pos1) && (c != gap_pos2)) len1++;
      }

      for (sj = si + 1; sj < nseqs; sj++) {
         m = seqlen_array[sj+1];
         if ( n == 0 || m == 0) {
            seq_output[si*nseqs+sj] = (int) 1.0;
         } else {
            int se1, se2, sb1, sb2, maxscore, seq1, seq2, g, gh;
            int displ[2*MAX_ALN_LENGTH+1];
            int print_ptr, last_print;

            for (i = 1, len2 = 0; i <= m; i++) {
               char c = seq_array[sj+1][i];
               if ((c != gap_pos1) && (c != gap_pos2)) len2++;
            }

            if ( dnaFlag == TRUE ) {
               g  = (int) ( 2 * INT_SCALE * pw_go_penalty * gap_open_scale ); // gapOpen
               gh = (int) (INT_SCALE * pw_ge_penalty * gap_extend_scale); //gapExtend
            } else {
               gg = pw_go_penalty + log((double) MIN(n, m)); // temporary value
               g  = (int) ((mat_avscore <= 0) ? (2 * INT_SCALE * gg) : (2 * mat_avscore * gg * gap_open_scale) ); // gapOpen
               gh = (int) (INT_SCALE * pw_ge_penalty); //gapExtend
            }

            seq1 = si + 1;
            seq2 = sj + 1;

            forward_pass(&seq_array[seq1][0], &seq_array[seq2][0], n, m, &se1, &se2, &maxscore, g, gh);
            reverse_pass(&seq_array[seq1][0], &seq_array[seq2][0], se1, se2, &sb1, &sb2, maxscore, g, gh);

            print_ptr  = 1;
            last_print = 0;

            diff(sb1-1, sb2-1, se1-sb1+1, se2-sb2+1, 0, 0, &print_ptr, &last_print, displ, seq1, seq2, g, gh);
            mm_score = tracepath(sb1, sb2, &print_ptr, displ, seq1, seq2);

            if (len1 == 0 || len2 == 0) mm_score  = 0.0;
            else                        mm_score /= (double) MIN(len1,len2);

            seq_output[si*nseqs+sj] = (int) mm_score;
         }
      }
   }
   return 0;
}


/***********************************************************************
 * : 
 **********************************************************************/
void init_matrix(void)
{
   int  i, j;
   char c1, c2;

   gap_pos1 = NUMRES - 2;
   gap_pos2 = NUMRES - 1;
   max_aa   = strlen(amino_acid_codes) - 2;

   for (i = 0; i < NUMRES; i++) def_aa_xref[i]  = -1;

   for (i = 0; (c1 = amino_acid_order[i]); i++)
      for (j = 0; (c2 = amino_acid_codes[j]); j++)
         if (c1 == c2) {def_aa_xref[i] = j; break;}
}

void pairalign_init (char *filename)
{
   int i;

   if (!filename || !filename[0]) {
      bots_error(0, "Please specify an input file with the -f option\n");
   }

   init_matrix();

   nseqs = readseqs(filename);

   bots_message("Multiple Pairwise Alignment (%d sequences)\n", nseqs);

   for (i = 1; i <= nseqs; i++)
      bots_debug("Sequence %d: %s %6.d aa\n", i, names[i], seqlen_array[i]);

   if ( clustalw == TRUE ) {
      gap_open_scale = 0.6667;
      gap_extend_scale = 0.751;
   } else {
      gap_open_scale = 1.0;
      gap_extend_scale = 1.0;
   }

   if ( dnaFlag == TRUE ) {
      // Using DNA parameters
      ktup          =  2;
      window        =  4;
      signif        =  4;
      gap_open      = 15.00;
      gap_extend    =  6.66;
      pw_go_penalty = 15.00;
      pw_ge_penalty =  6.66;
   } else {
      // Using protein parameters
      ktup          =  1;
      window        =  5;
      signif        =  5;
      gap_open      = 10.0;
      gap_extend    =  0.2;
      pw_go_penalty = 10.0;
      pw_ge_penalty =  0.1;
   }
}

void align_init ()
{
   int i,j;
   bench_output = (int *) malloc(sizeof(int)*nseqs*nseqs);

   for(i = 0; i<nseqs; i++)
      for(j = 0; j<nseqs; j++)
         bench_output[i*nseqs+j] = 0;
}

void align()
{
   pairalign();
}

void align_seq_init ()
{
   int i,j;
   seq_output = (int *) malloc(sizeof(int)*nseqs*nseqs);
   bench_output = (int *) malloc(sizeof(int)*nseqs*nseqs);

   for(i = 0; i<nseqs; i++)
      for(j = 0; j<nseqs; j++)
         seq_output[i*nseqs+j] = 0;
}

void align_seq()
{
   pairalign_seq();
}


void align_end ()
{
   int i,j;
   for(i = 0; i<nseqs; i++)
      for(j = 0; j<nseqs; j++)
         if (bench_output[i*nseqs+j] != 0)
            bots_debug("Benchmark sequences (%d:%d) Aligned. Score: %d\n",
               i+1 , j+1 , (int) bench_output[i*nseqs+j]);
}

int align_verify ()
{
   int i,j;
   int result = BOTS_RESULT_SUCCESSFUL;
   for(i = 0; i<nseqs; i++)
      for(j = 0; j<nseqs; j++)
         if (bench_output[i*nseqs+j] != seq_output[i*nseqs+j])
         {
            bots_message("Error: Optimized prot. (%3d:%3d)=%5d Sequential prot. (%3d:%3d)=%5d\n",
               i+1, j+1, (int) bench_output[i*nseqs+j],
               i+1, j+1, (int) seq_output[i*nseqs+j]);
            result = BOTS_RESULT_UNSUCCESSFUL;
         }
   return result;
}
      

