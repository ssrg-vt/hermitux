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


#ifndef FLASHMON_CTRL_H
#define FLASHMON_CTRL_H

typedef struct
{
  int total_erase_num;
  double mean_erase_counter;
  double erase_stdev;
  int erase_delta;
} flashmon_ctrl_erase_info;

int flashmon_test_loaded();
int flashmon_ctrl_reset();
int flashmon_ctrl_get_read_num();
int flashmon_ctrl_get_write_num();
int flashmon_ctrl_get_erase_info(flashmon_ctrl_erase_info *ei);
int flashmon_ctrl_get_block_num();

#endif /* FLASHMON_CTRL_H */
