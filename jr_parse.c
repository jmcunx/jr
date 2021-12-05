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
#include <ctype.h>
#include <stdlib.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jr.h"

#define MAX_FMT_SZ 100

/*
 * remove_chars() -- Remove characters from a string
 */
void remove_chars(char *value, char *sc)
{

  while ((*sc) != JLIB2_CHAR_NULL)
    {
      j2_bye_char(value, (*sc));
      sc++;
    }

} /* remove_chars() */

/*
 * fix_char() -- convert character based upon hints
 */
void fix_char(struct s_column *now, char *strip_chars)
{
  char *s;


  if (now->col_fmt != (char *) NULL)
    {
      if (strip_chars != (char *) NULL)
	{
	  if (strchr(now->col_fmt,JR_COL_FMT_RMV) != (char *) NULL)
	    remove_chars(now->col_value, strip_chars); /* needs tp be first */
	}
      if (strchr(now->col_fmt,JR_COL_FMT_TRIM) != (char *) NULL)
	{
	  j2_justleft(now->col_value);
	  j2_rtw(now->col_value);
	}
      if (strchr(now->col_fmt,JR_COL_FMT_EMB) != (char *) NULL)
	j2_bye_emb(now->col_value);
      if (strchr(now->col_fmt,JR_COL_FMT_LOW) != (char *) NULL)
	j2_strlwr(now->col_value);
      if (strchr(now->col_fmt,JR_COL_FMT_UP) != (char *) NULL)
	j2_strupr(now->col_value);
      if (strchr(now->col_fmt,JR_COL_LEAD_ZERO) != (char *) NULL)
	lead_zero(now->col_value, now->col_type, now->col_size_new);
    }

  if ( now->first_time )
    {
      now->first_time = (int) FALSE;
      strncpy(now->col_min, now->col_value, now->col_size);
      strncpy(now->col_max, now->col_value, now->col_size);
    }

  if (strcmp(now->col_value,now->col_min) < 0)
    strncpy(now->col_min, now->col_value, now->col_size);

  if (strcmp(now->col_value,now->col_max) > 0)
    strncpy(now->col_max, now->col_value, now->col_size);

  if (now->col_size > now->col_size_new)
    {
      s = &(now->col_value[(now->col_size_new)]);
      if (j2_is_all_spaces(s) != (int) TRUE)
	{
	  now->col_value[(now->col_size_new)] = JLIB2_CHAR_NULL;
	  now->truncated = (int) TRUE;
	}
    }

} /* fix_char() */

/*
 * fix_number() -- convert number based upon hints
 */
void fix_number(struct s_column *now, struct s_global_hints *gin, 
                struct s_global_hints *gout)
{
  char *fix_n;
  double num, max, min;

  j2_justleft(now->col_value);
  j2_rtw(now->col_value);
  fix_n = strdup(now->col_value);

  if (j2_fix_numr(fix_n, gin->thousand_char, gin->decimal_point) == (int) FALSE)
    {
      now->val_valid = (int) FALSE;
      if (strlen(now->col_value) > now->col_size_new)
	{
	  now->col_value[(now->col_size_new - 1)] = JLIB2_CHAR_NULL;
	  now->truncated = (int) TRUE;
	}
    }

  num = atof(fix_n);
  min = atof(now->col_min);
  max = atof(now->col_max);

  if ( now->first_time )
    {
      now->first_time = (int) FALSE;
      strncpy(now->col_min, now->col_value, now->col_size);
      strncpy(now->col_max, now->col_value, now->col_size);
    }

  if (num < min)
    strncpy(now->col_min, fix_n, now->col_size);
  if (num > max)
    strncpy(now->col_max, fix_n, now->col_size);

  if (now->col_fmt != (char *) NULL)
    {
      if (strchr(now->col_fmt,JR_COL_FMT_NUMB) != (char *) NULL)
	{
	  if (isdigit(*fix_n))
	    strncpy(now->col_value,fix_n, now->col_size);
	  else
	    {
	      if (num < (double) 0)
		{
		  if (num > (double) -1)
		    {
		      if (fix_n[1] == '0')
			strncpy(now->col_value,fix_n, now->col_size);
		      else
			snprintf(now->col_value, now->col_size, "-0%s",&(fix_n[1]));
		    }
		  else
		    strncpy(now->col_value,fix_n, now->col_size);
		}
	      else
		snprintf(now->col_value, now->col_size, "0%s",fix_n);
	    }
	}
    }

  if (strlen(now->col_value) > now->col_size_new)
    {
      now->col_value[(now->col_size_new - 1)] = JLIB2_CHAR_NULL;
      now->truncated = (int) TRUE;
    }

