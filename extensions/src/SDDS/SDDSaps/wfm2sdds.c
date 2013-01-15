/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/*
 *
 $Log: not supported by cvs2svn $
 Revision 1.6  2009/01/19 22:42:05  soliday
 Added support for FastFrames (multiple waveforms)

 Revision 1.5  2008/11/13 15:47:54  soliday
 Fixed an issue on 64bit machines.

 Revision 1.4  2006/12/14 22:22:01  soliday
 Updated a bunch of programs because SDDS_SaveLayout is now called by
 SDDS_WriteLayout and it is no longer required to be called directly.
 Also the AutoCheckMode is turned off by default now so I removed calls to
 SDDS_SetAutoCheckMode that would attempt to turn it off. It is now up to
 the programmer to turn it on in new programs until debugging is completed
 and then remove the call to SDDS_SetAutoCheckMode.

 Revision 1.3  2006/02/13 21:36:34  soliday
 Added support for WIN32.

 Revision 1.2  2006/02/13 16:04:34  soliday
 Added options to include an Index column. Also added options to output
 the data in float format instead of double format.

 Revision 1.1  2006/02/10 21:26:34  soliday
 Wrote program to convert binary WFM data from Tektronix TDS scopes into SDDS data.


 *
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#if defined(__BORLANDC__)
#define _setmode(handle, amode) setmode(handle, amode)
#endif
#endif


void swapdouble(double *l);
void swapfloat(float *l);
void swaplongLong(uint64_t *l);
void swaplong(uint32_t *l);
void swapshort(short *l);

void swaplonglong(uint64_t *l)
{
    unsigned char  *cp = (unsigned char *) l;
    unsigned char   temp;

    temp = cp[0];
    cp[0] = cp[7];
    cp[7] = temp;
    temp = cp[1];
    cp[1] = cp[6];
    cp[6] = temp;
    temp = cp[2];
    cp[2] = cp[5];
    cp[5] = temp;
    temp = cp[3];
    cp[3] = cp[4];
    cp[4] = temp;
}

void swapdouble(double *l)
{
    unsigned char  *cp = (unsigned char *) l;
    unsigned char   temp;

    temp = cp[0];
    cp[0] = cp[7];
    cp[7] = temp;
    temp = cp[1];
    cp[1] = cp[6];
    cp[6] = temp;
    temp = cp[2];
    cp[2] = cp[5];
    cp[5] = temp;
    temp = cp[3];
    cp[3] = cp[4];
    cp[4] = temp;
}

void swapfloat(float *l)
{
    unsigned char  *cp = (unsigned char *) l;
    unsigned char   temp;

    temp = cp[0];
    cp[0] = cp[3];
    cp[3] = temp;
    temp = cp[1];
    cp[1] = cp[2];
    cp[2] = temp;
}

void swaplong(uint32_t *l)
{
    unsigned char  *cp = (unsigned char *) l;
    unsigned char   temp;

    temp = cp[0];
    cp[0] = cp[3];
    cp[3] = temp;
    temp = cp[1];
    cp[1] = cp[2];
    cp[2] = temp;
}

void swapshort(short *l)
{
    unsigned char  *cp = (unsigned char *) l;
    unsigned char   temp;

    temp = cp[0];
    cp[0] = cp[1];
    cp[1] = temp;
}



int INTEL=0;

#define SET_ASCII 0
#define SET_BINARY 1
#define SET_DUMPHEADER 2
#define SET_PIPE 3
#define SET_WITHINDEX 4
#define SET_FLOAT 5
#define SET_DOUBLE 6
#define N_OPTIONS 7
 
char *option[N_OPTIONS] = {
  "ascii", "binary", "dumpheader", "pipe", "withindex", "float", "double"
};

char *USAGE = "wfm2sdds [<inputFile>] [<outputFile>] [-pipe[=in][,out]]\n\
[-ascii | -binary]\n\
[-withIndex]\n\
[-float | -double]\n\
[-dumpHeader]\n\n\
pipe            SDDS toolkit pipe option.\n\
ascii           Requests SDDS ASCII output.  Default is binary.\n\
binary          Requests SDDS BINARY output.\n\
withIndex       Add Index column.\n\
float           Output in float format. Default is double.\n\
double          Output in double format.\n\
dumpHeader      Print all header info to stdout.\n\n\
Converts Tektronix WFM files to SDDS.\n\
Program by Robert Soliday. ("__DATE__")\n";



int main(int argc,char **argv)
{
  FILE *fd;
  char *input=NULL, *output=NULL;
  SDDS_DATASET SDDSout;
  SCANNED_ARG *scanned;
  long iArg;
  long ascii=0, dumpheader=0;
  unsigned long pipeFlags=0;
  int i=0, j=0, n;
  int withIndex=0, floatValues=0, version=0;

/*  
 1=char
 2=short
 3=unsigned short
 4=long
 5=unsigned long
 6=unsigned long long
 7=float
 8=double
 9=char[]
*/

  short fileFormat[122] = {3, 9, 1, 4, 1, 4, 4, 7, 8, 7,  9, 5, 3, 5, 5, 6, 6, 5, 4, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 3, 5, 6, 8, 8, 5,  9, 8, 8, 8, 8, 5, 5, 4, 4, 4, 4, 4, 8,  9, 8, 5, 8, 8, 8, 8, 5,  9, 8, 8, 8, 8, 5, 5, 4, 4, 4, 4, 4, 8,  9, 8, 5, 8, 8, 8, 8, 5,  9, 8, 8, 8, 8, 5, 8,  9, 8, 5, 8, 8, 8, 8, 5,  9, 8, 8, 8, 8, 5, 8,  9, 8, 5, 8, 8, 5, 5, 5, 5, 5, 5, 5, 8, 8, 4, 7, 5, 2, 5, 5, 5, 5, 5};
  short fileBits[122] =   {2, 8, 1, 4, 1, 4, 4, 4, 8, 4, 32, 4, 2, 4, 4, 8, 8, 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 2, 4, 8, 8, 8, 4, 20, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 4, 8, 20, 8, 4, 8, 8, 8, 8, 4, 20, 8, 8, 8, 8, 4, 4, 4, 4, 4, 4, 4, 8, 20, 8, 4, 8, 8, 8, 8, 4, 20, 8, 8, 8, 8, 4, 8, 20, 8, 4, 8, 8, 8, 8, 4, 20, 8, 8, 8, 8, 4, 8, 20, 8, 4, 8, 8, 4, 4, 4, 4, 4, 4, 4, 8, 8, 4, 4, 4, 2, 4, 4, 4, 4, 4};

  char Char;
  short Short;
  unsigned short uShort;
  int32_t Long;
  uint32_t uLong;
  uint64_t uLLong;
  float Float;
  double Double;
  char buffer[50];

  short bytesPerPoint=2;
  unsigned long prechargeoffset=0;
  unsigned long *precharge;
  unsigned long RecLength=0;
  unsigned long *recordLength;
  unsigned long postchargestart=0, postchargestop=0, postcharge;
  double sampleInterval=1;
  char sampleUnits[50];
  double sampleStart=0;
  double triggerPositionPercent=0;
  int32_t *triggerPoint;
  long timeInt;
  double *time;
  double timeFrac=0;
  double expDimInterval=1, expDimStart=0;
  char expDimUnits[50];
  int32_t *index=NULL;
  double *sample=NULL, *curve=NULL;
  uint32_t waveform, waveforms=1;

  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv);
  if (argc<2) {
    fprintf(stderr, "%s", USAGE);
    return(1);
  }
    
  for (iArg=1; iArg<argc; iArg++) {
    if (scanned[iArg].arg_type==OPTION) {
      switch (match_string(scanned[iArg].list[0], option, N_OPTIONS, 0)) {
      case SET_ASCII:
        ascii = 1;
        break;
      case SET_BINARY:
        ascii = 0;
        break;
      case SET_DUMPHEADER:
        dumpheader = 1;
        break;
      case SET_WITHINDEX:
        withIndex = 1;
        break;
      case SET_FLOAT:
        floatValues = 1;
        break;
      case SET_DOUBLE:
        floatValues = 0;
        break;
      case SET_PIPE:
        if (!processPipeOption(scanned[iArg].list+1, scanned[iArg].n_items-1, &pipeFlags)) {
          fprintf(stderr, "invalid -pipe syntax\n");
          return(1);
        }
        break;
      default:
        fprintf(stderr, "invalid option seen\n");
        fprintf(stderr, "%s", USAGE);
        return(1);
      }
    } else {
      if (!input)
        input = scanned[iArg].list[0];
      else if (!output)
        output = scanned[iArg].list[0];
      else {
        fprintf(stderr, "too many filenames\n");
        fprintf(stderr, "%s", USAGE);
        return(1);
      }
    }
  }
  
  processFilenames("wfm2sdds", &input, &output, pipeFlags, 0, NULL);

  if (input) {
    if (!fexists(input)) {
      fprintf(stderr, "input file not found\n");
      return(1);
    }
    if (!(fd = fopen(input, "rb"))) {
      fprintf(stderr, "problem opening input file\n");
      return(1);
    }
  } else {
#if defined(_WIN32)
      if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
	fprintf(stderr, "error: unable to set stdin to binary mode\n");
	return(1);
      }      
