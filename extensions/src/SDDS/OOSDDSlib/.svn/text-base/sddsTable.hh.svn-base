#if !defined(_SDDS_table_)

#define _SDDS_table_ 1

#include "SDDS.h"
#include "SDDStypes.h"
#include "sddsError.hh"
#include "sddsLayout.hh"
#include "sddsFileBuffer.hh"

class sddsTable {

public:
  sddsTable(void);
  ~sddsTable(void);

  long initializeInput(char *filename);
  long terminate(void);
  sddsError errorStack;

private:
  char filename[PATH_MAX];  
  FILE *fp;
  short popenUsed;
  
  sddsLayout layout;
  sddsLayout original_layout;
  sddsFileBuffer fBuffer;
  long page_number;
  
  long rowcount_offset;  /* ftell() position of row count */
  long n_rows_written;   /* number of tabular data rows written to disk */
  long last_row_written; /* virtual index of last row written */
  long first_row_in_mem; /* virtual index of first row in memory */
  short writing_page;    /* 1/0 for writing/not-writing page */
  long n_rows_allocated; /* number of tabular data rows for which space 
			    is alloc'd */
  long n_rows;           /* number of rows stored in memory */
  long *row_flag;        /* accept/reject flags for rows stored in memory */
  short file_had_data;   /* indicates that file being appended to already 
			    had some data in it (i.e., more than just a 
			    header.  Affects no_row_counts=1 output.
			    */
  
  long n_of_interest;
  long *column_order;  
  /* column_order[i] = internal index of user's ith column */
  long *column_flag;           
  /* column_flag[i] indicates whether internal ith column has been selected */
  
  /* array of SDDS_ARRAY structures for storing array data */
  sddsArray *array;
  
  /* array for parameter data.  The address of the data for the ith parameter
   * is parameter[i].  So *(<type-name> *)parameter[i] gives the data itself.
   * For type
   * SDDS_STRING the "data itself" is actually the address of the string, and
   * the type-name
   * is "char *".
   */
  void **parameter;
  
  /* array for tabular data.  The starting address for the data for the ith 
   * column
   * is data[i].  So ((<type-name> *)data[i])[j] gives the jth data value.  
   * For type SDDS_STRING the "data value" is actually the address of the 
   * string, and
   * the type-name is "char *".
   */
  void **data;
  
};

#endif
