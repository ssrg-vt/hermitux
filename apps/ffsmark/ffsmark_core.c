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

#include "ffsmark_core.h"

#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

#include "flashmon_ctrl.h"
#include "syscaches.h"

#define RANDOM_DATA_SRC     "/dev/urandom"
#define VALID_FILE_NAME     "__ffsmark_valid__"
#define INVALID_FILE_NAME   "__ffsmark_invalid__"

typedef struct
{
  int flashmon_enabled;
  int drop_creation;
  int drop_transaction;
  double fill_valid_creation;
  double fill_invalid_creation;
  double fill_valid_transaction;
  double fill_invalid_transaction;
  char location[128];
} ffsmark_config;

int post_bench_read_num;
int post_bench_write_num;
flashmon_ctrl_erase_info post_bench_flash_ei;

static ffsmark_config cfg = {0, 0, 0, 0, 0, 0, 0, "./"};

static int fill_valid_invalid(char *path, double valid_ratio, 
  double invalid_ratio);
static int ffsmark_core_create_file(char *path, uint64_t size);


int cli_set_flashmon(char *param)
{
  if (param && (!strcmp(param, "true") || !strcmp(param, "false")))
  {
    if(!strcmp(param, "true"))
    {
      if(flashmon_test_loaded())
      {
        cfg.flashmon_enabled = 1;
        flashmon_ctrl_reset();
      }
      else
      {
        fprintf(stderr, "Error: flashmon is not loaded\n");
        cfg.flashmon_enabled = 0;
      }
    }
  }
  else
    fprintf (stderr, "Error: please indicate true or false\n");
  
  return 1;
}

int ffsmark_core_cli_show(FILE *fp)
{
  fprintf(fp, "Flashmon is %s.\n", cfg.flashmon_enabled ? "enabled" : \
    "disabled");
    
  fprintf(fp, "System caches drop: ");
  if(cfg.drop_creation && cfg.drop_transaction)
    fprintf(fp, "before creation and transaction.\n");
  else if(cfg.drop_creation)
    fprintf(fp, "before creation.\n");
  else if(cfg.drop_transaction)
    fprintf(fp, "before transaction.\n");
  else
    fprintf(fp, "disabled.\n");
    
  fprintf(fp, "Creation fill rate (valid/invalid): %lf/%lf.\n",
    cfg.fill_valid_creation, cfg.fill_invalid_creation);
    
  fprintf(fp, "Transaction fill rate (valid/invalid): %lf/%lf.\n",
    cfg.fill_valid_transaction, cfg.fill_invalid_transaction);
    
  return 0;
}

int ffsmark_cli_set_location(char *param)
{
  int ret;
  struct stat s;
  
  ret = stat(param, &s);
  if(ret == -1) 
  {
    if(errno == ENOENT)
    {
      fprintf(stderr, "ERR: %s does not exists.\n", param);
      return -1;
    }
    else
    {
      perror("stat");
      return -1;
    }
  } 
  else
  {
    if(S_ISDIR(s.st_mode))
      strcpy(cfg.location, param);
    else
    {
      fprintf(stderr, "ERR: %s is not a directory\n", param);
      return -1;
    }
  }
  
  return 0;
}

int ffsmark_core_verb_report(FILE *fp)
{
  if(cfg.flashmon_enabled)
  {
    fprintf(fp, "\nFlash:\n");
    
    fprintf(fp, "\tPage write num.: %d\n", post_bench_write_num);
    fprintf(fp, "\tPage read num.: %d\n", post_bench_read_num);
    
    fprintf(fp, "\tErase num : %d\n", post_bench_flash_ei.total_erase_num);
    fprintf(fp, "\tMean erase counter : %lf\n", 
      post_bench_flash_ei.mean_erase_counter);
    fprintf(fp, "\tErase counter delta : %d\n", 
      post_bench_flash_ei.erase_delta);
    fprintf(fp, "\tErase counter stdev : %lf\n", 
      post_bench_flash_ei.erase_stdev);
  }
  
  return 0;
}

int cli_set_drop_creation(char *param)
{
  if (param && !strcmp(param, "true"))
    cfg.drop_creation = 1;
  else if (param && !strcmp(param, "false"))
    cfg.drop_creation = 0;
  else
    fprintf (stderr, "Error: please indicate true or false\n");
  
  return 1;
}

int cli_set_drop_transactions(char *param)
{
  if (param && !strcmp(param, "true"))
    cfg.drop_transaction = 1;
  else if (param && !strcmp(param, "false"))
    cfg.drop_transaction = 0;
  else
    fprintf (stderr, "Error: please indicate true or false\n");
  
  return 1;
}

