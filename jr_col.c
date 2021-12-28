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
#include <langinfo.h>
#include <time.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jr.h"

/*
 * write_numb_col() -- write a rearrange numeric column
 */
void write_numb_col(FILE *fp, char *s, long int out_size)
{
  long int s_size, i;

  j2_justleft(s);
  j2_rtw(s);

  s_size = strlen(s);
  if (s_size > out_size)
    s[out_size] = JLIB2_CHAR_NULL;
  s_size = strlen(s);

  s_size = s_size - out_size;

  if (s_size < 0)
    {
      for (i = s_size ; i < 0L ; i++)
	fprintf(fp," ");
    }

  fprintf(fp,"%s", s);

} /* write_numb_col() */

/*
 * write_char_col() -- write a rearrange character column
 */
void write_char_col(FILE *fp, char *c, long int out_size)

{
  long int c_size;

  c_size = (long int) strlen(c);

  if (c_size > out_size)
    c[out_size] = JLIB2_CHAR_NULL;
  fprintf(fp,"%s", c);

  for ( ; c_size < out_size ; c_size++)
    fprintf(fp," ");

} /* write_char_col() */

/*
 * jmc_fmt_datetime()
 */
void jmc_fmt_datetime(char *date_format, char *time_format)
{
  char fmt_date[11];
  char fmt_time[9];

  char *dfmt  = (char *) NULL;
  char *tfmt  = (char *) NULL; 
  time_t t;
  struct tm *ts = (struct tm *) NULL;
  int i;

  t = time(NULL);
  ts = localtime(&t);

  dfmt=strdup(nl_langinfo(D_FMT));
  tfmt=nl_langinfo(T_FMT);

  /* replace year symbol to get 4 digit year */
  for (i = 0; dfmt[i] != JLIB2_CHAR_NULL; i++)
    {
      if (dfmt[i] == 'y')
	dfmt[i] = 'Y';
    }

  /*** get formatted date/time ***/
  strftime(fmt_date, 11, dfmt, ts);
  strftime(fmt_time, 9,  tfmt, ts);
  free(dfmt);

  strncpy(date_format, fmt_date, 11);
  strncpy(time_format, fmt_time, 9);

}  /* jmc_fmt_datetime() */

/*
 * free_col_list() -- releases memory on each element of the list
 */
void free_col_list(struct s_column *col)
{

  if (col->next_col != COL_LIST_NULL)
    free_col_list(col->next_col);

  if (col->col_value != (char *) NULL)
    free(col->col_value);
  if (col->col_min != (char *) NULL)
    free(col->col_min);
  if (col->col_max != (char *) NULL)
    free(col->col_max);
  if (col->title != (char *) NULL)
    free(col->title);

  if (col->next_col != COL_LIST_NULL)
    free(col->next_col);

  init_col_list(col); /* init freed values to NULL */

} /* free_col_list() */

/*
 * init_col_new() -- initialize volital variables in the structure
 */
void init_col_new(struct s_column *col)
{

  col->val_valid      = (int) TRUE;
  col->truncated      = (int) FALSE;

  if (col->col_value != (char *) NULL)
    j2_clr_str(col->col_value, JLIB2_CHAR_NULL, (col->max_size + 1) );

} /* init_col_new() */

/*
 * init_col_list() -- initialize structure elements
 */
void init_col_list(struct s_column *col)
{
  col->col_type       = JLIB2_CHAR_NULL;
  col->val_valid      = (int) TRUE;
  col->truncated      = (int) FALSE;
  col->first_time     = (int) TRUE;
  col->col_begin      = 0;
  col->col_size       = 0;
  col->col_size_new   = 0;
  col->max_size       = 0;
  col->col_value      = (char *) NULL;
  col->col_min        = (char *) NULL;
  col->col_max        = (char *) NULL;
  col->col_fmt        = (char *) NULL;
  col->title          = (char *) NULL;
  col->next_col       = COL_LIST_NULL;

} /* init_col_list() */

/*
 * append_col_list() -- appends an empty entry to the list
 */
