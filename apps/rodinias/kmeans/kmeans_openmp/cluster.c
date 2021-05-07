/*****************************************************************************/
/*IMPORTANT:  READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.         */
/*By downloading, copying, installing or using the software you agree        */
/*to this license.  If you do not agree to this license, do not download,    */
/*install, copy or use the software.                                         */
/*                                                                           */
/*                                                                           */
/*Copyright (c) 2005 Northwestern University                                 */
/*All rights reserved.                                                       */

/*Redistribution of the software in source and binary forms,                 */
/*with or without modification, is permitted provided that the               */
/*following conditions are met:                                              */
/*                                                                           */
/*1       Redistributions of source code must retain the above copyright     */
/*        notice, this list of conditions and the following disclaimer.      */
/*                                                                           */
/*2       Redistributions in binary form must reproduce the above copyright   */
/*        notice, this list of conditions and the following disclaimer in the */
/*        documentation and/or other materials provided with the distribution.*/ 
/*                                                                            */
/*3       Neither the name of Northwestern University nor the names of its    */
/*        contributors may be used to endorse or promote products derived     */
/*        from this software without specific prior written permission.       */
/*                                                                            */
/*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS    */
/*IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED      */
/*TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT AND         */
/*FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL          */
/*NORTHWESTERN UNIVERSITY OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,       */
/*INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES          */
/*(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR          */
/*SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)          */
/*HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,         */
/*STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    */
/*ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE             */
/*POSSIBILITY OF SUCH DAMAGE.                                                 */
/******************************************************************************/
/*************************************************************************/
/**   File:         cluster.c                                           **/
/**   Description:  Takes as input a file, containing 1 data point per  **/
/**                 per line, and performs a fuzzy c-means clustering   **/
/**                 on the data. Fuzzy clustering is performed using    **/
/**                 min to max clusters and the clustering that gets    **/
/**                 the best score according to a compactness and       **/
/**                 separation criterion are returned.                  **/
/**   Author:  Brendan McCane                                           **/
/**            James Cook University of North Queensland.               **/
/**            Australia. email: mccane@cs.jcu.edu.au                   **/
/**                                                                     **/
/**   Edited by: Jay Pisharath, Wei-keng Liao                           **/
/**              Northwestern University.                               **/
/**                                                                     **/
/**   ================================================================  **/
/**																		**/
/**   Edited by: Sang-Ha  Lee											**/
/**				 University of Virginia									**/
/**																		**/
/**   Description:	No longer supports fuzzy c-means clustering;	 	**/
/**					only regular k-means clustering.					**/
/**					Simplified for main functionality: regular k-means	**/
/**					clustering.											**/
/**                                                                     **/
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <omp.h>

#include "kmeans.h"


/*---< cluster() >-----------------------------------------------------------*/
int cluster(int      numObjects,      /* number of input objects */
            int      numAttributes,   /* size of attribute of each object */
            float  **attributes,      /* [numObjects][numAttributes] */            
            int      nclusters,
            float    threshold,       /* in:   */
            float ***cluster_centres /* out: [best_nclusters][numAttributes] */
    
            )
{
    int    *membership;
    float **tmp_cluster_centres;

    membership = (int*) malloc(numObjects * sizeof(int));
   
    srand(7);
	/* perform regular Kmeans */
    tmp_cluster_centres = kmeans_clustering(attributes,
                                            numAttributes,
                                            numObjects,
                                            nclusters,
                                            threshold,
                                            membership);      
	
    if (*cluster_centres) {
		free((*cluster_centres)[0]);
        free(*cluster_centres);
    }
    *cluster_centres = tmp_cluster_centres;

   
	free(membership);

    return 0;
}

