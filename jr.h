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

#ifndef JLIB2_CHAR_NULL
#define NO_JLIB 1
#define JLIB2_INT_NULL   ((int) '\0')
#define JLIB2_CHAR_NULL  ((char) '\0')
#define JLIB2_UCHAR_NULL ((unsigned char) '\0' )
#endif

#ifndef NULL
#define NULL '\0'
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef _MSDOS
#define SNPRINTF _snprintf
#define SSIZE_T unsigned long int
#endif
#ifndef SNPRINTF
#define SNPRINTF snprintf
#endif

#ifndef SSIZE_T
#define SSIZE_T ssize_t
#endif
#ifndef TAB_CHAR
#define TAB_CHAR 0x09
#endif

#ifdef NO_JLIB
#define DATE_FMT_US_2        1  /*  string MM/DD/YY               */
#define DATE_FMT_US_4        2  /*  string MM/DD/YYYY             */
#define DATE_FMT_YYYY        3  /*  string YYYY/MM/DD             */
#define DATE_FMT_YY          4  /*  string YY/MM/DD               */
#define DATE_FMT_EU_2        5  /*  string DD/MM/YY               */
#define DATE_FMT_EU_4        6  /*  string DD/MM/YYYY             */
#define DATE_STR_YY          7  /*  string YYMMDD                 */
#define DATE_STR_YYYY        8  /*  string YYYYMMDD               */
#define DATE_SIZE_MAX       27  /* largest size of a date string              */
#define DATE_IN_FMT_E        1  /* in date format DD/MM/YY or DD/MM/YYYY      */
#define DATE_IN_FMT_U        2  /* in date format MM/DD/YY or MM/DD/YYYY      */
#define DATE_IN_FMT_J        3  /* in date format YY/MM/DD or YYYY/MM/DD      */
#define DATE_IN_FMT_NOT      4  /* in date not formatted, YYYYMMDD YYMMDD ... */
#define DATE_TIME_SIZE_STR  18  /* for YYYYMMDDHHmmSSLLL */
#define CENTURY_CURRENT     19
#define CENTURY_NEXT        20
#define CUTOFF_YY           90  /* if less than this year use CENTURY_21 */
#endif /* NO_JLIB */

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
#ifdef NO_JLIB
struct s_j2_datetime
{
  int month;    /* Month,           01 --12             */
  int dd;       /* Day of Month,    01 -- [28|29|30|31] */
  int yy;       /* two digit year,  00 -- 99            */
  int yyyy;     /* four digit year, 0000 -- 9999        */
  int hh;       /* Hour of the day, 00 -- 23            */
  int minutes;  /* minutes,         00 -- 59            */
  int ss;       /* seconds,         00 -- 59            */
  int mil;      /* milliseconds,    000 -- 999          */
  int tm_isdst; /* Daylight ?  0 = no, > 0 = yes        */
} ;
#endif /* NO_JLIB */

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