struct s_column *append_col_list(FILE *fp, struct s_column *col, char arg_sw, char *arg, char *prog_name)

{
  static int first_time = (int) TRUE;
  static struct s_column *last_col_l = COL_LIST_NULL;

  if ( first_time == (int) TRUE )
    {
      first_time = (int) FALSE;
      last_col_l = col;
    }
  else
    {
      last_col_l->next_col = 
	(struct s_column *) malloc(sizeof(struct s_column));
      if (last_col_l->next_col == COL_LIST_NULL)
	return(COL_LIST_NULL);
      last_col_l = last_col_l->next_col;
    }

  /*** initialize and format ***/
  init_col_list(last_col_l);
  fmt_col(fp, last_col_l, arg_sw, arg, prog_name);

  return(last_col_l);

} /* append_col_list() */

/*
 * allocate_cols() -- allocates column space
 */
int allocate_cols(struct s_column *col)

{
  if (col == COL_LIST_NULL)
    return((int) TRUE);

  if (col->col_size_new > col->col_size)
    col->max_size = col->col_size_new + FUDGE;
  else
    col->max_size = col->col_size + FUDGE;

  if (col->col_type == JR_COL_TYPE_DATE) /* all possible date sizes */
    {
      if (col->max_size < DATE_SIZE_MAX)
	col->max_size = DATE_SIZE_MAX;
      col->max_size +=5;
    }

  /*** note title data loaded from arguments, no need to allocate ***/

  if (col->col_value == (char *) NULL)
    col->col_value = (char *) malloc( ((sizeof(char) * col->max_size) + 1) );
  if (col->col_min == (char *) NULL)
    col->col_min =  (char *) malloc( ((sizeof(char) * col->max_size) + 1) );
  if (col->col_max == (char *) NULL)
    col->col_max =  (char *) malloc( ((sizeof(char) * col->max_size) + 1) );

  j2_clr_str(col->col_value, JLIB2_CHAR_NULL, (col->max_size + 1) );
  j2_clr_str(col->col_min,   JLIB2_CHAR_NULL, (col->max_size + 1) );
  j2_clr_str(col->col_max,   JLIB2_CHAR_NULL, (col->max_size + 1) );

  allocate_cols(col->next_col);

  return((int) TRUE);

} /* allocate_cols() */

/*
 * find_col_last() -- finds the last node
 */
struct s_column *find_col_last(struct s_column *col_l)

{
  if (col_l == COL_LIST_NULL)
    return(COL_LIST_NULL);

  if (col_l->next_col == COL_LIST_NULL)
    return(col_l);

  return( find_col_last(col_l) );

} /* find_col_last() */

/*
 * find_col_pos() -- finds a column with a specific position
 */
struct s_column *find_col_pos(struct s_column *c, long int p)
{

  struct s_column *now;

  now = c;

  do
    {
      if (now->col_begin == p)
	return(now);
      now = now->next_col;
    } while (now != COL_LIST_NULL);

  return(COL_LIST_NULL);

} /* find_col_pos() */

/*
 * show_col_info() -- display detail column information
 */
void show_col_info(FILE *fp, struct s_column *col_l, int show_headings)
{

  char *type_str;

  if (col_l == COL_LIST_NULL)
    return;

  if ( show_headings )
    {
      fprintf(fp, "\n");
      fprintf(fp, MSG_RPT_R08);
      fprintf(fp, MSG_RPT_R09);
      fprintf(fp, MSG_RPT_R10);
    }

  switch (col_l->col_type)
    {
      case JR_COL_TYPE_NUMB:
	type_str = RERR_LIT_01;
        break;
      case JR_COL_TYPE_DATE:
        type_str = RERR_LIT_06;
	break;
      case JR_COL_TYPE_CHAR:
        type_str = RERR_LIT_02;
        break;
      default:
        type_str = RERR_LIT_03;
        break;
    }

  fprintf(fp, MSG_RPT_R11, 
	  type_str, 
	  col_l->col_fmt,
	  col_l->col_begin,
	  col_l->col_size,
	  col_l->col_size_new,
	  (col_l->title == (char *) NULL ? " " : col_l->title));

  show_col_info(fp, col_l->next_col, (int) FALSE);

} /* show_col_info() */

