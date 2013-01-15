// example how user inteface can be used
// V.Rybnikov -FLA-
// created 29 May 2008
// The example shows how to extract name of channels
// and dump the results to an ASCII file


#include "ttf2_daq_user.h"
#include "event_util.h"
#include <math.h>
#include "/home/helios/SOLIDAY/epics/extensions/src/SDDS/include/SDDS.h"
#include "/home/helios/SOLIDAY/epics/extensions/src/SDDS/include/scan.h"

// Here we find out the channels 

int nchans = 0;
char **chans = NULL;
FILE *fd = NULL;
char *file = (char*)"./user_out.txt";
char *sddsfile = (char*)"./user_out.sdds";
unsigned long pipeFlags=0;

extern int nbunch;
extern int outmode;

int first_event = -1;
int last_event = -1;
int bad_chans = 0;
bool zerocheck = false;

int quiet = 1;

SDDS_DATASET SDDS_dataset;

#define MY_MAX_IMAGE_PTS 10

void ttf2_user_help() {
 printf("-out file -zerocheck \n");
 printf("file       -  file name to dump\n");     
 printf("zerocheck  -  sets checcks for all ZEROs in spectra \n");  
}

int ttf2_user_init(int argc, char **argv, ttf2_daq_reader* reader){

  if(!reader) return -1;


  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "-out")) {
	if (i++ < argc) 
	  sddsfile = argv[i];
    } else if (!strcmp(argv[i], "-zerocheck")) {
      zerocheck = true;
    } else if (!strcmp(argv[i], "-pipe")) {
      pipeFlags = USE_STDOUT;
      sddsfile = NULL;
    }
  }


  nchans =  reader->get_number_of_channels();

  if(!nchans) {
    printf("ttf2_user_init(): no channels in the reader !!!\n");
    return -2;
  }

  chans = (char**) malloc(sizeof(char*)*nchans);
  if(!chans) {
    printf("ttf2_user_init(): failed to malloc chans !!!\n");
    return -3;
  }


  for(int i=0; i<nchans; i++) {
    chans[i] = reader->get_channel_name(i);
    if(!chans[i]) {
      printf("ttf2_user_init(): failed to get channel %d !!!\n", i);
      free(chans);
      chans = NULL;
      return -4;
    }
  }


  fd = fopen(file,"w+");
  if(!fd) {
    perror("fopen");
    printf("ttf2_user_init(): failed to open file for writing\n");
     return -5;
  }

  fputs("EventId\t",  fd);
  fputs("Time\t\t\t",  fd);

  if(outmode)  fputs("EvHeadStatus\t\t\t",  fd);

  if (!quiet) {
    printf("ttf2_user_init()==== CHANNELS ==============\n");
    for(int i=0; i<nchans; i++) {
      printf("[%d] \t %s\n", i, chans[i]);
      
      if(outmode) {
	fputs("SrvBlockStatus\t\t\t",  fd);
	fputs("ChanStatus\t\t\t",  fd);
      }
      
      fputs(chans[i],  fd);
      if(i != (nchans -1)) fputs("\t",  fd);
      else  fputs("\n",  fd);
      
    }
    printf("ttf2_user_init()===== CHANNELS END ==============\n");
  }

  return 0;
}



int ttf2_user_init_2010(int argc, char **argv, ttf2_daq_reader_2010* reader){

  if(!reader) return -1;


  for(int i=1; i<argc; i++) {
    if(!strcmp(argv[i], "-out")) {
	if(i++ < argc) file = argv[i];
    } else if(!strcmp(argv[i], "-zerocheck"))   zerocheck = true;
  }


  nchans =  reader->get_number_of_channels();

  if(!nchans) {
    printf("ttf2_user_init(): no channels in the reader !!!\n");
    return -2;
  }

  chans = (char**) malloc(sizeof(char*)*nchans);
  if(!chans) {
    printf("ttf2_user_init(): failed to malloc chans !!!\n");
    return -3;
  }


  for(int i=0; i<nchans; i++) {
    chans[i] = reader->get_channel_name(i);
    if(!chans[i]) {
      printf("ttf2_user_init(): failed to get channel %d !!!\n", i);
      free(chans);
      chans = NULL;
      return -4;
    }
  }


  fd = fopen(file,"w+");
  if(!fd) {
    perror("fopen");
    printf("ttf2_user_init(): failed to open file for writing\n");
     return -5;
  }

  fputs("EventId\t",  fd);
  fputs("Time\t\t\t",  fd);

  if(outmode)  fputs("EvHeadStatus\t\t\t",  fd);

  if (!quiet) {
    printf("ttf2_user_init()==== CHANNELS ==============\n");
    for(int i=0; i<nchans; i++) {
      printf("[%d] \t %s\n", i, chans[i]);
      
      if(outmode) {
	fputs("SrvBlockStatus\t\t\t",  fd);
	fputs("ChanStatus\t\t\t",  fd);
      }
      
      fputs(chans[i],  fd);
      if(i != (nchans -1)) fputs("\t",  fd);
      else  fputs("\n",  fd);
      
    }
    printf("ttf2_user_init()===== CHANNELS END ==============\n");
  }

  return 0;
}

