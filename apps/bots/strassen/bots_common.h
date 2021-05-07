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

#ifndef _COMMON_H
#define _COMMON_H

#ifndef CC
#define CC ""
#endif
#ifndef CFLAGS
#define CFLAGS ""
#endif
#ifndef LD
#define LD ""
#endif
#ifndef LDFLAGS
#define LDFLAGS ""
#endif
#ifndef CDATE
#define CDATE ""
#endif
#ifndef CMESSAGE
#define CMESSAGE ""
#endif

#define BOTS_ERROR                         0
#define BOTS_ERROR_NOT_ENOUGH_MEMORY       1
#define BOTS_ERROR_UNRECOGNIZED_PARAMETER  2

#define BOTS_WARNING                       0

void bots_get_date(char *str);
void bots_get_architecture(char *str);
void bots_get_load_average(char *str);
void bots_print_results(void);

#define BOTS_TMP_STR_SZ 256

#endif
