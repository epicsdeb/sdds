/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 * $Log: sddsendian.c,v $
 * Revision 1.10  2004/11/15 17:15:40  soliday
 * Moved and renamed the procedures to write non native binary data from
 * sddsendian to SDDSlib.
 *
 * Revision 1.9  2003/09/02 19:16:03  soliday
 * Cleaned up code for Linux.
 *
 * Revision 1.8  2002/08/14 17:12:44  soliday
 * Added Open License
 *
 * Revision 1.7  2001/01/10 19:35:36  soliday
 * Standardized usage message.
 *
 * Revision 1.6  2000/09/19 20:15:37  borland
 * Compatible with comment-based indication of little or big endian order.
 *
 * Revision 1.5  2000/06/20 14:30:46  soliday
 * Added the ability to convert non-native byte ordering into native byte
 * ordering. Also fixed problem when creating .gz files.
 *
 * Revision 1.4  1999/06/04 15:28:30  soliday
 * Fixed bug with SwapEndsParameterData
 *
 * Revision 1.3  1999/05/25 19:08:43  soliday
 * Removed compiler warning on linux.
 *
 * Revision 1.2  1998/08/25 01:45:09  borland
 * This version actually works.
 *
 * Revision 1.1  1998/08/06 18:38:24  borland
 * First version, minimally tested.
 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"

#define SET_PIPE 0
#define NONNATIVE 1
#define N_OPTIONS 2

char *option[N_OPTIONS] = {
    "pipe", "nonNative",
    } ;

char *USAGE = "sddsendian [<input>] [<output>]\n\
[-pipe=[input][,output]]\n\
[-nonNative]\n\n\
Converts from big-endian to little-endian or from little-endian\n\
to big-endian.\n\
\n\
-nonNative    This option is used when the file's byte order is not the\n\
              native byte order of the operating system.\n\
Program by Michael Borland and Robert Soliday.\n\
(This is version 1.5, June 2000.)\n";

int main(int argc, char **argv)
{
  SDDS_DATASET SDDSin, SDDSout;
  long i_arg, tmpfileUsed;
  SCANNED_ARG *s_arg;
  char *input, *output, *description_text, *description_contents;
  unsigned long pipeFlags;
  long pageNumber, nonNative = 0;
  char *outputEndianess = NULL;
  char buffer[40];
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&s_arg, argc, argv);
  if (argc<2) 
    bomb(NULL, USAGE);

  input = output = description_text = description_contents = NULL;
  pipeFlags = 0;
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (s_arg[i_arg].arg_type==OPTION) {
      delete_chars(s_arg[i_arg].list[0], "_");
      switch (match_string(s_arg[i_arg].list[0], option, N_OPTIONS, 0)) {
      case SET_PIPE:
        if (!processPipeOption(s_arg[i_arg].list+1, s_arg[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case NONNATIVE:
	nonNative = 1;
        break;
      default:
        fprintf(stderr, "Error (%s): unknown switch: %s\n", argv[0], s_arg[i_arg].list[0]);
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

  processFilenames("sddsendian", &input, &output, pipeFlags, 0, &tmpfileUsed);

  outputEndianess = getenv("SDDS_OUTPUT_ENDIANESS");

  if (outputEndianess) {
    putenv("SDDS_OUTPUT_ENDIANESS=");
  }

  if (!SDDS_InitializeInput(&SDDSin, input)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (!description_text)
    SDDS_GetDescription(&SDDSin, &description_text, &description_contents);
  if (!SDDS_InitializeCopy(&SDDSout, &SDDSin, output, "w") || 
      !SDDS_SetDataMode(&SDDSout, nonNative?SDDS_BINARY:-SDDS_BINARY) ||
      !SDDS_WriteLayout(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  if (nonNative) {
    while ((pageNumber=SDDS_ReadNonNativePage(&SDDSin))>=0) {
      if (!SDDS_CopyPage(&SDDSout, &SDDSin) ||
          !SDDS_WritePage(&SDDSout)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    }
  } else {
    while ((pageNumber=SDDS_ReadPage(&SDDSin))>=0) {
      if (!SDDS_CopyPage(&SDDSout, &SDDSin) ||
          !SDDS_WriteNonNativeBinaryPage(&SDDSout)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    }
  }

  if (!SDDS_Terminate(&SDDSin) || !SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  if (tmpfileUsed && !replaceFileAndBackUp(input, output))
    exit(1);

  if (outputEndianess) {
    sprintf(buffer, "SDDS_OUTPUT_ENDIANESS=%s", outputEndianess);
    putenv(buffer);
  }

  return(0);
}

