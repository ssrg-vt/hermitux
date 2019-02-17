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


#include "syscaches.h"

#include <stdio.h>
#include <unistd.h>

#define CONTROL_CACHE_FILE 	"/proc/sys/vm/drop_caches"

static int write_ctrl_file(int cmd);

int syscaches_drop_caches()
{
  sync();
  return write_ctrl_file(3);
}

int syscaches_drop_page_cache()
{
  sync();
  return write_ctrl_file(1);
}

int syscaches_drop_dentry_inode_caches()
{
  sync();
  return write_ctrl_file(2);
}

static int write_ctrl_file(int cmd)
{
  FILE *f;
  
  f = fopen(CONTROL_CACHE_FILE, "w+");
  
  fprintf(f, "%d", cmd);
  
  fclose(f);
  
  return 0;
}
