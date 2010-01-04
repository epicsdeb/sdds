#include "SDDS.h"
#include "mdb.h"

#define INITIAL_BIG_BUFFER_SIZE SDDS_MAXLINE

int main(int argc, char *argv[]) {
  char *filename=NULL; 
  char *outFile=NULL;
  SDDS_DATASET SDDS_in, SDDS_out;
  MPI_DATASET *MPI_in, *MPI_out;
  int  mpi_code, page=0, n_processors, myid;
  int32_t rows, columnOrder=0; 
  double startTime = 0;
  MPI_Comm comm;

  filename = argv[1];
  outFile =argv[2];
  MPI_Init(&argc,&argv);
  if (argc<3)
    SDDS_Bomb("Input and output files have to be provided.");
  
  /* 1. set the communicator for MPI IO */
 
  comm =  MPI_COMM_WORLD;

  if (MPI_SUCCESS != (mpi_code=MPI_Comm_rank(comm, &myid)))
    SDDS_MPI_GOTO_ERROR(stderr,  "MPI_Comm_rank failed", mpi_code, 1);
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_size(comm, &n_processors)))
    SDDS_MPI_GOTO_ERROR(stderr, "MPI_Comm_size failed", mpi_code, 1); 
 
  SDDS_MPI_Setup(&SDDS_in, 1, n_processors, myid, comm, 1);
  SDDS_MPI_Setup(&SDDS_out, 1, n_processors, myid, comm, 1);
  MPI_in = SDDS_in.MPI_dataset;
  MPI_out = SDDS_out.MPI_dataset;
  
  startTime = MPI_Wtime();
  if (MPI_in->myid==0)
    fprintf(stderr,"inputfile=%s, copy file=%s\n", filename, outFile);
  /* 2. open file (all processors) */
  fprintf(stderr, "columnorder %d\n", columnOrder);
  
  if (!SDDS_MPI_InitializeInput(&SDDS_in, filename)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  fprintf(stderr, "initialized input.\n");
  
  /*write to another file for comparison*/
  if (!SDDS_MPI_InitializeCopy(&SDDS_out, &SDDS_in, outFile, columnOrder)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    return 0;
  }
  
  while (SDDS_ReadPage(&SDDS_in)>0) {
    page++;
    rows = SDDS_CountRowsOfInterest(&SDDS_in); 
    if (!SDDS_CopyPage(&SDDS_out, &SDDS_in)) 
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
    if (!SDDS_WritePage(&SDDS_out))
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (!SDDS_Terminate(&SDDS_in) || !SDDS_Terminate(&SDDS_out))
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  
  if (MPI_in->myid==0)
    fprintf(stderr,"Time spent=%f\n", MPI_Wtime()-startTime);
  MPI_Finalize();
  return 0;
}
