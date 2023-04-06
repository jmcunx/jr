/*
 * Copyright (c) 1994 1995 1996 ... 2023 2024
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

/*
 * jr_j.c -- routines from lib j_lib2
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "jr.h"

#ifdef _AIX
#define HAS_GETLINE 1
#endif
#ifdef __NetBSD_Version__
#define HAS_GETLINE 1
#endif
#ifdef OpenBSD
#define HAS_GETLINE 1
#endif
#ifdef __FreeBSD__
#define HAS_GETLINE 1
#endif
#ifdef linux
#define HAS_GETLINE 1
#endif

#define SIZE_GETLINE_BUF 256
#define SIZ_YYYY 5
#define SIZ_YY   3

/*
 * j2_bye_char() -- removes all occurances of a specific a char from a string
 */
int j2_bye_char(char *buffer, char c)

{
  char *temp_str;
  int i, j, num_c = 0;
  int found = (int) FALSE;
#ifdef OpenBSD
  size_t len;
#endif
  
  if (c == JLIB2_CHAR_NULL)
    return(0);
  
  i = strlen(buffer) + 2;
  temp_str = calloc(i, sizeof(char));
  if (temp_str == (char *) NULL)
    return(-1);
  
  for ( i=0, j=0; buffer[i] != JLIB2_CHAR_NULL; i++)
    {
      if (buffer[i] == c)
	{
	  ++num_c;
	  found = (int) TRUE;
	}
      else
	{
	  temp_str[j] = buffer[i];
	  ++j;
	}
    }

  if (found == (int) FALSE)
    {
      free(temp_str);
      return(num_c);
    }

  /* note: size of buffer will never increase and   */
  /*       we removed at least 1 embedded character */
#ifdef OpenBSD
  len = strlen(temp_str) + 1;
  strlcpy(buffer, temp_str, len);
#else
  strcpy(buffer, temp_str);
#endif

  free(temp_str);
  return(num_c);

} /* j2_bye_char() */

/*
 * j2_bye_emb() -- remove embedded spaces from a string
 */
long int j2_bye_emb(char *s)
{
  char *n = (char *) NULL;
  int cfound = (int) FALSE;
  size_t len, i, j;

  if (s == (char *) NULL)
    return(-1L);

  len = strlen(s);
  if (len <  3)
    return(strlen(s));

  /* allocate space for new string */
  n = calloc((len + 2), sizeof(char));
  if (n == (char *) NULL)
    return(-1L);

  (*n) = (*s);
  for (i = 1, j = 1; s[i] != JLIB2_CHAR_NULL; i++)
    {
      if (cfound == (int) FALSE)
	{
	  if (! isspace(s[i]) )
	    cfound = (int) TRUE;
	  n[j] = s[i];
	  j++;
	  continue;
	}
      if ( isspace(s[i]) && isspace(s[(i - 1)]) )
	continue;
      n[j] = s[i];
      j++;
    }

  /* DONE, replace original string */
#ifdef OpenBSD
  strlcpy(s, n, len);
#else
  strcpy(s, n);
#endif

  free(n);

  return((long int) strlen(s));

} /* j2_bye_emb() */

/*
 * j2_bye_last() -- When the last byte = x, replaces it with NULL 
 *                  and returns string length
 */
long int j2_bye_last(char *x, char lastc)

{
  int i;
  
  if ( x == (char *) NULL )
    return(0L);
  if (strlen(x) < 1)
    return(0L);
  
  i = strlen(x) - (size_t) 1;
  
  if (x[i] == lastc)
    x[i] = JLIB2_CHAR_NULL;
  
  return((long int) strlen(x));
  
} /* j2_bye_last() */

/*
 * j2_chg_char() -- replaces all occurrences of one character
 *                  with another char
 */
long int j2_chg_char(char old, char new, char *s, SSIZE_T force_size)

{
  size_t found = (size_t) 0;

  if (s == (char *) NULL)
    return(found);

  if (force_size < (SSIZE_T) 1)
    {
      for ( ; (*s) != JLIB2_CHAR_NULL; s++)
        {
          if ((*s) == old)
	    {
	      found++;
	      (*s) = new;
	    }
        }
    }
  else
    {
      for ( ; found < force_size; found++)
        {
          if (s[found] == old)
	    (*s) = new;
        }
    }

  return((long int) found);

} /* j2_chg_char() */

