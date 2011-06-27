// User interface for data extracted from RAW files
// V.Rybnikov -FLA-
// created 29 May 2008




#include "ttf2_daq_reader.h"
#include "ttf2_daq_data.h"

#ifndef ttf2_daq_user_H
#define ttf2_daq_user_H

extern void ttf2_user_help();
extern int ttf2_user_init(int argc, char **argv, ttf2_daq_reader*);
extern int ttf2_user_init_2010(int argc, char **argv, ttf2_daq_reader_2010*);
extern int ttf2_user_loop(ttf2_daq_getdata*);
extern int ttf2_user_end(ttf2_daq_reader*);
extern int ttf2_user_end_2010(ttf2_daq_reader_2010*);

#endif
