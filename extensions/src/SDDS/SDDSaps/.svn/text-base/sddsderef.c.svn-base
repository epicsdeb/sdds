/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsderef.c
 * purpose: do array dereferencing into column or parameter data.
 * M. Borland, 1995
 $Log: not supported by cvs2svn $
 Revision 1.14  2006/10/19 17:55:39  soliday
 Updated to work with linux-x86_64.

 Revision 1.13  2005/11/07 21:48:10  soliday
 Updated to remove Linux compiler warnings.

 Revision 1.12  2005/11/04 22:46:13  soliday
 Updated code to be compiled by a 64 bit processor.

 Revision 1.11  2002/08/14 17:12:42  soliday
 Added Open License

 Revision 1.10  2002/01/16 17:54:20  soliday
 Added missing SDDS_Terminate functions.

 Revision 1.9  2001/01/10 19:35:34  soliday
 Standardized usage message.

 Revision 1.8  1999/05/25 19:07:50  soliday
 Removed compiler warning on linux.

 Revision 1.7  1999/01/06 19:54:41  borland
 Fixed the version number in the usage message.

 * Revision 1.6  1996/06/26  20:25:40  borland
 * Added better tests and error messages for out-of-bounds conditions.
 *
 * Revision 1.5  1996/04/02  17:04:32  borland
 * Fixed test of the number of required arguments for column derefence.
 *
 * Revision 1.4  1996/02/14  18:20:20  borland
 * Fixed parsing bug introduced by switch to scanItemList().
 *
 * Revision 1.3  1996/02/14  01:05:15  borland
 * Changed over from scan_item_list() to scanItemList().
 *
 * Revision 1.2  1995/11/13  15:55:14  borland
 * Added features for derefencing of columns or arrays, for dereferencing with
 * constant indices, and for control of behavior with indices are out of bounds.
 *
 * Revision 1.1  1995/11/08  23:59:53  borland
 * First version.
 *
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
 
#define SET_COLUMN 0
#define SET_PARAMETER 1
#define SET_PIPE 2
#define SET_OUTOFBOUNDS 3
#define SET_CONSTANT 4
#define N_OPTIONS 5

static char *optionName[N_OPTIONS] = {
    "column", "parameter", "pipe", "outofbounds", "constant",
    };

#define COLUMN_CLASS 0
#define PARAMETER_CLASS 1
#define ARRAY_CLASS 2
#define CONSTANT_CLASS 3
#define CLASS_NAMES 4
/*
static char *className[CLASS_NAMES] = {
    "column", "parameter", "array", "constant",
    } ;
*/
static char *USAGE="sddsderef [<inputfile>] [<outputfile>]\n\
[-pipe=[input][,output]]\n\
[-column=<newName>,{arraySource|columnSource}=<name>,<indexColumnName>[,...]]\n\
[-parameter=<newName>,{arraySource|columnSource}=<name>,<indexParameterName>[,...]]\n\
[-constant=<newName>,{arraySource|columnSource}=<name>,<indexValue>[,<indexValue>]]\n\
[-outOfBounds={exit|delete}]\n\n\
Program by Michael Borland. (This is version 2, June 1996.)\n";

typedef struct {
    char *target, *source, **indexElement;
    long targetIndex, indexElements, targetClass, sourceClass, sourceIndex;
    long *indexValue;
    } DEREF_REQUEST;

long DereferenceColumnWithParameter(SDDS_DATASET *outSet,  
    DEREF_REQUEST *deref, unsigned long outOfBounds);
long DereferenceColumnWithColumn(SDDS_DATASET *outSet,  
    DEREF_REQUEST *deref, unsigned long outOfBounds);
long DereferenceArrayWithParameter(SDDS_DATASET *outSet,  
    DEREF_REQUEST *deref, unsigned long outOfBounds);
long DereferenceArrayWithColumn(SDDS_DATASET *outSet,  
    DEREF_REQUEST *deref, unsigned long outOfBounds);
