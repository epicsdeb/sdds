/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/
/*
 * Programmer:  Hairong Shang
 * Purpose:	This is the MPI-2 I/O binary driver.
 *
 */
#include "SDDS.h"

static int32_t defaultStringLength = SDDS_MPI_STRING_COLUMN_LEN;
static int32_t number_of_string_truncated = 0;
static int32_t defaultTitleBufferSize = 2400000; 
static int32_t defaultBufferSize = 4000000;

int32_t SDDS_SetDefaultBufferSize(int32_t newSize)
{
  int32_t previous;
  if (newSize<=0)
    return defaultBufferSize;
  previous = defaultBufferSize;
  defaultBufferSize = newSize;
  return previous;
}

int32_t SDDS_SetDefaultTitleBufferSize(int32_t newSize)
{
  int32_t previous;
  if (newSize<=0)
    return defaultTitleBufferSize;
  previous = defaultTitleBufferSize;
  defaultTitleBufferSize = newSize;
  return previous;
}

int32_t SDDS_CheckStringTruncated(void) 
{
  return number_of_string_truncated;
}
void SDDS_StringTuncated(void)
{
  number_of_string_truncated++;
}
int32_t SDDS_SetDefaultStringLength(int32_t newValue)
{
  int32_t previous;
  if (newValue<0)
    return defaultStringLength;
  previous = defaultStringLength;
  defaultStringLength = newValue;
  return previous;
}


int32_t SDDS_MPI_WriteBinaryPage(SDDS_DATASET *SDDS_dataset)
{
  return SDDS_MPI_WriteContinuousBinaryPage(SDDS_dataset); 
}

int32_t SDDS_MPI_WriteBinaryString(SDDS_DATASET *SDDS_dataset, char *string)
{ 
  static char *dummy_string ="";
  int32_t length;
  
  if (!string) 
    string = dummy_string;
  
  length = strlen(string);
  if (!SDDS_MPI_BufferedWrite(&length, sizeof(length), SDDS_dataset))
    return 0;
  if (length && !SDDS_MPI_BufferedWrite(string, sizeof(*string)*length, SDDS_dataset))
    return 0;
  return 1;
}

int32_t SDDS_MPI_WriteNonNativeBinaryString(SDDS_DATASET *SDDS_dataset, char *string)
{ 
  static char *dummy_string ="";
  int32_t length;
  
  if (!string) 
    string = dummy_string;
  
  length = strlen(string);
  SDDS_SwapLong(&length);
  if (!SDDS_MPI_BufferedWrite(&length, sizeof(length), SDDS_dataset))
    return 0;
  SDDS_SwapLong(&length);
  if (length && !SDDS_MPI_BufferedWrite(string, sizeof(*string)*length, SDDS_dataset))
    return 0;
  return 1;
}

int32_t SDDS_MPI_WriteBinaryParameters(SDDS_DATASET *SDDS_dataset) {
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout; 
  int32_t i;

  MPI_dataset = SDDS_dataset->MPI_dataset;
  /*should only master processors write SDDS hearder,parameters and arrays */
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      continue; 
    if (layout->parameter_definition[i].type==SDDS_STRING) { 
      if (!SDDS_MPI_WriteBinaryString(SDDS_dataset, *(char**)SDDS_dataset->parameter[i]))
	return 0;
    } else {
      if (!SDDS_MPI_BufferedWrite(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], SDDS_dataset))
	return 0; 
    }
  }
  return(1);
}

int32_t SDDS_MPI_WriteNonNativeBinaryParameters(SDDS_DATASET *SDDS_dataset) {
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout; 
  int32_t i;

  MPI_dataset = SDDS_dataset->MPI_dataset;
  /*should only master processors write SDDS hearder,parameters and arrays */
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  SDDS_SwapEndsParameterData(SDDS_dataset);
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      continue; 
    if (layout->parameter_definition[i].type==SDDS_STRING) { 
      if (!SDDS_MPI_WriteNonNativeBinaryString(SDDS_dataset, *(char**)SDDS_dataset->parameter[i])) {
        SDDS_SwapEndsParameterData(SDDS_dataset);
	return 0;
      }
    } else {
      if (!SDDS_MPI_BufferedWrite(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], SDDS_dataset)) {
        SDDS_SwapEndsParameterData(SDDS_dataset);
	return 0; 
      }
    }
  }
  SDDS_SwapEndsParameterData(SDDS_dataset);
  return(1);
}
int32_t SDDS_MPI_WriteBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, zero=0, writeSize=0;
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  
  /*only the master processor write SDDS header, parameters and arrays*/
  MPI_dataset = SDDS_dataset->MPI_dataset;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryArray"))
    return(0);
  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_arrays; i++) {
    if (!SDDS_dataset->array[i].dimension) {
      for (j=0; j<layout->array_definition[i].dimensions; j++) {
	if (!SDDS_MPI_BufferedWrite(&zero, sizeof(zero), SDDS_dataset)) {
	  SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_MPI_WriteBinaryArrays)");
	  return 0;
	} 
      }
      continue;
    }
    writeSize = sizeof(*(SDDS_dataset->array)[i].dimension)* layout->array_definition[i].dimensions;
    if (!SDDS_MPI_BufferedWrite(SDDS_dataset->array[i].dimension, writeSize, SDDS_dataset)) {
      SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_MPI_WriteBinaryArrays)");
      return(0);
    }
    if (layout->array_definition[i].type==SDDS_STRING) {
      for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	if (!SDDS_MPI_WriteBinaryString(SDDS_dataset, ((char**)SDDS_dataset->array[i].data)[j])) {
	  SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteBinaryArrays)");
	  return(0);
	} 
      }
    }
    else  {
      writeSize = SDDS_type_size[layout->array_definition[i].type-1]* SDDS_dataset->array[i].elements;
      if (!SDDS_MPI_BufferedWrite(SDDS_dataset->array[i].data, writeSize, SDDS_dataset)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_MPI_WriteBinaryArrays)");
	return(0);
      } 
    }
  }
  return(1);
}

int32_t SDDS_MPI_WriteNonNativeBinaryArrays(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j, zero=0, writeSize=0;
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  
  /*only the master processor write SDDS header, parameters and arrays*/
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryArray"))
    return(0);
  SDDS_SwapEndsArrayData(SDDS_dataset);
  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_arrays; i++) {
    if (!SDDS_dataset->array[i].dimension) {
      for (j=0; j<layout->array_definition[i].dimensions; j++) {
	if (!SDDS_MPI_BufferedWrite(&zero, sizeof(zero), SDDS_dataset)) {
	  SDDS_SetError("Unable to write null array--failure writing dimensions (SDDS_MPI_WriteBinaryArrays)");
          SDDS_SwapEndsArrayData(SDDS_dataset);
	  return 0;
	} 
      }
      continue;
    }
    writeSize = sizeof(*(SDDS_dataset->array)[i].dimension)* layout->array_definition[i].dimensions;
    if (!SDDS_MPI_BufferedWrite(SDDS_dataset->array[i].dimension, writeSize, SDDS_dataset)) {
      SDDS_SetError("Unable to write arrays--failure writing dimensions (SDDS_MPI_WriteBinaryArrays)");
      SDDS_SwapEndsArrayData(SDDS_dataset);
      return(0);
    }
    if (layout->array_definition[i].type==SDDS_STRING) {
      for (j=0; j<SDDS_dataset->array[i].elements; j++) {
	if (!SDDS_MPI_WriteNonNativeBinaryString(SDDS_dataset, ((char**)SDDS_dataset->array[i].data)[j])) {
	  SDDS_SetError("Unable to write arrays--failure writing string (SDDS_WriteBinaryArrays)");
          SDDS_SwapEndsArrayData(SDDS_dataset);
	  return(0);
	} 
      }
    }
    else  {
      writeSize = SDDS_type_size[layout->array_definition[i].type-1]* SDDS_dataset->array[i].elements;
      if (!SDDS_MPI_BufferedWrite(SDDS_dataset->array[i].data, writeSize, SDDS_dataset)) {
	SDDS_SetError("Unable to write arrays--failure writing values (SDDS_MPI_WriteBinaryArrays)");
        SDDS_SwapEndsArrayData(SDDS_dataset);
	return(0);
      } 
    }
  }
  SDDS_SwapEndsArrayData(SDDS_dataset);
  return(1);
}
int32_t SDDS_MPI_WriteBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  int32_t i,  size, type; 
  SDDS_LAYOUT *layout;
  MPI_DATASET *MPI_dataset;
  /*char buff[defaultStringLength+1], format[256];*/
  char *buff;
  char format[256]; 
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryRow"))
    return(0); 

  layout = &SDDS_dataset->layout;
  
  sprintf(format, "%%-%ds", defaultStringLength);
  if (!(buff = malloc(sizeof(*buff)*(defaultStringLength+1)))) {
      SDDS_SetError("Can not allocate memory in SDDS_MPI_WriteBinaryRow!");
      return 0;
  }
  buff[defaultStringLength] = 0;
  for (i=0; i<layout->n_columns; i++) {
    type = layout->column_definition[i].type;
    size = SDDS_type_size[type-1];
    
    if (type==SDDS_STRING) {
      if (strlen( *((char**)SDDS_dataset->data[i]+row))<=defaultStringLength)
		sprintf(buff, format, *((char**)SDDS_dataset->data[i]+row));
      else {
		strncpy(buff,  *((char**)SDDS_dataset->data[i]+row), defaultStringLength);
		number_of_string_truncated++;
      }
      if (!SDDS_MPI_WriteBinaryString(SDDS_dataset,  buff)) {
	free(buff);
	return 0;
      }
    } else {
      size = SDDS_type_size[type-1];
      if (!SDDS_MPI_BufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, SDDS_dataset)) {
	free(buff);
	return 0;
      }
    }
  }
  free(buff);
  return(1);
}