  /*** change decimal/period characters ? ***/
  if (gin->decimal_point != gout->decimal_point)
    {
      j2_chg_char(gin->thousand_char, 'a', now->col_value, (size_t) 0);
      j2_chg_char(gin->decimal_point, gout->decimal_point, now->col_value, (size_t) 0);
      j2_chg_char('a', gout->thousand_char, now->col_value, (size_t) 0);
    }

  /*** add leading zeros ***/
  if (strchr(now->col_fmt,JR_COL_LEAD_ZERO) != (char *) NULL)
    lead_zero(now->col_value, now->col_type, now->col_size_new);

  /*** done ***/
  free(fix_n);

}  /* fix_number() */

/*
 * fix_date() -- convert date based upon hints
 */
void fix_date(struct s_column *now, struct s_global_hints *gin, struct s_global_hints *gout)
{

  int dtype;
  char *dcpy;

  j2_justleft(now->col_value);
  j2_rtw(now->col_value);
  dcpy = strdup(now->col_value);

  switch (gin->date_format)
    {
      case DATE_FMT_EU_2: /* DD/MM/YY */
      case DATE_FMT_EU_4: /* DD/MM/YYYY */
	dtype = DATE_IN_FMT_E;
	break;
      case DATE_FMT_US_2: /* MM/DD/YY */
      case DATE_FMT_US_4: /* MM/DD/YYYY */
	dtype = DATE_IN_FMT_U;
	break;
      case DATE_FMT_YY:   /* YY/MM/DD */
      case DATE_FMT_YYYY: /* YYYY/MM/DD */
	dtype = DATE_IN_FMT_J;
	break;
      default:
	dtype = DATE_IN_FMT_NOT;
	break;
    }

#ifdef DEBUG
    fprintf(stderr, "DEBUG %s LINE %d %s\n", __FILE__, __LINE__, dcpy); fflush(stderr);
#endif

  switch (gout->date_format)
    {
      case DATE_FMT_EU_2: /* DD/MM/YY */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_EU_2, dtype);
	break;
      case DATE_FMT_EU_4: /* DD/MM/YYYY */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_EU_4, dtype);
	break;
      case DATE_FMT_US_2: /* MM/DD/YY */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_US_2, dtype);
	break;
      case DATE_FMT_US_4: /* MM/DD/YYYY */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_US_4, dtype);
	break;
      case DATE_FMT_YY:   /* YY/MM/DD */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_YY, dtype);
	break;
      case DATE_FMT_YYYY: /* YYYY/MM/DD */
	j2_ds_fmt(now->col_value, dcpy, DATE_FMT_YYYY, dtype);
	break;
      default:
	j2_ds_fmt(now->col_value, dcpy, DATE_STR_YYYY, dtype);
	break;
    }

#ifdef DEBUG
    fprintf(stderr, "DEBUG %s LINE %d %s -> %s\n", __FILE__, __LINE__, dcpy, now->col_value); fflush(stderr);
#endif

  free(dcpy);

  if (strlen(now->col_value) > now->col_size_new)
    {
      now->col_value[(now->col_size_new - 1)] = JLIB2_CHAR_NULL;
      now->truncated = (int) TRUE;
    }

  if (gin->date_fmt_char != gout->date_fmt_char)
    j2_chg_char(gin->date_fmt_char, gout->date_fmt_char, now->col_value, (size_t) 0);

  /* save low/high value */
  if ( now->first_time )
    {
      now->first_time = (int) FALSE;
      strncpy(now->col_min, now->col_value, now->col_size);
      strncpy(now->col_max, now->col_value, now->col_size);
    }
  else
    {
      if (strcmp(now->col_value,now->col_min) < 0)
	strncpy(now->col_min, now->col_value, now->col_size);
      if (strcmp(now->col_value,now->col_max) > 0)
	strncpy(now->col_max, now->col_value, now->col_size);
    }

} /* fix_date() */

/*
 * clear_col_vals() -- clear column values
 */
void clear_col_vals(struct s_column *c)
{
  struct s_column *now;

  now = c;

  while (now != (struct s_column *) NULL)
    {
      init_col_new(now);
      now = now->next_col;
    }

} /* clear_col_vals() */

