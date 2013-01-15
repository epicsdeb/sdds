#include "SDDS.h"

int main(int argc, char *argv[]) {
  char *filename="test.sdds"; 
  SDDS_DATASET SDDS_dataset;
  MPI_DATASET *MPI_dataset;
  char char_data='a';
  static char *str_data[]={"test1","tttaakkdlsglasg","taskldglaglakeglkgnaslglakghh",
			   "kaetasdkllsahlah;ksdhkalskh;hkq;asdklhalh","dag","","","gadsklskglkagdlkshla",};
  
  int32_t rows, i, length, j, mpi_code, n_processors, myid;
  MPI_Comm comm;

  MPI_Init(&argc,&argv);
  /* 1. set the communicator for MPI IO */
  comm = MPI_COMM_WORLD;
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_rank(comm, &myid)))
    SDDS_MPI_GOTO_ERROR(stderr,  "MPI_Comm_rank failed", mpi_code, 1);
  if (MPI_SUCCESS != (mpi_code=MPI_Comm_size(comm, &n_processors)))
    SDDS_MPI_GOTO_ERROR(stderr, "MPI_Comm_size failed", mpi_code, 1); 
  SDDS_MPI_Setup(&SDDS_dataset, 1, n_processors, myid, comm, 1);
  MPI_dataset = SDDS_dataset.MPI_dataset;
  
  /* 3. initialize output, right now, only write binary files (all processors) */
  if (!SDDS_MPI_InitializeOutput(&SDDS_dataset, NULL, NULL, filename, SDDS_MPI_WRITE_ONLY, 0))
    SDDS_MPI_BOMB("SDDS_MPI_InitializeOutput failed.", &MPI_dataset->MPI_file); 
  fprintf(stderr, "rank=%d, initialize input \n", MPI_dataset->myid);
  /* 4. define parameters, arrays and columns (all processors) -- same as SDDS calls */
  if (!SDDS_DefineSimpleColumn(&SDDS_dataset, "ShortCol", NULL, SDDS_SHORT) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "LongCol", NULL, SDDS_LONG) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "UShortCol", NULL, SDDS_USHORT) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "ULongCol", NULL, SDDS_ULONG) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "FloatCol", NULL, SDDS_FLOAT) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "DoubleCol", NULL, SDDS_DOUBLE) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "StringCol", NULL, SDDS_STRING) ||
      !SDDS_DefineSimpleColumn(&SDDS_dataset, "CharCol", NULL, SDDS_CHARACTER) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "ShortPar", NULL, SDDS_SHORT) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "LongPar", NULL, SDDS_LONG) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "UShortPar", NULL, SDDS_USHORT) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "ULongPar", NULL, SDDS_ULONG) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "FloatPar", NULL, SDDS_FLOAT) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "DoublePar", NULL, SDDS_DOUBLE) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "StringPar", NULL, SDDS_STRING) ||
      !SDDS_DefineSimpleParameter(&SDDS_dataset, "CharPar", NULL, SDDS_CHARACTER))
    SDDS_MPI_BOMB("SDDS define column and parameters  failed.", &MPI_dataset->MPI_file);  
  fprintf(stderr, "rank=%d, layout defined. \n", MPI_dataset->myid);
  /* 5. write layout (SDDS header), only master processor write */
  if (SDDS_dataset.MPI_dataset!=NULL)
    fprintf(stderr, "MPI_dataset is ok. id=%d\n", SDDS_dataset.MPI_dataset->myid);
  
  if (!SDDS_WriteLayout(&SDDS_dataset))  
    SDDS_MPI_BOMB("SDDS_MPI_WriteLayout failed.", &MPI_dataset->MPI_file); 
  fprintf(stderr, "rank=%d, layout written. \n", MPI_dataset->myid);
  /*assume different processors contains different rows of the file, 
    processor 0 contains 0, 1, rows -1 of file, processor 1 contains rows, rows+1, ..., 2rows-1 of file, and so on*/
 
  for (j=0; j<3; j++) {
    rows = MPI_dataset->myid + 10 +j; 
    /* 6. start page (all processors), allocated memory for holding data */
    if (MPI_dataset->myid==0)
      rows = 0;
    if (!SDDS_StartPage(&SDDS_dataset, rows))
      SDDS_MPI_BOMB("SDDS start page  failed.", &MPI_dataset->MPI_file);  
    /* 7. set data values */
    if (!SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE,
                            "ShortPar", 1, 
                            "UShortPar", 2, 
                            "LongPar", 3,
                            "ULongPar", 4,
                            "FloatPar", 5.0,
                            "DoublePar", 6.0, "StringPar", "testgaghh",
                            "CharPar", 'c', NULL))
      SDDS_MPI_BOMB("SDDS_SetParameters failed.", &MPI_dataset->MPI_file); 
    fprintf(stderr, "rank=%d, par set. \n", MPI_dataset->myid);
    fprintf(stderr, "string par=%s\n", *((char **)SDDS_dataset.parameter[6]));
    for (i=0; i<rows; i++) {
      length =i+10;
      char_data = i%20;
      if (!SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME|SDDS_PASS_BY_VALUE, i,
                             "ShortCol", length, "UShortCol", length, "LongCol", length,
                             "ULongCol", length, "FloatCol", (float)length, "DoubleCol", (double)length,
                             "StringCol", str_data[i%6], "CharCol", char_data, NULL))
          SDDS_MPI_BOMB("SDDS_SetRowValues failed.", &MPI_dataset->MPI_file);
    } 
    fprintf(stderr, "rank=%d, row set. \n", MPI_dataset->myid);
    if (!SDDS_WritePage(&SDDS_dataset) || !SDDS_ShortenTable(&SDDS_dataset,1))
      SDDS_MPI_BOMB("SDDS_MPI_WritePage failed.", &MPI_dataset->MPI_file);
    fprintf(stderr, "rank=%d, page write. \n", MPI_dataset->myid);
  }
  if (!SDDS_Terminate(&SDDS_dataset))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);  
  /* close file */
  MPI_Finalize();
  return 0;
}