/*
 * show_cols() -- displays column information
 */
void show_cols(FILE *fp, int col_num, struct s_column *col_l, 
               int show_headings)
{

  char *type_str;

  if (col_l == COL_LIST_NULL)
    return;

  if ( show_headings )
    {
      fprintf(fp, "\n");
      fprintf(fp, MSG_RPT_R01);
      fprintf(fp, MSG_RPT_R03);
      fprintf(fp, MSG_RPT_R05);
    }

  if (col_l->val_valid == (int) FALSE)
    type_str = RERR_LIT_05;
  else
    {
      if (col_l->truncated == (int) TRUE)
	type_str = RERR_LIT_04;
      else
	{
	  switch (col_l->col_type)
	    {
	      case JR_COL_TYPE_NUMB:
		type_str = RERR_LIT_01;
		break;
	      case JR_COL_TYPE_DATE:
		type_str = RERR_LIT_06;
		break;
	      case JR_COL_TYPE_CHAR:
		type_str = RERR_LIT_02;
		break;
	      default:
		type_str = RERR_LIT_03;
		break;
	    }
	}
    }

  fprintf(fp, MSG_RPT_R06, 
	  col_num, type_str,
          col_l->col_begin, col_l->col_size, col_l->col_size_new,
          (col_l->col_min   == (char *) NULL ? LIT_NULL : col_l->col_min),
          (col_l->col_max   == (char *) NULL ? LIT_NULL : col_l->col_max) );

  show_cols(fp, (++col_num), col_l->next_col, (int) FALSE);

} /* show_cols() */

/*
 * show_cols_data_hdr() -- display heading data
 */
void show_cols_data_hdr(FILE *fp, char *fname, long int rec_num, 
                        char *prog_name, int no_sub_hdr)
{
  static int show_main = (int) TRUE;

  if (show_main == (int) TRUE)
    {
      char date_format[12];
      char time_format[10];
      show_main = (int) FALSE;
      jmc_fmt_datetime(date_format, time_format);
      fprintf(fp, "%-25s %s\n", prog_name, "Text File Conversion Utility");
      fprintf(fp, "%10s %8s       Output Text File: %s\n",
              date_format, time_format, 
              (fname == (char *) NULL ? LIT_STDOUT : fname));
      fprintf(fp, "%s\n", "====================================================================================================================================");
      if (no_sub_hdr == (int) TRUE)
	return;
    }

  /*** column heading ***/
  fprintf(fp, "\n");
  fprintf(fp, MSG_RPT_R02, rec_num);
  fprintf(fp, MSG_RPT_R04);
  fprintf(fp, MSG_RPT_R05);

} /* END: show_cols_data_hdr() */

/*
 * show_cols_data() -- displays column detail data information
 */
void show_cols_data(FILE *fp, char *fname, long int rec_num, 
                    int col_num, struct s_column *col_l, 
                    int show_headings, char *prog_name)
{

  char *type_str;

  if (col_l == COL_LIST_NULL)
    return;

  if ( show_headings )
    show_cols_data_hdr(fp, fname, rec_num, prog_name, (int) FALSE);

  switch (col_l->col_type)
    {
      case JR_COL_TYPE_NUMB:
	if (col_l->val_valid == (int) TRUE)
	  {
	    if (col_l->truncated == (int) TRUE)
	      type_str = RERR_LIT_04;
	    else
	      type_str = RERR_LIT_01;
	  }
	else
	  type_str = RERR_LIT_05;
	break;
      case JR_COL_TYPE_DATE:
        type_str = RERR_LIT_06;
	if (col_l->truncated == (int) TRUE)
	  type_str = RERR_LIT_04;
	else
	  type_str = RERR_LIT_06;
        break;
      case JR_COL_TYPE_CHAR:
	if (col_l->truncated == (int) TRUE)
	  type_str = RERR_LIT_04;
	else
	  type_str = RERR_LIT_02;
        break;
      default:
        type_str = RERR_LIT_03;
        break;
    }

  fprintf(fp, MSG_RPT_R07, 
	  col_num, type_str,
          col_l->col_begin, col_l->col_size, col_l->col_size_new,
          (col_l->col_value == (char *) NULL ? LIT_NULL : col_l->col_value) );

  show_cols_data(fp, fname, rec_num, (++col_num), col_l->next_col, (int) FALSE, prog_name);

} /* show_cols_data() */