/*
* parse_delim() -- process delimited data
*/
void parse_delim(struct s_column *c, char *buf, char delm)
{
  struct s_column *now;
  long int col;
  char *bcpy, *tok, d[2], *field;

  bcpy = (char *) NULL;
  d[0] = delm;
  d[1] = JLIB2_CHAR_NULL;

  j2_fix_delm(delm, &bcpy, buf);
  clear_col_vals(c);

  tok = strtok(bcpy, d);

  for (col = 1; tok != (char *) NULL; col++)
    {
      now = find_col_pos(c, col);
      if (now != COL_LIST_NULL)
	{
	  field = strdup(tok);
	  j2_justleft(field);
	  j2_rtw(field);
	  if (strlen(field) > now->col_size)
	    field[(now->col_size)] = JLIB2_CHAR_NULL;
	  strncpy(now->col_value,field, now->col_size);
	  free(field);
	}
      tok = strtok((char *) NULL, d);
    }

  free(bcpy);

} /* parse_delim() */

/*
 * parse_flat() -- load data from a flat fine into the column structure
 */
void parse_flat(struct s_column *c, char *buf)
{
  struct s_column *now;
  size_t bsize, cpy_size;
  char *bcpy;

  now = c;
  bsize = strlen(buf);

  clear_col_vals(c);

  do
    {
      if (now->col_begin <= bsize)
	{
	  bcpy = strdup(&(buf[(now->col_begin - 1)]));
	  cpy_size = strlen(bcpy);
	  if (cpy_size >= now->col_size)
	    bcpy[(now->col_size)] = JLIB2_CHAR_NULL;
	  strncpy(now->col_value,bcpy, now->col_size);
	  free(bcpy);
	}
      now = now->next_col;
    } while (now != COL_LIST_NULL);

} /* parse_flat() */

/*
 * convert_value() -- Correct column values based upon flags
 */
int convert_value(struct s_work *w, struct s_column *c, long int reads)
{

  struct s_column *now;
  int truncation = (int) FALSE;
  char *oldval = (char *) NULL;

  now = c;

  do
    {
      if (now->col_value != (char *) NULL)
	oldval = strdup(now->col_value);
      switch (now->col_type)
	{
	  case JR_COL_TYPE_CHAR:
#ifdef DEBUG
	    fprintf(stderr, "DEBUG %d %s - OLD: |%s|\n", __LINE__, __FILE__, now->col_value); fflush(stderr);
#endif
	    fix_char(now, w->strip_chars);
#ifdef DEBUG
	    fprintf(stderr, "DEBUG %d %s - NEW: |%s|\n", __LINE__, __FILE__, now->col_value); fflush(stderr);
#endif
	    break;
	  case JR_COL_TYPE_DATE:
	    fix_date(now, &(w->input_hints), &(w->output_hints));
	    break;
	  case JR_COL_TYPE_NUMB:
	    fix_number(now, &(w->input_hints), &(w->output_hints));
	    break;
	}
      if ((now->val_valid == (int) FALSE) || (now->truncated == (int) TRUE))
	{
	  truncation = (int) TRUE;
	  if (w->verbose == (int) TRUE)
	    {
	    if (now->truncated == (int) TRUE)
	      fprintf(w->err.fp, MSG_WARN_W006, reads, now->col_begin, oldval, now->col_value);
	    if (now->val_valid == (int) FALSE)
	      fprintf(w->err.fp, MSG_WARN_W007, reads, now->col_begin, now->col_value);
	    }
	}
      now = now->next_col;
      if (oldval != (char *) NULL)
	{
	  free(oldval);
	  oldval = (char *) NULL;
	}
    } while (now != COL_LIST_NULL);

  return(truncation);

} /* convert_value() */

/*
 * lead_zero() -- add leading zeros to the strt of a string
 */
void lead_zero(char *col, char col_type, long int max_size)
{

  char *nstr = (char *) NULL;
  char *s    = (char *) NULL;
  char fmt[MAX_FMT_SZ];
  int have_neg = (int) FALSE;

  if (col == (char *) NULL)
    return;
  if (max_size < 1L)
    return;
  if (max_size == 1L)
    {
      if (isspace((*col)) == 0)
	(*col) = '0';
      return;
    }
  if (strlen(col) > max_size)
    return;

  j2_rtw(col);
  j2_justleft(col);
  if(strlen(col) >= max_size)
    return;

  if ((col_type == JR_COL_TYPE_NUMB) && ((*col) == NEG_SIGN))
    {
      have_neg = (int) TRUE;
      (*col) = ' ';
      j2_justleft(col);
    }

  nstr = (char *) calloc((max_size + 2), sizeof(char));

  snprintf(fmt, MAX_FMT_SZ, "%%%lds", max_size);
  snprintf(nstr, (max_size + 1), fmt, col);

  strncpy(col, nstr, max_size);
  free(nstr);

  for (s = col; (*s) != JLIB2_CHAR_NULL; s++)
    {
      if (! isspace((*s)))
	break;
      (*s) = '0';
    }

  /* reapply the neg sign */
  if ( have_neg )
    (*col) = NEG_SIGN;

} /* lead_zero() */
