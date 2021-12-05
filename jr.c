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
#include <unistd.h>
#include <errno.h>

#ifdef IS_BSD
#include <err.h>
#endif

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jr.h"

/*
 * process_a_file()
 */
void process_a_file(struct s_work *w, struct s_column *c, char *fname,
                    char **buf, size_t *bsize)

{

  long int lines_read = 0L;
  FILE *fp;

#ifdef DEBUG
  fprintf(stderr, "DEBUG %d %s - \n", __LINE__, __FILE__); fflush(stderr);
#endif

  if ( ! open_in(&fp, fname, w->err.fp) )
    return;

  while (getline(buf, bsize, fp) > (ssize_t) -1)
    {
      lines_read++;
      j2_bye_nl((*buf));
      if (w->use_in_delim == (int) TRUE)
	parse_delim(c, (*buf), w->delim_in);
      else
	parse_flat(c, (*buf));
      if (convert_value(w, c, lines_read) == (int) TRUE)
	w->col_truncated = (int) TRUE;
      if (w->rpt.fp != (FILE *) NULL)
	show_cols_data(w->rpt.fp, w->out.fname, lines_read, 1L, c, (int) TRUE, w->prog_name);
      if (w->use_out_delim == (int) TRUE)
	write_col_delimited(w->out.fp, c, w->delim_out, &(w->total_writes));
      else
	write_col_fixed(w->out.fp, c, &(w->total_writes));
    }

#ifdef DEBUG
  fprintf(stderr, "DEBUG %d %s - \n", __LINE__, __FILE__); fflush(stderr);
#endif

  if (w->verbose == (int) TRUE)
    fprintf(w->err.fp, MSG_INFO_I003, lines_read, 0L, 
            (strcmp(fname,FILE_NAME_STDIN) == 0 ? LIT_STDIN : fname));

  close_in(&fp, fname);

} /* process_a_file() */

/*
 * process_all() -- Process all files
 */
void process_all(struct s_work *w, struct s_column *c, int argc, char **argv)

{
  int i;
  char *buf = (char *) NULL;
  size_t bsize = (size_t) 0;

  /* allocate initial read buffer memory (optional) */
  bsize = 200;
  buf = (char *) calloc(bsize, sizeof(char));
  if (buf == (char *) NULL)
    {
      fprintf(w->err.fp, MSG_ERR_E080, strerror(errno));
      return;
    }

  /* process data */
  for (i = optind; i < argc; i++)
    process_a_file(w, c, argv[i], &buf, &bsize);

  if (i == optind)
    process_a_file(w, c, FILE_NAME_STDIN, &buf, &bsize);

  /*** show stats ***/
  if (w->verbose == (int) TRUE)
    {
      fprintf(w->err.fp, MSG_INFO_I003, 0L, w->total_writes,
	      (w->out.fname == (char *) NULL ? LIT_STDOUT : w->out.fname));
    }
  if (w->rpt.fp != (FILE *) NULL)
    show_cols(w->rpt.fp, 1, c, (int) TRUE);

  if (buf != (char *) NULL)
    free(buf);

}  /* process_all() */

/*
 * main()
 */
int main(int argc, char **argv)

{
  struct s_work w;
  struct s_column c;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath",NULL) == -1)
    err(1,"pledge\n");
#endif

  init(argc, argv, &w, &c);

  /*** process all files ***/
  process_all(&w, &c, argc, argv);

  /*** done ***/
  if (w.rpt.fp != (FILE *) NULL)
    fprintf(w.rpt.fp, "\n%s\n", LIT_INFO_END);
  if ((w.col_truncated == (int) TRUE) && (w.verbose == (int) TRUE))
    fprintf(w.err.fp, MSG_WARN_W005);
  close_out(&(w.out));
  close_out(&(w.err));
  close_out(&(w.rpt));
  free_col_list(&c);

  exit(EXIT_SUCCESS);

}  /* main() */