int32_t SDDS_MPI_WriteNonNativeBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row)
{
  int32_t i,  size, type; 
  SDDS_LAYOUT *layout;
  MPI_DATASET *MPI_dataset;
  /*char buff[defaultStringLength+1], format[256];*/
  char *buff;
  char format[256];
  MPI_dataset = SDDS_dataset->MPI_dataset;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_WriteBinaryRow"))
    return(0); 

  layout = &SDDS_dataset->layout;
  
  sprintf(format, "%%-%ds", defaultStringLength);
  if (!(buff = malloc(sizeof(*buff)*(defaultStringLength+1)))) {
    SDDS_SetError("Can not allocate memory in SDDS_MPI_WriteNonNativeBinaryRow!");
    return 0;
  }
  buff[defaultStringLength] = 0;
  for (i=0; i<layout->n_columns; i++) {
    type = layout->column_definition[i].type;
    size = SDDS_type_size[type-1];
    
    if (type==SDDS_STRING) {
      if (strlen( *((char**)SDDS_dataset->data[i]+row))<=defaultStringLength)
        sprintf(buff, format, *((char**)SDDS_dataset->data[i]+row));
      else {
        strncpy(buff,  *((char**)SDDS_dataset->data[i]+row), defaultStringLength);
        number_of_string_truncated++;
      }
      if (!SDDS_MPI_WriteNonNativeBinaryString(SDDS_dataset,  buff)) {
	free(buff);
        return 0;
      }
    } else {
      size = SDDS_type_size[type-1];
      if (!SDDS_MPI_BufferedWrite((char*)SDDS_dataset->data[i]+row*size, size, SDDS_dataset)) {
	free(buff);
        return 0;
      }
    }
  }
  free(buff);
  return(1);
}
MPI_Offset SDDS_MPI_Get_Column_Size(SDDS_DATASET *SDDS_dataset)
{
  int32_t i;
  MPI_Offset column_offset=0;
  SDDS_LAYOUT *layout;

  layout = &SDDS_dataset->layout;
  for (i=0; i<layout->n_columns; i++) {
    if (layout->column_definition[i].type==SDDS_STRING) 
      /* for string type column, the fixed column size defined by user use SDDS_SetDefaultStringLength(value), 
	 and the length of string is written before the string*/
      column_offset += sizeof(int32_t) + defaultStringLength * sizeof(char);
    else
      column_offset += SDDS_type_size[layout->column_definition[i].type-1];
  }
  return column_offset;
}