/*
 * j2_clr_str() -- Clears a string with a char & ensure it ends with NULL
 */
long int j2_clr_str(char *s, char c, int size)
  
{
  if (s == (char *) NULL)
    return(0L);
  
  memset(s, (int) c, (size_t) size);
  s[size - 1 ] = JLIB2_CHAR_NULL;

  return((long int) strlen(s));

} /* j2_clr_str() */

/*
 * j2_f_exist() -- determines if a file exists
 */
int j2_f_exist(char *file_name)

{
  if (file_name == (char *) NULL)
    return((int) FALSE);

#ifdef _MSDOS
  if (access(file_name, 00) == -1)
    return (FALSE);
  else
    return (TRUE);
#else
  struct stat file_info;
  if (stat(file_name, &file_info) == 0)
    return (TRUE);
  else
    return (FALSE);
#endif

} /* j2_f_exist() */

/*
 * j2_fix_delm() -- generates a new string to allow strtok(3) to work
 *                  correctly. strtok(3) has problems when delimiters
 *                  are next to each other.
 */
long int j2_fix_delm(char delm, char **fixed, char *buf)
{
  size_t sz, i, j;

  (*fixed) = (char *) NULL;

  if (buf == (char *) NULL)
    return(0);
  if (strlen(buf) < 1)
    return(0);

  sz = (strlen(buf) * 2) + 5;

  (*fixed) = (char *) calloc(sz, sizeof(char));

  if ((*fixed) == (char *) NULL)
    return(0);

  for (i = 0, j=0; buf[i] != JLIB2_CHAR_NULL; i++)
    {
      if ((buf[i] == delm) && (buf[(i + 1)] == delm))
        {
          (*fixed)[j] = buf[i];
          j++;
          (*fixed)[j] = ' ';
        }
      else
        (*fixed)[j] = buf[i];
      j++;
    }

  return((long int)strlen((*fixed)));

} /* j2_fix_delm() */

/*
 * j2_fix_numr() -- make sure the string is numeric and if so move the
 *                  trailing minus to the beginning of the string
 *                  removing the thousand character(s)
 */
int j2_fix_numr(char *buffer, char thousand, char decimal_point)

{
  char *b = (char *) NULL;
  char *w = (char *) NULL;
  unsigned int u;
  int cd         = 0;
  int ct         = 0;
  int count_neg  = 0;
  int rc = TRUE;

  if (buffer == (char *) NULL)
    return(FALSE);
  switch(strlen(buffer))
    {
      case 0:
        return(FALSE);
      case 1:
	u = (unsigned int) (*buffer);
	if ( (u > (unsigned int) 57) || ((unsigned int) u < 48) )
	  {
	    rc = FALSE;
	    goto leave_j2_fix_numr;
	  }
        break;
    }

  w = strdup(buffer);
  if (w == (char *) NULL)
    return(FALSE);

  j2_rtw(w);
  j2_justleft(w);

  for (cd = 0, ct = 0, count_neg = 0, b=w; (*b) != JLIB2_CHAR_NULL; b++)
    {
      if (isspace((*b)) == TRUE)
      	{
      	  rc = FALSE;
      	  goto leave_j2_fix_numr;
      	}
      if ((*b) == thousand)
        {
          ct++;
          continue;
        }
      if ((*b) ==  '-')
        {
          count_neg++;
          continue;
        }
      if ((*b) == decimal_point)
        {
          cd++;
          continue;
        }
      u = (unsigned int) (*b);
      if ( (u > (unsigned int) 57) || ((unsigned int) u < 48) )
      	{
      	  rc = FALSE;
      	  goto leave_j2_fix_numr;
      	}
    }

  if (count_neg > 1)  /* more than 1 negative sign  */
    {
      rc = FALSE;
      goto leave_j2_fix_numr;
    }
  if (cd > 1)  /* more than 1 decimal points */
    {
      rc = FALSE;
      goto leave_j2_fix_numr;
    }

  /* remove thousand characters */
  if (ct > 0)
    {
      if (j2_bye_char(w, thousand) < 0)
	{
      	  rc = FALSE;
	  goto leave_j2_fix_numr;
	}
    }

  if (count_neg < 1)
    goto leave_j2_fix_numr;
  if ((*w) == '-')
    {
      count_neg = 0;  /* neg sign at the correct place */
      goto leave_j2_fix_numr;
    }

leave_j2_fix_numr:
  if (rc == TRUE)
    {
      cd = strlen(buffer);
      memset(buffer, JLIB2_CHAR_NULL, cd);
      if (count_neg < 1)
	strcpy(buffer, w);
      else
        {
          cd = strlen(w) - 1;
          w[cd] = JLIB2_CHAR_NULL;
          buffer[0] = '-';
          strcpy(&(buffer[1]), w);
        }
    }
  if (w != (char *) NULL)
    free(w);
  return(rc);

} /* j2_fix_numr() */

