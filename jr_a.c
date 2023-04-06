/*
 * Copyright (c) 2010 ... 2023 2024
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
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jr.h"

#define ARGLEN 100

/*
 * init_get_delm() -- translate a string into a delimiter
 */
char init_get_delm(FILE *fp, char *s)
{
  int d;
  int c = JLIB2_CHAR_NULL;

  if (s == (char *) NULL)
    return(c);

  if (strlen(s) == 1)
    {
      if ( ((*s) > 47)  && ((*s) < 58) ) /* 0 -- 9 */
	c = (*s) - 48;
      else
	c = (*s);
    }
  else
    {
      if (j2_is_numr(s) == (int) TRUE)
	{
	  d = atoi(s);
	  if (d < 256)
	    c = (char) d;
	  else
	    {
	      fprintf(fp, MSG_ERR_E049, s);
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  fprintf(fp, MSG_ERR_E049, s);
	  exit(EXIT_FAILURE);
	}
    }

  return(c);

} /* init_get_delm() */

/*
 * save_hints() -- save hints
 */
void save_hints(struct s_global_hints *g, char *s)
{
  char *tok = (char *) NULL;
  char *cpy;

  if (s == (char *) NULL)
    return;

  cpy = strdup(s);

  tok = strtok(cpy, ",");

  while  (tok != (char *) NULL)
    {
      if (strlen(tok) == 1)
	{
	  switch ((*tok))
	    {
	      case JR_COL_HINT_COMMA:
		g->decimal_point = ',';
		g->thousand_char = '.';
		break;
	      case JR_COL_HINT_SLASH:
		g->date_fmt_char = '/';
		break;
	      case JR_COL_HINT_DASH:
		g->date_fmt_char = '-';
		break;
	      case JR_COL_HINT_STR:
		g->date_format = DATE_STR_YYYY; /* YYYYMMDD */
		break;
	      case JR_COL_HINT_U_2:
		g->date_format = DATE_FMT_US_2; /* MM/DD/YY */
		break;
	      case JR_COL_HINT_E_2:
		g->date_format = DATE_FMT_EU_2; /* DD/MM/YY */
		break;
	      case JR_COL_HINT_J_2:
		g->date_format = DATE_FMT_YY;   /* YY/MM/DD */
		break;
	      case JR_COL_HINT_U_4:
		g->date_format = DATE_FMT_US_4; /* MM/DD/YYYY */
		break;
	      case JR_COL_HINT_E_4:
		g->date_format = DATE_FMT_EU_4; /* DD/MM/YYYY */
		break;
	      case JR_COL_HINT_J_4:
		g->date_format = DATE_FMT_YYYY; /* YYYY/MM/DD */
		break;
	      default:
		break;
	    }
	}
      tok = strtok((char *) NULL, ",");
    }

  free(cpy);

} /* save_hints() */

/*
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w, struct s_column *c)

{
  char ckarg[ARGLEN];
  int opt;
  int ok;
  int i;
  char *fout = (char *) NULL; 
  char *ferr = (char *) NULL; 
  char *ffmt = (char *) NULL; 
  char *frpt = (char *) NULL; 
  int display_help = (int) FALSE;
  int display_rev  = (int) FALSE;

  init_col_list(c);

  snprintf(ckarg, ARGLEN, "%c%c%c%c%c%c:%c:%c:%c:%c:%c:%c:%c:%c:%c:", 
		 ARG_FORCE, ARG_HELP, ARG_VERBOSE, ARG_VERSION, ARG_TITLE,
		 ARG_ERR, ARG_OUT, ARG_DELM, ARG_DELM_OUT, ARG_COL, 
		 ARG_REPORT, ARG_PARM, ARG_I_GLOBAL_HINT, ARG_O_GLOBAL_HINT,
	         ARG_STRIP_CHARS);


  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	  case ARG_REPORT:
	    frpt = optarg;
	    break;
	  case ARG_PARM:
	    ffmt = optarg;
	    break;
	  case ARG_TITLE:
	    w->print_titles =(int) TRUE;
	    break;
	  case ARG_COL:
	    if (append_col_list(w->err.fp, c, opt, optarg, w->prog_name) == COL_LIST_NULL)
	      {
		fprintf(w->err.fp, MSG_ERR_E036, SWITCH_CHAR, ARG_COL);
		fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
		exit(EXIT_FAILURE);
	      }
	    break;
	  case ARG_I_GLOBAL_HINT:
	    save_hints(&(w->input_hints), optarg);
	    break;
	  case ARG_O_GLOBAL_HINT:
	    save_hints(&(w->output_hints), optarg);
	    break;
	  case ARG_DELM:
	    w->use_in_delim = (int) TRUE;
	    w->delim_in     = init_get_delm(w->err.fp, optarg);
	    break;
	  case ARG_DELM_OUT:
	    w->use_out_delim = (int) TRUE;
	    w->delim_out     = init_get_delm(w->err.fp, optarg);
	    break;
	  case ARG_STRIP_CHARS:
	    if (w->strip_chars == (char *) NULL)
	      w->strip_chars = strdup(optarg);
	    break;
	  case ARG_FORCE:
	    w->force = (int) TRUE;
	    break;
	  case ARG_HELP:
	    display_help = (int) TRUE;
	    break;
	  case ARG_VERBOSE:
	    w->verbose = (int) TRUE;
	    break;
	  case ARG_VERSION:
	    display_rev = (int) TRUE;
	    break;
	  case ARG_ERR:
	    ferr = optarg;
	    break;
	  case ARG_OUT:
	    fout = optarg;
	    break;
	  default:
	    fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	    exit(EXIT_FAILURE);
	    break;
	}
    }

  /*** if necessary - save file names and open ***/
  ok = open_out(stderr, &(w->err), ferr, w->force);
  if (ok == EXIT_SUCCESS)
    ok = open_out(w->err.fp, &(w->out), fout, w->force);
  if (ok != EXIT_SUCCESS)
    {
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** show help/rev and exit ? ***/
  ok = EXIT_SUCCESS;
  if (display_help)
    ok = show_brief_help(w->out.fp, w->prog_name);
  if (display_rev)
    ok = show_rev(w->out.fp, w->prog_name);
  if (ok != EXIT_SUCCESS)
    {
      close_out(&(w->out));
      close_out(&(w->err));
      exit(ok);
    }

  /*** save format file name ***/
  if (ffmt != (char *) NULL)
    {
      w->fmt.fname = strdup(ffmt);
    }

  /*** Count number of files to process */
  for (i = optind; i < argc; i++)
    (w->num_files)++;
  if (w->num_files == 0)
    (w->num_files)++;  /* stdin when no files */

  /*** open report file (if required) ***/
  if (frpt != (char *) NULL)
    {
      if (strcmp(frpt, FILE_NAME_STDOUT) == 0)
	{
	  fprintf(w->err.fp, MSG_ERR_E037, frpt);
	  fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
      ok = open_out(w->err.fp, &(w->rpt), frpt, w->force);
      if (ok != EXIT_SUCCESS)
	{
	  fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

} /* process_arg() */
