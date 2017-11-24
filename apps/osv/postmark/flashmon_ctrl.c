/*
 * FFSMark
 * Pierre Olivier <pierre.olivier@univ-brest.fr>
 *
 * Copyright (c) of University of Occidental Britanny (UBO) <pierre.olivier@univ-brest.fr>, 2015.
 *
 *	This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * NO WARRANTY. THIS SOFTWARE IS FURNISHED ON AN "AS IS" BASIS.
 * UNIVERSITY OF OCCIDENTAL BRITANNY MAKES NO WARRANTIES OF ANY KIND, EITHER
 * EXPRESSED OR IMPLIED AS TO THE MATTER INCLUDING, BUT NOT LIMITED
 * TO: WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY
 * OF RESULTS OR RESULTS OBTAINED FROM USE OF THIS SOFTWARE. 
 * See the GNU General Public License for more details.
 *
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "flashmon_ctrl.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FLASHMON_CTRL_FILE "/proc/flashmon"

typedef struct
{
  int r, e, w;
} fmon_tab_entry;

static int get_blk_num();
static int get_flashmon_tab(fmon_tab_entry *tab, int size);

int flashmon_test_loaded()
{
  if(access(FLASHMON_CTRL_FILE, F_OK) != -1)
    return 1;
  return 0;	
}

int flashmon_ctrl_reset()
{
  FILE *f;
  
  f = fopen(FLASHMON_CTRL_FILE, "w+");
  if(f == NULL)
  {
    perror("fopen");
    return -1;
  }
  
  fprintf(f, "reset");
  
  fclose(f);
  return 0;
}

int flashmon_ctrl_get_read_num()
{
  int size, i;
  int res = -1;
  fmon_tab_entry *tab;
  
  size = get_blk_num();
  if(size == -1)
    return -1;
  
  tab = (fmon_tab_entry *)malloc(size*sizeof(fmon_tab_entry));
  if(get_flashmon_tab(tab, size) != 0)
    goto out_free;
    
  res = 0;
  for(i=0; i<size; i++)
    res += tab[i].r;

out_free:
  free(tab);
  return res;
}

int flashmon_ctrl_get_write_num()
{
  int size, i;
  int res = -1;
  fmon_tab_entry *tab;
  
  size = get_blk_num();
  if(size == -1)
    return -1;
  
  tab = (fmon_tab_entry *)malloc(size*sizeof(fmon_tab_entry));
  if(get_flashmon_tab(tab, size) != 0)
    goto out_free;
    
  res = 0;
  for(i=0; i<size; i++)
    res += tab[i].w;

out_free:
  free(tab);
  return res;
}

int flashmon_ctrl_get_erase_info(flashmon_ctrl_erase_info *ei)
{
  int size, i, max_erased_block, min_erased_block;
  double variance;
  int res = -1;
  fmon_tab_entry *tab;
  
  size = get_blk_num();
  if(size == -1)
    return -1;
  
  tab = (fmon_tab_entry *)malloc(size*sizeof(fmon_tab_entry));
  if(get_flashmon_tab(tab, size) != 0)
    goto out_free;
    
  max_erased_block = min_erased_block = tab[0].e;
  ei->total_erase_num = 0;
  for(i=0; i<size; i++)
  {
    int erase_count = tab[i].e;
    ei->total_erase_num += erase_count;
    
    if(erase_count > max_erased_block)
      max_erased_block = erase_count;
      
    if(erase_count < min_erased_block)
      min_erased_block = erase_count;
  }
    
  ei->mean_erase_counter = (double)(ei->total_erase_num)/(double)size;
  ei->erase_delta = max_erased_block - min_erased_block;
  
  /* compute stdev */
  variance = 0.0;
  for(i=0; i<size; i++)
  {
    double erase_count = (double)(tab[i].e);
    double mean_diff = erase_count - (double)(ei->mean_erase_counter);
    variance += mean_diff*mean_diff / (double)size;
  }
  ei->erase_stdev = sqrt(variance);

  res = 0;
out_free:
  free(tab);
  return res;
}

static int get_blk_num()
{
  FILE *f;
  char ch;
  int lines = 0;
  
  f = fopen(FLASHMON_CTRL_FILE, "r");
  if(f == NULL)
  {
    perror("fopen");
    return -1;
  }
  
  while(!feof(f))
  {
    ch = fgetc(f);
    if(ch == '\n')
    {
      lines++;
    }
  }
  
  fclose(f);
  return lines;
}

static int get_flashmon_tab(fmon_tab_entry *tab, int size)
{
  FILE *f;
  char line[64];
  int i;
  
  f = fopen(FLASHMON_CTRL_FILE, "r");
  if(f == NULL)
  {
    perror("fopen");
    return -1;
  }
  
  i=0;
  while (fgets(line, sizeof(line), f))
  {
    if(sscanf(line, "%d %d %d\n", &(tab[i].r), &(tab[i].w), &(tab[i].e)) != 3)
    {
      fprintf(stderr, "ERR: sscanf()\n");
      goto out_close;
    }
    i++;
  }
  
out_close:
  fclose(f);
  return 0;
}

int flashmon_ctrl_get_block_num()
{
  return get_blk_num();
}