long DoDereferencing(SDDS_DATASET *outSet,  
    DEREF_REQUEST *deref, long derefs, unsigned long outOfBounds);
void setupOutputFile(SDDS_DATASET *outSet, char *output, SDDS_DATASET *inSet, 
                     char *input, DEREF_REQUEST *deref, long derefs);
void addDerefRequest(DEREF_REQUEST **deref, long *derefs, char **argv, long argc,
                     long class);

#define EXIT_OUTBOUNDS   0x1U
#define DELETE_OUTBOUNDS 0x2U

int main(int argc, char **argv)
{
    SDDS_DATASET inSet, outSet;
    SCANNED_ARG *s_arg;
    long i_arg;
    char *input, *output;
    long derefRequests, code;
    unsigned long outOfBounds;
    DEREF_REQUEST *derefRequest;
    unsigned long pipeFlags;

    SDDS_RegisterProgramName(argv[0]);
    argc = scanargs(&s_arg, argc, argv);
    if (argc<2)
        bomb(NULL, USAGE);

    input = output = NULL;
    derefRequests = 0;
    derefRequest = NULL;
    pipeFlags = 0;
    outOfBounds = EXIT_OUTBOUNDS;

    for (i_arg=1; i_arg<argc; i_arg++) {
        if (s_arg[i_arg].arg_type==OPTION) {
            switch (match_string(s_arg[i_arg].list[0], optionName, N_OPTIONS, 0)) {
              case SET_COLUMN:
                if (s_arg[i_arg].n_items<4)
                    SDDS_Bomb("invalid -column syntax");
                addDerefRequest(&derefRequest, &derefRequests, s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, COLUMN_CLASS);
                break;
              case SET_PARAMETER:
                if (s_arg[i_arg].n_items<4)
                    SDDS_Bomb("invalid -parameter syntax");
                addDerefRequest(&derefRequest, &derefRequests, s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, PARAMETER_CLASS);
                break;
              case SET_CONSTANT:
                if (s_arg[i_arg].n_items<4)
                    SDDS_Bomb("invalid -constant syntax");
                addDerefRequest(&derefRequest, &derefRequests, s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, CONSTANT_CLASS);
                break;
              case SET_PIPE:
                if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
                    SDDS_Bomb("invalid -pipe syntax");
                break;
              case SET_OUTOFBOUNDS:
                s_arg[i_arg].n_items -= 1;
                outOfBounds = 0;
                if (s_arg[i_arg].n_items!=1 ||
                    !scanItemList(&outOfBounds, s_arg[i_arg].list+1,
                                    &s_arg[i_arg].n_items, 0,
                                    "exit", -1, NULL, 0, EXIT_OUTBOUNDS,
                                    "delete", -1, NULL, 0, DELETE_OUTBOUNDS,
                                    NULL) ||
                    !outOfBounds) {
                    SDDS_Bomb("invalid -outOfBounds syntax/values");
                    }
                break;
              default:
                fprintf(stderr, "error: unknown/ambiguous option: %s\n", s_arg[i_arg].list[0]);
                exit(1);
                break;
                }
            }
        else {
            if (input==NULL) 
                input = s_arg[i_arg].list[0];
            else if (output==NULL)
                output = s_arg[i_arg].list[0];
            else
                SDDS_Bomb("too many filenames");
            }
        }
    
    processFilenames("sddsderef", &input, &output, pipeFlags, 0, NULL);
    
    setupOutputFile(&outSet, output, &inSet, input, derefRequest, 
                    derefRequests);
    
    while ((code=SDDS_ReadPage(&inSet))>0) {
        if (!SDDS_CopyPage(&outSet, &inSet))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        if (!DoDereferencing(&outSet, derefRequest, derefRequests, outOfBounds))
            SDDS_Bomb("problem doing dereferencing");
        if (!SDDS_WritePage(&outSet))
            SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        }
    if (code==0)
        SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
    if (!SDDS_Terminate(&inSet) || !SDDS_Terminate(&outSet)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    return(0);
}

void setupOutputFile(SDDS_DATASET *outSet, char *output, SDDS_DATASET *inSet, 
                     char *input, DEREF_REQUEST *deref, long derefs)
{
  long i, j;
  if (!SDDS_InitializeInput(inSet, input) ||
      !SDDS_InitializeCopy(outSet, inSet, output, "w"))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
  for (i=0; i<derefs; i++) {
    switch (deref[i].sourceClass) {
    case ARRAY_CLASS:
      if (SDDS_GetArrayIndex(outSet, deref[i].source)<0) {
        fprintf(stderr, "error (sddsderef): no array %s in input\n",
                deref[i].source);
        exit(1);
      }
      break;
    case COLUMN_CLASS:
      if (SDDS_GetColumnIndex(outSet, deref[i].source)<0) {
        fprintf(stderr, "error (sddsderef): no column %s in input\n",
                deref[i].source);
        exit(1);
      }
      break;
    default:
      SDDS_Bomb("invalid source class in setupOutputFile (coding error)");
      break;
    }
    switch (deref[i].targetClass) {
    case PARAMETER_CLASS:
    case CONSTANT_CLASS:
      switch (deref[i].sourceClass) {
      case ARRAY_CLASS:
        if (!SDDS_DefineParameterLikeArray(outSet, outSet, deref[i].source,
                                           deref[i].target))
          SDDS_PrintErrors(stderr, 
                           SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        deref[i].sourceIndex = SDDS_GetArrayIndex(outSet, deref[i].source);
        break;
      case COLUMN_CLASS:
        if (!SDDS_DefineParameterLikeColumn(outSet, outSet, deref[i].source,
                                            deref[i].target))
          SDDS_PrintErrors(stderr, 
                           SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        deref[i].sourceIndex = SDDS_GetColumnIndex(outSet, deref[i].source);
        break;
      }
      deref[i].targetIndex = SDDS_GetParameterIndex(outSet, deref[i].target);
      if (deref[i].targetClass==PARAMETER_CLASS) {
        for (j=0; j<deref[i].indexElements; j++) {
          if (SDDS_GetParameterIndex(outSet, deref[i].indexElement[j])<0) {
            fprintf(stderr, "error (sddsderef): no parameter %s in input\n",
                    deref[i].indexElement[j]);
            exit(1);
          }
        }
      }
      break;
    case COLUMN_CLASS:
      switch (deref[i].sourceClass) {
      case ARRAY_CLASS:
        if (!SDDS_DefineColumnLikeArray(outSet, outSet, deref[i].source,
                                        deref[i].target))
          SDDS_PrintErrors(stderr, 
                           SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        deref[i].sourceIndex = SDDS_GetArrayIndex(outSet, deref[i].source);
        break;
      case COLUMN_CLASS:
        if (!SDDS_TransferColumnDefinition(outSet, outSet, deref[i].source,
                                           deref[i].target))
          SDDS_PrintErrors(stderr, 
                           SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
        deref[i].sourceIndex = SDDS_GetColumnIndex(outSet, deref[i].source);
        break;
      }
      deref[i].targetIndex = SDDS_GetColumnIndex(outSet, deref[i].target);
      for (j=0; j<deref[i].indexElements; j++) {
        if (SDDS_GetColumnIndex(outSet, deref[i].indexElement[j])<0) {
          fprintf(stderr, "error (sddsderef): no column %s in input\n",
                  deref[i].indexElement[j]);
          exit(1);
        }
      }
      break;
    default:
      fprintf(stderr, "error (sddsderef): invalid target data class (internal error)\n");
      exit(1);
      break;
    }
  }
  if (!SDDS_WriteLayout(outSet))
    SDDS_PrintErrors(stderr, SDDS_EXIT_PrintErrors|SDDS_VERBOSE_PrintErrors);
}

void addDerefRequest(DEREF_REQUEST **deref, long *derefs, char **argv, long argc,
                     long class)
{
    long i, items;
    unsigned long flags;
#define ARRAY_SOURCE  0x1U
#define COLUMN_SOURCE 0x2U
    if (argc<3) {
        fprintf(stderr, "error (sddsderef): too few values for -%s\n",
                optionName[class]);
        exit(1);
        }
    if (!(*deref=SDDS_Realloc(*deref, sizeof(**deref)*(*derefs+1))))
        SDDS_Bomb("memory allocation failure (addDerefRequest)");

    /* Store the first non-optional argument */
    (*deref)[*derefs].targetClass = class;
    (*deref)[*derefs].target = argv[0];
    argv++;
    argc--;

    /* parse the second argument (key=value form) */
    items = 1;
    if (!scanItemList(&flags, argv, &items,  0,
                        "arraySource", SDDS_STRING, &(*deref)[*derefs].source, 1, ARRAY_SOURCE,
                        "columnSource", SDDS_STRING, &(*deref)[*derefs].source, 1, COLUMN_SOURCE,
                        NULL) || !flags || flags==(ARRAY_SOURCE|COLUMN_SOURCE)) {
        fprintf(stderr, "error (sddsderef): one of arraySource or columnSource must be given with -column or -parameter\n");
        exit(1);
        }
    if (flags&ARRAY_SOURCE)
        (*deref)[*derefs].sourceClass = ARRAY_CLASS;
    else {
        (*deref)[*derefs].sourceClass = COLUMN_CLASS;
        if (argc>2) {
            fprintf(stderr, "error (sddsderef): too many indices for column dereference\n");
            exit(1);
            }
        }

    /* parse the remaining list of arguments (names of data elements) */
    argv++;
    argc--;
    (*deref)[*derefs].indexElements = argc;
    (*deref)[*derefs].indexElement = NULL;
    (*deref)[*derefs].indexValue = NULL;
    if (class==CONSTANT_CLASS) {
        if (!((*deref)[*derefs].indexValue = (long*)malloc(sizeof(long)*argc))) {
            SDDS_Bomb("memory allocation failure (addDerefRequest)");
            }
        for (i=0; i<argc; i++) {
            if (sscanf(argv[i], "%ld", (*deref)[*derefs].indexValue+i)!=1)
                SDDS_Bomb("constant doesn't contain scannable integer");
            }
        }
    else {
        if (!((*deref)[*derefs].indexElement 
              = (char**)malloc(sizeof(*(*deref)[*derefs].indexElement)*argc)))
            SDDS_Bomb("memory allocation failure (addDerefRequest)");
        for (i=0; i<argc; i++)
            (*deref)[*derefs].indexElement[i] = argv[i];
        }
    *derefs += 1;
    }


long DoDereferencing(SDDS_DATASET *outSet, 
                     DEREF_REQUEST *deref, long derefs, unsigned long outOfBounds)
{
    long i;
    for (i=0; i<derefs; i++) {
        switch (deref[i].targetClass) {
          case PARAMETER_CLASS:
          case CONSTANT_CLASS:
            switch (deref[i].sourceClass) {
              case ARRAY_CLASS:
                if (!DereferenceArrayWithParameter(outSet,  deref+i, outOfBounds))
                    return 0;
                break;
              case COLUMN_CLASS:
                if (!DereferenceColumnWithParameter(outSet,  deref+i, outOfBounds))
                    return 0;
                break;
              default:
                SDDS_Bomb("invalid source class--code error (DoDereferencing)");
                break;
                }
            break;
          case COLUMN_CLASS:
            switch (deref[i].sourceClass) {
              case ARRAY_CLASS:
                if (!DereferenceArrayWithColumn(outSet,  deref+i, outOfBounds))
                    return 0;
                break;
              case COLUMN_CLASS:
                if (!DereferenceColumnWithColumn(outSet,  deref+i, outOfBounds))
                    return 0;
                break;
              default:
                SDDS_Bomb("invalid source class--code error (DoDereferencing)");
                break;
                }
            break;
          default:
            SDDS_Bomb("invalid target class--code error (DoDereferencing)");
            break;
            }
        }
    return 1;
    }

long DereferenceColumnWithParameter(SDDS_DATASET *outSet, DEREF_REQUEST *deref, unsigned long outOfBounds)
{
  double data[4];
  long offset, size, rows, type;
  double value;
  int32_t zeroValue = 0;
  static char *blankString = "";

  if (!(rows=outSet->n_rows))
    return 1;
  size = SDDS_GetTypeSize(type=SDDS_GetColumnType(outSet, deref->sourceIndex));
  if (deref->targetClass==CONSTANT_CLASS) {
    value = deref->indexValue[0];
  }
  else if (!SDDS_GetParameterAsDouble(outSet, deref->indexElement[0], &value)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  offset = ((long)value);
  if (offset<0 || offset>=rows) {
    if (outOfBounds&EXIT_OUTBOUNDS){
      char s[1024];
      sprintf(s, "column index out of bounds: column %s, length %ld, offset %ld\n  %s=%ld",
              deref->source, rows, offset, deref->indexElement[0], offset);
      SDDS_Bomb(s);
    }
    if (type==SDDS_STRING)
      memcpy((char*)data, (char*)&blankString, sizeof(blankString));
    else
      SDDS_CastValue((void*)&zeroValue, 0, SDDS_LONG, type, (void*)data);
  }
  else  {
    if (!SDDS_GetValueByAbsIndex(outSet, deref->sourceIndex, offset, (void*)data)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return 0;
    }
  }

  if (!SDDS_SetParameters(outSet, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,
                          deref->targetIndex, (void*)data, 
                          -1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  return 1;
}

long DereferenceArrayWithParameter(SDDS_DATASET *outSet, DEREF_REQUEST *deref, unsigned long outOfBounds)
{
  SDDS_ARRAY *array;
  long i, j, offset, size;
  double value;
  int32_t zeroValue = 0;
  static char *blankString = "";
  double data[4];

  if (!(array = SDDS_GetArray(outSet, deref->source, NULL))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  if (deref->indexElements!=array->definition->dimensions) {
    fprintf(stderr, 
            "error: number of index elements (%ld) doesn't match number of array dimensions (%" PRId32 ") for array %s\n",
            deref->indexElements, array->definition->dimensions, 
            deref->source);
    return 0;
  }
  size = SDDS_GetTypeSize(array->definition->type);
  offset = 0;
  for (i=0, j=deref->indexElements-1; i<deref->indexElements; i++, j--) {
    if (deref->targetClass==CONSTANT_CLASS) {
      value = deref->indexValue[i];
    }
    else {
      /* get index values from parameters */
      if (!SDDS_GetParameterAsDouble(outSet, deref->indexElement[i], &value)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return 0;
      }
    }
    offset = ((long)value) + offset*array->dimension[j];
  }
  if (offset<0 || offset>=array->elements) {
    if (outOfBounds&EXIT_OUTBOUNDS) {
      char s[1024];
      sprintf(s, "array index out of bounds: array %s, length %" PRId32 ", offset %ld\n  %s=%ld",
              deref->source, array->elements, offset, deref->indexElement[0], offset);
      SDDS_Bomb(s);
    }
    if (array->definition->type==SDDS_STRING)
      memcpy((char*)data, (char*)&blankString, sizeof(blankString));
    else 
      SDDS_CastValue((void*)&zeroValue, 0, SDDS_LONG, 
                     array->definition->type, (void*)data);
    if (!SDDS_SetParameters(outSet, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,
                            deref->targetIndex, (void*)data, 
                            -1)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return 0;
    }
  }
  else if (!SDDS_SetParameters(outSet, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,
                               deref->targetIndex, (char*)array->data+offset*size,
                               -1)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  SDDS_FreeArray(array);
  return 1;
}

long DereferenceArrayWithColumn(SDDS_DATASET *outSet, DEREF_REQUEST *deref, unsigned long outOfBounds)
{
  SDDS_ARRAY *array;
  long i, offset, size, index, rows, row;
  int32_t **indexData;

  if (!(rows=outSet->n_rows))
    return 1;
  if (!(array = SDDS_GetArray(outSet, deref->source, NULL))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  if (deref->indexElements!=array->definition->dimensions) {
    fprintf(stderr, 
            "error: number of index elements (%ld) doesn't match number of array dimensions (%" PRId32 ") for array %s\n",
            deref->indexElements, array->definition->dimensions, 
            deref->source);
    return 0;
  }
  size = SDDS_GetTypeSize(array->definition->type);
  offset = 0;

  if (!(indexData = (int32_t**)malloc(sizeof(*indexData)*deref->indexElements)))
    SDDS_Bomb("memory allocation failure while derefencing column data");
  for (i=0; i<deref->indexElements; i++)
    if (!(indexData[i] 
          = (int32_t*)SDDS_GetNumericColumn(outSet, deref->indexElement[i], SDDS_LONG))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return 0;
    }
  for (row=0; row<rows; row++) {
    for (i=offset=0; i<deref->indexElements; i++) {
      /* get index values from columns */
      index = indexData[i][row];
      if (index<0 || index>array->dimension[i]) {
        offset = -1;
        break;
      }
      offset = index + offset*array->dimension[i];
    }
    
    if (offset<0 || offset>=array->elements) {
      if (outOfBounds&EXIT_OUTBOUNDS) {
        char s[1024], t[1024];
        sprintf(s, "array index out of bounds: array %s, length %" PRId32 ", offset %ld\n",
                deref->source, array->elements, offset);
        for (i=0; i<deref->indexElements; i++) {
          sprintf(t, "  %s=%" PRId32 " (dim=%" PRId32 ")", deref->indexElement[i], indexData[i][row],
                  array->dimension[i]);
          strcat(s, t);
        }
        SDDS_Bomb(s);
      }
      if (!SDDS_AssertRowFlags(outSet, SDDS_INDEX_LIMITS, row, row, 0)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return 0;
      }
    }
    else if (!SDDS_SetRowValues(outSet, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE,
                                row, deref->targetIndex, 
                                (char*)array->data+offset*size,
                                -1)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return 0;
    }
  }
  for (i=0; i<deref->indexElements; i++)
    free(indexData[i]);
  free(indexData);
  SDDS_FreeArray(array);
  return 1;
}

long DereferenceColumnWithColumn(SDDS_DATASET *outSet, DEREF_REQUEST *deref, unsigned long outOfBounds)
{
  double data[4];
  long offset, rows, row, type;
  long *indexData;

  if (!(rows=outSet->n_rows)) {
    return 1;
  }
  offset = 0;
  if (!(indexData
        = (long*)SDDS_GetNumericColumn(outSet, deref->indexElement[0], SDDS_LONG))) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return 0;
  }
  type = SDDS_GetColumnType(outSet, deref->sourceIndex);

  for (row=0; row<rows; row++) {
    offset = indexData[row];
    if (offset<0 || offset>=rows) {
      if (outOfBounds&EXIT_OUTBOUNDS) {
        char s[1024];
        sprintf(s, "column index out of bounds: column %s, length %ld, offset %ld\n  %s=%ld\n",
                deref->source, rows, offset, deref->indexElement[0], offset);
        SDDS_Bomb(s);
      }
      if (!SDDS_AssertRowFlags(outSet, SDDS_INDEX_LIMITS, row, row, 0)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return 0;
      }
    }
    else if (!SDDS_GetValueByAbsIndex(outSet, deref->sourceIndex, offset, (void*)data) ||
             !SDDS_SetRowValues(outSet, SDDS_SET_BY_INDEX|SDDS_PASS_BY_REFERENCE, row, 
                                deref->targetIndex, (void*)data, 
                                -1)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return 0;
    }
  }
  free(indexData);
  return 1;
}