/*
 * j2_getline() -- A front end to getline(3) or a hack for
 *                 systems without getline(3)
 */
SSIZE_T j2_getline(char **buf, size_t *n, FILE *fp)
{
#ifdef HAS_GETLINE
  return(getline(buf, n, fp));
#else

  if ((*buf) == (char *) NULL)
    {
      (*n) = SIZE_GETLINE_BUF + 1;
      (*buf) = (char *) malloc(((*n) * sizeof(char)));
      if ((*buf) == (char *) NULL)
	return(-1);
      j2_clr_str((*buf), (*n), JLIB2_CHAR_NULL);
    }

  if (fgets((*buf), (*n), fp) == (char *) NULL)
    return(-1);
  return((SSIZE_T) strlen((*buf)));

#endif

} /* j2_getline() */

/*
 * j2_get_prgname() -- return filename or default name.
 *
 *                     basename() is not used.  Some OS Systems do not
 *                     have basename() so this can be a basename()
 *                     replacement
 */
char *j2_get_prgname(char *argv_0, char *default_name)

{
  int i = 0, j = 0;

  if (argv_0 == (char *) NULL)
    {
      if (default_name == (char *) NULL)
	return((char *) NULL);
      else
	return(strdup(default_name));
    }

  /* basename(3) not in Coherent or MS-DOS */

  for (i = 0, j = 0; argv_0[i] != JLIB2_CHAR_NULL; i++)
    {
      if (argv_0[i] == '/')
	j = i + 1;
      else
	{
	  if (argv_0[i] == '\\')
	    j = i + 1;
	}
    }

  if (argv_0[j] == JLIB2_CHAR_NULL)
    if (default_name == (char *) NULL)
      return((char *) NULL);
    else
      return(strdup(default_name));
  else
    return(strdup(&(argv_0[j])));

} /* j2_get_prgname() */

/*
 * j2_is_all_spaces() -- determine if a string is all white space
 */
int j2_is_all_spaces(char *s)

{
  if (s == (char *) NULL)
    return((int) FALSE); /* NULL pointer */
  
  for ( ; (*s) != JLIB2_CHAR_NULL; s++)
    {
      if ( ! isspace((*s)) )
	return(FALSE);
    }
  return(TRUE);
  
} /* j2_is_all_spaces() */

/*
 * j2_is_numr() -- determines if all characters are numeric
 */
int j2_is_numr(char *s)

{
  if (s == (char *) NULL)
    return((int) FALSE); /* NULL pointer */

  for ( ; (*s) != JLIB2_CHAR_NULL; s++)
    {
      if ( ! isdigit((int)(*s)) )
	return(FALSE);
    }

  return(TRUE);

} /* j2_is_numr() */

/*
 * j2_justleft() -- left justifies a string (removes beginning white
 *                  space) returns -1 on failure, strlen on success
 */
long int j2_justleft(char *s)

{
  char *temp_s;
  int i;
#ifdef OpenBSD
  size_t len;
#endif
  
  if (s == (char *) NULL)
    return(0L); /* NULL pointer, pretend 0 */

  if (strlen(s) < 2)
    {
      if (isspace((*s)))
	{
	  (*s) = JLIB2_CHAR_NULL;
	  return(0L);
	}
      else
	return((long int) strlen(s));
    }
  
#ifdef OpenBSD
  len = strlen(s) + 1;
  temp_s = strndup(s, len);
#else
  temp_s = strdup(s);
#endif

  if ( temp_s == (char *) NULL )
    return(-1L); /* not enough memory */
  
  for (i=0; temp_s[i] != JLIB2_CHAR_NULL; i++)
    {
      if (! isspace((int) temp_s[i]) )
	break;
    }
 
#ifdef OpenBSD
  strlcpy(s,&(temp_s[i]),len);
#else
  strcpy(s,&(temp_s[i]));
#endif

  free(temp_s);
  
  return((long int) strlen(s));

} /* j2_justleft() */