#ifdef NO_JLIB
#define ARG_COL                   'C'  /* Column to start with               */
#define ARG_DELM                  'd'  /* Field Delimiter                    */
#define ARG_DELM_OUT              'D'  /* Output Field Delimiter             */
#define ARG_ERR                   'e'  /* Output Error File                  */
#define ARG_FORCE                 'f'  /* force create files                 */
#define ARG_HELP                  'h'  /* Show Help                          */
#define ARG_I_GLOBAL_HINT         'g'  /* Input Global data hints            */
#define ARG_OUT                   'o'  /* Output File                        */
#define ARG_O_GLOBAL_HINT         'G'  /* Output Global data hints           */
#define ARG_PARM                  'P'  /* file to get parameters             */
#define ARG_REPORT                'r'  /* Report File                        */
#define ARG_STRIP_CHARS           's'  /* Remove these characters from string*/
#define ARG_TITLE                 't'  /* Report Title                       */
#define ARG_VERBOSE               'v'  /* Verbose                            */
#define ARG_VERSION               'V'  /* Show Version Information           */
#define DECIMAL_POINT             '.'
#define FILE_NAME_STDIN           "-"
#define FILE_NAME_STDOUT          "-"
#define LIT_HINT_DATE_FMT         "Date Format"
#define LIT_HINT_DATE_SEP         "Date Separator"
#define LIT_HINT_DECIMAL          "Numeric Decimal Point"
#define LIT_HINT_THOUSAND         "Numeric Thousand Separator"
#define LIT_INFO_01               "\tBuild: %s %s\n"
#define LIT_INFO_END              "******* End of Report *******"
#define LIT_NULL                  "<null>"
#define LIT_REV                   "Revision"
#define LIT_STDIN                 "(standard input)"
#define LIT_STDOUT                "(standard output)"
#define MSG_ERR_E000              "Try '%s %c%c' for more information\n"
#define MSG_ERR_E002              "ERROR E002: Cannot open '%s' for write, processing aborted\n"
#define MSG_ERR_E006              "ERROR E006: '%s' is an invalid value for %c%c\n"
#define MSG_ERR_E008              "ERROR E008: '%s' is an invalid value for %c%c, must be numeric\n"
#define MSG_ERR_E021              "ERROR E021: Cannot Open file\n"
#define MSG_ERR_E025              "ERROR E025: File %s cannot be created, already exists\n"
#define MSG_ERR_E035              "ERROR E035: Required Argument missing\n" /* see # 22 above */
#define MSG_ERR_E036              "ERROR E036: Cannot allocate memory for argument %c%c\n"
#define MSG_ERR_E037              "ERROR E037: Invalid File Name : %s\n"
#define MSG_ERR_E049              "ERROR E049: delimiter '%s' invalid\n"
#define MSG_ERR_E080              "ERROR E080: cannot allocate initial memory : %s\n"
#define MSG_INFO_I003             "I003: Reads %-9ld : Writes %-9ld : File: %s\n"
#define MSG_RPT_R01               "Column Statistics:\n"
#define MSG_RPT_R02               "Data for Record # %ld:\n"
#define MSG_RPT_R03               "    col   type        Pos   Size  New Size Min / Max\n"
#define MSG_RPT_R04               "    col   type        Pos   Size  New Size Data Value\n"
#define MSG_RPT_R05               "    ----- ----------- ----- ----- -------- ---------------------------------\n"
#define MSG_RPT_R06               "    %5d %s %5ld %5ld %8ld %s / %s\n"
#define MSG_RPT_R07               "    %5d %s %5ld %5ld %8ld %s\n"
#define MSG_RPT_R08               "Column Parameter Information:\n"
#define MSG_RPT_R09               "    Col Type    Format Mode    Field/Pos      Size  New Size Name\n"
#define MSG_RPT_R10               "    ----------- -------------- --------- --------- --------- ------------------\n"
#define MSG_RPT_R11               "    %-11s %-14s %9ld %9ld %9ld %s\n"
#define MSG_WARN_W002             "W002: Open Error Bypass File '%s' : %s\n"
#define MSG_WARN_W005             "W005: Warning, one of more cols truncated or invalid\n"
#define MSG_WARN_W006             "W006: Warning, Record %ld, Pos/Field %ld truncated, value: old '%s' new '%s'\n"
#define MSG_WARN_W007             "W007: Warning, Record %ld, Pos/Field %ld INVALID, value '%s'\n"
#define SWITCH_CHAR               '-'
#define THOUSAND_SEP              ','
#define USG_MSG_ARG_COL_CHAR_N1          "\t%c%c type,pos,size[,new_size][,format][,title]\n"
#define USG_MSG_ARG_COL_CHAR_N2          "\t\t\t: Column, type, location, size.  Type: 'c/d/n' for char,\n"
#define USG_MSG_ARG_COL_CHAR_N3          "\t\t\t  date, number.  pos can be either a column number or\n"
#define USG_MSG_ARG_COL_CHAR_N4          "\t\t\t  a field location, depending upon argument '%c%c'.\n"
#define USG_MSG_ARG_COL_CHAR_N5          "\t\t\t  More than one of these are allowed, argument order\n"
#define USG_MSG_ARG_COL_CHAR_N6          "\t\t\t  determines format of the Output File.\n"
#define USG_MSG_ARG_DELM_A               "\t\t\t  the decimal # representing the character.\n"
#define USG_MSG_ARG_DELM_B               "\t\t\t  Ex: 7 = '^G', 9 = TAB, 124 = '|' or just '|'.\n"
#define USG_MSG_ARG_DELM_I               "\t%c%c c\t\t: Input File Field Delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_O               "\t%c%c c\t\t: Output File Field Delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_Z               "\t\t\t  If not specified, assumes a flat, fixed length file.\n"
#define USG_MSG_ARG_ERR                  "\t%c%c file\t\t: Write errors to file 'file', default stderr\n"
#define USG_MSG_ARG_FORCE                "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_HELP                 "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_I_GLOBAL_HINT        "\t%c%c string\t: Input Format Hints\n"
#define USG_MSG_ARG_OUT                  "\t%c%c file\t\t: Write output to file 'file', default stdout\n"
#define USG_MSG_ARG_O_GLOBAL_HINT        "\t%c%c string\t: Output Format Hints\n"
#define USG_MSG_ARG_PARM                 "\t%c%c file\t\t: Obtain some Column parameters from this file\n"
#define USG_MSG_ARG_REPORT               "\t%c%c file\t\t: Name of the log report file\n"
#define USG_MSG_ARG_STRIP_CHARS          "\t%c%c string\t: Strip characters in 'string' from field\n"
#define USG_MSG_ARG_TITLE_1              "\t%c%c\t\t: Print Column Titles on Output (if titles exist)\n"
#define USG_MSG_ARG_VERBOSE_4            "\t%c%c\t\t: Show detail File Stats\n"
#define USG_MSG_ARG_VERSION              "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_OPTIONS                  "Options\n"
#define USG_MSG_USAGE                    "usage:\t%s [OPTIONS] [FILES ...]\n"
#endif /* NO_JLIB */

