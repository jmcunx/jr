/*
 * Copyright (c) 2010 ... 2020 2021 
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

#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jr.h"

char *jr_i_c="$Id: jr_i.c,v 3.3 2021/02/21 20:41:02 jmccue Exp $";

/*
 * init_hints() -- initialize format hints
 */
void init_hints(struct s_global_hints *g)
{
    g->decimal_point  = DECIMAL_POINT;
    g->thousand_char  = THOUSAND_SEP;
    g->date_fmt_char  = DATE_SEPERATOR;
    g->date_format    = DATE_STR_YYYY;

} /* init_hints() */

/*
 * init_w() -- initialize work area
 */
void init_w(struct s_work *w, struct s_column *c, char *a)

{
  init_finfo(&(w->out));
  init_finfo(&(w->err));
  init_finfo(&(w->fmt));
  init_finfo(&(w->rpt));
  w->err.fp    = stderr;
  w->out.fp    = stdout;

  w->use_in_delim   = JLIB2_CHAR_NULL;
  w->use_out_delim  = JLIB2_CHAR_NULL;
  w->delim_in       = (int) FALSE;
  w->delim_out      = (int) FALSE;

  w->prog_name        = j2_get_prgname(a, PROG_NAME);
  w->strip_chars      = (char *) NULL;
  w->num_files        = 0;
  w->verbose          = (int) FALSE;
  w->force            = (int) FALSE;
  w->print_titles     = (int) FALSE;
  w->col_truncated    = (int) FALSE;
  w->total_writes     = (long int) 0;

  init_col_list(c);
  init_hints(&(w->input_hints));
  init_hints(&(w->output_hints));

}  /* init_w() */

/*
 * show_hints() -- display hints in use
 */
void show_hints(FILE *fp, struct s_global_hints *g, char *title)
{
  char *datestr;

  fprintf(fp, "\n%s\n", title);

  fprintf(fp, "%30s : %c\n", LIT_HINT_DECIMAL,  g->decimal_point);
  fprintf(fp, "%30s : %c\n", LIT_HINT_THOUSAND, g->thousand_char);
  fprintf(fp, "%30s : %c\n", LIT_HINT_DATE_SEP, g->date_fmt_char);

  switch (g->date_format)
    {
      case DATE_FMT_EU_2: 
	datestr = "DD/MM/YY";
	break;
      case DATE_FMT_US_2: 
	datestr = "MM/DD/YY";
	break;
      case DATE_FMT_EU_4: 
	datestr = "DD/MM/YYYY";
	break;
      case DATE_FMT_US_4: 
	datestr = "MM/DD/YYYY";
	break;
      case DATE_FMT_YY:   
	datestr = "YY/MM/DD";
	break;
      case DATE_FMT_YYYY: 
	datestr = "YYYY/MM/DD";
	break;
      case DATE_STR_YYYY: 
	datestr = "YYYYMMDD";
	break;
      default:
	datestr = "Unknown";
	break;
    }

  fprintf(fp, "%30s : %s\n", LIT_HINT_DATE_FMT, datestr);

} /* show_hints() */

/*
 * write_out_titles() -- write column title headings
 */
void write_out_titles(struct s_work *w, struct s_column *c)
{

  struct s_column *now;
  int title_found = (int) FALSE;

  /*** format titles for print ***/
  now = c;
  do
    {
      if (now->title != (char *) NULL)
	{
	  title_found = (int) TRUE;
	  strncpy(now->col_value, now->title, now->col_size_new);
	}
      now = now->next_col;
    } while (now != COL_LIST_NULL);

  /*** print the titles ***/
  if (title_found == (int) TRUE)
    {
      if (w->use_out_delim == (int) TRUE)
	write_col_delimited(w->out.fp, c, w->delim_out, &(w->total_writes));
      else
	write_col_fixed(w->out.fp, c, &(w->total_writes));
    }

  /*** clear column value ***/
  now = c;
  if (title_found == (int) TRUE)
    {
      do
	{
	  if (now->title != (char *) NULL)
	    init_col_new(now);
	  now = now->next_col;
	} while (now != COL_LIST_NULL);
    }

} /* write_out_titles() */

/*
 * init() -- initialize
 */
void init(int argc, char **argv, struct s_work *w, struct s_column *c)

{
  long int num_cols, parm_reads;

  init_w(w, c, argv[0]);

  process_arg(argc, argv, w, c);

  /*** add additional columns from format file (if any) ***/
  if (w->fmt.fname != (char *) NULL)
    {
      parm_reads = load_col_file(w->err.fp, c, w->fmt.fname, ARG_PARM, w->prog_name);
      if (w->verbose == (int) TRUE)
	fprintf(w->err.fp, MSG_INFO_I003, parm_reads, 0L,
		(w->out.fname == (char *) NULL ? LIT_STDOUT : w->fmt.fname));
    }

  /*** ensure we have column info, if not abort ***/
  num_cols = count_cols(c);
  if (num_cols < 1L)
    {
      fprintf(w->err.fp, MSG_ERR_E035);
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      close_out(&(w->out));
      close_out(&(w->err));
      close_out(&(w->rpt));
      exit(EXIT_FAILURE);
    }

  /*** alocate memory for columns ***/
  allocate_cols(c);

  /*** report headings and initialization information ***/
  if (w->rpt.fp != (FILE *) NULL)
    {
      show_cols_data_hdr(w->rpt.fp, w->out.fname, 0L, w->prog_name, (int) TRUE);
      show_hints(w->rpt.fp, &(w->input_hints),  JR_HINT_LIT_I);
      show_hints(w->rpt.fp, &(w->output_hints), JR_HINT_LIT_O);
      if (w->strip_chars != (char *) NULL)
	fprintf(w->rpt.fp,JR_STRIP_O, w->strip_chars);
      show_col_info(w->rpt.fp, c, (int) TRUE);
    }
  if (w->print_titles == (int) TRUE)
    write_out_titles(w, c);

}  /* init() */

/* END: jr_i.c */
