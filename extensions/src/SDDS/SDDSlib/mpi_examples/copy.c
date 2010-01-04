#include "SDDS.h"
#include "mdb.h"
#include "scan.h"

#define INITIAL_BIG_BUFFER_SIZE SDDS_MAXLINE

#define SET_COLUMN_ORDER 0
#define SET_COLLECTIVE_IO 1
#define N_OPTIONS 2
char *option[N_OPTIONS]={"columnOrder", "collectiveIO"};

static char *USAGE="copy <inputFile> <outputFile> [-columnOrder] [-collectiveIO]";

int main(int argc, char *argv[]) {
  char *filename=NULL; 
  char *outputFile=NULL;
  SDDS_DATASET SDDS_in, SDDS_out;
  MPI_DATASET *MPI_in, *MPI_out;
  int  mpi_code, page=0, n_processors, myid, collective_io=0;
  int32_t rows, columnOrder=0, i_arg; 
  double startTime = 0;
  MPI_Comm comm;
  SCANNED_ARG *s_arg;
 
  MPI_Init(&argc,&argv);
  /* 1. set the communicator for MPI IO */
  argc = scanargs(&s_arg, argc, argv); 
  if (argc<3)
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
      default:
        break;
      }
    } else if (!filename)
      filename = s_arg[i_arg].list[0];
    else if (!outputFile)
      outputFile = s_arg[i_arg].list[0];
    else
      SDDS_Bomb("Too many files given.");
  }
  SDDS_SetDefaultBufferSize(4000000);

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
    fprintf(stderr,"inputfile=%s, copy file=%s\n", filename, outputFile);
  /* 2. open file (all processors) */
  fprintf(stderr, "columnorder %d\n", columnOrder);
  
  if (!SDDS_MPI_InitializeInput(&SDDS_in, filename)) 
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  fprintf(stderr, "initialized input.\n");
  MPI_in->collective_io = collective_io;
  MPI_out->collective_io = collective_io;
  
  /*write to another file for comparison*/
  if (!SDDS_MPI_InitializeCopy(&SDDS_out, &SDDS_in, outputFile, columnOrder)) {
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