int32_t SDDS_MPI_BufferedWrite(void *target, size_t targetSize, SDDS_DATASET *SDDS_dataset)
{
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;
  int32_t mpi_code;

  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->fBuffer);
  
  if (!fBuffer->bufferSize) {
    if ((mpi_code=MPI_File_write(MPI_dataset->MPI_file,  target, targetSize,  MPI_BYTE, 
				    MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
    return 1;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    memcpy((char*)fBuffer->data, (char*)target, targetSize);
    fBuffer->data += targetSize;
#ifdef DEBUG
    fprintf(stderr, "SDDS_MPI_BufferedWrite of %" PRId32 " bytes done in-memory, %" PRId32 " bytes left\n", targetSize, fBuffer->bytesLeft);
#endif
    return 1;
  } else {
    int32_t lastLeft;
    /* add back what was subtracted in test above.
     * lastLeft is the number of bytes left in the buffer before doing anything 
     * and also the number of bytes from the users data that get copied into the buffer.
     */
    lastLeft = (fBuffer->bytesLeft += targetSize);  
    /* copy part of the data into the buffer and write the buffer out */
    memcpy((char*)fBuffer->data, (char*)target, fBuffer->bytesLeft);
    if ((mpi_code=MPI_File_write(MPI_dataset->MPI_file,  fBuffer->buffer, fBuffer->bufferSize,  MPI_BYTE, 
				 MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
   
    /* reset the data pointer and the bytesLeft value.
     * also, determine if the remaining data is too large for the buffer.
     * if so, just write it out.
     */
    fBuffer->data = fBuffer->buffer;
    if ((targetSize -= lastLeft)>(fBuffer->bytesLeft = fBuffer->bufferSize)) {
      if ((mpi_code=MPI_File_write_at(MPI_dataset->MPI_file, (MPI_Offset)(MPI_dataset->file_offset), (char*)target+lastLeft, targetSize,  MPI_BYTE, 
				      MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
	SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
	return 0;
      }
      return 1;
    }
    /* copy remaining data into the buffer.
     * could do this with a recursive call, but this is more efficient.
     */
    memcpy((char*)fBuffer->data, (char*)target+lastLeft, targetSize);
    fBuffer->data += targetSize;
    fBuffer->bytesLeft -= targetSize;
    return 1;
  }
}

int32_t SDDS_MPI_BufferedWriteAll(void *target, size_t targetSize, SDDS_DATASET *SDDS_dataset)
{
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;
  int32_t mpi_code;

  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->fBuffer);
  
  if (!fBuffer->bufferSize) {
    if ((mpi_code=MPI_File_write_all(MPI_dataset->MPI_file,  target, targetSize,  MPI_BYTE, 
				    MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
    return 1;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    memcpy((char*)fBuffer->data, (char*)target, targetSize);
    fBuffer->data += targetSize;
#ifdef DEBUG
    fprintf(stderr, "SDDS_MPI_BufferedWrite of %" PRId32 " bytes done in-memory, %" PRId32 " bytes left\n", targetSize, fBuffer->bytesLeft);
#endif
    return 1;
  } else {
    int32_t lastLeft;
    /* add back what was subtracted in test above.
     * lastLeft is the number of bytes left in the buffer before doing anything 
     * and also the number of bytes from the users data that get copied into the buffer.
     */
    lastLeft = (fBuffer->bytesLeft += targetSize);  
    /* copy part of the data into the buffer and write the buffer out */
    memcpy((char*)fBuffer->data, (char*)target, fBuffer->bytesLeft);
    if ((mpi_code=MPI_File_write_all(MPI_dataset->MPI_file,  fBuffer->buffer, fBuffer->bufferSize,  MPI_BYTE, 
				 MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
   
    /* reset the data pointer and the bytesLeft value.
     * also, determine if the remaining data is too large for the buffer.
     * if so, just write it out.
     */
    fBuffer->data = fBuffer->buffer;
    if ((targetSize -= lastLeft)>(fBuffer->bytesLeft = fBuffer->bufferSize)) {
      if ((mpi_code=MPI_File_write_all(MPI_dataset->MPI_file,  (char*)target+lastLeft, targetSize,  MPI_BYTE, 
					  MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
	SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_WriteBufferedWrite(MPI_File_write_at failed)", mpi_code, 0);
	return 0;
      }
      return 1;
    }
    /* copy remaining data into the buffer.
     * could do this with a recursive call, but this is more efficient.
     */
    memcpy((char*)fBuffer->data, (char*)target+lastLeft, targetSize);
    fBuffer->data += targetSize;
    fBuffer->bytesLeft -= targetSize;
    return 1;
  }
}


int32_t SDDS_MPI_FlushBuffer(SDDS_DATASET *SDDS_dataset)
{
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;
  int32_t writeBytes;
  int32_t mpi_code;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->fBuffer);
  
  if (!fBuffer->bufferSize) 
    return 1;
  
  if ((writeBytes = fBuffer->bufferSize - fBuffer->bytesLeft)) {
    if (writeBytes<0) {
      SDDS_SetError("Unable to flush buffer: negative byte count (SDDS_FlushBuffer).");
      return 0;
    }
#ifdef DEBUG
    fprintf(stderr, "Writing %" PRId32 " bytes to disk\n", writeBytes);
#endif
    if ((mpi_code=MPI_File_write(MPI_dataset->MPI_file,  fBuffer->buffer, writeBytes,  MPI_BYTE, 
				 MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_FlushBuffer(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
  }
  return 1;
}

int32_t SDDS_MPI_CountRowsOfInterest(SDDS_DATASET *SDDS_dataset, int32_t start_row, int32_t end_row)
{
  int32_t i, rows=0;
  for (i=start_row; i<end_row; i++) {
	if (i>SDDS_dataset->n_rows-1)
	  break;
	if (SDDS_dataset->row_flag[i])
	  rows++;
  }
  return rows;
}

 int32_t SDDS_MPI_GetTotalRows(SDDS_DATASET *SDDS_dataset)
 {
   int32_t total_rows;
   MPI_Reduce(&(SDDS_dataset->n_rows), &total_rows, 1, MPI_INT, MPI_SUM, 0, SDDS_dataset->MPI_dataset->comm);
   return total_rows;
 }

int32_t SDDS_MPI_WriteNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset)
{
  int32_t  row,  prev_rows, i, total_rows, fixed_rows,  rows;
  int mpi_code, type=0; 
  MPI_Offset column_offset, rowcount_offset, offset;
  int32_t *n_rows=NULL;
  MPI_Status status;

  MPI_DATASET *MPI_dataset = NULL;
  SDDS_FILEBUFFER *fBuffer;
 
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryPage"))
    return(0);
  
  fBuffer= &SDDS_dataset->fBuffer;
  if (SDDS_dataset->layout.version==3)
    /*write by column ignores the row flag and buffer is not needed for writing data by column*/
    rows = SDDS_dataset->n_rows;
  else {
    rows = SDDS_CountRowsOfInterest(SDDS_dataset);
    if (!fBuffer->buffer) {
      fBuffer->bufferSize = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
	return 0;
      }
      fBuffer->bytesLeft = fBuffer->bufferSize;
      fBuffer->data[0] = 0;
    }
  }
  if (MPI_dataset->n_page>=1)
    MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL); 
  rowcount_offset = MPI_dataset->file_offset + SDDS_MPI_GetTitleOffset(SDDS_dataset); /* the offset before writing rows */ 
  /*get total number of rows writing */
  column_offset = MPI_dataset->column_offset;
  if (!(n_rows=calloc(sizeof(*n_rows), MPI_dataset->n_processors))) {
    SDDS_SetError("Memory allocation failed!");
    return 0;
  }
  MPI_Allgather(&rows, 1, MPI_INT, n_rows, 1, MPI_INT, MPI_dataset->comm);
  prev_rows = 0;
  for (i=0; i<MPI_dataset->myid; i++)
    prev_rows +=n_rows[i];
  total_rows = 0;
  for (i=0; i<MPI_dataset->n_processors; i++) 
    total_rows +=n_rows[i];
  if (MPI_dataset->myid==0) {
    fixed_rows = total_rows;
    if (!fBuffer->buffer) {
      fBuffer->bufferSize = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
        SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
        return 0;
      }
      fBuffer->bytesLeft = fBuffer->bufferSize;
      fBuffer->data[0] = 0;
    }
    SDDS_SwapLong(&fixed_rows);
    if (!SDDS_MPI_BufferedWrite(&fixed_rows, sizeof(fixed_rows), SDDS_dataset))
      return 0; 
    if (!SDDS_MPI_WriteNonNativeBinaryParameters(SDDS_dataset) || !SDDS_MPI_WriteNonNativeBinaryArrays(SDDS_dataset))
      return 0;
    /* flush buffer, write everything in the buffer to file*/
    if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
      return 0;
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
  if (SDDS_dataset->layout.version==3) {
    /*write data by column */
    offset = rowcount_offset;
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      type = SDDS_dataset->layout.column_definition[i].type;
      MPI_dataset->file_offset = offset + (MPI_Offset)prev_rows * SDDS_type_size[type-1];
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not write string column to SDDS3 (SDDS_MPI_WriteContinuousBinaryPage");
	return 0;
      }
      if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
	SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary rows", mpi_code, 0);
	SDDS_SetError("Unable to set view for read binary rows");
	return 0;
      }
      if ((mpi_code = MPI_File_write(MPI_dataset->MPI_file, SDDS_dataset->data[i], rows * SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	SDDS_SetError("Unable to write binary columns (SDDS_MPI_WriteContinuousBinaryPage");
	return 0;
      }
      
      offset += (MPI_Offset)total_rows * SDDS_type_size[type-1];
    }
    MPI_dataset->file_offset = offset;
  } else {
    /* now all processors write column data row by row */ 
    MPI_dataset->file_offset = rowcount_offset + (MPI_Offset)prev_rows * column_offset;
    /* set view to the position where the processor starts writing data */ 
    MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL); 
    if (!MPI_dataset->collective_io) {
      row = 0;
      for (i=0; i<SDDS_dataset->n_rows; i++) {
	if (SDDS_dataset->row_flag[i] && !SDDS_MPI_WriteNonNativeBinaryRow(SDDS_dataset,  i))  
	  return 0;
	row ++;
      }
      /*get current file position until now */ 
      SDDS_dataset->n_rows = row;
      if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
	return 0;
    } else {
      if (!SDDS_MPI_CollectiveWriteByRow(SDDS_dataset))
	return 0;
      row = SDDS_dataset->n_rows;
    }
    MPI_Allreduce(&row, &total_rows,  1, MPI_INT, MPI_SUM, MPI_dataset->comm);
    MPI_dataset->file_offset = rowcount_offset + total_rows * column_offset;
    rows = row;
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
  free(n_rows);
  /*skip checking if all data has been written for now */
  SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
  SDDS_dataset->n_rows_written = rows;
  SDDS_dataset->writing_page = 1;
  MPI_dataset->n_page ++;
  return 1;
}

int32_t SDDS_MPI_WriteContinuousBinaryPage(SDDS_DATASET *SDDS_dataset)
{
  int32_t  row,  prev_rows, i, total_rows, fixed_rows,  rows;
  int mpi_code, type=0; 
  MPI_Offset column_offset, rowcount_offset, offset;
  int32_t *n_rows=NULL;
  MPI_Status status;

  MPI_DATASET *MPI_dataset = NULL;
  SDDS_FILEBUFFER *fBuffer;
  char *outputEndianess = NULL;
  
  if ((outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS"))) {
    if (((strncmp(outputEndianess, "big", 3) == 0) && (SDDS_IsBigEndianMachine() == 0)) || \
        ((strncmp(outputEndianess, "little", 6) == 0) && (SDDS_IsBigEndianMachine() == 1)))
      return SDDS_MPI_WriteNonNativeBinaryPage(SDDS_dataset);
  }
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_WriteBinaryPage"))
    return(0);
  
  fBuffer= &SDDS_dataset->fBuffer;
  if (SDDS_dataset->layout.version==3)
    /*write by column ignores the row flag and buffer is not needed for writing data by column*/
    rows = SDDS_dataset->n_rows;
  else {
    rows = SDDS_CountRowsOfInterest(SDDS_dataset);
    if (!fBuffer->buffer) {
      fBuffer->bufferSize = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
	return 0;
      }
      fBuffer->bytesLeft = fBuffer->bufferSize;
      fBuffer->data[0] = 0;
    }
  }
  if (MPI_dataset->n_page>=1)
    MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL); 
  rowcount_offset = MPI_dataset->file_offset + SDDS_MPI_GetTitleOffset(SDDS_dataset); /* the offset before writing rows */ 
  /*get total number of rows writing */
  column_offset = MPI_dataset->column_offset;
  if (!(n_rows=calloc(sizeof(*n_rows), MPI_dataset->n_processors))) {
    SDDS_SetError("Memory allocation failed!");
    return 0;
  }
  MPI_Allgather(&rows, 1, MPI_INT, n_rows, 1, MPI_INT, MPI_dataset->comm);
  prev_rows = 0;
  for (i=0; i<MPI_dataset->myid; i++)
    prev_rows +=n_rows[i];
  total_rows = 0;
  for (i=0; i<MPI_dataset->n_processors; i++) 
    total_rows +=n_rows[i];
  if (MPI_dataset->myid==0) {
    fixed_rows = total_rows;
    if (!fBuffer->buffer) {
      fBuffer->bufferSize = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
        SDDS_SetError("Unable to do buffered read--allocation failure (SDDS_WriteBinaryPage)");
        return 0;
      }
      fBuffer->bytesLeft = fBuffer->bufferSize;
      fBuffer->data[0] = 0;
    }
    if (!SDDS_MPI_BufferedWrite(&fixed_rows, sizeof(fixed_rows), SDDS_dataset))
      return 0; 
    if (!SDDS_MPI_WriteBinaryParameters(SDDS_dataset) || !SDDS_MPI_WriteBinaryArrays(SDDS_dataset))
      return 0;
    /* flush buffer, write everything in the buffer to file*/
    if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
      return 0;
  }
  
  if (SDDS_dataset->layout.version==3) {
    /*write data by column */
    offset = rowcount_offset;
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      type = SDDS_dataset->layout.column_definition[i].type;
      MPI_dataset->file_offset = offset + (MPI_Offset)prev_rows * SDDS_type_size[type-1];
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not write string column to SDDS3 (SDDS_MPI_WriteContinuousBinaryPage");
	return 0;
      }
      if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
	SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary rows", mpi_code, 0);
	SDDS_SetError("Unable to set view for read binary rows");
	return 0;
      }
      if ((mpi_code = MPI_File_write(MPI_dataset->MPI_file, SDDS_dataset->data[i], rows * SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	SDDS_SetError("Unable to write binary columns (SDDS_MPI_WriteContinuousBinaryPage");
	return 0;
      }
      
      offset += (MPI_Offset)total_rows * SDDS_type_size[type-1];
    }
    MPI_dataset->file_offset = offset;
  } else {
    /* now all processors write column data row by row */ 
    MPI_dataset->file_offset = rowcount_offset + (MPI_Offset)prev_rows * column_offset;
    /* set view to the position where the processor starts writing data */ 
    MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL); 
    if (!MPI_dataset->collective_io) {
      row = 0;
      for (i=0; i<SDDS_dataset->n_rows; i++) {
	if (SDDS_dataset->row_flag[i] && !SDDS_MPI_WriteBinaryRow(SDDS_dataset,  i))  
	  return 0;
	row ++;
      }
      /*get current file position until now */ 
      SDDS_dataset->n_rows = row;
      if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
	return 0;
    } else {
      if (!SDDS_MPI_CollectiveWriteByRow(SDDS_dataset))
	return 0;
      row = SDDS_dataset->n_rows;
    }
    MPI_Allreduce(&row, &total_rows,  1, MPI_INT, MPI_SUM, MPI_dataset->comm);
    MPI_dataset->file_offset = rowcount_offset + total_rows * column_offset;
    rows = row;
  }
  free(n_rows);
  /*skip checking if all data has been written for now */
  SDDS_dataset->last_row_written = SDDS_dataset->n_rows - 1;
  SDDS_dataset->n_rows_written = rows;
  SDDS_dataset->writing_page = 1;
  MPI_dataset->n_page ++;
  return 1;
}

int32_t SDDS_MPI_BufferedRead(void *target, size_t targetSize, SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t mpi_code, bytesRead;
  MPI_Status status;
  MPI_DATASET *MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!fBuffer || !fBuffer->bufferSize) {
    /* just read into users buffer or seek if no buffer given */
    if (!target) 
      mpi_code = MPI_File_seek(MPI_dataset->MPI_file, targetSize, MPI_SEEK_CUR);
    else {
      mpi_code = MPI_File_read(MPI_dataset->MPI_file, target, targetSize, MPI_BYTE, &status);
      MPI_Get_count(&status, MPI_BYTE, &bytesRead);
      if (!bytesRead) {
        MPI_dataset->end_of_file = 1;
	return -1;
      }
      if (bytesRead<targetSize)
        return 0;
    }
    if (mpi_code!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_BufferedRead(MPI_File_read failed)", mpi_code, 0);
      return 0;
    }
    return 1;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    /* sufficient data is already in the buffer */
    if (target) {
      memcpy((char*)target, (char*)fBuffer->data, targetSize);
    }
    fBuffer->data += targetSize;
    return 1;
  } else {
    /* need to read additional data into buffer */
    int32_t bytesNeeded, offset;
    fBuffer->bytesLeft += targetSize;  /* adds back amount subtracted above */
    /* first, use the data that is already available. this cleans out the buffer */
    if ((offset=fBuffer->bytesLeft)) {
      /* some data is available in the buffer */
      if (target) {
        memcpy((char*)target, (char*)fBuffer->data, offset);
      }
      bytesNeeded = targetSize - offset;
      fBuffer->bytesLeft = 0;
    } else {
      bytesNeeded = targetSize;
    }
    fBuffer->data = fBuffer->buffer;
    if (fBuffer->bufferSize < bytesNeeded) {
      /* just read what is needed directly into user's memory or seek */
      if (!target)
        mpi_code = MPI_File_seek(MPI_dataset->MPI_file, targetSize, MPI_SEEK_CUR);
      else {
        mpi_code = MPI_File_read(MPI_dataset->MPI_file, (char*)target+offset, bytesNeeded, MPI_BYTE, &status);
        MPI_Get_count(&status, MPI_BYTE, &bytesRead);
        if (!bytesRead) {
          MPI_dataset->end_of_file = 1;
	  return -1;
	}
        if (bytesRead<bytesNeeded)
          return 0;
      }
      if (mpi_code!=MPI_SUCCESS) {
        SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_ReadBufferedRead(MPI_File_read failed)", mpi_code, 0);
        return 0;
      }
      return 1;
    }
    /* fill the buffer */
    mpi_code = MPI_File_read(MPI_dataset->MPI_file, fBuffer->data, fBuffer->bufferSize, MPI_BYTE, &status);
    MPI_Get_count(&status, MPI_BYTE, &(fBuffer->bytesLeft));
    if (!(fBuffer->bytesLeft))
      MPI_dataset->end_of_file = 1;
    if (fBuffer->bytesLeft<bytesNeeded)
      return 0;
    if (target)
      memcpy((char*)target+offset, (char*)fBuffer->data, bytesNeeded);
    fBuffer->data += bytesNeeded;
    fBuffer->bytesLeft -= bytesNeeded;
    return 1;
  }
}

int32_t SDDS_MPI_BufferedReadAll(void *target, size_t targetSize, SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t mpi_code, bytesRead;
  MPI_Status status;
  MPI_DATASET *MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!fBuffer || !fBuffer->bufferSize) {
    /* just read into users buffer or seek if no buffer given */
    if (!target) 
      mpi_code = MPI_File_seek(MPI_dataset->MPI_file, targetSize, MPI_SEEK_CUR);
    else {
      mpi_code = MPI_File_read_all(MPI_dataset->MPI_file, target, targetSize, MPI_BYTE, &status);
      MPI_Get_count(&status, MPI_BYTE, &bytesRead);
      if (!bytesRead) {
        MPI_dataset->end_of_file = 1;
	return -1;
      }
      if (bytesRead<targetSize)
        return 0;
    }
    if (mpi_code!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_BufferedRead(MPI_File_read failed)", mpi_code, 0);
      return 0;
    }
    return 1;
  }
  if ((fBuffer->bytesLeft-=targetSize)>=0) {
    /* sufficient data is already in the buffer */
    if (target) {
      memcpy((char*)target, (char*)fBuffer->data, targetSize);
    }
    fBuffer->data += targetSize;
    return 1;
  } else {
    /* need to read additional data into buffer */
    int32_t bytesNeeded, offset;
    fBuffer->bytesLeft += targetSize;  /* adds back amount subtracted above */
    /* first, use the data that is already available. this cleans out the buffer */
    if ((offset=fBuffer->bytesLeft)) {
      /* some data is available in the buffer */
      if (target) {
        memcpy((char*)target, (char*)fBuffer->data, offset);
      }
      bytesNeeded = targetSize - offset;
      fBuffer->bytesLeft = 0;
    } else {
      bytesNeeded = targetSize;
    }
    fBuffer->data = fBuffer->buffer;
    if (fBuffer->bufferSize < bytesNeeded) {
      /* just read what is needed directly into user's memory or seek */
      if (!target)
        mpi_code = MPI_File_seek(MPI_dataset->MPI_file, targetSize, MPI_SEEK_CUR);
      else {
        mpi_code = MPI_File_read_all(MPI_dataset->MPI_file, (char*)target+offset, bytesNeeded, MPI_BYTE, &status);
        MPI_Get_count(&status, MPI_BYTE, &bytesRead);
        if (!bytesRead) {
          MPI_dataset->end_of_file = 1;
	  return -1;
	}
        if (bytesRead<bytesNeeded)
          return 0;
      }
      if (mpi_code!=MPI_SUCCESS) {
        SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_ReadBufferedRead(MPI_File_read failed)", mpi_code, 0);
        return 0;
      }
      return 1;
    }
    /* fill the buffer */
    mpi_code = MPI_File_read_all(MPI_dataset->MPI_file, fBuffer->data, fBuffer->bufferSize, MPI_BYTE, &status);
    MPI_Get_count(&status, MPI_BYTE, &(fBuffer->bytesLeft));
    if (!(fBuffer->bytesLeft))
      MPI_dataset->end_of_file = 1;
    if (fBuffer->bytesLeft<bytesNeeded)
      return 0;
    if (target)
      memcpy((char*)target+offset, (char*)fBuffer->data, bytesNeeded);
    fBuffer->data += bytesNeeded;
    fBuffer->bytesLeft -= bytesNeeded;
    return 1;
  }
}


char *SDDS_MPI_ReadBinaryString(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string; 
  if (!SDDS_MPI_BufferedRead(&length, sizeof(length), SDDS_dataset, fBuffer) || length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL); 
  if (length && !SDDS_MPI_BufferedRead(skip?NULL:string, sizeof(*string)*length, SDDS_dataset, fBuffer))
    return(NULL); 
  string[length] = 0;
  return(string);
}

char *SDDS_MPI_ReadNonNativeBinaryString(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer, int32_t skip)
{
  int32_t length;
  char *string;

  if (!SDDS_MPI_BufferedRead(&length, sizeof(length), SDDS_dataset, fBuffer))
    return(0);
  SDDS_SwapLong(&length);
  if (length<0)
    return(0);
  if (!(string=SDDS_Malloc(sizeof(*string)*(length+1))))
    return(NULL);
  if (length && !SDDS_MPI_BufferedRead(skip?NULL:string, sizeof(*string)*length, SDDS_dataset, fBuffer))
    return(NULL);
  string[length] = 0;
  return(string);
}

int32_t SDDS_MPI_ReadBinaryParameters(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t i;
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  static char buffer[SDDS_MAXLINE]; 
 
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_parameters)
    return(1);
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if (layout->parameter_definition[i].fixed_value) {
      strcpy(buffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(buffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
        SDDS_SetError("Unable to read page--parameter scanning error (SDDS_MPI_ReadBinaryParameters)");
        return(0);
      }
    }
    else if (layout->parameter_definition[i].type==SDDS_STRING) {
      if (*(char**)SDDS_dataset->parameter[i])
        free(*(char**)SDDS_dataset->parameter[i]); 
      if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_MPI_ReadBinaryString(SDDS_dataset, fBuffer, 0))) {
        SDDS_SetError("Unable to read parameters--failure reading string (SDDS_MPI_ReadBinaryParameters)");
        return(0);
      }
    } else {
      if (!SDDS_MPI_BufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
                                 SDDS_dataset, fBuffer)) {
        SDDS_SetError("Unable to read parameters--failure reading value (SDDS_MPI_ReadBinaryParameters)");
        return(0);
      }
    }
  }
  return(1);
}


int32_t SDDS_MPI_ReadBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row, int32_t skip)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout; 
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;

  MPI_dataset = SDDS_dataset->MPI_dataset;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;
  
  for (i=0; i<layout->n_columns; i++) {
    if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if ((type=layout->column_definition[i].type)==SDDS_STRING) { 
      if (!skip) {
        if (((char***)SDDS_dataset->data)[i][row])
          free((((char***)SDDS_dataset->data)[i][row]));
        if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_MPI_ReadBinaryString(SDDS_dataset, fBuffer, 0))) {
          SDDS_SetError("Unable to read rows--failure reading string (SDDS_MPI_ReadBinaryRows)");
          return(0);
        }
      } else {
        if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_MPI_ReadBinaryString(SDDS_dataset, fBuffer, 1))) {
          SDDS_SetError("Unable to read rows--failure reading string (SDDS_MPI_ReadBinaryRows)");
          return 0;
        }
      }
    } else { 
      size = SDDS_type_size[type-1];
      if (!SDDS_MPI_BufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, SDDS_dataset, 
                                 fBuffer)) {
        SDDS_SetError("Unable to read row--failure reading value (SDDS_MPI_ReadBinaryRow)");
        return(0);
      }
    }
  }
  return(1);
}