int sdds_header_written = 0;
int spectrumMode = 0;
int singleMode = 0;
int ttf2_sdds_header(ttf2_daq_getdata* data) {
  int ddtt;
  int nchan;
  char buffer[100];
  for(int i=0; i<nchans; i++) {
    ddtt = data->SetProp(chans[i]);
    if (ddtt == 2) {
      singleMode = 1;
    } else if (ddtt == 3) {
      spectrumMode = 1;
    } else if (ddtt == -11) {
      fprintf(stderr, "Error: channel %s is not available for the selected pulse\n", chans[i]);
      exit(1);
    } else {
      fprintf(stderr, "Error: Cannot handle this type of channel\n");
      exit(1);
    }
  }
  
  if (pipeFlags == USE_STDOUT) {
    SDDS_InitializeOutput(&SDDS_dataset, SDDS_BINARY, 1, NULL, NULL, NULL);
  } else {
    SDDS_InitializeOutput(&SDDS_dataset, SDDS_BINARY, 1, NULL, NULL, sddsfile);
  }
  
  if (spectrumMode) {
    SDDS_DefineSimpleParameter(&SDDS_dataset, (char*)"Time", NULL, SDDS_DOUBLE);
    SDDS_DefineSimpleParameter(&SDDS_dataset, (char*)"EventID", NULL, SDDS_ULONG);
    SDDS_DefineSimpleColumn(&SDDS_dataset, (char*)"BunchNumber", NULL, SDDS_LONG);
    for(int i=0; i<nchans; i++) {
      ddtt = data->SetProp(chans[i]);
      if (ddtt == 2) {
	SDDS_DefineSimpleParameter(&SDDS_dataset, chans[i], NULL, SDDS_FLOAT);
      } else if (ddtt == 3) {
	nchan = data->GetNmemb();
	if (nchan > 1) {
	  for (int j=0; j<nchan; j++) {
	    sprintf(buffer, "%s:%d", chans[i], j);
	    SDDS_DefineSimpleColumn(&SDDS_dataset, buffer, NULL, SDDS_FLOAT);		  
	  }
	} else { 
	  SDDS_DefineSimpleColumn(&SDDS_dataset, chans[i], NULL, SDDS_FLOAT);
	}
      }
    }
    singleMode = 0;
  } else if (singleMode) {
    SDDS_DefineSimpleColumn(&SDDS_dataset, (char*)"Time", NULL, SDDS_DOUBLE);
    SDDS_DefineSimpleColumn(&SDDS_dataset, (char*)"EventID", NULL, SDDS_ULONG);
    for(int i=0; i<nchans; i++) {
      ddtt = data->SetProp(chans[i]);
      if (ddtt == 2) {
	SDDS_DefineSimpleColumn(&SDDS_dataset, chans[i], NULL, SDDS_FLOAT);
      }
    }
  }
  SDDS_WriteLayout(&SDDS_dataset);

  return 0;
}