/*
 * j2_rtw() -- removes trailing white space
 */
long int j2_rtw(char *buffer)

{
  char *last_non_space;
  char *b = buffer;

  if (buffer == (char *) NULL)
    return(0L); /* NULL pointer */

  last_non_space = buffer;

  for ( ; (*buffer) != JLIB2_CHAR_NULL; buffer++)
    {
      if ( ! isspace((int)(*buffer)) )
	last_non_space = buffer;
    }

  if ( ! isspace ((int) *last_non_space) )
    last_non_space++;

  (*last_non_space) = JLIB2_CHAR_NULL;

  return((long int) strlen(b));

} /* j2_rtw() */

/*
 * j2_strlwr() -- converts a string to lower case
 */
void j2_strlwr(char *s)
{
  if (s != (char *) NULL)
    {
      for ( ; (*s) != JLIB2_CHAR_NULL; s++)
        {
          if ( isalpha((*s)) )
	    (*s) = tolower((*s));
        }
    }

} /* j2_strlwr() */

/*
 * j2_strupr() -- converts a string to upper case
 */
void j2_strupr(char *s)
{
  if (s != (char *) NULL)
    {
      for ( ; (*s) != JLIB2_CHAR_NULL; s++)
        {
          if ( isalpha((*s)) )
	    (*s) = toupper((*s));
        }
    }

} /* j2_strupr() */

/*
 * j2_ds_fmt() -- converts a String Date to one of these formats:
 *                        type            to String
 *                        --------------- -----------
 *                        DATE_FMT_US_2   MM/DD/YY
 *                        DATE_FMT_US_4   MM/DD/YYYY
 *                        DATE_FMT_YYYY   YYYY/MM/DD
 *                        DATE_FMT_YY     YY/MM/DD
 *                        DATE_FMT_EU_2   DD/MM/YY
 *                        DATE_FMT_EU_4   DD/MM/YYYY
 *                        DATE_STR_YY     YYMMDD
 *                        DATE_STR_YYYY   YYYYMMDD
 *                the in date can be almost any format, will return
 *                FALSE if the date does not look valid.
 *                In type is used to determine if US or European
 */
int j2_ds_fmt(char *date_format, char *date_s, int out_type, int fmt)
{
  struct s_j2_datetime d;

  if ( ! j2_ds_split(&d, date_s, fmt) )
    {
      date_format[0] = JLIB2_CHAR_NULL;
      return((int) FALSE);
    }
  
  switch (out_type)
    {
    case DATE_FMT_US_2: /* MM/DD/YY */
      SNPRINTF(date_format, 9, "%02d%c%02d%c%02d",
	      d.month,DATE_SEPERATOR,d.dd,DATE_SEPERATOR,d.yy);
      break;
    case DATE_FMT_US_4: /* MM/DD/YYYY */
      SNPRINTF(date_format, 11, "%02d%c%02d%c%04d",
	      d.month,DATE_SEPERATOR,d.dd,DATE_SEPERATOR,d.yyyy);
      break;
    case DATE_FMT_YYYY:  /* YYYY/MM/DD */
      SNPRINTF(date_format, 11, "%04d%c%02d%c%02d",
	      d.yyyy,DATE_SEPERATOR,d.month,DATE_SEPERATOR,d.dd);
      break;
    case DATE_FMT_YY: /* YY/MM/DD */
      SNPRINTF(date_format, 9, "%02d%c%02d%c%02d",
	      d.yy,DATE_SEPERATOR,d.month,DATE_SEPERATOR,d.dd);
      break;
    case DATE_FMT_EU_2:  /* DD/MM/YY */
      SNPRINTF(date_format, 9, "%02d%c%02d%c%02d",
	      d.dd,DATE_SEPERATOR,d.month,DATE_SEPERATOR,d.yy);
      break;
    case DATE_FMT_EU_4: /* DD/MM/YYYY */
      SNPRINTF(date_format, 11, "%02d%c%02d%c%04d",
	      d.dd,DATE_SEPERATOR,d.month,DATE_SEPERATOR,d.yyyy);
      break;
    case DATE_STR_YY  :  /* YYMMDD */
      SNPRINTF(date_format, 7, "%02d%02d%02d", d.yy,d.month,d.dd);
      break;
    case DATE_STR_YYYY: /* YYYYMMDD */
      SNPRINTF(date_format, 9, "%04d%02d%02d", d.yyyy,d.month,d.dd);
      break;
    default:
      date_format[0] = JLIB2_CHAR_NULL;
      return((int) FALSE);
    }

  return((int) TRUE);

} /* j2_ds_fmt() */

