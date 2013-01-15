/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* file: awe.h
 * purpose: define structures for use with awe data format 
 * (formerly dpp.h)
 *
 * Michael Borland, 1988
 $Log: not supported by cvs2svn $
 Revision 1.2  1995/09/05 21:14:50  saunders
 First test release of the SDDS1.5 package.

 */

#ifndef _AWE_
#define _AWE_ 1
#define DOUBLE_PRECISION ((int)0)
#define SINGLE_PRECISION ((int)1)
#define AWE_VERSION 2

#include "linked_list.h"

typedef struct {
    long version;
    char *run_identifier;
    long n_quantities;
    char **quantity_name;
    char **quantity_unit;
    char **quantity_definition;
    long *memory_number;
    long n_defined_quantities;
    long *quantity_precision;
    long point_length;
    char *time_name;
    char *time_unit;
    long time_precision;
    long n_auxiliary_quantities;
    char **auxiliary;
    char **auxiliary_unit;
    long *auxiliary_precision;
    long *auxiliary_memory_number;
    } DUMP_SPECS;

typedef struct {
    long number;
    double time;
    long n_points;
    char *dump_label;
    double **point;
    char **pt_label;
    double *auxiliary_value;
    } DUMP;


/* define FILTERS structure, which applies to an individual pair or sequence
 * being extracted, and contains information on all the filters to be 
 * applied to it.
 */
struct FILTERS {
    long n_filters;                        /* number of filters to apply */
    long *quantity_to_filter;              /* numbers of quantities to filter */
    double *lower_limit, *upper_limit;    /* windows for these quantities */
    } ;

/* define FILTER_SPEC structure, which contains the specification of a
 * single filter, which may be applied to several output pairs or sequences.
 * Command-line input is parsed into structures of this type first, then
 * put in FILTERS structures, after other all command-line input has been
 * parsed.
 */

struct FILTER_SPEC {
    long n_outputs_to_filter;
    long *output_to_filter;
    char *quantity_to_filter;
    long quantity_number;
    double lower_limit, upper_limit;
    } ;

typedef struct {
    char **string;
    long n_strings;
    long logical_operation;
    } MATCH_EXPR;
/* possible values for logical_operation in MATCH_EXPR */
#define OR_OPERATION    0
#define AND_OPERATION   1
#define NOR_OPERATION   2
#define NAND_OPERATION  3
#define XOR_OPERATION   4
#define EQUIV_OPERATION 5

extern char *AWE_USAGE;

/* flag bit masks */
#define FLAG_VERBOSE              0x00000001
#define FLAG_COMBINE_DUMPS        0x00000002
#define FLAG_SIMPLE_MAKE_GRID     0x00000004
#define FLAG_INVERT_MATCH_PTLABEL 0x00000008
#define FLAG_INVERT_MATCH_DPLABEL 0x00000010
#define FLAG_FIND_ENVELOPES       0x00000020
#define FLAG_ANNOUNCE_OPENINGS    0x00000040

#define PROCESS_DUMP_NOPROCESS 0
#define PROCESS_DUMP_MEAN 1
#define PROCESS_DUMP_RMS 2
#define PROCESS_DUMP_SUM 3
#define PROCESS_DUMP_STAND_DEV 4
#define PROCESS_DUMP_MAD 5
#define PROCESS_DUMP_MEDIAN 6
#define PROCESS_DUMP_MINIMUM 7
#define PROCESS_DUMP_MAXIMUM 8
#define PROCESS_DUMP_SMALLEST 9
#define PROCESS_DUMP_LARGEST 10
#define PROCESS_DUMP_FIRST 11
#define PROCESS_DUMP_LAST 12
#define PROCESS_DUMP_COUNT 13
#define PROCESS_DUMP_CENTER 14
#define PROCESS_DUMP_FRACDEVS 15
#define PROCESS_DUMP_NORMALIZE 16
#define PROCESS_DUMP_PEAKFIND 17
#define PROCESS_DUMP_SPREAD 18
#define N_PROCESS_DUMP_MODES 19
extern char *process_dump_mode[N_PROCESS_DUMP_MODES];
extern char *process_dump_name[N_PROCESS_DUMP_MODES];
long process_data(long mode, double *data, long n_data);
long process_data_pairs(long xmode, double *xdata, long ymode, double *ydata, long n_data);

int set_up_awe_dump(FILE *fpo, char *label, DUMP_SPECS *specs,  DUMP *dump, int precision,
    char **quantity_name, char **quantity_unit, int n_quantities, char *indep_name, char *indep_unit, 
    char **auxiliary_name, char **auxiliary_unit, int n_auxiliaries);
long read_dump(DUMP *dump, DUMP_SPECS *dump_specs, FILE *fp);
long read_dump_s(DUMP *dump, DUMP_SPECS *dump_specs, FILE *fp,
    long n_dumps,  long *dump_num);