int cli_set_fill_valid_creation(char *param)
{
  double val = atof(param);
  
  if((cfg.fill_invalid_creation + val) > 1.0)
    fprintf(stderr, "Error : creation invalid rate (%lf) + %lf > 1.0\n",
      cfg.fill_invalid_creation, val);
  else
    cfg.fill_valid_creation = val;
  
  return 1;
}

int cli_set_fill_valid_transaction(char *param)
{
  double val = atof(param);
  
  if((cfg.fill_invalid_transaction + val) > 1.0)
    fprintf(stderr, "Error : transaction invalid rate (%lf) + %lf > 1.0\n",
      cfg.fill_invalid_transaction, val);
  else
    cfg.fill_valid_transaction = val;
  
  return 1;
}

int cli_set_fill_invalid_creation(char *param)
{
  double val = atof(param);
  
  if((cfg.fill_valid_creation + val) > 1.0)
    fprintf(stderr, "Error : creation valid rate (%lf) + %lf > 1.0\n",
      cfg.fill_valid_creation, val);
  else
    cfg.fill_invalid_creation = val;
  
  return 1;
}

int cli_set_fill_invalid_transaction(char *param)
{
  double val = atof(param);
  
  if((cfg.fill_valid_transaction + val) > 1.0)
    fprintf(stderr, "Error : transaction valid rate (%lf) + %lf > 1.0\n",
      cfg.fill_valid_transaction, val);
  else
    cfg.fill_invalid_transaction = val;
  
  return 1;
}

int ffsmark_core_terse_report(FILE *fp)
{
  fprintf(stderr, "Terse report not available\n");
  return -1;
}

  int flashmon_enabled;
  int drop_creation;
  int drop_transaction;
  double fill_valid_creation;
  double fill_invalid_creation;
  double fill_valid_transaction;
  double fill_invalid_transaction;
  char location[128];

int ffsmark_reset_config()
{
  cfg.flashmon_enabled = 0;
  cfg.drop_creation = cfg.drop_transaction = 0;
  cfg.fill_invalid_creation = cfg.fill_valid_creation = 0;
  cfg.fill_invalid_transaction = cfg.fill_valid_transaction = 0;
  strcpy(cfg.location, "./");
  
  return 0;
}

/**
 * Fill the file system at path with valid data and invalid data
 * (ratios are floating point percentages between 0 and 1)
 */
static int fill_valid_invalid(char *path, double valid_ratio, 
  double invalid_ratio)
{
  struct statvfs vstat;
  uint64_t free_space, valid_size, invalid_size;
  char valid_path[128], invalid_path[128];
  
  if(statvfs(path, &vstat) != 0)
  {
    perror("statvfs");
    return -1;
  }
  
  free_space = (uint64_t)(vstat.f_bfree*vstat.f_bsize);
  valid_size = (uint64_t)(valid_ratio*((double)(free_space)));
  invalid_size = (uint64_t)(invalid_ratio*((double)(free_space)));
  
  sprintf(valid_path, "%s/%s.%d", cfg.location, VALID_FILE_NAME, 
    rand()%50000);
  sprintf(invalid_path, "%s/%s.%d", cfg.location, INVALID_FILE_NAME,
    rand()%50000);
    
  while(access(valid_path, F_OK) != -1)
    sprintf(valid_path, "%s/%s.%d", cfg.location, VALID_FILE_NAME, 
    rand()%50000);
    
  while(access(invalid_path, F_OK) != -1)
    sprintf(invalid_path, "%s/%s.%d", cfg.location, INVALID_FILE_NAME, 
    rand()%50000);
    
  if(valid_size)
  {
    if(ffsmark_core_create_file(valid_path, valid_size))
    {
      fprintf(stderr, "Error creating valid data\n");
      return -1;
    }
  }
    
  if(invalid_size)
  {
    if(ffsmark_core_create_file(invalid_path, invalid_size))
    {
      fprintf(stderr, "Error creating invalid data\n");
      return -1;
    }
    remove(invalid_path);
  }
    
  return 0;
}

/**
 * Create file filled with random data
 */