/*
 * j2_dl_fmt() -- converts a Long Date to a string then calls j2_dl_fmt()
 */
int j2_dl_fmt(char *date_format, long date_l, int out_type)
{
  char ds[50];

  memset(ds, JLIB2_CHAR_NULL, 50);
  SNPRINTF(ds, 49, "%ld",date_l);

  return( j2_ds_fmt(date_format, ds, out_type, DATE_IN_FMT_NOT) );

} /* j2_dl_fmt() */

/*
 * j2_ds_date_time() -- break to date/time portion
 */
void j2_ds_date_time(char **date_part, char **time_part, char *date_time)
{
  char *all, *t;
  int i, sp;

  if (date_time == (char *) NULL)
    return;
  if (strlen(date_time) < 1)
    return;

  all = strdup(date_time);
  t = (char *) NULL;
  j2_rtw(all);
  j2_justleft(all);

  for (i = 0, sp = -1; all[i] != JLIB2_CHAR_NULL; i++)
    {
      if (all[i] == ' ')
	{
	  sp = i;
	  break;
	}
    }

  /*** load time data ***/
  if (sp > 0)
    {
      t = &(all[sp + 1]);
      j2_rtw(t);
      j2_justleft(t);
      (*time_part) = strdup(t);
      all[sp] = JLIB2_CHAR_NULL;
    }
  else
    {
      switch (strlen(all))
	{
	  case 12:  /* YYMMDDhhmmss        */
	    (*time_part) = strdup(&(all[6]));
	    all[6] = JLIB2_CHAR_NULL;
	    break;
	  case 14:  /* YYYYMMDDhhmmss      */
	    (*time_part) = strdup(&(all[8]));
	    all[8] = JLIB2_CHAR_NULL;
	    break;
	  case 15:  /* YYMMDDhhmmsslll     */
	    (*time_part) = strdup(&(all[6]));
	    all[6] = JLIB2_CHAR_NULL;
	    break;
	  case 17:  /* YYYYMMDDhhmmsslll   */
	    (*time_part) = strdup(&(all[8]));
	    all[8] = JLIB2_CHAR_NULL;
	    break;
	  default:
	    /* leave (*time_part) as null */
	    break;
	}
    }

  /*** load date part ***/
  (*date_part) = strdup(all);

  /*** done ***/
  free(all);

} /* j2_ds_date_time() */

/*
 * j2_time_split() -- splits a time string into a structure
 *                    can adjust for PM
 */