void free_dump(DUMP *dump, DUMP_SPECS *dump_specs);
long read_dump_specs(DUMP_SPECS *dump_specs, FILE *fp);
void free_dump_specs(DUMP_SPECS *dump_specs);
long write_dump_header(DUMP *dump, DUMP_SPECS *dump_specs, FILE *fp);
long write_dump(DUMP *dump, DUMP_SPECS *dump_specs, FILE *fp);
long write_dump_point(DUMP_SPECS *dump_specs, double *point, char *label, FILE *fp);
long write_dump_specs(DUMP_SPECS *dump_specs, FILE *fp);
long is_in_list(long number, long *list, long n_list);

char *nread_string(FILE *fp, long n);
char *read_string(FILE *fp);
int write_string(FILE *fp, char *s);
int read_int(FILE *fp, int *data, int n_data);
int read_long(FILE *fp, long *data, int n_data);
int read_float(FILE *fp, float *data, int n_data);
int read_double(FILE *fp, double *data, int n_data);
int write_int(FILE *fp, int *data, int n_data);
int write_long(FILE *fp, long *data, int n_data);
int write_float(FILE *fp, float *data, int n_data);
int write_double(FILE *fp, double *data, int n_data);

/*
void do_table_processing(char *table_file, char **quantity, long n_quantities, 
    struct FILTERS *filter, char *match_ptlabel, long sort_mode, char *sort_by, long n_to_keep_after_sort, 
    long sample_interval, long default_number, char *input, FILE *fpin, DUMP_SPECS *dump_specs, 
    long n_dumps, long *dump_num, char *match_dplabel, char *format, FILE *fp_log, long flags);
void do_pair_processing(char **first_quantity, char **second_quantity, long n_pairs, long do_average, 
    long do_separate_dumps, struct FILTERS *filter, char *match_ptlabel, 
    long sort_mode, char *sort_by, long n_to_keep_after_sort, long sample_interval,
    char *input, FILE *fpin, DUMP_SPECS *dump_specs, long n_dumps, long *dump_num, char *match_dplabel,
    char *output_root, long default_number, char *format, FILE *fp_log, long flags);
 */

void do_table_processing(char *table_file, char **quantity, long n_quantities, 
    struct FILTERS *filter, MATCH_EXPR *match_ptlabel, long sort_mode, char *sort_by, long n_to_keep_after_sort, 
    long sample_interval, long default_number, char *input, FILE *fpin, DUMP_SPECS *dump_specs, 
    long n_dumps, long *dump_num, MATCH_EXPR *match_dplabel, char *format, FILE *fp_log, long flags);
void do_pair_processing(char **first_quantity, char **second_quantity, long n_pairs, long do_process, long do_process1,
    long do_separate_dumps, struct FILTERS *filter, MATCH_EXPR *match_ptlabel, long *pt_list, long n_pt_list,
    long sort_mode, char *sort_by, long n_to_keep_after_sort, long sample_interval,
    char *input, FILE *fpin, DUMP_SPECS *dump_specs, long n_dumps, long *dump_num, MATCH_EXPR *match_dplabel,
    char *output_root, long default_number, char *format, FILE *fp_log, long flags);
void do_sequence_processing(char **quantity, char **indep_quantity, long n_quantities, long do_average, 
    struct FILTERS *filter, MATCH_EXPR *match_ptlabel, long *pt_list, long n_pt_list,
    long sort_mode, char *sort_by, long n_to_keep_after_sort, long sample_interval, char *input, FILE *fpin,
    DUMP_SPECS *dump_specs, long n_dumps, long *dump_num, MATCH_EXPR *match_dplabel, char *output_root, long default_number,
    char *format, FILE *fp_log, long flags);
void do_contour_processing(char **quantity, long n_quantities, long n_contours,
    char **first_quantity , double x1_min, double x1_max, long n1_grid, 
    char **second_quantity, double x2_min, double x2_max, long n2_grid,
    char *input, FILE *fpin, DUMP_SPECS *dump_specs, long n_dumps, long *dump_num, MATCH_EXPR *match_dplabel,
    char *mask_file, char *output_root, FILE *fp_log, long flags);
long make_grid(double ***z_grid, double x1_min, double dx1, double x2_min,
    double dx2, long nx1, long nx2, double *x1, double *x2, double *z, long np, 
    long flags, double minimum_allowed, double maximum_allowed);

char *translate_name(char *name, char *file);
long get_number_list(long **array, char **item, long n_items);
void setup_filters(struct FILTERS *filter, int n_outputs, struct node *filter_spec_ll,
    int n_filter_specs, DUMP_SPECS *dump_specs, FILE *fp_log, char **first_quantity, 
    char **second_quantity, char **quantity, int do_sequence, int do_table);
void print_dump_info(FILE *fp, DUMP *dump, DUMP_SPECS *dump_specs);
int passes_filters(double *quantity, struct FILTERS *filter);
long passes_label_match(char *match_label, char **pt_label, long ipt, long invert);
long expression_match(MATCH_EXPR *match, char *label);
long passes_labels_match(MATCH_EXPR *match, char **pt_label, long ipt);

#include "rpn.h"

#endif  /* _AWE_ */