/*
* count_cols() -- count the number of colums
*/
long int count_cols(struct s_column *c)
{
  long int i = 1L;
  struct s_column *now;

  if (c->col_type == JLIB2_CHAR_NULL)
    return(0L);

  now = c->next_col;

  while (now != COL_LIST_NULL)
    {
      i++;
      now = now->next_col;
    }

  return(i);

} /* count_cols() */

/*
 * fmt_col() -- format argument for a column
 */
void fmt_col(FILE *fp, struct s_column *col, char arg_sw, 
             char *arg, char *prog_name)

{
  char *a    = (char *) NULL;
  char *tok1 = (char *) NULL;
  char *tok2 = (char *) NULL;
  char *tok3 = (char *) NULL;
  char *tok4 = (char *) NULL;
  char *tok5 = (char *) NULL;
  char *tok6 = (char *) NULL;
  char *tok7 = (char *) NULL;

  if (arg == (char *) NULL)
    {
      fprintf(fp, MSG_ERR_E006, LIT_NULL, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** fix/clone argument string ***/
  j2_fix_delm(',', &a, arg);
  if (a == (char *) NULL)
    {
      fprintf(fp, MSG_ERR_E036, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      free(a);
      exit(EXIT_FAILURE);
    }
  j2_rtw(a);
  j2_justleft(a);

  /*** Field Type ***/
  tok1 = strtok(a, ",");
  if (tok1 == (char *) NULL)
    {
      fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      free(a);
      exit(EXIT_FAILURE);
    }
  if (strlen(tok1) != 1)
    {
      fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      free(a);
      exit(EXIT_FAILURE);
    }
  col->col_type = (*tok1);

  switch (col->col_type)
    {
      case JR_COL_TYPE_CHAR:
	break;
      case JR_COL_TYPE_NUMB:
	break;
      case JR_COL_TYPE_DATE:
	break;
      default:
	fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
	fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	free(a);
	exit(EXIT_FAILURE);
    }

  /*** column position or field position ***/
  tok2 = strtok((char *) NULL, ",");
  if (tok2 == (char *) NULL)
    {
      fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      free(a);
      exit(EXIT_FAILURE);
    }
  col->col_begin = get_num(fp, arg_sw, tok2, 1L, -1L, prog_name);

  /*** column size ***/
  tok3 = strtok((char *) NULL, ",");
  if (tok3 == (char *) NULL)
    {
      fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
      fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      free(a);
      exit(EXIT_FAILURE);
    }
  col->col_size = get_num(fp, arg_sw, tok3, 1L, -1L, prog_name);

  /*** output column 'new size', will default if empty  ***/
  tok4 = strtok((char *) NULL, ",");
  if (tok4 == (char *) NULL)
    col->col_size_new = col->col_size;
  else
    {
      j2_rtw(tok4);
      if (strlen(tok4) > 0)
	col->col_size_new = get_num(fp, arg_sw, tok4, 1L, -1L, prog_name);
      else
	col->col_size_new = col->col_size;
    }

  /*** get optional format information ***/
  if (tok4 != (char *) NULL)
    {
      tok5 = strtok((char *) NULL, ",");
      if (tok5 != (char *) NULL)
	col->col_fmt = strdup(tok5);
    }
  /*** get optional column title information ***/
  if (tok5 != (char *) NULL)
    {
      tok6 = strtok((char *) NULL, ",");
      if (tok6 != (char *) NULL)
	col->title = strdup(tok6);
    }

  /*** make sure there is not an extra argument ***/
  if (tok6 != (char *) NULL)
    {
      tok7 = strtok((char *) NULL, ",");
      if (tok7 != (char *) NULL)
	{
	  fprintf(fp, MSG_ERR_E006, arg, SWITCH_CHAR, arg_sw);
	  fprintf(fp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  free(a);
	  exit(EXIT_FAILURE);
	}
    }

  /*** Done ***/
  free(a);

} /* fmt_col() */

/*
 * get_num() -- convert char to a num and validate
 */
long int get_num(FILE *fp, char arg, char *str, long min, 
                 long max, char *progname)
{
  long int i;

  if (j2_is_numr(str) != (int) TRUE)
    {
      fprintf(fp, MSG_ERR_E008, str, SWITCH_CHAR, arg);
      fprintf(fp, MSG_ERR_E000, progname, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  i = atol(str);

  if (min > -1)
    {
      if (i < min)
	{
	  fprintf(fp, MSG_ERR_E006, str, SWITCH_CHAR, arg);
	  fprintf(fp, MSG_ERR_E000, progname, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  if (max > -1)
    {
      if (i > max)
	{
	  fprintf(fp, MSG_ERR_E006, str, SWITCH_CHAR, arg);
	  fprintf(fp, MSG_ERR_E000, progname, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  return(i);

}  /* get_num() */

/*
 * load_col_file() -- load column data from a file
 */
long int load_col_file(FILE *fp_err, struct s_column *c, 
                       char *fname, char arg_sw, char *prog_name)
{
  FILE *fp;
  long int lines_read = 0L;
  char *buf = (char *) NULL;
  size_t bsize = (size_t) 0;

  buf = (char *) NULL;

  if (strcmp(fname, FILE_NAME_STDIN) == 0)
    {
      fprintf(fp_err, MSG_ERR_E037, fname);
      exit(EXIT_FAILURE);
    }

  if ( ! open_in(&fp, fname, fp_err) )
    {
      fprintf(fp_err, MSG_ERR_E021);
      exit(EXIT_FAILURE);
    }

  while (j2_getline(&buf, &bsize, fp) > (ssize_t) -1)
    {
      lines_read++;
      j2_bye_nl(buf);
      j2_chg_char('#', JLIB2_CHAR_NULL, buf, (size_t) 0);
      j2_rtw(buf);
      j2_justleft(buf);
      if (strlen(buf) > 0)
	{
	  if (append_col_list(fp_err, c, arg_sw, buf, prog_name) == COL_LIST_NULL)
	    {
	      fprintf(fp_err, MSG_ERR_E036, SWITCH_CHAR, arg_sw);
	      fprintf(fp_err, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
    }

  close_in(&fp, fname);
  if (buf != (char *) NULL)
    free(buf);
  return(lines_read);

} /* load_col_list() */

/*
 * write_col_delimited() -- displays column detail data information
 */
void write_col_delimited(FILE *fp, struct s_column *c, 
                         char delm, long int *writes)
{
  struct s_column *now;

  now = c;

  do
    {
      fprintf(fp, "%s%c", 
	      (now->col_value == (char *) NULL ? "" : now->col_value), delm);
      now = now->next_col;
    } while (now != COL_LIST_NULL);

  fprintf(fp, "\n");
  (*writes)++;

} /* write_col_delimited() */

/*
 * write_col_fixed() -- displays column detail data information
 */
void write_col_fixed(FILE *fp, struct s_column *c, long int *writes)
{
  struct s_column *now;

  now = c;

  do
    {
      switch (now->col_type)
	{
	  case JR_COL_TYPE_CHAR:
	    write_char_col(fp, now->col_value, now->col_size_new);
	    break;
	  case JR_COL_TYPE_NUMB:
	    write_numb_col(fp, now->col_value, now->col_size_new);
	    break;
	  case JR_COL_TYPE_DATE:
	    write_numb_col(fp, now->col_value, now->col_size_new);
	    break;
	  default:
	    write_char_col(fp, now->col_value, now->col_size_new);
	    break;
	}
      now = now->next_col;
    } while (now != COL_LIST_NULL);

  fprintf(fp, "\n");
  (*writes)++;

} /* write_col_delimited() */