int j2_time_split(struct s_j2_datetime *d, char *t_string)
{
  char hh[3], mm[3], ss[3], ml[4];
  double sec;
  int i, colon_found, ssize, c1, c2;
  int rstat = (int) TRUE;
  char *pmm = (char *) NULL;
  char *pss = (char *) NULL;
  char *t   = (char *) NULL;

  if (t_string == (char *) NULL)
    return((int) TRUE); /* this is OK */

  j2_clr_str(hh, JLIB2_CHAR_NULL, 3);
  j2_clr_str(mm, JLIB2_CHAR_NULL, 3);
  j2_clr_str(ss, JLIB2_CHAR_NULL, 3);
  j2_clr_str(ml, JLIB2_CHAR_NULL, 4);

  /* allocate work string, initial checks */
  t      = strdup(t_string);
  ssize  = strlen(t);
  j2_rtw(t);
  j2_justleft(t);
  if (ssize == 0)
    {
      free(t);
      return((int) TRUE); /* this is OK */
    }

  colon_found = j2_count_c(TIME_SEPERATOR, t);

  /* we have a time string, try and split */
  if (colon_found == 0)
    {
      switch (ssize)
	{
	  case 9: /* hhmmsslll */
	    ml[0]      = t[6];
	    ml[1]      = t[7];
	    ml[2]      = t[8];
	    /* fall through to 6 */
	  case 6: /* hhmmss    */
	    ss[0]      = t[4];
	    ss[1]      = t[5];
	    /* fall through to 4 */
	  case 4: /* hhmm      */
	    hh[0] = t[0];
	    hh[1] = t[1];
	    mm[0] = t[2];
	    mm[1] = t[3];
	    if (j2_is_numr(hh) && j2_is_numr(mm) && j2_is_numr(ss))
	      {
		d->hh      = atoi(hh);
		d->minutes = atoi(mm);
		d->ss      = atoi(ss);
		d->mil     = atoi(ml);
	      }
	    break;
	  default:
	    if (ssize > 6)
	      {
		hh[0]      = t[0];
		hh[1]      = t[1];
		mm[0]      = t[2];
		mm[1]      = t[3];
		sec        = atof(&(t[4]));
		if (j2_is_numr(hh) && j2_is_numr(mm))
		  { /* seconds have PM/AM attached, so best of luck */
		    d->hh      = atoi(hh);
		    d->minutes = atoi(mm);
		    d->ss      = sec;
		    d->mil     = (sec * 1000) - (d->ss * 1000); /* avoids rounding */
		  }
	      }
	    break;
	}
    }
  else
    {
      c1 = 0;
      c2 = 0;
      for (i = 0; t[i] != JLIB2_CHAR_NULL; i++)
	{
	  if (t[i] == TIME_SEPERATOR)
	    {
	      if (c1 == 0)
		c1 = i;
	      else
		c2 = i;
	    }
	}
      if (c1 == 0)
	rstat = (int) FALSE;
      else
	{
	  pmm = &(t[(c1 + 1)]);
	  t[c1] = JLIB2_CHAR_NULL;
	  if (c2 != 0)
	    {
	      pss = &(t[(c2 + 1)]);
	      t[c2] = JLIB2_CHAR_NULL;
	    }
	  d->hh      = atoi(t);
	  d->minutes = atoi(pmm);
	  if (c2 != 0)
	    {
	      sec        = atof(pss);
	      d->ss      = sec;
	      d->mil     = (sec * 1000) - (d->ss * 1000); /* avoids rounding */
	    }
	}
    }

  if ((d->hh > 24) || (d->minutes > 59) || (d->ss > 59))
    rstat = (int) FALSE;
  else
    {
      if ( (d->hh == 24) && ((d->minutes > 0) || (d->ss > 0)) )
	rstat = (int) FALSE;
    }

  if (d->hh < 13)
    {
      if (strstr(t_string, "PM") == (char *) NULL)
	{
	  if (strstr(t_string, "pm") != (char *) NULL)
	    d->hh += 12;
	}
      else
	d->hh += 12;
    }

  free(t);
  return(rstat);

} /* j2_time_split() */

/*
 * j2_date_split_fmt() -- splits date based on a delimiter
 */
int j2_date_split_fmt(struct s_j2_datetime *d, char *dstr, char *delim, int date_type)
{
  char yy[SIZ_YY];
  char *f1 = (char *) NULL;
  char *f2 = (char *) NULL;
  char *f3 = (char *) NULL;
  char *f4 = (char *) NULL;
  char fmt_yyyy[SIZ_YYYY];

  j2_clr_str(yy, JLIB2_CHAR_NULL, 3);

  /* split on delim */
  f1 = strtok(dstr, delim);
  if (f1 != (char *) NULL)
    {
      f2 = strtok((char *) NULL, delim);
      if (f2 != (char *) NULL)
	{
	  f3 = strtok((char *) NULL, delim);
	  if (f3 != (char *) NULL)
	    f4 = strtok((char *) NULL, delim);
	}
    }

  /* make sure we at least seem OK */
  if (f4 != (char *) NULL)
    return((int) FALSE);
  if (f3 == (char *) NULL)
    return((int) FALSE);
  if (f2 == (char *) NULL)
    return((int) FALSE);
  if (f1 == (char *) NULL)
    return((int) FALSE);
  if ( ! j2_is_numr(f1) )
    return((int) FALSE);
  if ( ! j2_is_numr(f2) )
    return((int) FALSE);
  if ( ! j2_is_numr(f2) )
    return((int) FALSE);

  /* seems fine, continue on */
  if (strlen(f1) == 4) /* must be YYYY-MM-DD */
    {
      yy[0]    = f1[2];
      yy[1]    = f1[3];
      d->yyyy  = atoi(f1);
      d->month = atoi(f2);
      d->dd    = atoi(f3);
      d->yy    = atoi(yy);
      return((int) TRUE);
    }

  /* some other format, so we need date_type */
  switch (date_type)
    {
      case DATE_IN_FMT_J:  /* YY/MM/DD or YYYY/MM/DD */
	d->dd    = atoi(f3);
	d->month = atoi(f2);
	if (strlen(f1) == 2)
	  d->yy   = atoi(f1);
	else
	  d->yyyy = atoi(f1);
	break;
	break;
      case DATE_IN_FMT_E:  /* DD/MM/YY or DD/MM/YYYY */
	d->month = atoi(f2);
	d->dd    = atoi(f1);
	if (strlen(f3) == 2)
	  d->yy   = atoi(f3);
	else
	  d->yyyy = atoi(f3);
	break;
      default:             /* default to MM/DD/YY or MM/DD/YYYY */
	d->month = atoi(f1);
	d->dd    = atoi(f2);
	if (strlen(f3) == 2)
	  d->yy   = atoi(f3);
	else
	  d->yyyy = atoi(f3);
	break;
    }

  /* format other year item */
  if (d->yy == 0)
    {
      if ((d->yyyy < 10000) && (d->yyyy > -1))
	{
	  SNPRINTF(fmt_yyyy, SIZ_YYYY, "%04d", d->yyyy);
	  d->yy = atoi((&(fmt_yyyy[2])));
	}
      else
	d->yy = d->yyyy % 1000;
    }
  else
    {
      if (d->yyyy == 0)
	d->yyyy   = j2_d_get_yyyy(d->yy);
    }

  /* done */
  return((int) TRUE);

} /* j2_date_split_fmt() */