#endif
    fd = stdin;
  }
  
  if (dumpheader) {
    fprintf(stdout, "Read www.tektronix.com/Measurement/Solutions/openchoice/docs/articles/001137801.pdf for definitions for the header elements.\n");
  }
  for (i=0;i<110;i++) {
    if ((i == 29) && (version == 1)) {
      continue;
    }
    n = 0;
    if (fileFormat[i] == 1) {
      n = fread(&Char, 1, 1, fd);
    } else if (fileFormat[i] == 2) {
      n = fread(&Short, 1, 2, fd);
      if (INTEL) {
        swapshort((short*)&Short);
      }
    } else if (fileFormat[i] == 3) {
      n = fread(&uShort, 1, 2, fd);
      if (INTEL) {
        swapshort((short*)&uShort);
      }
    } else if (fileFormat[i] == 4) {
      n = fread(&Long, 1, 4, fd);
      if (INTEL) {
        swaplong((uint32_t*)&Long);
      }
    } else if (fileFormat[i] == 5) {
      n = fread(&uLong, 1, 4, fd);
      if (INTEL) {
        swaplong((uint32_t*)&uLong);
      }
    } else if (fileFormat[i] == 6) {
      n = fread(&uLLong, 1, 8, fd);
      if (INTEL) {
        swaplonglong((uint64_t*)&uLLong);
      }
    } else if (fileFormat[i] == 7) {
      n = fread(&Float, 1, 4, fd);
      if (INTEL) {
        swapfloat((float*)&Float);
      }
    } else if (fileFormat[i] == 8) {
      n = fread(&Double, 1, 8, fd);
      if (INTEL) {
        swapdouble((double*)&Double);
      }
    } else if (fileFormat[i] == 9) {
      n = fread(buffer, 1, fileBits[i], fd);
      buffer[fileBits[i]] = '\0';
    }

    if (n != fileBits[i]) {
      fprintf(stderr, "Error: unable to read data from input file\n");
      fclose(fd);
      return(1);
    }
    if (dumpheader) {
      fprintf(stdout, "#%d \tBit Offset=%d \t", i, j);
      if (fileFormat[i] == 1) {
        fprintf(stdout, "Char = %d\n", Char);
      } else if (fileFormat[i] == 2) {
        fprintf(stdout, "Short = %d\n", Short);
      } else if (fileFormat[i] == 3) {
        fprintf(stdout, "uShort = %u\n", uShort);
      } else if (fileFormat[i] == 4) {
        fprintf(stdout, "Long = %ld\n", Long);
      } else if (fileFormat[i] == 5) {
        fprintf(stdout, "uLong = %lu\n", uLong);
      } else if (fileFormat[i] == 6) {
        fprintf(stdout, "uLLong = %llu\n", uLLong);
      } else if (fileFormat[i] == 7) {
        fprintf(stdout, "Float = %15.15f\n", Float);
      } else if (fileFormat[i] == 8) {
        fprintf(stdout, "Double = %15.15lf\n", Double);
      } else if (fileFormat[i] == 9) {
        fprintf(stdout, "Char[] = %s\n", buffer);
      }
      j += fileBits[i];
    }
    if (i==0) {
      if (uShort == 3855) {
        if (SDDS_IsBigEndianMachine())
          INTEL = 1;
        else
          INTEL = 0;
      } else if (uShort == 61680) {
        if (SDDS_IsBigEndianMachine())
          INTEL = 0;
        else
          INTEL = 1;
      } else {
        fprintf(stderr, "Error: invalid WFM file\n");
        fclose(fd);
        return(1);
      }
    }
    if (i==1) {
      if (strcmp(buffer, ":WFM#001") == 0) {
        version = 1;
      } else if (strcmp(buffer, ":WFM#002") == 0) {
        version = 2;
      } else {
        fprintf(stderr, "Error: invalid WFM file, expected :WFM#001 or :WFM#002 as version number.\n");
        fclose(fd);
        return(1);
      }
    }
    if (i==4) {
      bytesPerPoint = Char;
    }
    if (i==11) {
      if (uLong != 0) {
        waveforms = uLong + 1;
      };
    }
    if (i==14) {
      if (uLong != 1) {
        fprintf(stderr, "Error: cannot convert WFM files that include multiple waveforms.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==20) {
      if (uLong != 1) {
        fprintf(stderr, "Error: cannot convert WFM files that include multiple implicit dimensions.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==21) {
      if (uLong != 1) {
        fprintf(stderr, "Error: cannot convert WFM files that include multiple explicit dimensions.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==22) {
      if (uLong != 2) {
        fprintf(stderr, "Error: cannot convert WFM files that don't include WFMDATA_VECTOR data.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==26) {
      if (uLong != 1) {
        fprintf(stderr, "Error: cannot convert WFM files that include multiple curve objects.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==32) {
      expDimInterval = Double;
    }
    if (i==33) {
      expDimStart = Double;
    }
    if (i==35) {
      sprintf(expDimUnits, "%s", buffer);
    }
    if (i==40) {
      if (uLong != 0) {
        fprintf(stderr, "Error: Currently can only convert WFM files that contain data values in 16bit integers.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==41) {
      if ((uLong != 0) && (uLong != 1)) {
        fprintf(stderr, "Error: Unable to convert WMF file due to unsupported data storage layout.\n");
        fclose(fd);
        return(1);
      };
    }
    if (i==51) {
      triggerPositionPercent = Double;
    }
    if (i==74) {
      sampleInterval = Double;
    }
    if (i==75) {
      sampleStart = Double;
    }
    if (i==76) {
      RecLength = uLong;
    }
    if (i==77) {
      sprintf(sampleUnits, "%s", buffer);
    }
  }

  time = malloc(sizeof(double) * waveforms);
  precharge = malloc(sizeof(unsigned long) * waveforms);
  recordLength = malloc(sizeof(unsigned long) * waveforms);
  triggerPoint = malloc(sizeof(int32_t) * waveforms);

  time[0]=0;
  precharge[0]=0;
  recordLength[0]=0;
  triggerPoint[0]=0;
  for (i=110;i<122;i++) {
    n = 0;
    if (fileFormat[i] == 1) {
      n = fread(&Char, 1, 1, fd);
    } else if (fileFormat[i] == 2) {
      n = fread(&Short, 1, 2, fd);
      if (INTEL) {
        swapshort((short*)&Short);
      }
    } else if (fileFormat[i] == 3) {
      n = fread(&uShort, 1, 2, fd);
      if (INTEL) {
        swapshort((short*)&uShort);
      }
    } else if (fileFormat[i] == 4) {
      n = fread(&Long, 1, 4, fd);
      if (INTEL) {
        swaplong((uint32_t*)&Long);
      }
    } else if (fileFormat[i] == 5) {
      n = fread(&uLong, 1, 4, fd);
      if (INTEL) {
        swaplong((uint32_t*)&uLong);
      }
    } else if (fileFormat[i] == 6) {
      n = fread(&uLLong, 1, 8, fd);
      if (INTEL) {
        swaplonglong((uint64_t*)&uLLong);
      }
    } else if (fileFormat[i] == 7) {
      n = fread(&Float, 1, 4, fd);
      if (INTEL) {
        swapfloat((float*)&Float);
      }
    } else if (fileFormat[i] == 8) {
      n = fread(&Double, 1, 8, fd);
      if (INTEL) {
        swapdouble((double*)&Double);
      }
    } else if (fileFormat[i] == 9) {
      n = fread(buffer, 1, fileBits[i], fd);
      buffer[fileBits[i]] = '\0';
    }
    
    if (n != fileBits[i]) {
      fprintf(stderr, "Error: unable to read data from input file\n");
      fclose(fd);
      return(1);
    }
    if (dumpheader) {
      fprintf(stdout, "#%d \tBit Offset=%d \t", i, j);
      if (fileFormat[i] == 1) {
        fprintf(stdout, "Char = %d\n", Char);
      } else if (fileFormat[i] == 2) {
        fprintf(stdout, "Short = %d\n", Short);
      } else if (fileFormat[i] == 3) {
        fprintf(stdout, "uShort = %u\n", uShort);
      } else if (fileFormat[i] == 4) {
        fprintf(stdout, "Long = %ld\n", Long);
      } else if (fileFormat[i] == 5) {
        fprintf(stdout, "uLong = %lu\n", uLong);
      } else if (fileFormat[i] == 6) {
        fprintf(stdout, "uLLong = %llu\n", uLLong);
      } else if (fileFormat[i] == 7) {
        fprintf(stdout, "Float = %15.15f\n", Float);
      } else if (fileFormat[i] == 8) {
        fprintf(stdout, "Double = %15.15lf\n", Double);
      } else if (fileFormat[i] == 9) {
        fprintf(stdout, "Char[] = %s\n", buffer);
      }
      j += fileBits[i];
    }
    if (i==112) {
      timeFrac = Double;
    }
    if (i==113) {
      timeInt = Long;
      time[0] = timeInt + timeFrac;
    }
    if (i==117) {
      prechargeoffset = uLong;
      precharge[0] = prechargeoffset / bytesPerPoint;
    }
    if (i==119) {
      postchargestart = uLong;
    }
    if (i==120) {
      postchargestop = uLong;
      postcharge = (postchargestop - postchargestart) / bytesPerPoint;
      recordLength[0] = RecLength - precharge[0] - postcharge;
      triggerPoint[0] = round(recordLength[0] * (triggerPositionPercent / 100));
    }
  }

  for (waveform=1 ; waveform < waveforms ; waveform++) {
    time[waveform]=0;
    precharge[waveform]=0;
    recordLength[waveform]=0;
    triggerPoint[waveform]=0;
    for (i=110;i<114;i++) {
      n = 0;
      if (fileFormat[i] == 1) {
        n = fread(&Char, 1, 1, fd);
      } else if (fileFormat[i] == 2) {
        n = fread(&Short, 1, 2, fd);
        if (INTEL) {
          swapshort((short*)&Short);
        }
      } else if (fileFormat[i] == 3) {
        n = fread(&uShort, 1, 2, fd);
        if (INTEL) {
          swapshort((short*)&uShort);
        }
      } else if (fileFormat[i] == 4) {
        n = fread(&Long, 1, 4, fd);
        if (INTEL) {
          swaplong((uint32_t*)&Long);
        }
      } else if (fileFormat[i] == 5) {
        n = fread(&uLong, 1, 4, fd);
        if (INTEL) {
          swaplong((uint32_t*)&uLong);
        }
      } else if (fileFormat[i] == 6) {
        n = fread(&uLLong, 1, 8, fd);
        if (INTEL) {
          swaplonglong((uint64_t*)&uLLong);
        }
      } else if (fileFormat[i] == 7) {
        n = fread(&Float, 1, 4, fd);
        if (INTEL) {
          swapfloat((float*)&Float);
        }
      } else if (fileFormat[i] == 8) {
        n = fread(&Double, 1, 8, fd);
        if (INTEL) {
          swapdouble((double*)&Double);
        }
      } else if (fileFormat[i] == 9) {
        n = fread(buffer, 1, fileBits[i], fd);
        buffer[fileBits[i]] = '\0';
      }
      
      if (n != fileBits[i]) {
        fprintf(stderr, "Error: unable to read data from input file\n");
        fclose(fd);
        return(1);
      }
      if (dumpheader) {
        fprintf(stdout, "#%d \tBit Offset=%d \t", i, j);
        if (fileFormat[i] == 1) {
          fprintf(stdout, "Char = %d\n", Char);
        } else if (fileFormat[i] == 2) {
          fprintf(stdout, "Short = %d\n", Short);
        } else if (fileFormat[i] == 3) {
          fprintf(stdout, "uShort = %u\n", uShort);
        } else if (fileFormat[i] == 4) {
          fprintf(stdout, "Long = %ld\n", Long);
        } else if (fileFormat[i] == 5) {
          fprintf(stdout, "uLong = %lu\n", uLong);
        } else if (fileFormat[i] == 6) {
          fprintf(stdout, "uLLong = %llu\n", uLLong);
        } else if (fileFormat[i] == 7) {
          fprintf(stdout, "Float = %15.15f\n", Float);
        } else if (fileFormat[i] == 8) {
          fprintf(stdout, "Double = %15.15lf\n", Double);
        } else if (fileFormat[i] == 9) {
          fprintf(stdout, "Char[] = %s\n", buffer);
        }
        j += fileBits[i];
      }
      if (i==112) {
        timeFrac = Double;
      }
      if (i==113) {
        timeInt = Long;
        time[waveform] = timeInt + timeFrac;
      }
    }
  }
  for (waveform=1 ; waveform < waveforms ; waveform++) {
    for (i=114;i<122;i++) {
      n = 0;
      if (fileFormat[i] == 1) {
        n = fread(&Char, 1, 1, fd);
      } else if (fileFormat[i] == 2) {
        n = fread(&Short, 1, 2, fd);
        if (INTEL) {
          swapshort((short*)&Short);
        }
      } else if (fileFormat[i] == 3) {
        n = fread(&uShort, 1, 2, fd);
        if (INTEL) {
          swapshort((short*)&uShort);
        }
      } else if (fileFormat[i] == 4) {
        n = fread(&Long, 1, 4, fd);
        if (INTEL) {
          swaplong((uint32_t*)&Long);
        }
      } else if (fileFormat[i] == 5) {
        n = fread(&uLong, 1, 4, fd);
        if (INTEL) {
          swaplong((uint32_t*)&uLong);
        }
      } else if (fileFormat[i] == 6) {
        n = fread(&uLLong, 1, 8, fd);
        if (INTEL) {
          swaplonglong((uint64_t*)&uLLong);
        }
      } else if (fileFormat[i] == 7) {
        n = fread(&Float, 1, 4, fd);
        if (INTEL) {
          swapfloat((float*)&Float);
        }
      } else if (fileFormat[i] == 8) {
        n = fread(&Double, 1, 8, fd);
        if (INTEL) {
          swapdouble((double*)&Double);
        }
      } else if (fileFormat[i] == 9) {
        n = fread(buffer, 1, fileBits[i], fd);
        buffer[fileBits[i]] = '\0';
      }
      
      if (n != fileBits[i]) {
        fprintf(stderr, "Error: unable to read data from input file\n");
        fclose(fd);
        return(1);
      }
      if (dumpheader) {
        fprintf(stdout, "#%d \tBit Offset=%d \t", i, j);
        if (fileFormat[i] == 1) {
          fprintf(stdout, "Char = %d\n", Char);
        } else if (fileFormat[i] == 2) {
          fprintf(stdout, "Short = %d\n", Short);
        } else if (fileFormat[i] == 3) {
          fprintf(stdout, "uShort = %u\n", uShort);
        } else if (fileFormat[i] == 4) {
          fprintf(stdout, "Long = %ld\n", Long);
        } else if (fileFormat[i] == 5) {
          fprintf(stdout, "uLong = %lu\n", uLong);
        } else if (fileFormat[i] == 6) {
          fprintf(stdout, "uLLong = %llu\n", uLLong);
        } else if (fileFormat[i] == 7) {
          fprintf(stdout, "Float = %15.15f\n", Float);
        } else if (fileFormat[i] == 8) {
          fprintf(stdout, "Double = %15.15lf\n", Double);
        } else if (fileFormat[i] == 9) {
          fprintf(stdout, "Char[] = %s\n", buffer);
        }
        j += fileBits[i];
      }
      if (i==117) {
        prechargeoffset = uLong;
        precharge[waveform] = prechargeoffset / bytesPerPoint;
      }
      if (i==119) {
        postchargestart = uLong;
      }
      if (i==120) {
        postchargestop = uLong;
        postcharge = (postchargestop - postchargestart) / bytesPerPoint;
        recordLength[waveform] = RecLength - precharge[waveform] - postcharge;
        triggerPoint[waveform] = round(recordLength[waveform] * (triggerPositionPercent / 100));
      }
    }
  }





  if (!SDDS_InitializeOutput(&SDDSout, ascii?SDDS_ASCII:SDDS_BINARY,1, NULL, NULL, output)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_DefineSimpleParameter(&SDDSout, "TriggerPoint", NULL, SDDS_LONG)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_DefineSimpleParameter(&SDDSout, "SampleInterval", NULL, SDDS_DOUBLE)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_DefineSimpleParameter(&SDDSout, "Time", NULL, SDDS_DOUBLE)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (withIndex) {
    if (!SDDS_DefineSimpleColumn(&SDDSout, "Index", NULL, SDDS_LONG)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
  }
  if (!SDDS_DefineSimpleColumn(&SDDSout, "t", sampleUnits, floatValues?SDDS_FLOAT:SDDS_DOUBLE)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_DefineSimpleColumn(&SDDSout, "Signal", expDimUnits, floatValues?SDDS_FLOAT:SDDS_DOUBLE)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }
  if (!SDDS_WriteLayout(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }



  for (waveform=0 ; waveform < waveforms ; waveform++) {
    if (withIndex) {
      index = malloc(sizeof(int32_t) * recordLength[waveform]);
    }
    sample = malloc(sizeof(double) * recordLength[waveform]);
    curve = malloc(sizeof(double) * recordLength[waveform]);

    for (i=0;i<precharge[waveform];i++) {
      n = fread(&Short, 1, 2, fd);
    }

    for (i=0;i<recordLength[waveform];i++) {
      n = fread(&Short, 1, 2, fd);
      if (INTEL) {
        swapshort((short*)&Short);
      }
      if (n != 2) {
        fprintf(stderr, "Error: unable to read data from input file\n");
        fclose(fd);
        return(1);
      }
      sample[i] = sampleStart + (sampleInterval * i);
      curve[i] = expDimStart + (Short * expDimInterval);
    }

    for (i=recordLength[waveform];i<RecLength;i++) {
      n = fread(&Short, 1, 2, fd);
    }

    if (withIndex) {
      for (i=0;i<recordLength[waveform];i++) {
        index[i] = i;
      }
    }
    
    if (!SDDS_StartTable(&SDDSout, recordLength[waveform])) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (!SDDS_SetParameters(&SDDSout, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, "TriggerPoint", triggerPoint[waveform], "SampleInterval", sampleInterval, "Time", time[waveform], NULL)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (withIndex) {
      if (!SDDS_SetColumnFromLongs(&SDDSout, SDDS_SET_BY_NAME, index, recordLength[waveform], "Index")) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        return(1);
      }
    }
    if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, sample, recordLength[waveform], "t")) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (!SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, curve, recordLength[waveform], "Signal")) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (!SDDS_WriteTable(&SDDSout)) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      return(1);
    }
    if (withIndex)
      free(index);
    free(sample);
    free(curve);
  }

  fclose(fd);

  if (!SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    return(1);
  }

  free_scanargs(&scanned, argc);

  return(0);
}