int row = 0;
int ttf2_user_loop(ttf2_daq_getdata* data){
  u_int evid, tup, tlow;
  int status;
  u_int len;
  int32_t *col1Values=NULL;
  float *col2Values;
  uint32_t index;
  int pageStarted = 0;
  int rows = 0;
  int firstSample = 0;

  if(data->GetEventInfo(&evid, NULL, &len, &tup, &tlow, &status, NULL)) {
    printf("ttf2_user_loop(): failed to get event info\n");
    return -1;
  }

  char number[64];

  if(first_event==-1) first_event = evid;
  last_event = evid;

  if(outmode) {
    sprintf(number,"0x%X\t\t\t",  status);
    fputs(number, fd);
  }

  float dt;

  int ndata;
  float start, inc;
  int groups;
  float groups_inc;
  ttf2_spect_statistics_2010 *stat;
  int nchan;

  int samples = 12;

  if(nbunch) samples = nbunch;

  if (!sdds_header_written) {
    ttf2_sdds_header(data);
    sdds_header_written=1;
    if (singleMode) {
      SDDS_StartTable(&SDDS_dataset, 0);
    }
  }
  if (spectrumMode) {
    SDDS_StartTable(&SDDS_dataset, 0);
  }
  if (singleMode) {
    SDDS_LengthenTable(&SDDS_dataset, 1);
    sprintf(number,"%d.%d", tup,tlow );
    SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, row, (char*)"Time", strtod(number, (char**)NULL), (char*)"EventID", (uint32_t)evid, NULL);
  }

  for(int i=0; i<nchans; i++) {
    int ddtt = data->SetProp(chans[i]);
    if (ddtt == -11) {
      fprintf(stderr, "Error: Not all channels are available for the same pulses\n");
      exit(1);
    }
    if(data->GetServBlockInfo(NULL, NULL, NULL,
			      NULL, NULL, &status, NULL)) {
      printf("ttf2_user_loop(): failed to get Server Block info\n");
      return -1;
    }
    
    if(outmode) {  
      sprintf(number,"0x%X\t\t\t",  status);
      fputs(number, fd);
    }
    
    if(data->GetChanInfo(NULL, NULL, &status, NULL)) {
      printf("ttf2_user_loop(): failed to get Server Block info\n");
      return -1;
    }
    if(outmode) {
      sprintf(number,"0x%X\t\t\t",  status);
      fputs(number, fd);
    }
    

    if(status) bad_chans++;
    switch(ddtt) {
    case 1: // - string,
      sprintf(number,"1");
      fputs(number, fd); fputs("\t", fd);
      break;
    case 2: // - single,
      // first check it it's slow or NameValue
      if(data->GetValNamPrm(NULL, NULL, NULL, &nchan, NULL)) {
	if (spectrumMode) {
	  SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, chans[i], data->GetSingle(0), NULL);
	} else if (singleMode) {
	  SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, row, chans[i], data->GetSingle(0), NULL);
	}
      } else {
	if (nchan != 1) {
	  fprintf(stderr, "Error: cannot handle channels with multiple sub-channels yet\n");
	  exit(1);
	}
	for(int ch=0; ch<nchan; ch++) {
	  char *nm;
	  data->GetValNamChan(&nm, &dt, ch);
	  if (spectrumMode) {
	    SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, chans[i], dt, NULL);
	  } else if (singleMode) {
	    SDDS_SetRowValues(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, row, chans[i], data->GetSingle(0), NULL);
	  }
	}
      }
      break;
    case 3: // - spectrum
      nchan = data->GetNmemb();
      for(int ch=0; ch<nchan; ch++) {
	if(!data->GetSpectPar(&start, &inc, NULL, &ndata, ch, &groups, &groups_inc, &stat)) {
	  if(!ndata) {
	    fputs("Not found",  fd); fputs("\t", fd);
	  } else {

	    int st = 0;

	    if(nbunch) {
	      if(start != 0.0) {
		st = (int)(700. - start);
		if(st <0) st = 0;
	      }
	    } 
	    
	    if(zerocheck) {
	      bool all_zero = true;
	      for(int ii=0; ii<ndata; ii++) if(data->GetSpectVal(ch, ii) != 0.0) {
		all_zero = false;
		break;
	      }
	      if(all_zero) printf("ALL ZEROS for %d ndata: %d st: %d\n", evid, ndata, st);
	    }
	    if (!pageStarted)
	      col1Values = (int32_t*)malloc(sizeof(int32_t) * (ndata<samples+st?ndata:samples+st));
	    col2Values = (float*)malloc(sizeof(float) * (ndata<samples+st?ndata:samples+st));
	    index=0;
	    for(int ii=st; ii<ndata && ii<samples+st; ii++){
	      if (!pageStarted)
		col1Values[index] = (int32_t)(start+index);
	      col2Values[index] = (float)data->GetSpectVal(ch, ii);
	      index++;
	    }
	    if (!pageStarted) {
	      SDDS_LengthenTable(&SDDS_dataset, index);
	      sprintf(number,"%d.%d", tup,tlow );
	      SDDS_SetParameters(&SDDS_dataset, SDDS_SET_BY_NAME | SDDS_PASS_BY_VALUE, (char*)"Time", strtod(number, (char**)NULL), (char*)"EventID", (uint32_t)evid, NULL);
	      SDDS_SetColumn(&SDDS_dataset, SDDS_SET_BY_NAME, col1Values, index, (char*)"BunchNumber");
	      free(col1Values);
	      pageStarted = 1;
	      rows = index;
	      firstSample = st;
	    } else {
	      if (rows != index) {
		fprintf(stderr, "Error: all channels must have the same number of samples\n");
		exit(1);
	      }
	      if (st != firstSample) {
		fprintf(stderr, "Error: the sample numbers don't match up for all the channels\n");
		exit(1);
	      }
	    }
	    if (nchan > 1) {
	      char buffer[100];
	      sprintf(buffer, "%s:%d", chans[i], ch);
	      SDDS_SetColumn(&SDDS_dataset, SDDS_SET_BY_NAME, col2Values, index, buffer);
	    } else {
	      SDDS_SetColumn(&SDDS_dataset, SDDS_SET_BY_NAME, col2Values, index, chans[i]);
	    }
	    free(col2Values);

	  }
	} else {
	  fputs("No data",  fd); fputs("\t", fd);
	}
      }
      break;
    case 4: // - images
      {
	u_int num;
	data->GetEventInfo(&num, NULL, NULL,
		   &tup, &tlow, &status, NULL);
	int nframe,compr, bpp, leni;
	if(!data->GetImagePar(&nframe, &compr, &bpp, &leni)) {
	  int xstart,  xinc, nx, ystart, yinc, ny;
	  if(data->GetImageSize(&xstart, &xinc, &nx,
				&ystart, &yinc, &ny) == leni) {
	    ttf2_image_ext_frame_head*  im_header =  data->GetImageHeader();
	    char* buf = data->GetImage();
	    short *sh = (short *)buf;
	    u_int *in;
	    ttf2_image_ext_frame_head* head =   data->GetImageHeader();
	    	    	    printf("%i:%i_%i(%d) %s  size:%d:%d start:%d:%d bin:%d:%d bpp:%d compr:%d frame:%d length:%d data:[%d %d %d %d %d %d]\n",
	    	   tup,tlow,num,status,
	    	   chans[i], nx, ny, xstart, ystart,  xinc, yinc, bpp, compr, nframe, leni, 
	    	   sh[0]&0xFFFF, sh[1]&0xFFFF, sh[2]&0xFFFF, sh[3]&0xFFFF, sh[4]&0xFFFF, sh[5]&0xFFFF);

	    	    print_data_type(data->GetChanHead(), 1, 0); 
	    

	    u_int dti = 0;
	    switch(bpp) {
	    case 1:
	       fputs("[", fd);
	      for(int kk=0; kk<leni/bpp && kk<MY_MAX_IMAGE_PTS; kk++) {
		dti = (u_int)(buf[kk]&0xFF);
		sprintf(number,"%d\t",dti);
		fputs(number, fd);
	      }
	      fputs("]", fd);

	      break;
	    case 2:
	      sh = (short*)buf;
	      fputs("[", fd);
	      for(int kk=0; kk<leni/bpp && kk<MY_MAX_IMAGE_PTS; kk++) {
		dti = (u_int)(sh[kk]&0xFFFF);
		sprintf(number,"%d\t",dti);
		fputs(number, fd);
	      }
	      fputs("]", fd);
	      break;
	    case 3:
	      dti = (u_int)(buf[0]&0xFF); // only one color
	      break;
	    case 4:
	      in = (u_int*)buf;
	      dti = in[0];
	      break;
	    default:
	      printf("Invalid bpp:%d for %s\n", bpp, chans[i]);
	    }
	    // sprintf(number,"[%d]",dti);
	    // fputs(number, fd); fputs("\t", fd);
	  } else {
	    fputs("Invalid Image", fd); fputs("\t", fd);
	  }
	} else {
	    fputs("No Image", fd); fputs("\t", fd);
	} 
      }
      break;
    default:
      if(fd)  fputs("Not found",  fd);
    }

    
  }
  if (spectrumMode) {
    SDDS_WriteTable(&SDDS_dataset);
  } else if (singleMode) {
    row++;
  }

  

  return 0;
}