/*
 * j2_date_split() -- splits a date string into a structure
 */
int j2_date_split(struct s_j2_datetime *d, char *d_string, int fmt)
{

  char mm[3], dd[3], yy[3], yyyy[5];
  int i, ssize;
  int rstat = (int) TRUE;
  char delim[2];
  char *dstr = (char *) NULL;

  if (d_string == (char *) NULL)
    return((int) FALSE);
  j2_clr_str(mm,    JLIB2_CHAR_NULL, 3);
  j2_clr_str(dd,    JLIB2_CHAR_NULL, 3);
  j2_clr_str(yy,    JLIB2_CHAR_NULL, 3);
  j2_clr_str(yyyy,  JLIB2_CHAR_NULL, 5);
  j2_clr_str(delim, JLIB2_CHAR_NULL, 2);
  dstr = strdup(d_string);
  j2_rtw(dstr);
  j2_justleft(dstr);

  ssize = strlen(dstr);
  if (ssize < 1)
    {
      if (dstr != (char *) NULL)
	free(dstr);
      return((int) FALSE);
    }

  /* find delimiter (if any) */
  i = j2_count_c(DATE_SEPERATOR, dstr);
  if (i > 0)
    delim[0] = DATE_SEPERATOR;
  else
    {
      i = j2_count_c('-', dstr);
      if (i > 0)
	delim[0] = '-';
    }

  /* process if no delimiter is found */
  if (delim[0] == JLIB2_CHAR_NULL)
    {
      if (j2_is_numr(dstr))
	{
	  switch (ssize)
	    {
	      case 6: /* YYMMDD */
		yy[0] = dstr[0];
		yy[1] = dstr[1];
		mm[0] = dstr[2];
		mm[1] = dstr[3];
		dd[0] = dstr[4];
		dd[1] = dstr[5];
		d->month  = atoi(mm);
		d->dd     = atoi(dd);
		d->yy     = atoi(yy);
		d->yyyy   = j2_d_get_yyyy(d->yy);
		break;
	      case 8: /* YYYYMMDD */
		yyyy[0]   = dstr[0];
		yyyy[1]   = dstr[1];
		yyyy[2]   = dstr[2];
		yyyy[3]   = dstr[3];
		yy[0]     = dstr[2];
		yy[1]     = dstr[3];
		mm[0]     = dstr[4];
		mm[1]     = dstr[5];
		dd[0]     = dstr[6];
		dd[1]     = dstr[7];
		d->month  = atoi(mm);
		d->dd     = atoi(dd);
		d->yy     = atoi(yy);
		d->yyyy   = atoi(yyyy);
		break;
	      default:
		rstat = (int) FALSE;
	    }
	}
      else
	rstat = (int) FALSE;
    }
  else
    rstat = j2_date_split_fmt(d, dstr, delim, fmt);
 
  /* done */
  if (dstr != (char *) NULL)
    free(dstr);
  return(rstat);

} /* j2_date_split() */

