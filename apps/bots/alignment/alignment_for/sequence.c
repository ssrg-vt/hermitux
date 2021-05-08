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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sequence.h"
#include "sequence_extern.h"
#include "alignment.h"
#include "bots.h"

/***********************************************************************
 * :
 **********************************************************************/
size_t strlcpy(char *dst, const char *src, size_t siz)
{
   char *d = dst;
   const char *s = src;
   size_t n = siz;

   /* Copy as many bytes as will fit */
   if (n != 0) {
      while (--n != 0) {
         if ((*d++ = *s++) == '\0')
         break;
      }
   }

   /* Not enough room in dst, add NUL and traverse rest of src */
   if (n == 0) {
      if (siz != 0)
         *d = '\0';                /* NUL-terminate dst */
      while (*s++)
         ;
   }

   return(s - src - 1);        /* count does not include NUL */
}

/***********************************************************************
 * : 
 **********************************************************************/
void fill_chartab(char *chartab)
{
   int i;

   for (i = 0; i < 128; i++) chartab[i] = 0;

   for (i = 0; i < 25; i++) {
      char c = amino_acid_codes[i];
      chartab[(int)c] = chartab[tolower(c)] = c;
   }
}

/***********************************************************************
 * : 
 **********************************************************************/
void encode(char *seq, char *naseq, int l) 
{
   int i, j;
   char c, *t;

   for (i = 1; i <= l; i++)
      if (seq[i] == '-') {
         naseq[i] = (char) gap_pos2;
      } else {
         j = 0;
         c = seq[i];
         t = amino_acid_codes;
         naseq[i] = -1;
         while (t[j]) {if (t[j] == c) {naseq[i] = (char) j; break;} j++;}
      }

   naseq[l + 1] = -3;
}


/***********************************************************************
 * : 
 **********************************************************************/
void alloc_aln(int nseqs)
{
   int i;

   names        = (char   **) malloc((nseqs + 1) * sizeof(char *));
   seq_array    = (char   **) malloc((nseqs + 1) * sizeof(char *));
   seqlen_array = (int     *) malloc((nseqs + 1) * sizeof(int));

   for (i = 0; i < nseqs + 1; i++) {
      names[i]     = (char *  ) malloc((MAXNAMES + 1) * sizeof(char));
      seq_array[i] = NULL;
   }
}

/***********************************************************************
 * : 
 **********************************************************************/
char * get_seq(char *sname, int *len, char *chartab, FILE *fin)
{
   int  i, j;
   char c, *seq;
   static char line[MAXLINE+1];

   *len = 0;
   seq  = NULL;

   while (*line != '>' && fgets(line, MAXLINE+1, fin) != NULL );
   for (i = 1; i <= strlen(line); i++) if (line[i] != ' ') break;
   for (j = i; j <= strlen(line); j++) if (line[j] == ' ') break;

   strlcpy(sname, line + i, j - i + 1);;
   sname[j - i] = EOS;

   while (fgets(line, MAXLINE+1, fin) != NULL) {
      if (seq == NULL)
         seq = (char *) malloc((MAXLINE + 2) * sizeof(char));
      else
         seq = (char *) realloc(seq, ((*len) + MAXLINE + 2) * sizeof(char));
      for (i = 0; i <= MAXLINE; i++) {
         c = line[i];
         if (c == '\n' || c == EOS || c == '>') break;
         if (c == chartab[(int)c]) {*len += 1; seq[*len] = c;}
      }
      if (c == '>') break;
   }

   seq[*len + 1] = EOS;
   return seq;
}

int readseqs(char *filename)
{
   int  i, l1, no_seqs;
   FILE *fin;
   char *seq1, chartab[128];

   if ((fin = fopen(filename, "r")) == NULL) {
      bots_message("Could not open sequence file (%s)\n", filename);
      exit (-1);
   }

   if ( fscanf(fin,"Number of sequences is %d", &no_seqs) == EOF ) {
      bots_message("Sequence file is bogus (%s)\n", filename);
      exit(-1);
   };
   
   fill_chartab(chartab);
   bots_message("Sequence format is Pearson\n");

   alloc_aln(no_seqs);

   for (i = 1; i <= no_seqs; i++) {
      seq1 = get_seq(names[i], &l1, chartab, fin);

      seqlen_array[i] = l1;
      seq_array[i]    = (char *) malloc((l1 + 2) * sizeof (char));

      encode(seq1, seq_array[i], l1);

      free(seq1);
   }

   return no_seqs;
}

