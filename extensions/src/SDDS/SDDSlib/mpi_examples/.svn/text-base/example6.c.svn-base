#include "SDDS.h"

int main(int argc, char *argv[]) {
  char *filename="test.sdds"; 
  MPI_DATASET *MPI_dataset;
  SDDS_DATASET SDDS_dataset;
  char buf[256];
  int32_t total_rows=37500000, i, j, mpi_code, cols=8, rows, columnOrder=0, ID_offset, myid, n_processors;
  double startTime =0;
  MPI_Comm comm;

  MPI_Init(&argc,&argv);
  filename = argv[1];
  if (argc>2)
      sscanf(argv[2], "%d", &total_rows);
  if (argc>3)
      sscanf(argv[3], "%d", &columnOrder);
  fprintf(stderr,"arg1=%s, arg2=%s, arg3=%s\n", argv[1], argv[2], argv[3]);
 
  /* 1. set the communicator for MPI IO */
  SDDS_SetDefaultIOBufferSize(4000000);

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