/*
 * j2_date_is_valid() -- determines if a date is valid
 */
int j2_date_is_valid(struct s_j2_datetime *d)
{
  char cyyyy[SIZ_YYYY];
  int test_yy;

  if (d->yyyy < 0)
    return((int) FALSE);
  if (d->yyyy > 9999)
    return((int) FALSE);
  if (d->yy > 99)
    return((int) FALSE);
  if (d->yy < 0)
    return((int) FALSE);
  if (d->month < 1)
    return((int) FALSE);
  if (d->month > 12)
    return((int) FALSE);
  if (d->dd < 1)
    return((int) FALSE);

  SNPRINTF(cyyyy, SIZ_YYYY, "%d",d->yyyy);

  if(d->yyyy > 999)
    {
      test_yy = atoi(&(cyyyy[2]));
    }
  else
    {
      if (d->yyyy > 99)
	test_yy = atoi(&(cyyyy[1]));
      else
	test_yy = atoi(cyyyy);
    }
  if (test_yy != d->yy)
    return(FALSE);

  switch (d->month)
    {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
        if (d->dd > 31)
          return((int) FALSE);
        break;
      case 4:
      case 6:
      case 9:
      case 11:
        if (d->dd > 30)
          return((int) FALSE);
        break;
      case 2:
	if (j2_d_isleap(d->yyyy))
	  {
	    if (d->dd > 29)
	      return((int) FALSE);
	  }
	else
	  {
	    if (d->dd > 28)
	      return((int) FALSE);
	  }
        break;
      default:
        return((int) FALSE);
    }

  return((int) TRUE);

} /* j2_date_is_valid() */

/*
 * j2_ds_split() -- split a date/time string
 */
int j2_ds_split(struct s_j2_datetime *d, char *date_time, int fmt)
{

  char *datestr = (char *) NULL;
  char *timestr = (char *) NULL;
  int rstat = (int) TRUE;

  j2_d_init(d);

  if (date_time == ((char *) NULL))
    return((int) FALSE);

  /* split date/time on space */
  j2_ds_date_time(&datestr, &timestr, date_time);
  if (datestr == (char *) NULL)
    rstat = (int) FALSE;

  /* load date/time data */
  if (rstat == (int) TRUE)
    rstat = j2_time_split(d, timestr);
  if (rstat == (int) TRUE)
    rstat = j2_date_split(d, datestr, fmt);

  /*** DONE ***/
  if (j2_date_is_valid(d) == (int) FALSE)
    {
      j2_d_init(d);
      rstat = (int) FALSE;
    }

  if (datestr != (char *) NULL)
    free(datestr);
  if (timestr != (char *) NULL)
    free(timestr);
  return(rstat);

} /* j2_ds_split() */

/*
 * j2_d_init() -- Initializes the split structure
 */
void j2_d_init(struct s_j2_datetime *d)
{
  d->month   = 0;
  d->dd      = 0;
  d->yyyy    = 0;
  d->yy      = 0;
  d->hh      = 0;
  d->minutes = 0;
  d->ss      = 0;
  d->mil     = 0;
} /* j2_d_init() */

/*
 * j2_d_isleap() -- Leap Year, True or False
 */
int j2_d_isleap(int year_yyyy)
{

  if ((year_yyyy % 4) == 0)
    {
      if (((year_yyyy % 100) == 0) && ((year_yyyy % 400) != 0))
	return(FALSE);
      else
	return(TRUE);
    }

  return(FALSE);

} /* j2_d_isleap() */

/*
 * j2_count_c() -- counts the number of a specific character in a string
 */
long int j2_count_c(char c, char *s)
{
  int i = 0;

  if (s != (char *) NULL)
    {
      for ( ; (*s) != JLIB2_CHAR_NULL; s++)
        {
          if ((*s) == c)
	    ++i;
        }
    }

  return(i);

} /* j2_count_c() */

/*
 * j2_d_get_yyyy() -- returns a 4 digit year
 */
int j2_d_get_yyyy(int yy)
{
  if (yy > 99)
    return( (1900 + yy) );

  if (yy > -1)
    {
      if (yy > CUTOFF_YY)
	return( (CENTURY_CURRENT * 100) + yy );
      else
	return( (CENTURY_NEXT * 100) + yy );
    }

  return(yy);
} /* j2_d_get_yyyy() */