static int ffsmark_core_create_file(char *path, uint64_t size)
{
  //FILE *src, *dest;
  int src, dest;
  char buf[4096];
  int i, rest, r, ret=-1;
  
  //~ src = fopen(RANDOM_DATA_SRC, "r");
  //~ if(src == NULL)
  //~ {
    //~ fprintf(stderr, "Error openning %s\n", RANDOM_DATA_SRC);
    //~ perror("fopen");
    //~ return -1;
  //~ }
  
  src = open(RANDOM_DATA_SRC, O_RDONLY);
  if(src == -1)
  {
    perror("open");
    return -1;
  }
  
  //~ dest = fopen(path, "w+");
  //~ if(dest == NULL)
  //~ {
    //~ fprintf(stderr, "Error openning %s for creation\n", path);
    //~ perror("fopen");
    //~ return -1;
  //~ }
  
  dest = open(path, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
  if(dest == -1)
  {
    perror("open");
    return -1;
  }
  
  //~ for(i=0; i<(size/4096); i++)
  //~ {
    //~ if((r = fread(buf, 4096, 1, src)) != 1)
    //~ {
      //~ fprintf(stderr, "read : %d\n", r);
      //~ perror("fread");
      //~ goto out_close;
    //~ }
    //~ 
    //~ if(fwrite(buf, 4096, 1, dest) != 1)
    //~ {
      //~ perror("fwrite");
      //~ goto out_close;
    //~ }
  //~ }
  
  for(i=0; i<(size/4096); i++)
  {
    if((r = read(src, buf, 4096)) != 4096)
    {
      fprintf(stderr, "read : %d\n", r);
      perror("read");
      goto out_close;
    }
    
    if((r = write(dest, buf, 4096)) != 4096)
    {
      fprintf(stderr, "write : %d\n", r);
      perror("write");
      goto out_close;
    }
  }
  
  rest = size % 4096;
  //~ if((r = fread(buf, rest, 1, src)) != 1)
  //~ {
    //~ fprintf(stderr, "read : %d\n", r);
    //~ perror("fread");
    //~ goto out_close;
  //~ }
  //~ 
  //~ if(fwrite(buf, rest, 1, dest) != 1)
  //~ {
    //~ perror("fwrite");
    //~ goto out_close;
  //~ }
  
  if((r = read(src, buf, rest)) != rest)
  {
    fprintf(stderr, "read : %d\n", r);
    perror("read");
    goto out_close;
  }
  
  if((r = write(dest, buf, rest)) != rest)
  {
    fprintf(stderr, "write : %d\n", r);
    perror("write");
    goto out_close;
  }
  
  ret = 0;
out_close:
  //~ fclose(dest);
  //~ fclose(src);
  close(dest);
  close(src);
  return ret;
}

int ffsmark_hooks_pre_subdirs_creation()
{
  if(cfg.fill_invalid_creation || cfg.fill_valid_creation)
  {
    printf("Creating valid (%lf) and invalid (%lf) data for creation "
      "phase\n", cfg.fill_valid_creation, cfg.fill_invalid_creation);
    if(fill_valid_invalid(cfg.location, cfg.fill_valid_creation,
      cfg.fill_invalid_creation) != 0)
        return -1;
  }
  
  if(cfg.drop_creation)
  {
    printf("Cache drop before creation phase\n");
    syscaches_drop_caches();
  }
  
  if(cfg.flashmon_enabled)
  {
    printf("Reset flashmon\n");
    if(flashmon_ctrl_reset() != 0)
      return -1;
  }
  
  return 0;
}

int ffsmark_hooks_pre_files_creation()
{
  //printf("ffsmark_hooks_pre_files_creation\n");
  return 0;
}

int ffsmark_hooks_pre_transactions()
{
  if(cfg.fill_invalid_transaction || cfg.fill_valid_transaction)
  {
    printf("Creating valid (%lf) and invalid (%lf) data for transaction"
      " phase\n", cfg.fill_valid_transaction, 
      cfg.fill_invalid_transaction);
    if(fill_valid_invalid(cfg.location, cfg.fill_valid_transaction,
      cfg.fill_invalid_transaction) != 0)
        return -1;
  }
  
  if(cfg.drop_transaction)
  {
    printf("Cache drop before transaction phase\n");
    syscaches_drop_caches();
   } 
/*  if(cfg.flashmon_enabled)
    if(flashmon_ctrl_reset() != 0)
      return -1;
*/
  return 0;
}

int ffsmark_hooks_pre_files_deletion()
{
  //printf("ffsmark_hooks_pre_files_deletion\n");
  return 0;
}

int ffsmark_hooks_pre_subdirs_deletion()
{
  //printf("ffsmark_hooks_pre_subdirs_deletion\n");
  return 0;
}

int ffsmark_hooks_post_subdirs_deletion()
{
  //printf("ffsmark_hooks_post_subdirs_deletion\n");
  if(cfg.flashmon_enabled)
  {
    post_bench_read_num = flashmon_ctrl_get_read_num();
    post_bench_write_num = flashmon_ctrl_get_write_num();
    
    if(flashmon_ctrl_get_erase_info(&post_bench_flash_ei) == -1)
      return -1;
  }
  return 0;
}

