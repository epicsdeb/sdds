/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* program: sddsconvolve.c
 * purpose: do convolution, deconvolution, and correlation
 * 
 * Michael Borland, 1991, 1993, 1998
 */
#include "mdb.h"
#include "SDDS.h"
#include "scan.h"
#include "fftpackC.h"

#define CLO_DECONVOLVE 0
#define CLO_PIPE 1
#define CLO_NOISE_FRACTION 2
#define CLO_CORRELATE 3
#define CLO_INDEPENDENTCOLUMN 4
#define CLO_SIGNALCOLUMN 5
#define CLO_RESPONSECOLUMN 6
#define CLO_OUTPUTCOLUMN 7
#define CLO_WIENER_FILTER 8
#define N_OPTIONS 9

char *option[N_OPTIONS] = {
  "deconvolve", "pipe", "noisefraction", "correlate", 
  "independentcolumn", "signalcolumn", "responsecolumn",
  "outputcolumn", "wienerfilter",
} ;

#define USAGE "sddsconvolve <signal-file> <response-file> <output>\n\
 -signalColumns=<indepColumn>,<dataName>\n\
 -responseColumns=<indepColumn>,<dataName>\n\
 -outputColumns=<indepColumn>,<dataName>\n\
 [{-deconvolve [{-noiseFraction=value | -wienerFilter=value}] | -correlate}]\n\n\
Performs discrete Fourier convolution/deconvolution/correlation.  Assumes that spacing of \n\
points is the same in both input files and that both files have the same number of data\n\
points.\n\
In terms of Fourier transforms of the signal (S), response (R), and output (O), the program\n\
computes:\n\
O = S*R         for convolution\n\
O = S/R         for deconvolution\n\
O = S*Conj(R)   for correlation\n\n\
If deconvolution is used, one may specify a noise fraction to avoid divide-by-zero problems,\n\
or else use a Wiener filter, specifying the fraction of the maximum  magnitude below\n\
which everything is considered to be noise.\n\
The standard -pipe option may be used in place of <signal-file> and/or <output>.\n\
Program by Michael Borland. (This is version 2, April 2003.)\n"

void complex_multiply(double *r0, double *i0, double  r1, double  i1,
                      double  r2, double  i2);
void complex_divide(double *r0, double *i0, double  r1, double  i1,
                    double  r2, double  i2, double threshold);
void wrap_around_order(double *response1, double *t, double *response, long nres, long nsig);

#define MODE_CONVOLVE   1
#define MODE_DECONVOLVE 2
#define MODE_CORRELATE  3