int32_t SDDS_MPI_ReadBinaryArrays(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t i, j;
  SDDS_LAYOUT *layout;
  /*  char *predefined_format;*/
  /*  static char buffer[SDDS_MAXLINE];*/ 
  SDDS_ARRAY *array; 
  MPI_DATASET *MPI_dataset;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadBinaryArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_arrays)
    return(1);
  
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to read array--pointer to structure storage area is NULL (SDDS_MPI_ReadBinaryArrays)");
    return(0);
  }
  for (i=0; i<layout->n_arrays; i++) {
    array = SDDS_dataset->array+i;
    if (array->definition && !SDDS_FreeArrayDefinition(array->definition)) {
      SDDS_SetError("Unable to get array--array definition corrupted (SDDS_MPI_ReadBinaryArrays)");
      return(0);
    }
    if (!SDDS_CopyArrayDefinition(&array->definition, layout->array_definition+i)) {
      SDDS_SetError("Unable to read array--definition copy failed (SDDS_MPI_ReadBinaryArrays)");
      return(0);
    }
    /*if (array->dimension) free(array->dimension); */
    if (!(array->dimension = SDDS_Realloc(array->dimension, 
                                          sizeof(*array->dimension)*array->definition->dimensions))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_MPI_ReadBinaryArrays)");
      return(0);
    } 
    if (!SDDS_MPI_BufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
			     SDDS_dataset, fBuffer)) {
      SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_MPI_ReadBinaryArrays)");
	return(0);
    }
    array->elements = 1;
    for (j=0; j<array->definition->dimensions; j++)
      array->elements *= array->dimension[j];
    if (array->data) free(array->data);
    array->data = array->pointer = NULL;
    if (array->elements==0)
      continue;
    if (array->elements<0) {
      SDDS_SetError("Unable to read array--number of elements is negative (SDDS_MPI_ReadBinaryArrays)");
      return(0);
    }
    if (!(array->data = SDDS_Realloc(array->data, array->elements*SDDS_type_size[array->definition->type-1]))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_MPI_ReadBinaryArrays)");
      return(0);
    }
    if (array->definition->type==SDDS_STRING) {
      for (j=0; j<array->elements; j++) {
        if (!( ((char**)(array->data))[j] = SDDS_MPI_ReadBinaryString(SDDS_dataset, fBuffer, 0))) {
          SDDS_SetError("Unable to read arrays--failure reading string (SDDS_MPI_ReadBinaryArrays)");
          return(0);
        }
      }
    } else {
      if (!SDDS_MPI_BufferedRead(array->data,
                                 SDDS_type_size[array->definition->type-1]*array->elements,
                                 SDDS_dataset, fBuffer)) {
        SDDS_SetError("Unable to read arrays--failure reading values (SDDS_MPI_ReadBinaryArrays)");
        return(0);
      }
    }
  }
  return(1);
}


