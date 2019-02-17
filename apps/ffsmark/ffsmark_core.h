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


#ifndef FFSMARK_CORE_H
#define FFSMARK_CORE_H

#include <stdio.h>

int cli_set_flashmon(char *param);
int cli_set_drop_creation(char *param);
int cli_set_drop_transactions(char *param);
int cli_set_fill_valid_creation(char *param);
int cli_set_fill_valid_transaction(char *param);
int cli_set_fill_invalid_creation(char *param);
int cli_set_fill_invalid_transaction(char *param);
int ffsmark_cli_set_location(char *param);
int ffsmark_reset_config();

int ffsmark_core_cli_show(FILE *fp);
int ffsmark_core_verb_report(FILE *fp);
int ffsmark_core_terse_report(FILE *fp);

/**
 * The hooks are called in that order :
 * (start)
 * 1.  Pre-subdir creation
 * 2.  Postmark creates its subdirectories pool
 * 3.  Pre file creation
 * 4.  Postmark creates the initial pool of file for the benchmark
 * 5.  Pre transactions
 * 6.  Postmark performs its transactions
 * 7.  Pre file deletion
 * 8.  Postmark deletes the remaining files after the benchmark
 * 9.  Pre subdirs deletion
 * 10. Postmark deletes the subdirectories created in 2.
 * 11. Post subdirs deletion 
 * (end)
 */

int ffsmark_hooks_pre_subdirs_creation();
int ffsmark_hooks_pre_files_creation();
int ffsmark_hooks_pre_transactions();
int ffsmark_hooks_pre_files_deletion();
int ffsmark_hooks_pre_subdirs_deletion();
int ffsmark_hooks_post_subdirs_deletion();

#endif /* FFSMARK_CORE_H */
