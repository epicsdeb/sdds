
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "lzoconf.h"
#include "lzo1x.h"
#include "lzo1y.h"
#include <zlib.h>
#include <assert.h>
#include <event.hh>
#include <event_util.h>
#include <mtp.h>
#include "parse_chan_descr.h"
#include <pthread.h>
#include <sys/mman.h>
#include "ttf2_daq_slicer.h"
#include "ttf2_daq_reader.h"
#include "ttf2_daq_data.h"
#include "ttf2_daq_reader_2010.h"
#include "daq_server_util_def.h"
#include "ttf2_daq_user.h"



int debug = 0;

int nbunch = 10000;
int outmode = 0;
int quietmode = 1;

int main (int argc, char **argv) {

  char * xml = NULL;
  bool list = false;
  bool all = false;    
  bool files = false;    
  bool tmode = false;
  bool check = false;
  int m;
  int cycles = 1;
  char t_start[32];
  int t_start_option, t_stop_option;
  char t_stop[32];
  char exp[TTF2_EXP_NAME_LENGTH];
  char ddir[1024];
  char cdir[1024];
  char **channelName=NULL;
  char channelNames=0;
  uint *eventID=NULL;
  uint eventIDs=0;

  

  sprintf(exp, "linac");
  sprintf(ddir, "/daq_data/ilc/LINAC");
  sprintf(cdir, "/daq/ttf2/admtemp");
  t_start_option = t_stop_option = 0;

  //  ttf2_eventheader *event_header;

  //  ttf2_serv_block_head *srvb_header;
  // ttf2_serv_block_subhead *chan_header;


  ttf2_time_id ev_id_old;

  memset((void*)&ev_id_old, 0, sizeof(ev_id_old));


  m = 0;


  ttf2_daq_reader reader;
  ttf2_daq_reader_2010 reader_2010;
  ttf2_daq_getdata data;

  for(int i=1; i<argc; i++) {
    if(strstr(argv[i], "-") == argv[i]) {
      if(!strcmp(argv[i], "-xml")) {
	if(i++ < argc) xml = argv[i];
      } else if(!strcmp(argv[i], "-list")) list = true;
      else if(!strcmp(argv[i], "-files")) files = true;
      else if(!strcmp(argv[i], "-tmode")) tmode = true;
      else if(!strcmp(argv[i], "-all")) all = true;
      else if(!strcmp(argv[i], "-check")) check = true;
      else if(strstr(argv[i], "-g")) sscanf(argv[i], "-g%d", &debug);
      else if(!strcmp(argv[i], "-tstart")) {
	i++;
	if(i < argc) {
	  sprintf(t_start, argv[i]);
	  t_start_option = 1;
	}
      }
      else if(!strcmp(argv[i], "-tstop")) {
	i++;
	if(i < argc) {
	  sprintf(t_stop, argv[i]);
	  t_stop_option = 1;
	}
      }
      else if(!strcmp(argv[i], "-exp")) {
	i++;
	if(i < argc) {
	  sprintf(exp, argv[i]);
	}
      }
      else if(!strcmp(argv[i], "-ddir")) {
	i++;
	if(i < argc) {
	  sprintf(ddir, argv[i]);
	}
      }
      else if(!strcmp(argv[i], "-cdir")) {
	i++;
	if(i < argc) {
	  sprintf(cdir, argv[i]);
	}
      }
      else if(!strcmp(argv[i], "-chan")) {
	i++;
	if(i < argc) {
	  if (channelNames == 0) {
	    channelName = (char**)malloc(sizeof(char*));
	  } else {
	    channelName = (char**)realloc(channelName, sizeof(*channelName) * (channelNames+1));
	  }
	  channelName[channelNames] = (char*)malloc(strlen(argv[i]) + 1);
	  sprintf(channelName[channelNames], argv[i]);
	  channelNames++;
	}
      }
      else if(!strcmp(argv[i], "-nbunch")) {
	i++;
	if(i < argc) {
	  nbunch = atoi(argv[i]);
	}
      }
      else if(!strcmp(argv[i], "-eventid")) {
	i++;
	if(i < argc) {
	  if (eventIDs == 0) {
	    eventID = (uint*)malloc(sizeof(*eventID));
	  } else {
	    eventID = (uint*)realloc(eventID, sizeof(*eventID) * (eventIDs+1));
	  }
	  eventID[eventIDs] = atoi(argv[i]);
	  eventIDs++;
	}
      } else if(!strcmp(argv[i], "-outmode")) {
	i++;
	if(i < argc) {
	  outmode = atoi(argv[i]);
	}
      } else if(!strcmp(argv[i], "-cycles")) {
	i++;
	if(i < argc) {
	  cycles = atoi(argv[i]);
	}
      } 
    } 
  }
  
  if (channelNames == 0) {
    fprintf(stderr, "Error: missing the -chan option\n");
    exit(1);
  }
  if (t_start_option == 0) {
    fprintf(stderr, "Error: missing the -tstart option\n");
    exit(1);
  }
  if (t_stop_option == 0) {
    fprintf(stderr, "Error: missing the -tstop option\n");
    exit(1);
  }
  /*
  if(!xml) {
    printf("Usage: %s -xml file (-list) (-all) (-files) (-tmode) (-gmm) (-nbunch nbunch) (-outmode outmode) (-cycles cycles)\n", argv[0]);
    printf(" file       - XML file with request for precessing\n");
    printf("  -list     - will print out channel list\n");
    printf("  -files    - will print out file list\n");
    printf("  -tmode    - event synchronization by time (otherwise default by EvID) \n");
    printf("  -check    - check files with data\n");
    printf("  -all      - data will be processed only if all channels found\n");
    printf(" mm         - debug level\n");
    printf("        1   - events ids (possible jumps) \n");
    printf("        2   - dumps the request for processing \n");
    printf("        3   - dumps list of files found for time period\n");
    printf("        4   - dumps number of channels returned by reader\n");
    printf("        5   - dumps names of channels form reader\n");
    printf("        6   - dumps statistics\n");
    printf("       -1   - dumps everything\n");
    printf("    nbunch  - number of required bunches (can be used in user code)\n");
    printf("   outmode  - 0 - short (default) 1 - extended (can be used in user code)\n");
    printf("   cycles   - number of times to run the whole procesing (default: %d)\n", cycles);
    printf("\n\n User help:\n");
    ttf2_user_help();
    exit(-1);
  }
  */
  daq_server_request request;

  for(int kk=0; kk<cycles; kk++) {
 
    reader.set_data_dir((char*)"");
    reader_2010.set_data_dir((char*)"");
    ttf2_daq_request daq_request;
    

    
    memset((void*)&request, 0, sizeof(request));
    
    request.id = (u_int)time(NULL);
    request.scan_mode = TTF2_DAQ_SCANM_MAX;
    request.comm_mode = TTF2_DAQ_COMM_PUSH;
    request.req_type = TTF2_DAQ_REQ_DATA;
    request.run_first = -1;
    request.run_last = -1;
    
    // reading the XML file
    /*
      if(parse_daq_request_from_file(xml, &request)) {
      printf("Parsing of XML file failed\n");
      exit(-3);
    }
    */

    sprintf(request.t_start, t_start);
    sprintf(request.t_stop, t_stop);
    sprintf(request.exp, exp);
    sprintf(request.ddir, ddir);
    sprintf(request.cdir, cdir);

    int length, ii;
    daq_channel_entry *entry;
    for (ii=0; ii<channelNames; ii++) {
      length = strlen(channelName[ii]) +1;
      entry = (daq_channel_entry*)malloc(sizeof(daq_channel_entry));
      memset((void*)entry, 0, sizeof(daq_channel_entry));
      entry->name = (char*)malloc(length);
      snprintf(entry->name, length, channelName[ii]);
      request.chan_list.push_back(entry);
    }


    int err;
    
    if((err = validate_daq_request(&request, false))) {
      printf("Something wrong with request (dates ?) err: %d \n", err); 
      clean_request(&request);
      exit(-3);
    }
    
    
    if(debug == -1 || debug == 2) print_daq_request(&request);
    
    // configuring reader
    int ready, ready_2010=0;
    
    struct timeval start, end;
    double diff;
    int idx = 0;
    int evs = 0, ev_full = 0;
    gettimeofday(&start, NULL);
    vector<ttf2_channel_entry_long*> res;
    
    
    
    reader.set_data_dir(request.ddir);
    reader.set_catalog_dir(request.cdir);

    reader_2010.set_data_dir(request.ddir);
    reader_2010.set_catalog_dir(request.cdir);
    
    
    
    if(request.file_list.size()) {
      // we have the file list
      
      ttf2_time_id start_stop;
      memset((void*)&start_stop, 0, sizeof(start_stop));
      
      start_stop.time.timeup = request.t_start_sec;
      
      if((ready = reader.set_start_point(&start_stop)) != TTF2_DAQ_READER_OK &&
	 (ready_2010 = reader_2010.set_start_point(&start_stop)) != TTF2_DAQ_READER_OK) {
	printf("Failed to set start point\n");
	goto error;
      }
      start_stop.time.timeup = request.t_stop_sec;
      if((ready = reader.set_stop_point(&start_stop)) != TTF2_DAQ_READER_OK &&
	 (ready_2010 = reader_2010.set_stop_point(&start_stop)) != TTF2_DAQ_READER_OK) {
	printf("Failed to set stop point\n");
	goto error;
      }
      if((ready = reader.init(&request.file_list, &request.chan_list))  != TTF2_DAQ_READER_OK &&
	 (ready_2010 = reader_2010.init(&request.file_list, &request.chan_list))  != TTF2_DAQ_READER_OK) {
	printf("Failed to init reader with lists \n");
	goto error;
      }

      printf("ready: %d ready_2010: %d\n", ready, ready_2010);


    } else {
      // we have to find the file list
      
      
      memset((void*)&daq_request, 0, sizeof(daq_request));
      
      daq_request.start_time.time.timeup = request.t_start_sec;
      daq_request.stop_time.time.timeup = request.t_stop_sec;
      
      sprintf(daq_request.exp_name, "%s", request.exp);
      
      if(strlen(request.ddir)) {
	sprintf(daq_request.ddir, "%s", request.ddir);
      }
      
      if(strlen(request.cdir)) sprintf(daq_request.cdir, "%s", request.cdir);
      daq_request.time_diff.timeup = 0;
      daq_request.time_diff.timelow =  TTF2_DAQ_READ_CHAN_MAX_TIME_DIFF;
      daq_request.period.event_number = TTF2_DAQ_READ_CHAN_EVENT_ID_STEP_DEF;
      daq_request.period.time.timeup = 0;
      daq_request.period.time.timelow = (u_int)TTF2_DAQ_READ_CHAN_EVENT_PERIOD;
      daq_request.synch_mode = TTF2_DAQ_SLICER_SYNCH_MODE_DEF;
      
      daq_request.run_first =  request.run_first;
      daq_request.run_last =  request.run_last;
      
      
      if((ready = reader.init(&daq_request, &request.chan_list)) != TTF2_DAQ_READER_OK) {
	daq_request.time_diff.timelow =  TTF2_DAQ_READ_CHAN_MAX_TIME_DIFF_2010;
	daq_request.period.time.timelow = (u_int)TTF2_DAQ_READ_CHAN_EVENT_PERIOD_2010;
	if((ready_2010 = reader_2010.init(&daq_request, &request.chan_list)) != TTF2_DAQ_READER_OK) {
	  printf("Failed to init reader with request error: %d\n", ready);
	  goto error;
	}
      }
    }
    
    if(list) {
      // we have to provide only channel list
      vector<chan_descr_2010*> vc;
      vc.clear();
      
      if(ready == TTF2_DAQ_READER_OK) {

	if(!reader.get_file_name(0) || ((ready = reader.fill_chan_descr(reader.get_file_name(0), &vc)) != TTF2_DAQ_READER_OK) || !vc.size()) { 
	  if(!reader.get_file_name(0))  printf("No files found (check the request)\n");
	  else if(ready != TTF2_DAQ_READER_OK) printf("Failed to extract descriptions from %s\n", reader.get_file_name(0));
	  else printf("No description list available (error: %s) from %s\n", reader.get_last_error(), reader.get_file_name(0));
	} else {
	  char dt_name[256];
	  // we can produce the channel list
	  for(u_int i=0; i<vc.size(); i++) {
	    chan_descr_2010* ent = vc[i];
	    if(ent) {
	      printf("[%d]\tName: %s \tType: %s\tEvents: %d\n", i+1, ent->daq_name, 
		     block_name_from_mask(dt_name, sizeof(dt_name), ent->dtype, NULL),
		     ent->count);
	      
	    }
	  }
	  vc.clear();
	  
	  // the memory for entries will be cleaned up later in reader.clean_up()
	}
	
	clean_request(&request);
	reader.clean_up();
	if(ready == TTF2_DAQ_READER_OK) return 0;
	return (-4);
      } else {
	if(!reader_2010.get_file_name(0) || ((ready = reader_2010.fill_chan_descr(reader.get_file_name(0), &vc)) != TTF2_DAQ_READER_OK) || !vc.size()) { 
	  if(!reader_2010.get_file_name(0))  printf("No files found (check the request)\n");
	  else if(ready != TTF2_DAQ_READER_OK) printf("Failed to extract descriptions from %s\n", reader_2010.get_file_name(0));
	  else printf("No description list available (error: %s) from %s\n", reader_2010.get_last_error(), reader_2010.get_file_name(0));
	} else {
	  char dt_name[256];
	  // we can produce the channel list
	  for(u_int i=0; i<vc.size(); i++) {
	    chan_descr_2010* ent = vc[i];
	    if(ent) {
	      printf("[%d]\tName: %s \tType: %s\tEvents: %d\n", i+1, ent->daq_name, 
		     block_name_from_mask(dt_name, sizeof(dt_name), ent->dtype, NULL),
		     ent->count);
	    }
	  }
	  vc.clear();
	  
	  // the memory for entries will be cleaned up later in reader.clean_up()
	}
	
	clean_request(&request);
	reader_2010.clean_up();
	if(ready == TTF2_DAQ_READER_OK) return 0;
	return (-4);
      }
    } else if(check) {
      // we have to check the files
      if(ready == TTF2_DAQ_READER_OK) {
	int nfiles = reader.get_number_of_files();
	if(nfiles) {
	  for(int i=0; i<nfiles; i++) {
	    char *fl = reader.get_file_name(i);
	    
	    printf("FILE: %s ", fl);
	  
	    if(!fl || !strlen(fl)) {
	      printf("Failed to get %d-th file name\n", i);
	      continue;
	    }
	    ttf2_file_get_tables gt;
	    if(!gt.get_all_tables(fl)) {
	      printf("OK\n");
	    } else {
	      printf("BAD\n");
	    }
	    gt.clean_up();
	  }
	  return 0;
	} else {
	  printf("No files found\n");
	  return -5;
	}
      } else {
	int nfiles = reader_2010.get_number_of_files();
	if(nfiles) {
	  for(int i=0; i<nfiles; i++) {
	    char *fl = reader_2010.get_file_name(i);
	    
	    printf("FILE: %s ", fl);
	  
	    if(!fl || !strlen(fl)) {
	      printf("Failed to get %d-th file name\n", i);
	      continue;
	    }
	    ttf2_file_get_tables_2010 gt;
	    if(!gt.get_all_tables(fl)) {
	      printf("OK\n");
	    } else {
	      printf("BAD\n");
	    }
	    gt.clean_up();
	  }
	  return 0;
	} else {
	  printf("No files found\n");
	  return -5;
	}
      }
    }
    
    //  processing data
    
    gettimeofday(&start, NULL);

    if(ready == TTF2_DAQ_READER_OK) {

      if(tmode || (request.scan_mode & TTF2_DAQ_SCAN_BY_TIME))  reader.set_synch_mode(TTF2_DAQ_SLICER_SYNCH_MODE_TIME);
      
      clean_request(&request);
      
      if(!quietmode) 
	printf("SYNCH_MODE: %d\n", reader.get_synch_mode());

      if((ready = reader.start()) != TTF2_DAQ_READER_OK) {
	printf("Failed to start reader status: %d\n", ready);
	reader.clean_up();
	return (-5);
      }
      
      if(debug == -1 || debug == 5)  reader.print_channels();
      
      if(debug == -1 || debug == 3 || files)  {
	reader.print_files();
	
	if(files) {
	  reader.clean_up();
	  return 0;
	}
      }
      
      
      
      
      ttf2_user_init(argc, argv, &reader);
      {
	ttf2_time_id *st = reader.get_start_point();
	ttf2_time_id *stp = reader.get_stop_point();
	if (!quietmode)
	  printf("Start: %d stop:%d\n", st->time.timeup, stp->time.timeup);
      }
      
      int eventIndex = 0;

      while((ready = reader.get_data(NULL, &res)) == TTF2_DAQ_READER_OK) {

	if(debug == -1 || debug == 4) printf("RES.SIZE() : %d\n", res.size());
	
	if(res.size()) {
	  
	  evs++;
	  if(res.size() == reader.get_number_of_channels())   ev_full++;
	  
	  if(debug == -1 || debug == 6) {
	    if(!(evs%100)) { 
	      printf("%d\r", evs); fflush(0);
	    }
	  }
	  
	  if(!all || (res.size() == reader.get_number_of_channels())) {
	    u_int evid, tup, tlow;
	    int status;
	    u_int len;

	    data.Clean();
	    data.SetRes(&res);

	    if(data.GetEventInfo(&evid, NULL, &len, &tup, &tlow, &status, NULL)) {
	      printf("ttf2_user_loop(): failed to get event info\n");
	      return -1;
	    }
	    if (eventIDs == 0) {
	      ttf2_user_loop(&data);
	    } else if (eventIndex < eventIDs) {
	      
	      while (evid > eventID[eventIndex]) {
		eventIndex++;
	      }
	      if (evid == eventID[eventIndex]) {
		ttf2_user_loop(&data);
		eventIndex++;
	      }
	    }
	  }
	}
      }
      
      
      
      
      gettimeofday(&end, NULL);
      
      ttf2_user_end(&reader);
      
      if(ready != TTF2_DAQ_READER_OUT_OF_WINDOW) printf("Reader has finished with status: %d\n", ready);
      
      diff = ((double)end.tv_sec * 1000000. +  (double)end.tv_usec) -
	((double)start.tv_sec * 1000000. +  (double)start.tv_usec);
      
      if(debug == -1 || debug == 6) {
	
	if(evs) printf("Took %f sec for %d events (%d)  Rate: %f ms/ev\n", diff/1000000., evs,ev_full,  diff/1000./evs);
	else  printf("Took %f sec \n", diff/1000000.);
      }
      
      reader.clean_up();
      
    } else {
      if(tmode || (request.scan_mode & TTF2_DAQ_SCAN_BY_TIME))  reader_2010.set_synch_mode(TTF2_DAQ_SLICER_SYNCH_MODE_TIME);
     

      printf("READER_2010 SYNCH MODE: %d\n", reader_2010.get_synch_mode());
 
      clean_request(&request);
      
      if((ready_2010 = reader_2010.start()) != TTF2_DAQ_READER_OK) {
	printf("Failed to start reader_2010 status: %d\n", ready_2010);
	reader_2010.clean_up();
	return (-5);
      }
      
      if(debug == -1 || debug == 5)  reader_2010.print_channels();
      
      if(debug == -1 || debug == 3 || files)  {
	reader_2010.print_files();
	
	if(files) {
	  reader_2010.clean_up();
	  return 0;
	}
      }
      
      
      
      
      ttf2_user_init_2010(argc, argv, &reader_2010);
      {
	ttf2_time_id *st = reader_2010.get_start_point();
	ttf2_time_id *stp = reader_2010.get_stop_point();
	if (!quietmode)
	  printf("Start: %d stop:%d\n", st->time.timeup, stp->time.timeup);
      }
      
      while((ready_2010 = reader_2010.get_data(NULL, &res)) == TTF2_DAQ_READER_OK) {
	if(debug == -1 || debug == 4) printf("RES.SIZE() : %d\n", res.size());
	
	if(res.size()) {
	  
	  evs++;
	  if(res.size() == reader_2010.get_number_of_channels())   ev_full++;
	  
	  if(debug == -1 || debug == 6) {
	    if(!(evs%100)) { 
	      printf("%d\r", evs); fflush(0);
	    }
	  }
	  
	  if(!all || (res.size() == reader_2010.get_number_of_channels())) {
	    data.Clean();
	    data.SetRes(&res);
	    ttf2_user_loop(&data);
	  }
	}
      }
      
      
      
      
      gettimeofday(&end, NULL);
      
      ttf2_user_end_2010(&reader_2010);
      
      if(ready_2010 != TTF2_DAQ_READER_OUT_OF_WINDOW) printf("Reader has finished with status: %d\n", ready_2010);
      
      diff = ((double)end.tv_sec * 1000000. +  (double)end.tv_usec) -
	((double)start.tv_sec * 1000000. +  (double)start.tv_usec);
      
      if(debug == -1 || debug == 6) {
	
	if(evs) printf("Took %f sec for %d events (%d)  Rate: %f ms/ev\n", diff/1000000., evs,ev_full,  diff/1000./evs);
	else  printf("Took %f sec \n", diff/1000000.);
      }
      
      reader_2010.clean_up();
    }
  }


  return 0;      
  
 error:
  
  clean_request(&request);
  reader.clean_up();
  reader_2010.clean_up();
  
  return (-5);
}