int32_t SDDS_MPI_ReadNonNativeBinaryParameters(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t i;
  SDDS_LAYOUT *layout; 
  MPI_DATASET *MPI_dataset;
  static char buffer[SDDS_MAXLINE];
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadNonNativeBinaryParameters"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_parameters)
    return(1);
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if (layout->parameter_definition[i].fixed_value) {
      strcpy(buffer, layout->parameter_definition[i].fixed_value);
      if (!SDDS_ScanData(buffer, layout->parameter_definition[i].type, 0, SDDS_dataset->parameter[i], 0, 1)) {
        SDDS_SetError("Unable to read page--parameter scanning error (SDDS_MPI_ReadNonNativeBinaryParameters)");
        return(0);
      }
    }
    else if (layout->parameter_definition[i].type==SDDS_STRING) {
      if (*(char**)SDDS_dataset->parameter[i])
        free(*(char**)SDDS_dataset->parameter[i]);
      if (!(*((char**)SDDS_dataset->parameter[i]) = SDDS_MPI_ReadNonNativeBinaryString(SDDS_dataset, fBuffer, 0))) {
        SDDS_SetError("Unable to read parameters--failure reading string (SDDS_MPI_ReadNonNativeBinaryParameters)");
        return(0);
      }
    } else {
      if (!SDDS_MPI_BufferedRead(SDDS_dataset->parameter[i], SDDS_type_size[layout->parameter_definition[i].type-1], 
                             SDDS_dataset, fBuffer)) {
        SDDS_SetError("Unable to read parameters--failure reading value (SDDS_MPI_ReadNonNativeBinaryParameters)");
        return(0);
      }
    }
  }
  SDDS_SwapEndsParameterData(SDDS_dataset);
  return(1);
}

