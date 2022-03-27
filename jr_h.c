/*
 * Copyright (c) 2010 ... 2021 2022
 *     John McCue <jmccue@jmcunx.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jr.h"

/*** Messages ***/
#define MSG_HELP_11  "Rearrange Fields in a Flat Fixed Length File "
#define MSG_HELP_12  "OR on a delimited variable length file"

/*
 * show_brief_help()
 */
int show_brief_help(FILE *fp, char *pname)

{

  fprintf(fp, USG_MSG_USAGE, pname);
  fprintf(fp, "\t%s\n", MSG_HELP_11);
  fprintf(fp, "\t%s\n", MSG_HELP_12);
  fprintf(fp, USG_MSG_OPTIONS);

  fprintf(fp, USG_MSG_ARG_COL_CHAR_N1,    SWITCH_CHAR, ARG_COL);
  fprintf(fp, USG_MSG_ARG_COL_CHAR_N2);
  fprintf(fp, USG_MSG_ARG_COL_CHAR_N3);
  fprintf(fp, USG_MSG_ARG_COL_CHAR_N4,    SWITCH_CHAR, ARG_DELM);
  fprintf(fp, USG_MSG_ARG_COL_CHAR_N5);
  fprintf(fp, USG_MSG_ARG_COL_CHAR_N6);

  fprintf(fp, USG_MSG_ARG_DELM_I,    SWITCH_CHAR, ARG_DELM);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_DELM_B);
  fprintf(fp, USG_MSG_ARG_DELM_Z);
  fprintf(fp, USG_MSG_ARG_DELM_O,    SWITCH_CHAR, ARG_DELM_OUT);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_DELM_B);
  fprintf(fp, USG_MSG_ARG_DELM_Z);

  fprintf(fp, USG_MSG_ARG_ERR,       SWITCH_CHAR, ARG_ERR);
  fprintf(fp, USG_MSG_ARG_FORCE,     SWITCH_CHAR, ARG_FORCE);
  fprintf(fp, USG_MSG_ARG_HELP,      SWITCH_CHAR, ARG_HELP);

  fprintf(fp, USG_MSG_ARG_O_GLOBAL_HINT, SWITCH_CHAR, ARG_O_GLOBAL_HINT);
  fprintf(fp, USG_MSG_ARG_I_GLOBAL_HINT, SWITCH_CHAR, ARG_I_GLOBAL_HINT);

  fprintf(fp, USG_MSG_ARG_OUT,         SWITCH_CHAR, ARG_OUT);
  fprintf(fp, USG_MSG_ARG_PARM,        SWITCH_CHAR, ARG_PARM);
  fprintf(fp, USG_MSG_ARG_REPORT,      SWITCH_CHAR, ARG_REPORT);
  fprintf(fp, USG_MSG_ARG_STRIP_CHARS, SWITCH_CHAR, ARG_STRIP_CHARS);
  fprintf(fp, USG_MSG_ARG_TITLE_1,     SWITCH_CHAR, ARG_TITLE);
  fprintf(fp, USG_MSG_ARG_VERBOSE_4,   SWITCH_CHAR, ARG_VERBOSE);
  fprintf(fp, USG_MSG_ARG_VERSION,     SWITCH_CHAR, ARG_VERSION);

  return(EXIT_FAILURE);

}  /* show_brief_help() */

/*
 * show_rev()
 */
int show_rev(FILE *fp, char *pname)

{

  fprintf(fp,"%s %s:\n", pname, LIT_REV);

#ifdef J_LIB2_H
  fprintf(fp,"\t%s %s\n", LIT_INFO_02, j2_get_build());
#endif

#ifdef OSTYPE
  fprintf(fp,"\t%s\n",OSTYPE);
#endif  /* OSTYPE  */
#ifdef PRETTY_NAME
  fprintf(fp,"\t%s\n",PRETTY_NAME);
#endif  /* PRETTY_NAME  */
  fprintf(fp, LIT_INFO_01, __DATE__, __TIME__);

  return(EXIT_FAILURE);

}  /* show_rev() */
