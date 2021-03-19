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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "app-desc.h"
#include "bots.h"

// bots_arg_size == Number of nodes
// bots_arg_size_1 == Maximum number of neighbors per node
// bots_arg_size_2 == Number of links in the entire graph

node *nodes;
int *visited, *components;

// Checks to see if two nodes can be linked
int linkable(int N1, int N2) {
    int i;

    if (N1 == N2) return (0);
    if (nodes[N1].n >= bots_arg_size_1) return (0);
    if (nodes[N2].n >= bots_arg_size_1) return (0);
    
    for (i = 0; i < nodes[N1].n; i++)
        if (nodes[N1].neighbor[i] == N2) return (0);

    return (1);
}
// Allocates and creates a graph with random links between nodes
// also allocates visited and components vectors
void initialize() {
   int i, l1, l2, N1, N2;
   double RN;
   
   nodes = (node *) malloc(bots_arg_size * sizeof(node)); 
   visited = (int *) malloc(bots_arg_size * sizeof(int)); 
   components = (int *) malloc(bots_arg_size * sizeof(int)); 
   /* initialize nodes */
   for (i = 0; i < bots_arg_size; i++) {
      nodes[i].n = 0;
      nodes[i].neighbor = (int *) malloc(bots_arg_size_1 * sizeof(int));
   }
   /* for each link, generate end nodes and link */
   for (i = 0; i < bots_arg_size_2; i++)
   {
      RN = rand() / (double) RAND_MAX;
      N1 = (int) ((bots_arg_size-1) * RN);
      RN = rand() / (double) RAND_MAX;
      N2 = (int) ((bots_arg_size-1) * RN);
      if (linkable(N1, N2)) {
         l1 = nodes[N1].n;
         l2 = nodes[N2].n;
         nodes[N1].neighbor[l1] = N2;
         nodes[N2].neighbor[l2] = N1;
         nodes[N1].n += 1;
         nodes[N2].n += 1;
      }   
   }
}
// Writes the number of CCs
void write_outputs(int n, int cc) {
  int i;

  printf("Graph %d, Number of components %d\n", n, cc);

  if (bots_verbose_mode)
     for (i = 0; i < cc; i++)
         printf("Component %d       Size: %d\n", i, components[i]);
}
// Marks a node and all its neighbors as part of the CC
void CC_par (int i, int cc)
{
   int j, n;
   /* if node has not been visited */
   if (visited[i] == 0) {
      /* add node to current component */
      if (bots_verbose_mode) printf("Adding node %d to component %d\n", i, cc);
      #pragma omp critical
      {
         visited[i] = 1;
         components[cc]++;
      }
      /* add each neighbor's subtree to the current component */
      for (j = 0; j < nodes[i].n; j++)
      {
         n = nodes[i].neighbor[j];
         #pragma omp task untied firstprivate (i,cc)
         {CC_par(n, cc);}
      }
      #pragma omp taskwait
   }  
}
void CC_seq (int i, int cc)
{
   int j, n;
   /* if node has not been visited */
   if (visited[i] == 0) {
      /* add node to current component */
      if (bots_verbose_mode) printf("Adding node %d to component %d\n", i, cc);
      {
         visited[i] = 1;
         components[cc]++;
      }
      /* add each neighbor's subtree to the current component */
      for (j = 0; j < nodes[i].n; j++)
      {
         n = nodes[i].neighbor[j];
         CC_seq(n, cc);
      }
   }  
}
void cc_init()
{
    int i;
   /* initialize global data structures */      
   for (i = 0; i < bots_arg_size; i++)
   {
      visited[i] = 0;
      components[i] = 0;
   }
}
void cc_par(int *cc)
{
   int i;
   *cc = 0;
   /* for all nodes ... unvisited nodes start a new component */
   #pragma omp parallel
   #pragma omp single
   #pragma omp task untied
   for (i = 0; i < bots_arg_size; i++)
   {
      if (visited[i] == 0)
      {
         #pragma omp task untied firstprivate (i,cc)
         {CC_par(i, *cc);}
         #pragma omp taskwait
         (*cc)++;
      }
   }
}
void cc_seq(int *cc)
{
   int i;
   (*cc) = 0;
   /* for all nodes ... unvisited nodes start a new component */
   for (i = 0; i < bots_arg_size; i++)
   {
      if (visited[i] == 0)
      {
         CC_par(i, *cc);
         (*cc)++;
      }
   }
}
int cc_check(int ccs, int ccp)
{
  if (bots_verbose_mode) fprintf(stdout, "Sequential = %d CC, Parallel =%d CC\n", ccs, ccp);
  if (ccs == ccp) return BOTS_RESULT_SUCCESSFUL;
  else return BOTS_RESULT_UNSUCCESSFUL;
}