int32_t SDDS_MPI_ReadNonNativeBinaryArrays(SDDS_DATASET *SDDS_dataset, SDDS_FILEBUFFER *fBuffer)
{
  int32_t i, j;
  SDDS_LAYOUT *layout; 
  SDDS_ARRAY *array;
  MPI_DATASET *MPI_dataset;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadNonNativeBinaryArrays"))
    return(0);
  layout = &SDDS_dataset->layout;
  if (!layout->n_arrays)
    return(1);
  
  if (!SDDS_dataset->array) {
    SDDS_SetError("Unable to read array--pointer to structure storage area is NULL (SDDS_MPI_ReadNonNativeBinaryArrays)");
    return(0);
  }
  for (i=0; i<layout->n_arrays; i++) {
    array = SDDS_dataset->array+i;
    if (array->definition && !SDDS_FreeArrayDefinition(array->definition)) {
      SDDS_SetError("Unable to get array--array definition corrupted (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (!SDDS_CopyArrayDefinition(&array->definition, layout->array_definition+i)) {
      SDDS_SetError("Unable to read array--definition copy failed (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    }
    /*if (array->dimension) free(array->dimension);*/
    if (!(array->dimension = SDDS_Realloc(array->dimension, 
                                          sizeof(*array->dimension)*array->definition->dimensions))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    } 
    if (!SDDS_MPI_BufferedRead(array->dimension, sizeof(*array->dimension)*array->definition->dimensions, 
                           SDDS_dataset, fBuffer)) {
      SDDS_SetError("Unable to read arrays--failure reading dimensions (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    }
    SDDS_SwapLong(array->dimension);
    array->elements = 1;
    for (j=0; j<array->definition->dimensions; j++)
      array->elements *= array->dimension[j];
    if (array->data) free(array->data);
    array->data = array->pointer = NULL;
    if (array->elements==0)
      continue;
    if (array->elements<0) {
      SDDS_SetError("Unable to read array--number of elements is negative (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (!(array->data = SDDS_Realloc(array->data, array->elements*SDDS_type_size[array->definition->type-1]))) {
      SDDS_SetError("Unable to read array--allocation failure (SDDS_MPI_ReadNonNativeBinaryArrays)");
      return(0);
    }
    if (array->definition->type==SDDS_STRING) {
      for (j=0; j<array->elements; j++) {
        if (!( ((char**)(array->data))[j] = SDDS_MPI_ReadNonNativeBinaryString(SDDS_dataset, fBuffer, 0))) {
          SDDS_SetError("Unable to read arrays--failure reading string (SDDS_MPI_ReadNonNativeBinaryArrays)");
          return(0);
        }
      }
    } else {
      if (!SDDS_MPI_BufferedRead(array->data, 
                                 SDDS_type_size[array->definition->type-1]*array->elements, 
                                 SDDS_dataset, fBuffer)) {
        SDDS_SetError("Unable to read arrays--failure reading values (SDDS_MPI_ReadNonNativeBinaryArrays)");
        return(0);
      }
    }
  }
  SDDS_SwapEndsArrayData(SDDS_dataset);
  return(1);
}

int32_t SDDS_MPI_ReadNonNativeBinaryRow(SDDS_DATASET *SDDS_dataset, int32_t row, int32_t skip)
{
  int32_t i, type, size;
  SDDS_LAYOUT *layout; 
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;

  MPI_dataset= SDDS_dataset->MPI_dataset;
  
  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadNonNativeBinaryRow"))
    return(0);
  layout = &SDDS_dataset->layout;
  fBuffer = &SDDS_dataset->fBuffer;

  for (i=0; i<layout->n_columns; i++) {
    if (layout->column_definition[i].definition_mode&SDDS_WRITEONLY_DEFINITION)
      continue;
    if ((type=layout->column_definition[i].type)==SDDS_STRING) {
      if (!skip) {
        if (((char***)SDDS_dataset->data)[i][row])
          free((((char***)SDDS_dataset->data)[i][row]));
        if (!(((char***)SDDS_dataset->data)[i][row] = SDDS_MPI_ReadNonNativeBinaryString(SDDS_dataset, fBuffer, 0))) {
          SDDS_SetError("Unable to read rows--failure reading string (SDDS_MPI_ReadNonNativeBinaryRow)");
          return(0);
        }
      } else {
        if (!SDDS_MPI_ReadNonNativeBinaryString(SDDS_dataset, fBuffer, 1)) {
          SDDS_SetError("Unable to read rows--failure reading string (SDDS_MPI_ReadNonNativeBinaryRow)");
          return 0;
        }
      }
    } else {
      size = SDDS_type_size[type-1];
      if (!SDDS_MPI_BufferedRead(skip?NULL:(char*)SDDS_dataset->data[i]+row*size, size, SDDS_dataset, 
                                 fBuffer)) {
        SDDS_SetError("Unable to read row--failure reading value (SDDS_MPI_ReadNonNativeBinaryRow)");
        return(0);
      }
    }
  }
  return(1);
}


int32_t SDDS_MPI_BroadcastTitleData(SDDS_DATASET *SDDS_dataset) {
  char *par_data=NULL;
  int32_t i, count=0, *data_len=NULL, type, size;
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  char *string=NULL;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  layout = &(SDDS_dataset->layout);
  if (!layout->n_parameters && !layout->n_arrays) {
    /* no parameters and arrays, only broadcast total_rows*/
    MPI_Bcast(&(MPI_dataset->total_rows), 1, MPI_INT, 0, MPI_dataset->comm);
  } else {
    /* broadcast the total_rows and parameter data*/
    data_len = calloc(sizeof(*data_len), 1+layout->n_parameters);
    if (MPI_dataset->myid==0) {
      data_len[0] = sizeof(int32_t);
      count = data_len[0];
      for (i=0; i<layout->n_parameters; i++) {
	type = layout->parameter_definition[i].type;
	if (type==SDDS_STRING) 
	  data_len[i+1] = strlen(*((char**)SDDS_dataset->parameter[i]))*sizeof(char);
	else
	  data_len[i+1] = SDDS_type_size[type-1];;
	count +=data_len[i+1];
      }
      par_data = (char*)malloc(sizeof(char)*count);
      memcpy((char*)par_data, &(MPI_dataset->total_rows), data_len[0]);
      count = data_len[0];
      for (i=0; i<layout->n_parameters; i++) {
	if (layout->parameter_definition[i].type==SDDS_STRING) 
	  memcpy((char*)par_data+count, *(char**)SDDS_dataset->parameter[i], data_len[i+1]);
	else
	  memcpy((char*)par_data + count, (char*)SDDS_dataset->parameter[i], data_len[i+1]);
	count += data_len[i+1];
      }
    }
    MPI_Bcast(data_len, 1+layout->n_parameters, MPI_INT, 0, MPI_dataset->comm);
    if (MPI_dataset->myid) {
      count = data_len[0];
      for (i=0; i<layout->n_parameters; i++)
	count += data_len[i+1];
      par_data = (char*)malloc(sizeof(char)*count);
    }
 
    MPI_Bcast(par_data, count, MPI_BYTE, 0, MPI_dataset->comm);
    if (!SDDS_StartPage(SDDS_dataset, 0)) {
      SDDS_SetError("Unable to read page--couldn't start page (SDDS_MPI_BroadcastTitleData)");
      return(0);
    }
    if (MPI_dataset->myid) {
      memcpy(&(MPI_dataset->total_rows), (char*)par_data, data_len[0]);
      count = data_len[0];
      for (i=0; i<layout->n_parameters; i++) {
	if (layout->parameter_definition[i].type==SDDS_STRING) {
	  string = malloc(sizeof(char)*(data_len[i+1]+1));
	  memcpy((char*)string, (char*)par_data+count, data_len[i+1]);
	  string[data_len[i+1]] = '\0';
	  *(char**)SDDS_dataset->parameter[i] = string;
	} else
	  memcpy((char*)(SDDS_dataset->parameter[i]), (char*)par_data+count, data_len[i+1]);
	count +=data_len[i+1];
      }
    }
  }
  free(data_len);
  free(par_data);
  data_len=NULL;
  par_data=NULL;
  if (layout->n_arrays) {
    data_len = malloc(sizeof(*data_len)*layout->n_arrays);
    if (MPI_dataset->myid==0) {
      for (i=0; i<layout->n_arrays; i++)
	data_len[i] = layout->array_definition[i].dimensions;
    }
    MPI_Bcast(data_len, layout->n_arrays, MPI_INT, 0, MPI_dataset->comm);
    for (i=0; i<layout->n_arrays; i++) {
      type = layout->array_definition[i].type;
      size = SDDS_type_size[type-1];
      if (data_len[i]) {
	if (type==SDDS_STRING) {
	  if (MPI_dataset->myid==0) {
	    /* it is not easy to broad cast string array, will implement it in the future*/
	  }
	} else
	  MPI_Bcast((char*)SDDS_dataset->array[i].data, data_len[i]*size, MPI_BYTE, 0, MPI_dataset->comm);
      }
    }
  }
  /*MPI_dataset->file_offset += SDDS_MPI_GetTitleOffset(MPI_dataset);*/
  return 1;
}

/*flag master_read: 1 master processor will read rows; 0: master processor does not read rows.*/
int32_t SDDS_MPI_ReadBinaryPage(SDDS_DATASET *SDDS_dataset)
{
  int32_t n_rows, j,  prev_rows, mpi_code, type=0, i, retval, master_read;
  MPI_DATASET *MPI_dataset;
  SDDS_FILEBUFFER *fBuffer;
  MPI_Status status;
  MPI_Offset offset;
  long ID_offset;  

  MPI_dataset = SDDS_dataset->MPI_dataset;
  master_read = MPI_dataset->master_read;
  
  if (SDDS_dataset->autoRecovered)
    return -1;
  if (SDDS_dataset->swapByteOrder) {
    return SDDS_MPI_ReadNonNativeBinaryPage(SDDS_dataset);
  }
  /*  static char s[SDDS_MAXLINE];*/
  n_rows = 0;
  SDDS_SetReadRecoveryMode(0);
  if (MPI_dataset->file_offset>=MPI_dataset->file_size)
    return(SDDS_dataset->page_number=-1); 
  if ((mpi_code=MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
    SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary page", mpi_code, 0);
    SDDS_SetError("Unable to set view for read binary page(1)");
    return 0;
  }
#if defined(MASTER_READTITLE_ONLY)
  if (MPI_dataset->myid==0)
#endif 
    retval=SDDS_MPI_BufferedReadBinaryTitle(SDDS_dataset);
#if defined(MASTER_READTITLE_ONLY)
  MPI_Bcast(&retval, 1, MPI_INT, 0, MPI_dataset->comm);
#endif
  /*end of file reached */
  if (retval<0)
    return(SDDS_dataset->page_number=-1); 
  if (retval==0) {
    SDDS_SetError("Unable to read the SDDS title (row number, parameter and/or array) data");
    return 0;
  }
#if defined(MASTER_READTITLE_ONLY)
  SDDS_MPI_BroadcastTitleData(SDDS_dataset);
#endif
   MPI_dataset->file_offset += SDDS_MPI_GetTitleOffset(SDDS_dataset);
  if (MPI_dataset->total_rows<0) {
    SDDS_SetError("Unable to read page--negative number of rows (SDDS_MPI_ReadBinaryPage)");
    return(0);
  }
  if (MPI_dataset->total_rows>SDDS_GetRowLimit()) {
    /* the number of rows is "unreasonably" large---treat like end-of-file */
    return(SDDS_dataset->page_number=-1);
  }
  prev_rows = 0;
  if (master_read) {
    n_rows = MPI_dataset->total_rows/MPI_dataset->n_processors;
    prev_rows = MPI_dataset->myid * n_rows;
    if (MPI_dataset->myid < (ID_offset=MPI_dataset->total_rows%(MPI_dataset->n_processors))) {
      n_rows ++;
      prev_rows += MPI_dataset->myid;
    }
    else
      prev_rows += ID_offset;
  } else {
    if (MPI_dataset->myid==0)
      n_rows = 0;
    else {
      n_rows = MPI_dataset->total_rows/(MPI_dataset->n_processors-1);
      prev_rows = (MPI_dataset->myid - 1 ) * n_rows;
      if (MPI_dataset->myid <= (ID_offset=MPI_dataset->total_rows%(MPI_dataset->n_processors-1))) {
	n_rows ++;
	prev_rows += (MPI_dataset->myid - 1);
      }
      else
	prev_rows += ID_offset;
    }
  }
  MPI_dataset->start_row = prev_rows; /* This  number will be used as the paritlce ID offset */
  if (!SDDS_StartPage(SDDS_dataset, 0) || !SDDS_LengthenTable(SDDS_dataset, n_rows)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_MPI_ReadBinaryPage)");
    return(0);
  }
  offset = MPI_dataset->file_offset; 
  fBuffer = &SDDS_dataset->fBuffer;
 
  if (SDDS_dataset->layout.version==3) {
    /*read by column buffer is not need*/
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      type =  SDDS_dataset->layout.column_definition[i].type;
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not read string column from SDDS3 (SDDS_MPI_ReadBinaryPage");
	return 0;
      }
      MPI_dataset->file_offset = offset + (MPI_Offset)prev_rows * SDDS_type_size[type-1];
      if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
	SDDS_SetError("Unable to set view for read binary columns");
	return 0;
      }
      if (!MPI_dataset->collective_io) {
	if ((mpi_code = MPI_File_read(MPI_dataset->MPI_file, (char*)SDDS_dataset->data[i], n_rows*SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	  SDDS_SetError("Unable to set view for read binary columns");
	  return 0;
	}
      } else {
	if ((mpi_code = MPI_File_read_all(MPI_dataset->MPI_file, (char*)SDDS_dataset->data[i], n_rows*SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	  SDDS_SetError("Unable to set view for read binary columns");
	  return 0;
	}
      }
      offset += (MPI_Offset)MPI_dataset->total_rows * SDDS_type_size[type-1];
    }
    MPI_dataset->n_rows = SDDS_dataset->n_rows = n_rows;
    MPI_dataset->file_offset = offset;
  } else {
    /* read row by row */
    if (!fBuffer->buffer) {
      fBuffer->bufferSize  = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure");
	return 0;
      }
      fBuffer->bytesLeft = 0;
      fBuffer->data[0] = 0; 
    }
    if (fBuffer->bytesLeft>0) {
      /* discard the extra data for reading next page*/
      fBuffer->data[0] = 0; 
      fBuffer->bytesLeft = 0;
    }
    MPI_dataset->file_offset += (MPI_Offset)prev_rows*MPI_dataset->column_offset; 
    if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary rows", mpi_code, 0);
      SDDS_SetError("Unable to set view for read binary rows");
      return 0;
    }
    if (!master_read || !MPI_dataset->collective_io) {
      for (j=0; j<n_rows; j++) {
	if (!SDDS_MPI_ReadBinaryRow(SDDS_dataset, j, 0)) {
	  SDDS_dataset->n_rows = j;
	  if (SDDS_dataset->autoRecover) {
#if defined(DEBUG)
	    fprintf(stderr, "Doing auto-read recovery\n");
#endif
	    SDDS_dataset->autoRecovered = 1;
	    SDDS_ClearErrors(); 
	    return(SDDS_dataset->page_number=MPI_dataset->n_page);
	  }
	  SDDS_SetError("Unable to read page--error reading data row (SDDS_MPI_ReadBinaryPage)");
	  SDDS_SetReadRecoveryMode(1);
	  return(0);
	}
      }
      MPI_dataset->n_rows = SDDS_dataset->n_rows = j; 
    } else {
      MPI_dataset->n_rows = SDDS_dataset->n_rows = n_rows;
      if (!SDDS_MPI_CollectiveReadByRow(SDDS_dataset))
	return 0;
    }
    MPI_dataset->file_offset = offset + MPI_dataset->total_rows * MPI_dataset->column_offset;
  }
  MPI_dataset->n_page ++;
  return(SDDS_dataset->page_number=MPI_dataset->n_page);
}



int32_t SDDS_MPI_ReadNonNativePage(SDDS_DATASET *SDDS_dataset)
{
  return SDDS_MPI_ReadNonNativePageSparse(SDDS_dataset, 0);
}

int32_t SDDS_MPI_ReadNonNativePageSparse(SDDS_DATASET *SDDS_dataset, uint32_t mode)
/* the mode argument is to support future expansion */
{
  int32_t retval;

  if (!SDDS_CheckDataset(SDDS_dataset, "SDDS_MPI_ReadNonNativePageSparse"))
    return(0);
  if (SDDS_dataset->layout.disconnected) {
    SDDS_SetError("Can't read page--file is disconnected (SDDS_MPI_ReadNonNativePageSparse)");
    return 0;
  }
 
  if (SDDS_dataset->original_layout.data_mode.mode==SDDS_ASCII) {
    SDDS_SetError("Can not read assii file with parallel io.");
    return 0;
  } else if (SDDS_dataset->original_layout.data_mode.mode==SDDS_BINARY) {
    if ((retval=SDDS_MPI_ReadNonNativeBinaryPage(SDDS_dataset))<1) {
      return(retval);
    }
  } else {
    SDDS_SetError("Unable to read page--unrecognized data mode (SDDS_MPI_ReadNonNativePageSparse)");
    return(0);
  }
  return(retval);
}

int32_t SDDS_MPI_ReadNonNativeBinaryPage(SDDS_DATASET *SDDS_dataset)
{
  int32_t n_rows, j, i, total_rows, prev_rows, ID_offset, mpi_code, master_read, type, retval;
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;
  MPI_Offset offset;
  MPI_Status status;
  
   MPI_dataset = SDDS_dataset->MPI_dataset;
  master_read = MPI_dataset->master_read;
  /*  static char s[SDDS_MAXLINE];*/
  n_rows = 0;
  SDDS_SetReadRecoveryMode(0);
  if (MPI_dataset->file_offset>=MPI_dataset->file_size)
    return(SDDS_dataset->page_number=-1);
  
  if ((mpi_code=MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
    SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary page", mpi_code, 0);
    SDDS_SetError("Unable to set view for read binary page(1)");
    return 0;
  }
   /* read the number of rows in current page */
#if defined(MASTER_READTITLE_ONLY)
  if (MPI_dataset->myid==0)
#endif
    retval=SDDS_MPI_BufferedReadNonNativeBinaryTitle(SDDS_dataset);
#if defined(MASTER_READTITLE_ONLY)
  MPI_Bcast(&retval, 1, MPI_INT, 0, MPI_dataset->comm);
#endif
  /*end of file reached */
  if (retval<0)
    return(SDDS_dataset->page_number=-1); 
  if (retval==0) {
    SDDS_SetError("Unable to read the SDDS title (row number, parameter and/or array) data");
    return 0;
  }
  #if defined(MASTER_READTITLE_ONLY)
  SDDS_MPI_BroadcastTitleData(SDDS_dataset);
#endif
   MPI_dataset->file_offset += SDDS_MPI_GetTitleOffset(SDDS_dataset);
  if (MPI_dataset->total_rows<0) {
    SDDS_SetError("Unable to read page--negative number of rows (SDDS_MPI_ReadBinaryPage)");
    return(0);
  }
  if (MPI_dataset->total_rows>SDDS_GetRowLimit()) {
    /* the number of rows is "unreasonably" large---treat like end-of-file */
    return(SDDS_dataset->page_number=-1);
  }
  total_rows = MPI_dataset->total_rows;
  prev_rows = 0;
  if (master_read) {
    n_rows = total_rows/MPI_dataset->n_processors;
    prev_rows = MPI_dataset->myid * n_rows;
    if (MPI_dataset->myid < (ID_offset=total_rows%(MPI_dataset->n_processors))) {
      n_rows ++;
      prev_rows += MPI_dataset->myid;
    }
    else
      prev_rows += ID_offset;
  } else {
    if (MPI_dataset->myid==0)
      n_rows = 0;
    else {
      n_rows = total_rows/(MPI_dataset->n_processors-1);
      prev_rows = (MPI_dataset->myid - 1 ) * n_rows;
      if (MPI_dataset->myid <= (ID_offset=total_rows%(MPI_dataset->n_processors-1))) {
	n_rows ++;
	prev_rows += (MPI_dataset->myid - 1);
      }
      else
	prev_rows += ID_offset;
    }
  }
  MPI_dataset->start_row = prev_rows; /* This  number will be used as the paritlce ID offset */
  if (!SDDS_StartPage(SDDS_dataset, 0) || !SDDS_LengthenTable(SDDS_dataset, n_rows)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_MPI_ReadNonNativeBinaryPage)");
    return(0);
  }
  
  offset = MPI_dataset->file_offset; 
  fBuffer = &SDDS_dataset->fBuffer;
  if (SDDS_dataset->layout.version==3) {
    /*read by column buffer is not need*/
    for (i=0; i<SDDS_dataset->layout.n_columns; i++) {
      type =  SDDS_dataset->layout.column_definition[i].type;
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not read string column from SDDS3 (SDDS_MPI_ReadBinaryPage");
	return 0;
      }
      MPI_dataset->file_offset = offset + (MPI_Offset)prev_rows * SDDS_type_size[type-1];
      if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
	SDDS_SetError("Unable to set view for read binary columns");
	return 0;
      }
      if (!MPI_dataset->collective_io) {
	if ((mpi_code = MPI_File_read(MPI_dataset->MPI_file, (char*)SDDS_dataset->data[i], n_rows*SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	  SDDS_SetError("Unable to set view for read binary columns");
	  return 0;
	}
      } else {
	if ((mpi_code = MPI_File_read_all(MPI_dataset->MPI_file, (char*)SDDS_dataset->data[i], n_rows*SDDS_type_size[type-1], MPI_BYTE, &status))!=MPI_SUCCESS) {
	  SDDS_SetError("Unable to set view for read binary columns");
	  return 0;
	}
      }
      offset += (MPI_Offset)MPI_dataset->total_rows * SDDS_type_size[type-1];
    }
    MPI_dataset->n_rows = SDDS_dataset->n_rows = n_rows;
    MPI_dataset->file_offset = offset;
  } else {
    /* read row by row */
    if (!fBuffer->buffer) {
      fBuffer->bufferSize  = defaultBufferSize;
      if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
	SDDS_SetError("Unable to do buffered read--allocation failure");
	return 0;
      }
      fBuffer->bytesLeft = 0;
      fBuffer->data[0] = 0; 
    }
    if (fBuffer->bytesLeft>0) {
      /* discard the extra data for reading next page*/
      fBuffer->data[0] = 0; 
      fBuffer->bytesLeft = 0;
    }
    MPI_dataset->file_offset += (MPI_Offset)prev_rows*MPI_dataset->column_offset;
    if ((mpi_code = MPI_File_set_view(MPI_dataset->MPI_file, MPI_dataset->file_offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "Unable to set view for read binary rows", mpi_code, 0);
      SDDS_SetError("Unable to set view for read binary rows");
      return 0;
    }
    if (!MPI_dataset->collective_io || !master_read) {
      for (j=0; j<n_rows; j++) {
	if (!SDDS_MPI_ReadBinaryRow(SDDS_dataset, j, 0)) {
	  SDDS_dataset->n_rows = j - 1;
	  if (SDDS_dataset->autoRecover) {
	    SDDS_ClearErrors();
	    SDDS_SwapEndsColumnData(SDDS_dataset);
	    return(SDDS_dataset->page_number=MPI_dataset->n_page);
	  }
	  SDDS_SetError("Unable to read page--error reading data row (SDDS_MPI_ReadNonNativeBinaryPage)");
	  SDDS_SetReadRecoveryMode(1);
	  return(0);
	}
      }
      SDDS_dataset->n_rows = j;
    } else {
      MPI_dataset->n_rows = SDDS_dataset->n_rows = n_rows;
      if (!SDDS_MPI_CollectiveReadByRow(SDDS_dataset))
	return 0;
    }
    MPI_dataset->file_offset = offset + MPI_dataset->total_rows * MPI_dataset->column_offset;
  }
  SDDS_SwapEndsColumnData(SDDS_dataset);
  MPI_dataset->n_page ++;
  MPI_Barrier(MPI_dataset->comm);
  return(SDDS_dataset->page_number=MPI_dataset->n_page);
}

int32_t SDDS_MPI_BufferedReadNonNativeBinaryTitle(SDDS_DATASET *SDDS_dataset)
{
  SDDS_FILEBUFFER *fBuffer=NULL;
  MPI_DATASET *MPI_dataset=NULL;
  int32_t ret_val;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->titleBuffer);
  if (!fBuffer->buffer) {
    fBuffer->bufferSize = defaultTitleBufferSize;
    if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
      SDDS_SetError("Unable to do buffered read--allocation failure(SDDS_MPI_ReadBinaryTitle)");
      return 0;
    }
    fBuffer->bytesLeft = 0;
  }
  if (fBuffer->bytesLeft>0) {
    /* discard the extra data for reading next page */
    fBuffer->data[0] = 0; 
    fBuffer->bytesLeft = 0;
  }
  if ((ret_val = SDDS_MPI_BufferedRead((void*)&(MPI_dataset->total_rows), sizeof(int32_t), SDDS_dataset, fBuffer))<0)
    return -1;
  SDDS_SwapLong(&(MPI_dataset->total_rows));
  
  if (!ret_val)
    return 0;
  if (!SDDS_StartPage(SDDS_dataset, 0)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_MPI_BufferedReadBinaryTitle)");
    return(0);
  }
  /*read parameters */
  if (!SDDS_MPI_ReadNonNativeBinaryParameters(SDDS_dataset, fBuffer)) {
    SDDS_SetError("Unable to read page--parameter reading error (SDDS_MPI_BufferedReadTitle)");
    return(0);
  }
  /*read arrays */
  if (!SDDS_MPI_ReadNonNativeBinaryArrays(SDDS_dataset, fBuffer)) {
    SDDS_SetError("Unable to read page--array reading error (SDDS_MPI_BufferedReadTitle)");
    return(0);
  }
 
  return 1;
}

/*obtain the offset of n_rows, parameters and arrays which are written by master processor */
MPI_Offset SDDS_MPI_GetTitleOffset(SDDS_DATASET *SDDS_dataset)
{
  int32_t i, j;
  MPI_Offset offset=0;
  SDDS_LAYOUT *layout;
  
  layout = &(SDDS_dataset->layout);
  offset += sizeof(int32_t); /*written n_rows to each page before written parameters and arrays*/ 
  
  for (i=0; i<layout->n_parameters; i++) {
    if (layout->parameter_definition[i].fixed_value)
      continue;
    if (layout->parameter_definition[i].type==SDDS_STRING) {
      if (*(char**)SDDS_dataset->parameter[i]) 
        offset += sizeof(int32_t) + sizeof(char)*strlen(*(char**)SDDS_dataset->parameter[i]);
      else
        offset += sizeof(int32_t); /*write length 0 for NULL string */
    } else {
      offset += SDDS_type_size[layout->parameter_definition[i].type-1];
    }
  }
  for (i=0; i<layout->n_arrays; i++) {
    if (!(SDDS_dataset->array[i].dimension)) {
      offset += layout->array_definition[i].dimensions * sizeof(int32_t);
      continue;
    }
    offset += sizeof(*(SDDS_dataset->array)[i].dimension)* layout->array_definition[i].dimensions;
    if (layout->array_definition[i].type==SDDS_STRING) {
      for (j=0; j<SDDS_dataset->array[i].elements; j++) {
        if (((char**)SDDS_dataset->array[i].data)[j]) 
          offset += sizeof(int32_t) + sizeof(char)*strlen(((char**)SDDS_dataset->array[i].data)[j]);
        else
          offset += sizeof(int32_t);
      }
    } else {
      offset += SDDS_type_size[layout->array_definition[i].type-1]* SDDS_dataset->array[i].elements;
    }
  }
  return offset;
}

/*use a small size of buffer to read the total_rows, parameters and arrays of each page */
int32_t SDDS_MPI_BufferedReadBinaryTitle(SDDS_DATASET *SDDS_dataset)
{
  SDDS_FILEBUFFER *fBuffer=NULL;
  MPI_DATASET *MPI_dataset=NULL;
  int32_t ret_val;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->titleBuffer);
  if (!fBuffer->buffer) {
    fBuffer->bufferSize = defaultTitleBufferSize;
    if (!(fBuffer->buffer = fBuffer->data = SDDS_Malloc(sizeof(char)*(fBuffer->bufferSize+1)))) {
      SDDS_SetError("Unable to do buffered read--allocation failure(SDDS_MPI_ReadBinaryTitle)");
      return 0;
    }
    fBuffer->bytesLeft = 0;
  }
  if (fBuffer->bytesLeft>0) {
    /* discard the extra data for reading next page */
    fBuffer->data[0] = 0; 
    fBuffer->bytesLeft = 0;
  }
  if ((ret_val = SDDS_MPI_BufferedRead((void*)&(MPI_dataset->total_rows), sizeof(int32_t), SDDS_dataset, fBuffer))<0)
    return -1;
  if (!ret_val)
    return 0;
  if (!SDDS_StartPage(SDDS_dataset, 0)) {
    SDDS_SetError("Unable to read page--couldn't start page (SDDS_MPI_BufferedReadBinaryTitle)");
    return(0);
  }
  /*read parameters */
  if (!SDDS_MPI_ReadBinaryParameters(SDDS_dataset, fBuffer)) {
    SDDS_SetError("Unable to read page--parameter reading error (SDDS_MPI_BufferedReadTitle)");
    return(0);
  }
  /*read arrays */
  if (!SDDS_MPI_ReadBinaryArrays(SDDS_dataset, fBuffer)) {
    SDDS_SetError("Unable to read page--array reading error (SDDS_MPI_BufferedReadTitle)");
    return(0);
  }
 
  return 1;
}

int32_t SDDS_MPI_CollectiveWriteByRow(SDDS_DATASET *SDDS_dataset)
{
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  int32_t writeBytes, mpi_code, n_rows, i, j, type, min_rows, size;
  SDDS_FILEBUFFER *fBuffer;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  layout = &(SDDS_dataset->layout);
  fBuffer = &(SDDS_dataset->fBuffer);
  n_rows = SDDS_dataset->n_rows;
  MPI_Allreduce(&n_rows, &min_rows, 1, MPI_INT, MPI_MIN, MPI_dataset->comm);
  for (i=0; i<min_rows; i++) {
    for (j=0; j<layout->n_columns; j++) {
      type = layout->column_definition[j].type;
      size = SDDS_type_size[type-1];
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not write binary string in collective io.");
	return 0;
      }
      if (!SDDS_MPI_BufferedWriteAll((char*)SDDS_dataset->data[j]+i*size, size, SDDS_dataset))
	return 0;
    }
  }
 
  if ((writeBytes = fBuffer->bufferSize - fBuffer->bytesLeft)) {
    if (writeBytes<0) {
      SDDS_SetError("Unable to flush buffer: negative byte count (SDDS_FlushBuffer).");
      return 0;
    }
    if ((mpi_code=MPI_File_write_all(MPI_dataset->MPI_file,  fBuffer->buffer, writeBytes,  MPI_BYTE, 
				     MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_FlushBuffer(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
  }
  
  for (i=min_rows; i<n_rows; i++) 
    if (!SDDS_MPI_WriteBinaryRow(SDDS_dataset,  i))  
      return 0;
  if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
    return 0;
  return 1;
}

int32_t SDDS_MPI_CollectiveWriteNonNativeByRow(SDDS_DATASET *SDDS_dataset)
{
  MPI_DATASET *MPI_dataset;
  SDDS_LAYOUT *layout;
  int32_t writeBytes, mpi_code, n_rows, i, j, type, min_rows, size;
  SDDS_FILEBUFFER *fBuffer;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  layout = &(SDDS_dataset->layout);
  fBuffer = &(SDDS_dataset->fBuffer);
  n_rows = SDDS_dataset->n_rows;
  MPI_Allreduce(&n_rows, &min_rows, 1, MPI_INT, MPI_MIN, MPI_dataset->comm);
  for (i=0; i<min_rows; i++) {
    for (j=0; j<layout->n_columns; j++) {
      type = layout->column_definition[j].type;
      size = SDDS_type_size[type-1];
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not write binary string in collective io.");
	return 0;
      }
      if (!SDDS_MPI_BufferedWriteAll((char*)SDDS_dataset->data[j]+i*size, size, SDDS_dataset))
	return 0;
    }
  }
 
  if ((writeBytes = fBuffer->bufferSize - fBuffer->bytesLeft)) {
    if (writeBytes<0) {
      SDDS_SetError("Unable to flush buffer: negative byte count (SDDS_FlushBuffer).");
      return 0;
    }
    if ((mpi_code=MPI_File_write_all(MPI_dataset->MPI_file,  fBuffer->buffer, writeBytes,  MPI_BYTE, 
				     MPI_STATUS_IGNORE))!=MPI_SUCCESS) {
      SDDS_MPI_GOTO_ERROR(stderr, "SDDS_MPI_FlushBuffer(MPI_File_write_at failed)", mpi_code, 0);
      return 0;
    }
    fBuffer->bytesLeft = fBuffer->bufferSize;
    fBuffer->data = fBuffer->buffer;
  }
  
  for (i=min_rows; i<n_rows; i++) 
    if (!SDDS_MPI_WriteNonNativeBinaryRow(SDDS_dataset,  i))  
      return 0;
  if (!SDDS_MPI_FlushBuffer(SDDS_dataset))
    return 0;
  return 1;
}

int32_t SDDS_MPI_CollectiveReadByRow(SDDS_DATASET *SDDS_dataset)
{
  
  SDDS_FILEBUFFER *fBuffer;
  MPI_DATASET *MPI_dataset;
  int32_t  min_rows, i, j, type, size;
  SDDS_LAYOUT *layout;
  
  MPI_dataset = SDDS_dataset->MPI_dataset;
  fBuffer = &(SDDS_dataset->fBuffer);
  layout = &(SDDS_dataset->layout);
 
  if (!MPI_dataset->master_read) {
    SDDS_SetError("Cannot read row with collective io when master is not reading the data.");
    return 0;
  }
  min_rows = MPI_dataset->total_rows/MPI_dataset->n_processors;
  for (i=0; i<min_rows; i++) {
    for (j=0; j<layout->n_columns; j++) {
      type = layout->column_definition[j].type;
      size = SDDS_type_size[type-1];
      if (type==SDDS_STRING) {
	SDDS_SetError("Can not write binary string in collective io.");
	return 0;
      }
      if (!SDDS_MPI_BufferedReadAll((char*)SDDS_dataset->data[j]+i*size, size, SDDS_dataset, fBuffer))
	return 0;
    }
  }
  for (i=min_rows; i<MPI_dataset->n_rows; i++) 
    if (!SDDS_MPI_ReadBinaryRow(SDDS_dataset,  i, 0))  
      return 0;
  
  return 1;
}