int main(int argc, char **argv)
{
  SDDS_DATASET SDDS1, SDDS2, SDDSout;
  int i_arg;
  SCANNED_ARG *scanned;
  char *input1, *input2, *output;
  long i, mode, nfreq, doWiener;
  double *fft_sig, *fft_res, noise, mag2, threshold, range;
  double *signal1, *signal2, *indep1, *indep2;
  double WienerFraction, *WienerFilter=NULL;
  unsigned long pipeFlags;
  char *input1Column[2], *input2Column[2], *outputColumn[2];
  char description[1024];
  long tmpfile_used, code1, code2, rows1, rows2, parameters=0;
  char **parameterName=NULL;
  
  SDDS_RegisterProgramName(argv[0]);
  argc = scanargs(&scanned, argc, argv); 
  if (argc<4 || argc>(4+N_OPTIONS)) 
    bomb(NULL, USAGE);

  input1 = input2 = output = NULL;
  mode = MODE_CONVOLVE;
  noise = 1e-14;
  input1Column[0] = input1Column[1] = NULL;
  input2Column[0] = input2Column[1] = NULL;
  outputColumn[0] = outputColumn[1] = NULL;
  pipeFlags = 0;
  doWiener = 0;
  
  for (i_arg=1; i_arg<argc; i_arg++) {
    if (scanned[i_arg].arg_type==OPTION) {
      /* process options here */
      switch (match_string(scanned[i_arg].list[0], option, N_OPTIONS, 0)) {
      case CLO_DECONVOLVE:
        mode = MODE_DECONVOLVE;
        break;
      case CLO_CORRELATE:
        mode = MODE_CORRELATE;
        break;
      case CLO_PIPE:
        if (!processPipeOption(scanned[i_arg].list+1, scanned[i_arg].n_items-1, &pipeFlags))
          SDDS_Bomb("invalid -pipe syntax");
        break;
      case CLO_NOISE_FRACTION:
        if (scanned[i_arg].n_items!=2 ||
            sscanf(scanned[i_arg].list[1], "%lf", &noise)!=1 ||
            noise<=0)
          SDDS_Bomb("invalid -noise_fraction syntax");
        break;
      case CLO_WIENER_FILTER:
        if (scanned[i_arg].n_items!=2 ||
            sscanf(scanned[i_arg].list[1], "%lf", &WienerFraction)!=1 ||
            WienerFraction<=0 || WienerFraction>=1)
          SDDS_Bomb("invalid -WienerFraction syntax or value");
        doWiener = 1;
        break;
      case CLO_SIGNALCOLUMN:
        if (scanned[i_arg].n_items!=3 ||
            !strlen(input1Column[0]=scanned[i_arg].list[1]) ||
            !strlen(input1Column[1]=scanned[i_arg].list[2])) 
          SDDS_Bomb("invalid -signalColumn syntax");
        break;
      case CLO_RESPONSECOLUMN:
        if (scanned[i_arg].n_items!=3 ||
            !strlen(input2Column[0]=scanned[i_arg].list[1]) ||
            !strlen(input2Column[1]=scanned[i_arg].list[2]))
          SDDS_Bomb("invalid -responseColumn syntax");
        break;
      case CLO_OUTPUTCOLUMN:
        if (scanned[i_arg].n_items!=3 ||
            !strlen(outputColumn[0]=scanned[i_arg].list[1]) ||
            !strlen(outputColumn[1]=scanned[i_arg].list[2]))
          SDDS_Bomb("invalid -outputColumn syntax");
        break;
      default:
        SDDS_Bomb("unknown option given");
        break;
      }
    }
    else {
      if (!input1) 
        input1 = scanned[i_arg].list[0];
      else if (!input2) 
        input2 = scanned[i_arg].list[0];
      else if (!output)
        output = scanned[i_arg].list[0];
      else
        SDDS_Bomb("too many filenames");
    }
  }
  
  if (pipeFlags&USE_STDIN && input1) {
    if (output)
      SDDS_Bomb("too many filenames");
    output = input2;
    input2 = input1;
    input1 = NULL;
  }
  if (!input1Column[0] || !input1Column[1] || !strlen(input1Column[0]) || !strlen(input1Column[1])) 
    SDDS_Bomb("singalColumn not provided.");
  if (!input2Column[0] || !input2Column[1] || !strlen(input2Column[0]) || !strlen(input2Column[1])) 
    SDDS_Bomb("responseColumn not provided.");
  if (!outputColumn[0] || !outputColumn[1] || !strlen(outputColumn[0]) || !strlen(outputColumn[1])) 
    SDDS_Bomb("outputColumn not provided.");
  processFilenames("sddsconvolve", &input1, &output, pipeFlags, 1, &tmpfile_used);
  if (!input2)
    SDDS_Bomb("second input file not specified");

  if (!SDDS_InitializeInput(&SDDS1, input1) ||
      !SDDS_InitializeInput(&SDDS2, input2)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  switch (mode) {
  case MODE_CONVOLVE:
    sprintf(description, "convolution of signal %s with response %s",
            input1Column[1], input2Column[1]);
    break;
  case MODE_DECONVOLVE:
    sprintf(description, "deconvolution of signal %s with response %s",
            input1Column[1], input2Column[1]);
    break;
  case MODE_CORRELATE:
    sprintf(description, "correlation of signal %s with signal %s",
            input1Column[1], input2Column[1]);
    break;
  }
  parameterName = SDDS_GetParameterNames(&SDDS1, &parameters);
  if (!SDDS_InitializeOutput(&SDDSout, SDDS_BINARY, 1, NULL, NULL, output) ||
      !SDDS_TransferColumnDefinition(&SDDSout, &SDDS1, input1Column[0], outputColumn[0]) || 
      0>SDDS_DefineColumn(&SDDSout, outputColumn[1], NULL, NULL, description,
                          NULL, SDDS_DOUBLE, 0)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  if (parameters) {
    for (i=0; i<parameters; i++)
      if (!SDDS_TransferParameterDefinition(&SDDSout, &SDDS1, parameterName[i], parameterName[i]))
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors|SDDS_EXIT_PrintErrors);
  }
  
  if (!SDDS_WriteLayout(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }
  
  while ((code1=SDDS_ReadPage(&SDDS1))>0 && (code2=SDDS_ReadPage(&SDDS2))>0) {
    if ((rows1=SDDS_RowCount(&SDDS1))<=0 || (rows2=SDDS_RowCount(&SDDS2))<0) {
      fprintf(stderr, "Warning (sddconvolve): skipping page due to no signal or response rows\n");
      continue;
    }
    if (rows1!=rows2)
      SDDS_Bomb("different numbers of points for signal and response");
    if (!(signal1=SDDS_GetColumnInDoubles(&SDDS1, input1Column[1])) ||
        !(indep1=SDDS_GetColumnInDoubles(&SDDS1, input1Column[0])) ||
        !(signal2=SDDS_GetColumnInDoubles(&SDDS2, input2Column[1])) ||
        !(indep2=SDDS_GetColumnInDoubles(&SDDS2, input2Column[0]))) {
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }
    
    if (!(fft_sig = SDDS_Calloc(sizeof(*fft_sig), (2*rows1+2))) ||
        !(fft_res = SDDS_Calloc(sizeof(*fft_res), (2*rows1+2)))) {
      SDDS_SetError("memory allocation failure");
      SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
      exit(1);
    }

    /* return array fft_res with size 2*rows1 + 2 */
    wrap_around_order(fft_res, indep2, signal2, rows2, rows1);
    for (i=0; i<rows1; i++)
      fft_sig[i] = signal1[i];  

    /* signal and response are padded with rows1 additional zeroes. signal
       will have 2*rows1 points and fft will have (rows + 1) frequency points.
       Normally a simple FFT of signal would start with nrows real points
       and have (rows1/2 + 1) frequency points. The normalization factor 
       with which the response FFT must be multiplied is 
       (delta_t)*(number of points), a general result whether padding is 
       done or not. The factor is
       is (t_last-t_first)/(rows1-1) * (2*rows1).
    */

    realFFT2(fft_sig, fft_sig, 2*rows1, 0); 
    realFFT2(fft_res, fft_res, 2*rows1, 0); 
    nfreq = rows1 + 1;
    range = 2 * rows1 * (indep1[rows1-1] - indep1[0])/ (rows1 - 1);

    if (mode==MODE_CONVOLVE || mode==MODE_CORRELATE) {
      if (mode==MODE_CORRELATE) 
        /* take complex conjugate of response FFT */
        for (i=0; i<nfreq; i++)
          fft_res[2*i+1] = -fft_res[2*i+1];
      /* multiply FFTs */
      for (i=0; i<nfreq; i++) {
        complex_multiply(fft_sig+2*i,  fft_sig+2*i+1,
                         fft_sig[2*i], fft_sig[2*i+1],
                         fft_res[2*i], fft_res[2*i+1]);
      }
      realFFT2(fft_sig, fft_sig, 2*rows1, INVERSE_FFT);
      /* response normalization factor is applied in the final result. */
      for (i=0; i<rows1; i++)
        fft_sig[i] *= range;
      if (!SDDS_StartPage(&SDDSout, rows1) ||
          !SDDS_CopyParameters(&SDDSout, &SDDS1) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, fft_sig, rows1, 
                                     outputColumn[1]) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, indep1, rows1, 
                                     outputColumn[0]) ||
          !SDDS_WritePage(&SDDSout)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    } else if (mode==MODE_DECONVOLVE) {
      double maxMag2;
      for (i=threshold=0; i<nfreq; i++) {
        if ((mag2=sqr(fft_res[2*i])+sqr(fft_res[2*i+1]))>threshold)
          threshold = mag2;
      }
      maxMag2 = threshold;
      threshold = threshold*noise;
      if (doWiener) {
        /* compute and apply Wiener filter */
        /* The filter is just
         *   the signal itself if signal<WienerFraction*maxSignal, or
         *   noiseLevel*maxSignal otherwise
         */
        double *S=NULL, *N=NULL, wThreshold;
        if (!(WienerFilter = malloc(sizeof(*WienerFilter)*nfreq)) ||
            !(S = malloc(sizeof(*S)*nfreq)) || !(N = malloc(sizeof(*N)*nfreq)))
          SDDS_Bomb("memory allocation failure");
        wThreshold = maxMag2*sqr(WienerFraction);
        for (i=0; i<nfreq; i++) {
          S[i] = sqrt(sqr(fft_res[2*i])+sqr(fft_res[2*i+1]));
          if (sqr(S[i])<wThreshold) {
            N[i] = S[i];
            S[i] = 0;
          }
          else {
            S[i] = S[i] - wThreshold;
            N[i] = wThreshold;
          }
        }
        for (i=0; i<nfreq; i++)
          WienerFilter[i] = sqr(S[i])/(sqr(S[i])+sqr(N[i])+threshold);
        free(N);
        free(S);
      }
      for (i=0; i<nfreq; i++) {
        complex_divide(fft_sig+2*i,  fft_sig+2*i+1,
                       fft_sig[2*i], fft_sig[2*i+1],
                       fft_res[2*i], fft_res[2*i+1], threshold);
      }
      if (doWiener) {
        for (i=0; i<nfreq; i++) {
          fft_sig[2*i]   *= WienerFilter[i];
          fft_sig[2*i+1] *= WienerFilter[i];
        }
        free(WienerFilter);
      }
      realFFT2(fft_sig, fft_sig, 2*rows1, INVERSE_FFT);
      /* response normalization factor is applied in the final result. */
      for (i=0; i<rows1; i++)
        fft_sig[i] = fft_sig[i]/range;
      if (!SDDS_StartPage(&SDDSout, rows1) ||
          !SDDS_CopyParameters(&SDDSout, &SDDS1) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, fft_sig, rows1,
                                     outputColumn[1]) ||
          !SDDS_SetColumnFromDoubles(&SDDSout, SDDS_SET_BY_NAME, indep1, rows1,
                                     outputColumn[0]) ||
          !SDDS_WritePage(&SDDSout)) {
        SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
        exit(1);
      }
    } else {
      SDDS_Bomb("this can't happen");
    }
  }
  
  if (!SDDS_Terminate(&SDDS1) || !SDDS_Terminate(&SDDS2) || !SDDS_Terminate(&SDDSout)) {
    SDDS_PrintErrors(stderr, SDDS_VERBOSE_PrintErrors);
    exit(1);
  }

  if (tmpfile_used && !replaceFileAndBackUp(input1, output))
    exit(1);
  return(0);
}

void wrap_around_order
  (
   double *response1,
   double *t,
   double *response,
   long nres,
   long nsig                       
   )
{
  long i, zero_seen;
  long iz;

  zero_seen = 0;
  for (iz=0; iz<nres; iz++)
    if (t[iz]>=0)
      break;
  if (iz==nres)
    bomb("response function is acausal", NULL);

  fill_double_array(response1, 2*nsig+2, 0.0L);
  for (i=iz; i<nres; i++)
    response1[i-iz] = response[i];
  for (i=0; i<iz; i++)
    response1[2*nsig-(iz-i)] = response[i];
}
