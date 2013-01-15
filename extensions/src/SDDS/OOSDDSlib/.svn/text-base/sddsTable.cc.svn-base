
#include "sddsTable.hh"
#include <string.h>
#include "mdb.h"
#include "match_string.h"
#include "SDDS_internal.h"
#include "namelist.h"
#include "scan.h"

sddsTable::sddsTable(void) 
  :layout(errorStack), original_layout(errorStack)
{
  // layout and original_layout initialized by their constructors

  printf("sddsTable: construct\n");
  filename[0] = '\0';
  fp = NULL;
  popenUsed = 0;
  page_number = 0;
  rowcount_offset = 0;
  n_rows_written = 0;
  last_row_written = 0;
  first_row_in_mem = 0;
  writing_page = 0;
  n_rows_allocated = 0;
  n_rows = 0;
  row_flag = NULL;
  file_had_data = 0;

  n_of_interest = 0;
  column_order = NULL;
  column_flag = NULL;
  array = NULL;
  parameter = NULL;
  data = NULL;
}

sddsTable::~sddsTable(void) {
  terminate();
  printf("sddsTable: destruct\n");
}

long 
sddsTable::initializeInput(char *userFilename) {
  char s[SDDS_MAXLINE];
  if (!userFilename) {
    filename[0] = '\0';
  } else { 
    strncpy(filename, userFilename, PATH_MAX);
    filename[PATH_MAX-1] = '\0';
  }
  popenUsed = 0;
  if (!userFilename) {
    fp = stdin;
  } else if (!(fp = UnpackFopen(userFilename,
				UNPACK_REQUIRE_SDDS|UNPACK_USE_PIPE, 
				&popenUsed, NULL))) {
    sprintf(s,"Unable to open file %s for reading (SDDS_InitializeInput)", 
	    userFilename);
    errorStack.SDDS_SetError(s);
    return 0;
  }
  page_number = 0;
  file_had_data = 0;
  layout.SDDS_DeferSavingLayout(1);
  if (!layout.SDDS_ReadLayout(fp)) {
    fclose(fp);
    return(0);
  }

  return 1;
}

long
sddsTable::terminate(void) {

  if (fp && strlen(filename)) {
    fclose(fp);
  }
  return 1;

}