int ttf2_user_end(ttf2_daq_reader* reader){

  if (!quiet) {
    printf("READER FINISHED statistics: events: %d  empty_events:%d\n", 
	   reader->get_number_of_events(),
	   reader->get_number_of_empty_events());
    
    if(first_event != -1 && last_event !=-1) {
      printf("Event efficiency: %f \n",   ((float)(reader->get_number_of_events()-bad_chans-1))/((float)(last_event - first_event)) * 100);
    }
    printf("Bad chans: %d\n", bad_chans);
  }
  if(fd) fclose(fd);
  if (singleMode) {
    SDDS_WriteTable(&SDDS_dataset);
  }
  SDDS_Terminate(&SDDS_dataset);
  return 0;
}

int ttf2_user_end_2010(ttf2_daq_reader_2010* reader){

  if (!quiet) {
    printf("READER FINISHED statistics: events: %d  empty_events:%d\n", 
	   reader->get_number_of_events(),
	   reader->get_number_of_empty_events());
    
    if(first_event != -1 && last_event !=-1) {
      printf("Event efficiency: %f \n",   ((float)(reader->get_number_of_events()-bad_chans-1))/((float)(last_event - first_event)) * 100);
    }
    printf("Bad chans: %d\n", bad_chans);
  }
  if(fd) fclose(fd);
  if (singleMode) {
    SDDS_WriteTable(&SDDS_dataset);
  }
  SDDS_Terminate(&SDDS_dataset);
  return 0;
}