#ifndef DATE_SEPERATOR
#  define DATE_SEPERATOR '/'
#endif
#ifndef TIME_SEPERATOR
#  define TIME_SEPERATOR ':'
#endif
#ifndef NEG_SIGN
#define NEG_SIGN         '-'
#endif

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

#ifdef NO_JLIB
SSIZE_T j2_getline(char **buf, size_t *n, FILE *fp);
char *j2_get_prgname(char *argv_0, char *default_name);
int j2_bye_char(char *buffer, char c);
int j2_d_get_yyyy(int yy);
int j2_d_isleap(int year_yyyy);
int j2_date_split(struct s_j2_datetime *d, char *d_string, int fmt);
int j2_date_split_fmt(struct s_j2_datetime *d, char *dstr, char *delim, int date_type);
int j2_ds_fmt(char *date_format, char *date_s, int out_type, int fmt);
int j2_ds_split(struct s_j2_datetime *d, char *date_time, int fmt);
int j2_f_exist(char *file_name);
int j2_fix_numr(char *buffer, char thousand, char decimal_point);
int j2_is_all_spaces(char *s);
int j2_is_numr(char *s);
long int j2_bye_emb(char *s);
long int j2_bye_last(char *x, char lastc);
long int j2_chg_char(char old, char new, char *s, SSIZE_T force_size);
long int j2_clr_str(char *s, char c, int size);
long int j2_count_c(char c, char *s);
long int j2_fix_delm(char delm, char **fixed, char *buf);
long int j2_justleft(char *s);
long int j2_rtw(char *buffer);
void j2_d_init(struct s_j2_datetime *d);
void j2_strlwr(char *s);
void j2_strupr(char *s);
#define j2_bye_nl(x)    j2_bye_last((x), '\n')
#define j2_bye_ctlm(x)  j2_bye_last((x), '\r')
#endif /* NO_JLIB */

#endif /*  JR_H  */
