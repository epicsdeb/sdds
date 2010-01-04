#include "SDDS.h"
#include "mdb.h"

#define INITIAL_BIG_BUFFER_SIZE SDDS_MAXLINE

int main(int argc, char *argv[]) {
  char *filename; 
  SDDS_DATASET SDDS_dataset; 
  MPI_DATASET *MPI_dataset; 
  int  mpi_code, n_processors, myid;
  int32_t rows=0;
  double startTime;
  MPI_Comm comm;
  
  filename = argv[1];
  MPI_Init(&argc,&argv);
  
  /* 1. set the communicator for MPI IO */
  comm = MPI_COMM_WORLD; 
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_rank(comm, &myid)))
    SDDS_MPI_GOTO_ERROR(stderr,  "MPI_Comm_rank failed", mpi_code, 1);
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_size(comm, &n_processors)))
    SDDS_MPI_GOTO_ERROR(stderr, "MPI_Comm_size failed", mpi_code, 1); 
  SDDS_MPI_Setup(&SDDS_dataset, 1, n_processors, myid, comm, 1);
  MPI_dataset = SDDS_dataset.MPI_dataset;
  if (MPI_dataset->myid==0)
    fprintf(stderr,"filename=%s\n", filename);
  startTime = MPI_Wtime();
 
  /* 2. open file (all processors) */
  if (!SDDS_MPI_InitializeInput(&SDDS_dataset, filename)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  rows =  0;
  while (1) {
    rows=SDDS_ReadPage(&SDDS_dataset);
    fprintf(stderr, "read page=%d\n", rows);
    if (rows==0) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    } else if (rows==-1)
      break;
    else {
      rows =  SDDS_CountRowsOfInterest(&SDDS_dataset);
      fprintf(stderr, "rank=%d, rows=%d\n", MPI_dataset->myid, rows);
      fprintf(stderr, "total rows %d\n", MPI_dataset->total_rows);
    }
  }
  if (!SDDS_Terminate(&SDDS_dataset) )
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);

  if (MPI_dataset->myid==0)
    fprintf(stderr,"rank=%d, time spent=%f\n", MPI_dataset->myid, MPI_Wtime()-startTime);
  MPI_Finalize();
  return 0;
}
