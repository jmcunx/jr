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
#ifndef JR_H

#define JR_H "INCLUDED"

#define PROG_NAME  "jr"

#define FUDGE 20  /* extract to add to column size */

/*** column type defines ***/
#define JR_COL_TYPE_CHAR 'c'
#define JR_COL_TYPE_NUMB 'n'
#define JR_COL_TYPE_DATE 'd'

#define JR_COL_FMT_EMB     'e'  /* Remove Embedded White Space   */
#define JR_COL_FMT_NUMB    'f'  /* correct numeric data          */
#define JR_COL_FMT_LOW     'L'  /* translate to lower case       */
#define JR_COL_FMT_TRIM    't'  /* rtw / justleft character data */
#define JR_COL_FMT_UP      'U'  /* translate to upper case       */
#define JR_COL_FMT_RMV     'r'  /* Remove specified characters   */
#define JR_COL_LEAD_ZERO   'z'  /* Pad Leading Zeros             */

#define JR_COL_HINT_COMMA  'c'  /* decimal point is a comma            */
#define JR_COL_HINT_SLASH  's'  /* date separator -s a '-'             */
#define JR_COL_HINT_DASH   'd'  /* date separator -s a '/'  (default)  */
#define JR_COL_HINT_STR    '1'  /* date format YYYYMMDD                */
#define JR_COL_HINT_U_2    '2'  /* date format MM/DD/YY                */
#define JR_COL_HINT_E_2    '3'  /* date format DD/MM/YY                */
#define JR_COL_HINT_J_2    '4'  /* date format YY/MM/DD                */
#define JR_COL_HINT_U_4    '5'  /* date format MM/DD/YYYY              */
#define JR_COL_HINT_E_4    '6'  /* date format DD/MM/YYYY              */
#define JR_COL_HINT_J_4    '7'  /* date format YYYY/MM/DD              */

/*** structures ***/
struct s_file_info
  {
    FILE *fp;
    char *fname;
  } ;

struct s_global_hints
  {
    char decimal_point;
    char thousand_char;
    char date_fmt_char;
    int  date_format;
  } ;

struct s_column
{
  char col_type;
  int val_valid;       /* TRUE or FALSE                         */
  int truncated;       /* TRUE or FALSE                         */
  int first_time;      /* TRUE or FALSE, fixed min on empty val */
  long int col_begin;
  long int col_size;
  long int col_size_new;
  long int max_size;   /* real amount of memory allocated, col_size + FUDGE */
  char *col_value;
  char *col_min;
  char *col_max;
  char *col_fmt; 
  char *title;
  struct s_column *next_col;
} ;

struct s_work
  {
    int col_truncated;              /* TRUE or FALSE         */
    struct s_file_info out;         /* default stdout        */
    struct s_file_info err;         /* default stderr        */
    struct s_file_info fmt;         /* optional layout file  */
    struct s_file_info rpt;         /* Output Data Report    */
    char *prog_name;                /* real program name     */
    char *strip_chars;              /* Chracters to strip    */
    int num_files;                  /* # of files to process */
    int verbose;                    /* TRUE or FALSE         */
    int force;                      /* TRUE or FALSE         */
    int print_titles;               /* TRUE or FALSE         */
    int use_in_delim;               /* TRUE or FALSE         */
    int use_out_delim;              /* TRUE or FALSE         */
    char delim_in;
    char delim_out;
    long int total_writes;
    struct s_global_hints input_hints;
    struct s_global_hints output_hints;
  } ;

/*** messages and defines ***/
#define COL_LIST_NULL ((struct s_column *) NULL)
#define NUM_TRUNCATED 't' /* MUST not equal n or c */
#define RERR_LIT_01 "Numeric    "
#define RERR_LIT_02 "Character  "
#define RERR_LIT_03 "*BAD_TYPE* "
#define RERR_LIT_04 "*TRUNCATED*"
#define RERR_LIT_05 "*INVALID*  "
#define RERR_LIT_06 "Date       "

#define RERR_LIT_FMT_T "Trim Character"
#define RERR_LIT_FMT_F "Fix Number    "
#define RERR_LIT_FMT_D "Convert Date  "

#define JR_HINT_LIT_I "Global Input Data Hints:"
#define JR_HINT_LIT_O "Global Output Data Hints:"
#define JR_STRIP_O    "\nCharacters to strip if specified in format:\n    %s\n"

/*** prototypes ***/
void init(int, char **, struct s_work *, struct s_column *);
void init_finfo(struct s_file_info *);
int  show_brief_help(FILE *, char *);
int  show_rev(FILE *, char *);
void process_arg(int, char **, struct s_work *, struct s_column *);
int  open_out(FILE *, struct s_file_info *, char *, int);
void close_out(struct s_file_info *);
int  open_in(FILE **, char *, FILE *);
void close_in(FILE **, char *);
void lead_zero(char *, char, long int);

void init_col_list(struct s_column *);
void free_col_list(struct s_column *);
struct s_column *append_col_list(FILE *, struct s_column *, char, char *, char *);
int allocate_cols(struct s_column *);
struct s_column *find_col_last(struct s_column *);
void show_cols(FILE *, int, struct s_column *, int);
void fmt_col(FILE *, struct s_column *, char, char *, char *);
long int get_num(FILE *, char, char *, long, long, char *);
long int count_cols(struct s_column *);
long int load_col_file(FILE *, struct s_column *, char *, char, char *);
void show_cols_data(FILE *, char *, long int, int, struct s_column *, int, char *);
void parse_flat(struct s_column *, char *);
void parse_delim(struct s_column *, char *, char);
int  convert_value(struct s_work *, struct s_column *, long int);
void init_col_new(struct s_column *);
struct s_column *find_col_pos(struct s_column *, long int);
void write_col_delimited(FILE *, struct s_column *, char, long int *);
void write_col_fixed(FILE *, struct s_column *, long int *);
void init_hints(struct s_global_hints *);
void show_cols_data_hdr(FILE *, char *, long int, char *, int);
void show_col_info(FILE *, struct s_column *, int);
void clear_col_vals(struct s_column *);

#endif /*  JR_H  */
