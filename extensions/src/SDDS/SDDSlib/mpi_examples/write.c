#include "SDDS.h"
#include "mdb.h"
#include "scan.h"

#define SET_TOTAL_ROWS 0
#define SET_COLUMN_ORDER 1
#define SET_COLLECTIVE_IO 2
#define N_OPTIONS 3
char *option[N_OPTIONS]={"totalRows", "columnOrder", "collectiveIO"};
static char *USAGE="write <filename> [-totalRows=<value>] [-columnOrder] [-collectiveIO]";

int main(int argc, char *argv[]) {
  char *filename=NULL; 
  MPI_DATASET *MPI_dataset;
  SDDS_DATASET SDDS_dataset;
  char buf[256];
  int32_t total_rows=37500000, i, j, mpi_code, cols=8, rows, columnOrder=0, ID_offset, myid, n_processors, collective_io=0, i_arg;
  double startTime =0;
  MPI_Comm comm;
  SCANNED_ARG *s_arg;

  MPI_Init(&argc,&argv);
  argc = scanargs(&s_arg, argc, argv); 
  if (argc<2)
    bomb(NULL, USAGE);
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_COLUMN_ORDER:
        columnOrder = 1;
        break;
      case SET_COLLECTIVE_IO:
        collective_io = 1;
        break;
      case SET_TOTAL_ROWS:
        if (s_arg[i_arg].n_items!=2)
          SDDS_Bomb("Invalid -totalrows syntax given!");
        if (!get_long(&total_rows, s_arg[i_arg].list[1]))
          SDDS_Bomb("Invalid -totalrows value given!");
        break;
      default:
        break;
      }
    } else if (!filename)
      filename = s_arg[i_arg].list[0];
    else 
      SDDS_Bomb("Too many files.");
  }
 
  /* 1. set the communicator for MPI IO */
  SDDS_SetDefaultBufferSize(4000000);

  comm = MPI_COMM_WORLD;
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_rank(comm, &myid)))
    SDDS_MPI_GOTO_ERROR(stderr,  "MPI_Comm_rank failed", mpi_code, 1);
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_size(comm, &n_processors)))
    SDDS_MPI_GOTO_ERROR(stderr, "MPI_Comm_size failed", mpi_code, 1);
  SDDS_MPI_Setup(&SDDS_dataset, 1,  n_processors, myid, comm, 1);
  MPI_dataset = SDDS_dataset.MPI_dataset;
 
  /* 2. open file (all processors) */
  
  /* 3. initialize output, right now, only write binary files (all processors) */
  
  fprintf(stderr, "rank=%d, initialize input \n", myid);
  if (!SDDS_MPI_InitializeOutput(&SDDS_dataset, NULL, NULL, filename, SDDS_MPI_WRITE_ONLY, columnOrder))
    SDDS_Bomb("Unable to initalize output");
  MPI_dataset->collective_io = collective_io;
  for (i=0; i<cols; i++) {
    sprintf(buf,"col%d", i);
    if ( !SDDS_DefineSimpleColumn(&SDDS_dataset, buf, NULL, SDDS_DOUBLE))
      SDDS_MPI_BOMB("SDDS define column and parameters  failed.", &(MPI_dataset->MPI_file));
  }
  if (!SDDS_SaveLayout(&SDDS_dataset))
    SDDS_MPI_BOMB("SDDS define column and parameters  failed.", &(MPI_dataset->MPI_file));
  fprintf(stderr, "rank=%d, layout defined. \n", myid);
  
  startTime = MPI_Wtime();
  if (!SDDS_WriteLayout(&SDDS_dataset))  
    SDDS_MPI_BOMB("SDDS_MPI_WriteLayout failed.", &(MPI_dataset->MPI_file));
  fprintf(stderr, "rank=%d, total_rows=%d, layout written spent %f seconds. \n", myid, total_rows, MPI_Wtime() - startTime);
  
  rows = total_rows/n_processors;
  if (myid < (ID_offset=total_rows%(n_processors))) {
      rows ++;
  }
  if (!SDDS_StartPage(&SDDS_dataset, rows))
    SDDS_MPI_BOMB("SDDS start page  failed.", &MPI_dataset->MPI_file);  
    /* 7. set data values */
  for (i=0; i<rows; i++) {
    for (j=0; j<cols; j++) {
      sprintf(buf,"col%d", j);
      if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i,
                             buf, (double)i, NULL))
	SDDS_MPI_BOMB("SDDS_SetRowValues failed.", &MPI_dataset->MPI_file);
    }
  }
  fprintf(stderr, "rank=%d, row set. \n", myid);
  startTime = MPI_Wtime();
  if (!SDDS_WritePage(&SDDS_dataset))
    SDDS_MPI_BOMB("SDDS_MPI_WritePage failed.", &MPI_dataset->MPI_file);
  fprintf(stderr, "rank=%d, page write spent %f. \n", myid, MPI_Wtime() - startTime);
  
  if (!SDDS_Terminate(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);  
  /* close file is done inside SDDS_MPI_Terminate*/
  MPI_Finalize();
  return 0;
}
