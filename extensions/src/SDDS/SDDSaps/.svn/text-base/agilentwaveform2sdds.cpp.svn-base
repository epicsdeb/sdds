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
 Revision 1.3  2009/12/10 17:31:46  soliday
 Changed so that it builds on 64bit solaris.

 Revision 1.2  2009/10/26 18:40:26  soliday
 Fixed issue on Mac computers.

 Revision 1.1  2009/08/18 19:13:07  soliday
 Agilent scope communication software based off of software from Steve Sharples
 of the Univ. of Nottingham

 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <pthread.h>
#include "SDDS.h"

typedef long Device_Link;
typedef long Device_Flags;
typedef long Device_ErrorCode;

struct Device_Error {
  Device_ErrorCode error;
};
typedef struct Device_Error Device_Error;

struct Create_LinkParms {
  long clientId;
  bool_t lockDevice;
  u_long lock_timeout;
  char *device;
};
typedef struct Create_LinkParms Create_LinkParms;

struct Create_LinkResp {
  Device_ErrorCode error;
  Device_Link lid;
  u_short abortPort;
  u_long maxRecvSize;
};
typedef struct Create_LinkResp Create_LinkResp;

struct Device_WriteParms {
  Device_Link lid;
  u_long io_timeout;
  u_long lock_timeout;
  Device_Flags flags;
  struct {
    u_int data_len;
    char *data_val;
  } data;
};
typedef struct Device_WriteParms Device_WriteParms;

struct Device_WriteResp {
  Device_ErrorCode error;
  u_long size;
};
typedef struct Device_WriteResp Device_WriteResp;

struct Device_ReadParms {
  Device_Link lid;
  u_long requestSize;
  u_long io_timeout;
  u_long lock_timeout;
  Device_Flags flags;
  char termChar;
};
typedef struct Device_ReadParms Device_ReadParms;

struct Device_ReadResp {
  Device_ErrorCode error;
  long reason;
  struct {
    u_int data_len;
    char *data_val;
  } data;
};
typedef struct Device_ReadResp Device_ReadResp;

#define create_link 10
#define device_write 11
#define device_read 12
#define destroy_link 23

#define	VXI11_DEFAULT_TIMEOUT	10000	/* in ms */
#define	VXI11_READ_TIMEOUT	2000	/* in ms */
#define	VXI11_CLIENT		CLIENT
#define	VXI11_LINK		Create_LinkResp
#define	VXI11_MAX_CLIENTS	256	/* maximum no of unique IP addresses/clients */
#define	VXI11_NULL_READ_RESP	50	/* vxi11_receive() return value if a query
					 * times out ON THE INSTRUMENT (and so we have
					 * to resend the query again) */
#define	VXI11_NULL_WRITE_RESP	51	/* vxi11_send() return value if a sent command
					 * times out ON THE INSTURMENT. */

struct	CLINK {
  VXI11_CLIENT	*client;
  VXI11_LINK	*link;
} ;
typedef	struct	CLINK CLINK;

#define DEVICE_CORE 0x0607AF
#define DEVICE_CORE_VERSION 1
#define RCV_END_BIT	0x04	// An end indicator has been read
#define RCV_CHR_BIT	0x02	// A termchr is set in flags and a character which matches termChar is transferred


char	VXI11_IP_ADDRESS[VXI11_MAX_CLIENTS][20];
CLIENT	*VXI11_CLIENT_ADDRESS[VXI11_MAX_CLIENTS];
int	VXI11_DEVICE_NO = 0;
int	VXI11_LINK_COUNT[VXI11_MAX_CLIENTS];

static struct timeval TIMEOUT = { 25, 0 };


#ifndef	BOOL
#define	BOOL	int
#endif
#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

BOOL sc(char *con,char *var);

int agilent_init(CLINK *clink);
int agilent_set_averages(CLINK *clink, 
                         int no_averages);
void agilent_set_for_auto(CLINK *clink);
long agilent_get_data(CLINK *clink, 
                      char chan, 
                      char *buf, 
                      unsigned long buf_len, 
                      unsigned long timeout);
long agilent_get_data(CLINK *clink, 
                      char chan, 
                      int digitise, 
                      char *buf, 
                      unsigned long buf_len, 
                      unsigned long timeout);
int agilent_get_preamble(CLINK *clink, 
                         char* buf, 
                         unsigned long buf_len);
void agilent_scope_channel_str(char chan, 
                               char *source);
int agilent_set_for_capture(CLINK *clink, 
                            double s_rate, 
                            long npoints, 
                            unsigned long timeout);
long agilent_calculate_no_of_bytes(CLINK *clink, 
                                   char chan, 
                                   unsigned long timeout);

int vxi11_open_device(const char *ip, 
                      CLINK *clink);
int vxi11_open_device(const char *ip, 
                      CLINK *clink, 
                      char *device);
int vxi11_open_device(const char *ip, 
                      CLIENT **client, 
                      VXI11_LINK **link, 
                      char *device);
int vxi11_open_link(const char *ip, 
                    CLIENT **client, 
                    VXI11_LINK **link, 
                    char *device);
int vxi11_send(CLINK *clink, 
               const char *cmd);
int vxi11_send(CLINK *clink, 
               const char *cmd, 
               unsigned long len);
int vxi11_send(CLIENT *client, 
               VXI11_LINK *link, 
               const char *cmd);
int vxi11_send(CLIENT *client, 
               VXI11_LINK *link, 
               const char *cmd, 
               unsigned long len);
int vxi11_close_device(const char *ip, 
                       CLINK *clink);
int vxi11_close_device(const char *ip, 
                       CLIENT *client, 
                       VXI11_LINK *link);
int vxi11_close_link(const char *ip, 
                     CLIENT *client, 
                     VXI11_LINK *link);
double vxi11_obtain_double_value(CLINK *clink, 
                                 const char *cmd);
double vxi11_obtain_double_value(CLINK *clink, 
                                 const char *cmd, 
                                 unsigned long timeout);
long vxi11_send_and_receive(CLINK *clink, 
                            const char *cmd, 
                            char *buf, 
                            unsigned long buf_len, 
                            unsigned long timeout);
long vxi11_receive(CLINK *clink, 
                   char *buffer, 
                   unsigned long len);
long vxi11_receive(CLIENT *client, 
                   VXI11_LINK *link, 
                   char *buffer, 
                   unsigned long len, 
                   unsigned long timeout);
long vxi11_receive(CLINK *clink, 
                   char *buffer, 
                   unsigned long len, 
                   unsigned long timeout);
long vxi11_obtain_long_value(CLINK *clink, 
                             const char *cmd, 
                             unsigned long timeout);
long vxi11_obtain_long_value(CLINK *clink, 
                             const char *cmd);
long vxi11_receive_data_block(CLINK *clink, 
                              char *buffer, 
                              unsigned long len, 
                              unsigned long timeout);

enum clnt_stat create_link_1(Create_LinkParms *argp, 
                             Create_LinkResp *clnt_res, 
                             CLIENT *clnt);
enum clnt_stat device_write_1(Device_WriteParms *argp, 
                              Device_WriteResp *clnt_res, 
                              CLIENT *clnt);
enum clnt_stat destroy_link_1(Device_Link *argp, 
                              Device_Error *clnt_res, 
                              CLIENT *clnt);
enum clnt_stat device_read_1(Device_ReadParms *argp, 
                             Device_ReadResp *clnt_res, 
                             CLIENT *clnt);

bool_t xdr_Create_LinkParms (XDR *xdrs, 
                             Create_LinkParms *objp);
bool_t xdr_Create_LinkResp (XDR *xdrs, 
                            Create_LinkResp *objp);
bool_t xdr_Device_ErrorCode (XDR *xdrs, 
                             Device_ErrorCode *objp);
bool_t xdr_Device_Link (XDR *xdrs, 
                        Device_Link *objp);
bool_t xdr_Device_WriteParms (XDR *xdrs, 
                              Device_WriteParms *objp);
bool_t xdr_Device_WriteResp (XDR *xdrs, 
                             Device_WriteResp *objp);
bool_t xdr_Device_Flags (XDR *xdrs, 
                         Device_Flags *objp);
bool_t xdr_Device_Error (XDR *xdrs, 
                         Device_Error *objp);
bool_t xdr_Device_ReadParms (XDR *xdrs, 
                             Device_ReadParms *objp);
bool_t xdr_Device_ReadResp (XDR *xdrs, 
                            Device_ReadResp *objp);

int main(int argc,char **argv)
{
  static char   *progname;
  static char   *serverIP;
  char          chnl; /* we use '1' to '4' for channels, and 'A' to 'D' for FUNC[1...4] */
  FILE          *f_wf;
  char          wfname[256];
  long          buf_size;
  char          *buf;
  unsigned long timeout=10000; /* in ms (= 10 seconds) */

  long          bytes_returned;
  BOOL          got_ip=FALSE;
  BOOL          got_scope_channel=FALSE;
  BOOL          got_file=FALSE;
  BOOL          got_no_averages=FALSE;
  int           no_averages;
  int           index=1;
  double        s_rate=0;
  long          npoints=0;
  double        actual_s_rate;
  long          actual_npoints;
  CLINK         *clink; /* client link (actually a structure contining CLIENT and VXI11_LINK pointers) */

  short         val;
  double        vgain,voffset,hinterval,hoffset;
  long          i;



  clink = new CLINK; /* allocate some memory */
  progname = argv[0];

  while(index<argc){
    if(sc(argv[index],(char*)"-filename")||sc(argv[index],(char*)"-f")||sc(argv[index],(char*)"-file")){
      snprintf(wfname,256,"%s.sdds",argv[++index]);
      got_file=TRUE;
    }

    if(sc(argv[index],(char*)"-ip")||sc(argv[index],(char*)"-ip_address")||sc(argv[index],(char*)"-IP")){
      serverIP = argv[++index];
      got_ip = TRUE;
    }

    if(sc(argv[index],(char*)"-channel")||sc(argv[index],(char*)"-c")||sc(argv[index],(char*)"-scope_channel")){
      sscanf(argv[++index],"%c",&chnl);
      got_scope_channel=TRUE;
    }

    if(sc(argv[index],(char*)"-sample_rate")||sc(argv[index],(char*)"-s")||sc(argv[index],(char*)"-rate")){
      sscanf(argv[++index],"%lg",&s_rate);
    }

    if(sc(argv[index],(char*)"-no_points")||sc(argv[index],(char*)"-n")||sc(argv[index],(char*)"-points")){
      sscanf(argv[++index],"%ld",&npoints);
    }
			
    if(sc(argv[index],(char*)"-averages")||sc(argv[index],(char*)"-a")||sc(argv[index],(char*)"-aver")){
      sscanf(argv[++index],"%d",&no_averages);
      got_no_averages=TRUE;
    }
			
    if(sc(argv[index],(char*)"-timeout")||sc(argv[index],(char*)"-t")){
      sscanf(argv[++index],"%lu",&timeout);
    }
			
    index++;
  }

  if(got_file==FALSE||got_scope_channel==FALSE||got_ip==FALSE){
    printf("%s: grabs a waveform from an Agilent scope via ethernet, by Steve (June 06)\n",progname);
    printf("Run using %s [arguments]\n\n",progname);
    printf("REQUIRED ARGUMENTS:\n");
    printf("-ip    -ip_address     -IP      : IP address of scope (eg 128.243.74.232)\n");
    printf("-f     -filename       -file    : filename (without extension)\n");
    printf("-c     -scope_channel  -channel : scope channel (1,2,3,4,A,B,C,D)\n");
    printf("OPTIONAL ARGUMENTS:\n");
    printf("-t     -timeout                 : timout (in milliseconds)\n");
    printf("-s     -sample_rate    -rate    : set sample rate (eg 1e9 = 1GS/s)\n");
    printf("-n     -no_points      -points  : set minimum no of points\n");
    printf("-a     -averages       -aver    : set no of averages (<=0 means none)\n\n");
    printf("OUTPUTS:\n");
    printf("filename.sdds  : ascii data of waveform\n\n");
    printf("EXAMPLE:\n");
    printf("%s -ip 128.243.74.232 -f output -c 2 -s 1e9\n",progname);
    exit(1);
  }

  f_wf=fopen(wfname,"w");
  if (f_wf > 0) {
    if (vxi11_open_device(serverIP,clink) != 0) {
      printf("Quitting...\n");
      exit(2);
    }

    if (agilent_init(clink) !=0 ) {
      printf("Quitting...\n");
      exit(2);
    }

    agilent_set_for_capture(clink, s_rate, npoints, timeout);

    if (got_no_averages == TRUE) agilent_set_averages(clink, no_averages);


    buf_size = agilent_calculate_no_of_bytes(clink, chnl, timeout);
    buf=new char[buf_size];

    hinterval = vxi11_obtain_double_value(clink, ":WAV:XINC?", timeout);
    hoffset = vxi11_obtain_double_value(clink, ":WAV:XORIGIN?");
    vgain = vxi11_obtain_double_value(clink, ":WAV:YINC?");
    voffset = vxi11_obtain_double_value(clink, ":WAV:YORIGIN?");

    bytes_returned = agilent_get_data(clink, chnl, 0, buf, buf_size, timeout);
    if (bytes_returned <=0) {
      printf("Problem reading the data, quitting...\n");
      exit(2);
    }

    actual_s_rate = vxi11_obtain_double_value(clink, ":ACQ:SRAT?");
    actual_npoints = vxi11_obtain_long_value(clink, ":ACQ:POINTS?");
    printf("Sample rate used: %g (%g GSa/s); acquisition points: %ld\n",actual_s_rate, (actual_s_rate/1e9),actual_npoints);

    agilent_set_for_auto(clink);
                
    fprintf(f_wf, "SDDS1\n");
    fprintf(f_wf, "&parameter name=VerticalGain, type=double,  &end\n");
    fprintf(f_wf, "&parameter name=VerticalOffset, type=double,  &end\n");
    fprintf(f_wf, "&parameter name=HorizontalInterval, type=double,  &end\n");
    fprintf(f_wf, "&parameter name=HorizontalOffset, type=double,  &end\n");
    fprintf(f_wf, "&column name=Index, type=long,  &end\n");
    fprintf(f_wf, "&column name=Waveform, type=double,  &end\n");
    fprintf(f_wf, "&column name=Timebase, type=double,  &end\n");
    fprintf(f_wf, "&column name=DelayedTimebase, type=double,  &end\n");
    fprintf(f_wf, "&data mode=ascii, &end\n");
    fprintf(f_wf, "%lg\n", vgain);
    fprintf(f_wf, "%lg\n", voffset);
    fprintf(f_wf, "%lg\n", hinterval);
    fprintf(f_wf, "%lg\n", hoffset);
    fprintf(f_wf, "\t%ld\n", bytes_returned / 2);
                
    for (i=0; i < bytes_returned; i += 2) {
      *((char*)(&val)) = buf[i];
      *((char*)(&val)+1) = buf[i+1];
      fprintf(f_wf, "%ld %lg %lg %lg\n", (i/2), val * vgain - voffset, (i/2) * hinterval, (i/2) * hinterval + hoffset);
    }
                
    fclose(f_wf);
    delete[] buf;

    vxi11_close_device(serverIP,clink);

  }
  else {
    printf("error: could not open file for writing, quitting...\n");
    exit(3);
  }
}

/* string compare (sc) function for parsing... ignore */
BOOL sc(char *con,char *var) {
  if(strcmp(con,var)==0){
    return TRUE;
  }
  return FALSE;
}

int vxi11_open_device(const char *ip, CLINK *clink) {
  char device[6];
  strncpy(device,"inst0",6);
  return vxi11_open_device(ip, clink, device);
}

int vxi11_open_device(const char *ip, CLIENT **client, VXI11_LINK **link, char *device) {
  
#ifdef darwin
  *client = clnt_create((char*)ip, DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
#else
  *client = clnt_create(ip, DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
#endif

  if (*client == NULL) {
#ifdef darwin
    clnt_pcreateerror((char*)ip);
#else
    clnt_pcreateerror(ip);
#endif
    return -1;
  }
  
  return vxi11_open_link(ip, client, link, device);
}

int vxi11_open_device(const char *ip, CLINK *clink, char *device) {
  int	ret;
  int	l;
  int	device_no=-1;

  for (l=0; l<VXI11_MAX_CLIENTS; l++){
    if (strcmp(ip,VXI11_IP_ADDRESS[l]) == 0 ) {
      device_no=l;
    }
  }

  if (device_no < 0) {
    if (VXI11_DEVICE_NO >= VXI11_MAX_CLIENTS) {
      printf("Error: maximum of %d clients allowed\n",VXI11_MAX_CLIENTS);
      ret = -VXI11_MAX_CLIENTS;
    } else {
      ret = vxi11_open_device(ip, &(clink->client), &(clink->link), device);
      strncpy(VXI11_IP_ADDRESS[VXI11_DEVICE_NO],ip,20);
      VXI11_CLIENT_ADDRESS[VXI11_DEVICE_NO] = clink->client;
      VXI11_LINK_COUNT[VXI11_DEVICE_NO]=1;
      VXI11_DEVICE_NO++;
    }
  } else {
    clink->client = VXI11_CLIENT_ADDRESS[device_no];
    ret = vxi11_open_link(ip, &(clink->client), &(clink->link), device);
    VXI11_LINK_COUNT[device_no]++;
  }
  return ret;
}

int vxi11_open_link(const char *ip, CLIENT **client, VXI11_LINK **link, char *device) {
  
  Create_LinkParms link_parms;
  
  /* Set link parameters */
  link_parms.clientId	= (long) *client;
  link_parms.lockDevice	= 0;
  link_parms.lock_timeout	= VXI11_DEFAULT_TIMEOUT;
  link_parms.device	= device;
  
  *link = (Create_LinkResp *) calloc(1, sizeof(Create_LinkResp));
  
  if (create_link_1(&link_parms, *link, *client) != RPC_SUCCESS) {
#ifdef darwin
    clnt_perror(*client, (char*)ip);
#else
    clnt_perror(*client, ip);
#endif
    return -2;
  }
  return 0;
}

enum clnt_stat create_link_1(Create_LinkParms *argp, Create_LinkResp *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, create_link,
                    (xdrproc_t) xdr_Create_LinkParms, (caddr_t) argp,
                    (xdrproc_t) xdr_Create_LinkResp, (caddr_t) clnt_res,
                    TIMEOUT));
}

bool_t xdr_Create_LinkParms (XDR *xdrs, Create_LinkParms *objp)
{

#if defined(SOLARIS) && !defined(_LP64)
  register long *buf;
#else
  register int32_t *buf;
#endif

  if (xdrs->x_op == XDR_ENCODE) {
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_long (xdrs, &objp->clientId))
        return FALSE;
      if (!xdr_bool (xdrs, &objp->lockDevice))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->lock_timeout))
        return FALSE;

    } else {
      IXDR_PUT_INT32(buf, objp->clientId);
      IXDR_PUT_BOOL(buf, objp->lockDevice);
      IXDR_PUT_U_INT32(buf, objp->lock_timeout);
    }
    if (!xdr_string (xdrs, &objp->device, ~0))
      return FALSE;
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_long (xdrs, &objp->clientId))
        return FALSE;
      if (!xdr_bool (xdrs, &objp->lockDevice))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->lock_timeout))
        return FALSE;

    } else {
      objp->clientId = IXDR_GET_INT32(buf);
      objp->lockDevice = IXDR_GET_BOOL(buf);
      objp->lock_timeout = IXDR_GET_U_INT32(buf);
    }
    if (!xdr_string (xdrs, &objp->device, ~0))
      return FALSE;
    return TRUE;
  }

  if (!xdr_long (xdrs, &objp->clientId))
    return FALSE;
  if (!xdr_bool (xdrs, &objp->lockDevice))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->lock_timeout))
    return FALSE;
  if (!xdr_string (xdrs, &objp->device, ~0))
    return FALSE;
  return TRUE;
}

bool_t xdr_Create_LinkResp (XDR *xdrs, Create_LinkResp *objp)
{
  if (!xdr_Device_ErrorCode (xdrs, &objp->error))
    return FALSE;
  if (!xdr_Device_Link (xdrs, &objp->lid))
    return FALSE;
  if (!xdr_u_short (xdrs, &objp->abortPort))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->maxRecvSize))
    return FALSE;
  return TRUE;
}

bool_t xdr_Device_ErrorCode (XDR *xdrs, Device_ErrorCode *objp)
{
  if (!xdr_long (xdrs, objp))
    return FALSE;
  return TRUE;
}

bool_t xdr_Device_Link (XDR *xdrs, Device_Link *objp)
{
  if (!xdr_long (xdrs, objp))
    return FALSE;
  return TRUE;
}

int agilent_init(CLINK *clink) {
  int	ret;
  ret=vxi11_send(clink, ":SYSTEM:HEADER 0");
  if(ret < 0) {
    printf("Error: vxi11_send_and_receive: could not send cmd.\n");
    printf("       The function vxi11_send returned %d. ",ret);
    return ret;
  }
  vxi11_send(clink, ":ACQUIRE:COMPLETE 100");
  if (SDDS_IsBigEndianMachine() == 0) {
    vxi11_send(clink, ":WAVEFORM:BYTEORDER LSBFIRST");
  } else {
    vxi11_send(clink, ":WAVEFORM:BYTEORDER MSBFIRST");
  }
  vxi11_send(clink, ":WAVEFORM:FORMAT BINARY");
  return 0;
}

int vxi11_send(CLINK *clink, const char *cmd) {
  return vxi11_send(clink, cmd, strlen(cmd));
}

int vxi11_send(CLINK *clink, const char *cmd, unsigned long len) {
  return vxi11_send(clink->client, clink->link, cmd, len);
}

int vxi11_send(CLIENT *client, VXI11_LINK *link, const char *cmd) {
  return vxi11_send(client, link, cmd, strlen(cmd));
}

int vxi11_send(CLIENT *client, VXI11_LINK *link, const char *cmd, unsigned long len) {
  Device_WriteParms write_parms;
  int	bytes_left = (int)len;
  char	*send_cmd;
  
  send_cmd = new char[len];
  memcpy(send_cmd, cmd, len);
  
  write_parms.lid			= link->lid;
  write_parms.io_timeout		= VXI11_DEFAULT_TIMEOUT;
  write_parms.lock_timeout	= VXI11_DEFAULT_TIMEOUT;
  
  /* We can only write (link->maxRecvSize) bytes at a time, so we sit in a loop,
   * writing a chunk at a time, until we're done. */
  
  do {
    Device_WriteResp write_resp;
    memset(&write_resp, 0, sizeof(write_resp));
    
    if ((unsigned int)bytes_left <= link->maxRecvSize) {
      write_parms.flags		= 8;
      write_parms.data.data_len	= bytes_left;
    }
    else {
      write_parms.flags		= 0;
      /* We need to check that maxRecvSize is a sane value (ie >0). Believe it
       * or not, on some versions of Agilent Infiniium scope firmware the scope
       * returned "0", which breaks Rule B.6.3 of the VXI-11 protocol. Nevertheless
       * we need to catch this, otherwise the program just hangs. */
      if (link->maxRecvSize > 0) {
        write_parms.data.data_len	= link->maxRecvSize;
      }
      else {
        write_parms.data.data_len	= 4096; /* pretty much anything should be able to cope with 4kB */
      }
    }
    write_parms.data.data_val	= send_cmd + (len - bytes_left);
    
    if(device_write_1(&write_parms, &write_resp, client) != RPC_SUCCESS) {
      delete[] send_cmd;
      return -VXI11_NULL_WRITE_RESP; /* The instrument did not acknowledge the write, just completely
                                        dropped it. There was no vxi11 comms error as such, the 
                                        instrument is just being rude. Usually occurs when the instrument
                                        is busy. If we don't check this first, then the following 
                                        line causes a seg fault */
    }
    if (write_resp . error != 0) {
      printf("vxi11_user: write error: %d\n",(int)(write_resp . error));
      delete[] send_cmd;
      return -(write_resp . error);
    }
    bytes_left -= write_resp . size;
  } while (bytes_left > 0);
  
  delete[] send_cmd;
  return 0;
}

enum clnt_stat device_write_1(Device_WriteParms *argp, Device_WriteResp *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, device_write,
                    (xdrproc_t) xdr_Device_WriteParms, (caddr_t) argp,
                    (xdrproc_t) xdr_Device_WriteResp, (caddr_t) clnt_res,
                    TIMEOUT));
}

bool_t xdr_Device_WriteParms (XDR *xdrs, Device_WriteParms *objp)
{
  if (!xdr_Device_Link (xdrs, &objp->lid))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->io_timeout))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->lock_timeout))
    return FALSE;
  if (!xdr_Device_Flags (xdrs, &objp->flags))
    return FALSE;
  if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0))
    return FALSE;
  return TRUE;
}

bool_t xdr_Device_WriteResp (XDR *xdrs, Device_WriteResp *objp)
{
  if (!xdr_Device_ErrorCode (xdrs, &objp->error))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->size))
    return FALSE;
  return TRUE;
}

bool_t xdr_Device_Flags (XDR *xdrs, Device_Flags *objp)
{
  if (!xdr_long (xdrs, objp))
    return FALSE;
  return TRUE;
}

int agilent_set_averages(CLINK *clink, int no_averages) {
  char	cmd[256];
  
  if (no_averages <= 0) {
    return vxi11_send(clink, ":ACQ:AVER 0");
  }
  else {
    sprintf(cmd, ":ACQ:COUNT %d", no_averages);
    vxi11_send(clink, cmd);
    return vxi11_send(clink, ":ACQ:AVER 1");
  }
}

void agilent_set_for_auto(CLINK *clink) {
  vxi11_send(clink, ":ACQ:SRAT:AUTO 1;:ACQ:POINTS:AUTO 1;:RUN");
}


int vxi11_close_device(const char *ip, CLINK *clink) {
  int     l,ret;
  int     device_no = -1;

  /* Which instrument are we referring to? */
  for (l=0; l<VXI11_MAX_CLIENTS; l++){
    if (strcmp(ip,VXI11_IP_ADDRESS[l]) == 0 ) {
      device_no=l;
    }
  }
  /* Something's up if we can't find the IP address! */
  if (device_no == -1) {
    printf("vxi11_close_device: error: I have no record of you ever opening device\n");
    printf("                    with IP address %s\n",ip);
    ret = -4;
  }
  else {	/* Found the IP, there's more than one link to that instrument,
		 * so keep track and just close the link */
    if (VXI11_LINK_COUNT[device_no] > 1 ) {
      ret = vxi11_close_link(ip,clink->client, clink->link);
      VXI11_LINK_COUNT[device_no]--;
    }
    /* Found the IP, it's the last link, so close the device (link
     * AND client) */
    else {
      ret = vxi11_close_device(ip, clink->client, clink->link);
    }
  }
  return ret;
}

int vxi11_close_device(const char *ip, CLIENT *client, VXI11_LINK *link) {
  int	ret;
  
  ret = vxi11_close_link(ip, client, link);
  
  clnt_destroy(client);
  
  return ret;
}

int vxi11_close_link(const char *ip, CLIENT *client, VXI11_LINK *link) {
  Device_Error dev_error;
  memset(&dev_error, 0, sizeof(dev_error)); 
  
  if (destroy_link_1(&link->lid, &dev_error, client) != RPC_SUCCESS) {
#ifdef darwin
    clnt_perror(client,(char*)ip);
#else
    clnt_perror(client,ip);
#endif
    return -1;
  }
  
  return 0;
}

enum clnt_stat destroy_link_1(Device_Link *argp, Device_Error *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, destroy_link,
                    (xdrproc_t) xdr_Device_Link, (caddr_t) argp,
                    (xdrproc_t) xdr_Device_Error, (caddr_t) clnt_res,
                    TIMEOUT));
}

bool_t xdr_Device_Error (XDR *xdrs, Device_Error *objp)
{
  if (!xdr_Device_ErrorCode (xdrs, &objp->error))
    return FALSE;
  return TRUE;
}

double vxi11_obtain_double_value(CLINK *clink, const char *cmd) {
  return vxi11_obtain_double_value(clink, cmd, VXI11_READ_TIMEOUT);
}

double vxi11_obtain_double_value(CLINK *clink, const char *cmd, unsigned long timeout) {
  char	buf[50]; /* 50=arbitrary length... more than enough for one number in ascii */
  double	val;
  memset(buf, 0, 50);
  if (vxi11_send_and_receive(clink, cmd, buf, 50, timeout) != 0) {
    printf("Returning 0.0\n");
    return 0.0;
  }
  val = strtod(buf, (char **)NULL);
  return val;
}

long vxi11_send_and_receive(CLINK *clink, const char *cmd, char *buf, unsigned long buf_len, unsigned long timeout) {
  int	ret;
  long	bytes_returned;
  do {
    ret = vxi11_send(clink, cmd);
    if (ret != 0) {
      if (ret != -VXI11_NULL_WRITE_RESP) {
        printf("Error: vxi11_send_and_receive: could not send cmd.\n");
        printf("       The function vxi11_send returned %d. ",ret);
        return -1;
      }
      else printf("(Info: VXI11_NULL_WRITE_RESP in vxi11_send_and_receive, resending query)\n");
    }

    bytes_returned = vxi11_receive(clink, buf, buf_len, timeout);
    if (bytes_returned <= 0) {
      if (bytes_returned >-VXI11_NULL_READ_RESP) {
        printf("Error: vxi11_send_and_receive: problem reading reply.\n");
        printf("       The function vxi11_receive returned %ld. ",bytes_returned);
        return -2;
      }
      else printf("(Info: VXI11_NULL_READ_RESP in vxi11_send_and_receive, resending query)\n");
    }
  } while (bytes_returned == -VXI11_NULL_READ_RESP || ret == -VXI11_NULL_WRITE_RESP);
  return 0;
}

long vxi11_receive(CLIENT *client, VXI11_LINK *link, char *buffer, unsigned long len, unsigned long timeout) {
  Device_ReadParms read_parms;
  Device_ReadResp  read_resp;
  long	curr_pos = 0;

  read_parms.lid			= link->lid;
  read_parms.requestSize		= len;
  read_parms.io_timeout		= timeout;	/* in ms */
  read_parms.lock_timeout		= timeout;	/* in ms */
  read_parms.flags		= 0;
  read_parms.termChar		= 0;

  do {
    memset(&read_resp, 0, sizeof(read_resp));

    read_resp.data.data_val = buffer + curr_pos;
    read_parms.requestSize = len    - curr_pos;	// Never request more total data than originally specified in len

    if(device_read_1(&read_parms, &read_resp, client) != RPC_SUCCESS) {
      return -VXI11_NULL_READ_RESP; /* there is nothing to read. Usually occurs after sending a query
                                       which times out on the instrument. If we don't check this first,
                                       then the following line causes a seg fault */
    }
    if (read_resp . error != 0) {
      /* Read failed for reason specified in error code.
       *  0     no error
       *  4     invalid link identifier
       *  11    device locked by another link
       *  15    I/O timeout
       *  17    I/O error
       *  23    abort
       */

      printf("vxi11_user: read error: %d\n",(int)(read_resp . error));
      return -(read_resp . error);
    }

    if((curr_pos + read_resp . data.data_len) <= len) {
      curr_pos += read_resp . data.data_len;
    }
    if( (read_resp.reason & RCV_END_BIT) || (read_resp.reason & RCV_CHR_BIT) ) {
      break;
    }
    else if( (unsigned long)curr_pos == len ) {
      printf("xvi11_user: read error: buffer too small. Read %ld bytes without hitting terminator.\n", curr_pos );
      return -100;
    }
  } while(1);
  return (curr_pos); /*actual number of bytes received*/

}

long vxi11_receive(CLINK *clink, char *buffer, unsigned long len) {
  return vxi11_receive(clink, buffer, len, VXI11_READ_TIMEOUT);
}

long vxi11_receive(CLINK *clink, char *buffer, unsigned long len, unsigned long timeout) {
  return vxi11_receive(clink->client, clink->link, buffer, len, timeout);
}

enum clnt_stat device_read_1(Device_ReadParms *argp, Device_ReadResp *clnt_res, CLIENT *clnt)
{
  return (clnt_call(clnt, device_read,
                    (xdrproc_t) xdr_Device_ReadParms, (caddr_t) argp,
                    (xdrproc_t) xdr_Device_ReadResp, (caddr_t) clnt_res,
                    TIMEOUT));
}

bool_t xdr_Device_ReadParms (XDR *xdrs, Device_ReadParms *objp)
{
#if defined(SOLARIS) && !defined(_LP64)
  register long *buf;
#else
  register int32_t *buf;
#endif

  if (xdrs->x_op == XDR_ENCODE) {
    if (!xdr_Device_Link (xdrs, &objp->lid))
      return FALSE;
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_u_long (xdrs, &objp->requestSize))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->io_timeout))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->lock_timeout))
        return FALSE;

    } else {
      IXDR_PUT_U_INT32(buf, objp->requestSize);
      IXDR_PUT_U_INT32(buf, objp->io_timeout);
      IXDR_PUT_U_INT32(buf, objp->lock_timeout);
    }
    if (!xdr_Device_Flags (xdrs, &objp->flags))
      return FALSE;
    if (!xdr_char (xdrs, &objp->termChar))
      return FALSE;
    return TRUE;
  } else if (xdrs->x_op == XDR_DECODE) {
    if (!xdr_Device_Link (xdrs, &objp->lid))
      return FALSE;
    buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
    if (buf == NULL) {
      if (!xdr_u_long (xdrs, &objp->requestSize))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->io_timeout))
        return FALSE;
      if (!xdr_u_long (xdrs, &objp->lock_timeout))
        return FALSE;

    } else {
      objp->requestSize = IXDR_GET_U_INT32(buf);
      objp->io_timeout = IXDR_GET_U_INT32(buf);
      objp->lock_timeout = IXDR_GET_U_INT32(buf);
    }
    if (!xdr_Device_Flags (xdrs, &objp->flags))
      return FALSE;
    if (!xdr_char (xdrs, &objp->termChar))
      return FALSE;
    return TRUE;
  }

  if (!xdr_Device_Link (xdrs, &objp->lid))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->requestSize))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->io_timeout))
    return FALSE;
  if (!xdr_u_long (xdrs, &objp->lock_timeout))
    return FALSE;
  if (!xdr_Device_Flags (xdrs, &objp->flags))
    return FALSE;
  if (!xdr_char (xdrs, &objp->termChar))
    return FALSE;
  return TRUE;
}

bool_t xdr_Device_ReadResp (XDR *xdrs, Device_ReadResp *objp)
{
  if (!xdr_Device_ErrorCode (xdrs, &objp->error))
    return FALSE;
  if (!xdr_long (xdrs, &objp->reason))
    return FALSE;
  if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0))
    return FALSE;
  return TRUE;
}

long vxi11_obtain_long_value(CLINK *clink, const char *cmd, unsigned long timeout) {
  char	buf[50]; /* 50=arbitrary length... more than enough for one number in ascii */
  memset(buf, 0, 50);
  if (vxi11_send_and_receive(clink, cmd, buf, 50, timeout) != 0) {
    printf("Returning 0\n");
    return 0;
  }
  return strtol(buf, (char **)NULL, 10);
}

/* Lazy wrapper function with default read timeout */
long vxi11_obtain_long_value(CLINK *clink, const char *cmd) {
  return vxi11_obtain_long_value(clink, cmd, VXI11_READ_TIMEOUT);
}

long agilent_get_data(CLINK *clink, char chan, char *buf, unsigned long buf_len, unsigned long timeout) {
  return agilent_get_data(clink, chan, 1, buf, buf_len, timeout);
}

long agilent_get_data(CLINK *clink, char chan, int digitise, char *buf, unsigned long buf_len, unsigned long timeout) {
  char	source[20];
  char	cmd[256];
  int	ret;
  long	bytes_returned;

  memset(source,0,20);
  agilent_scope_channel_str(chan, source);
  sprintf(cmd,":WAV:SOURCE %s",source);
  ret=vxi11_send(clink, cmd);
  if(ret < 0) {
    printf("error, could not send :WAV:SOURCE %s cmd, quitting...\n",source);
    return ret;
  }

  if (digitise != 0) {
    ret=vxi11_send(clink, ":DIG");
  }
  memset(cmd,0,256);
  do {
    ret=vxi11_send(clink, ":WAV:DATA?");
    bytes_returned=vxi11_receive_data_block(clink, buf, buf_len, timeout);
  } while (bytes_returned == -VXI11_NULL_READ_RESP);
  /* We have to check for this after a :DIG because it could take a very long time */
  return bytes_returned;
}

int agilent_get_preamble(CLINK *clink, char* buf, unsigned long buf_len) {
  int	ret;
  long	bytes_returned;

  ret=vxi11_send(clink, ":WAV:PRE?");
  if(ret < 0) {
    printf("error, could not send :WAV:PRE? cmd, quitting...\n");
    return ret;
  }

  bytes_returned=vxi11_receive(clink, buf, buf_len);

  return (int) bytes_returned;
}

void agilent_scope_channel_str(char chan, char *source) {
  switch (chan) {
  case 'A' :
  case 'a' : strcpy(source,"FUNC1");
    break;
  case 'B' :
  case 'b' : strcpy(source,"FUNC2");
    break;
  case 'C' :
  case 'c' : strcpy(source,"FUNC3");
    break;
  case 'D' :
  case 'd' : strcpy(source,"FUNC4");
    break;
  case '1' : strcpy(source,"CHAN1");
    break;
  case '2' : strcpy(source,"CHAN2");
    break;
  case '3' : strcpy(source,"CHAN3");
    break;
  case '4' : strcpy(source,"CHAN4");
    break;
  default :  printf("error: unknown channel '%c', using channel 1\n",chan);
    strcpy(source,"CHAN1");
    break;
  }
}

long vxi11_receive_data_block(CLINK *clink, char *buffer, unsigned long len, unsigned long timeout) {
  /* I'm not sure what the maximum length of this header is, I'll assume it's 
   * 11 (#9 + 9 digits) */
  unsigned long	necessary_buffer_size;
  char		*in_buffer;
  int		ret;
  int		ndigits;
  unsigned long	returned_bytes;
  int		l;
  char		scan_cmd[20];
  necessary_buffer_size=len+12;
  in_buffer=new char[necessary_buffer_size];
  ret=vxi11_receive(clink, in_buffer, necessary_buffer_size, timeout);
  if (ret < 0) return ret;
  if (in_buffer[0] != '#') {
    printf("vxi11_user: data block error: data block does not begin with '#'\n");
    printf("First 20 characters received were: '");
    for(l=0;l<20;l++) {
      printf("%c",in_buffer[l]);
    }
    printf("'\n");
    return -3;
  }

  /* first find out how many digits */
  sscanf(in_buffer,"#%1d",&ndigits);
  /* some instruments, if there is a problem acquiring the data, return only "#0" */
  if (ndigits > 0) {
    /* now that we know, we can convert the next <ndigits> bytes into an unsigned long */
    sprintf(scan_cmd,"#%%1d%%%dlu",ndigits);
    sscanf(in_buffer,scan_cmd,&ndigits,&returned_bytes);
    memcpy(buffer, in_buffer+(ndigits+2), returned_bytes);
    delete[] in_buffer;
    return (long) returned_bytes;
  }
  else return 0;
}

int agilent_set_for_capture(CLINK *clink, double s_rate, long npoints, unsigned long timeout) {
  long	 actual_npoints;		/* actual number of points returned */
  double time_range;
  double auto_srat;		/* sample rate whilst on auto setting */
  long	 auto_npoints;		/* no of points whilst on auto setting */
  double expected_s_rate;	/* based on s_rate passed to us, or npoints */
  double actual_s_rate;		/* what it ends up as */
  double xinc;			/* xincrement (only need for ETIM mode) */
  char	 cmd[256];
  char	 etim_result[256];
  int	 ret_val	= 0;
  int	 not_enough_memory = 0;

  /* First we need to find out if we're in "ETIM" (equivalent time) mode,
   * because things are done differently. You can't set the sample rate,
   * and if you query it, you get a meaningless answer. You must work out
   * what the effective sample rate is from the waveform xincrement. A
   * pain in the arse, quite frankly. */

  vxi11_send_and_receive(clink, ":ACQ:MODE?", etim_result, 256, VXI11_READ_TIMEOUT);

  /* Equivalent time (ETIM) mode: */
  if (strncmp("ETIM",etim_result,4) == 0) {
    /* Find out the time range displayed on the screen */
    time_range = vxi11_obtain_double_value(clink, ":TIM:RANGE?");

    /* Find the xincrement, whilst we're still in auto (points) mode */
    auto_npoints = vxi11_obtain_long_value(clink, ":ACQ:POINTS?");

    /* Set the no of acquisition points to manual */
    vxi11_send(clink, ":ACQ:POINTS:AUTO 0");

    if (npoints <= 0) { // if we've not been passed a value for npoints
      npoints = auto_npoints;
    }
    /* Remember we want at LEAST the number of points specified.
     * To some extent, the xinc value is determined by the
     * number of points. So to get the best xinc value we ask
     * for double what we actually want. */
    sprintf(cmd, ":ACQ:POINTS %ld",(2*npoints)-1);
    vxi11_send(clink, cmd);

    /* Unfortunately we have to do a :dig, to make sure our changes have
     * been registered */
    vxi11_send(clink, ":DIG");

    /* Find the xincrement is now*/
    xinc = vxi11_obtain_double_value(clink, ":WAV:XINC?", timeout);

    /* Work out the number of points there _should_ be to cover the time range */
    actual_npoints = (long) ( (time_range / xinc) + 0.5);

    /* Set the number of points accordingly. Hopefully the
     * xincrement won't have changed! */
    sprintf(cmd, ":ACQ:POINTS %ld",actual_npoints);
    vxi11_send(clink, cmd);

    /* This is a bit anal... we can work out very easily what the equivalent
     * sampling rate is (1 / xinc); the scope seems to store this value
     * somewhere, even though it doesn't use it. We may as well write it
     * to the scope, in case some user program asks for it while in
     * equivalent time mode. Should not be depended upon, though! */

    sprintf(cmd, ":ACQ:SRAT %G", (1/xinc));
    vxi11_send(clink, cmd);
  }

  /* Real time (RTIM, NORM or PDET) mode: */
  else {
    /* First find out what the sample rate is set to.
     * Each time you switch from auto to manual for either of these, the
     * scope remembers the values from last time you set these manually.
     * This is not very useful to us. We want to be able to set either the
     * sample rate (and derive npoints from that and the timebase), or the
     * minimum number of points (and derive the sample rate) or let
     * the scope choose sensible values for both of these. We only want to
     * capture the data for the time period displayed on the scope screen,
     * which is equal to the time range. If you leave the scope to do
     * everything auto, then it always acquires a bit more than what's on
     * the screen.
     */
    auto_srat = vxi11_obtain_double_value(clink, ":ACQ:SRAT?");

    /* Set the sample rate (SRAT) and no of acquisition points to manual */
    vxi11_send(clink, ":ACQ:SRAT:AUTO 0;:ACQ:POINTS:AUTO 0");

    /* Find out the time range displayed on the screen */
    time_range = vxi11_obtain_double_value(clink, ":TIM:RANGE?");

    /* If we've not been passed a sample rate (ie s_rate <= 0) then... */
    if (s_rate <= 0 ) {
      /* ... if we've not been passed npoints, let scope set rate */
      if (npoints <= 0) {
        s_rate = auto_srat;
      }
      /* ... otherwise set the sample rate based on no of points. */
      else {
        s_rate = (double) npoints / time_range;
      }
    }
    /* We make a note here of what we're expecting the sample rate to be.
     * If it has to change for any reason (dodgy value, or not enough
     * memory) we will know about it.
     */
    expected_s_rate = s_rate;

    /* Now we set the number of points to acquire. Of course, the scope
     * may not have enough memory to acquire all the points, so we just
     * sit in a loop, reducing the sample rate each time, until it's happy.
     */
    do {
      /* Send scope our desired sample rate. */
      sprintf(cmd, ":ACQ:SRAT %G",s_rate);
      vxi11_send(clink, cmd);
      /* Scope will choose next highest allowed rate.
       * Find out what this is */
      actual_s_rate = vxi11_obtain_double_value(clink, ":ACQ:SRAT?");

      /* Calculate the number of points on display (and round up for rounding errors) */
      npoints = (long) ((time_range * actual_s_rate)+0.5);

      /* Set the number of points accordingly */
      /* Note this won't necessarily be the no of points you receive, eg if you have
       * sin(x)/x interpolation turned on, you will probably get more. */
      sprintf(cmd, ":ACQ:POINTS %ld",npoints);
      vxi11_send(clink, cmd);

      /* We should do a check, see if there's enough memory */
      actual_npoints = vxi11_obtain_long_value(clink, ":ACQ:POINTS?");

      if (actual_npoints < npoints) {
        not_enough_memory = 1;
        ret_val = -1;		/* We should report this fact to the calling function */
        s_rate = s_rate * 0.75 * (double) ( (double) actual_npoints / (double) npoints );
      }
      else {
        not_enough_memory = 0;
      }
    } while (not_enough_memory == 1);
    /* Will possibly remove the explicit printf's here, maybe leave it up to the
     * calling function to spot potential problems (the user may not care!) */
    if (actual_s_rate != expected_s_rate) {
      //printf("Warning: the sampling rate has been adjusted,\n");
      //printf("from %g to %g, because ", expected_s_rate, actual_s_rate);
      if (ret_val == -1) {
        //printf("there was not enough memory.\n");
      }
      else {
        //printf("because %g Sa/s is not a valid sample rate.\n",expected_s_rate);
        ret_val = -2;
      }
    }
  }
  return ret_val;
}

long agilent_calculate_no_of_bytes(CLINK *clink, char chan, unsigned long timeout) {
  char	cmd[256];
  char	source[20];
  double	hinterval,time_range;
  double	srat;
  long	no_of_bytes;
  char	etim_result[256];

  // First we need to digitize, to get the correct values for the
  // waveform data. This is a pain in the arse.
  agilent_scope_channel_str(chan, source);
  sprintf(cmd,":WAV:SOURCE %s",source);
  vxi11_send(clink, cmd);
  vxi11_send(clink, ":DIG");

  /* Now find the info we need to calculate the number of points */
  hinterval = vxi11_obtain_double_value(clink, ":WAV:XINC?", timeout);
  time_range = vxi11_obtain_double_value(clink, ":TIM:RANGE?");

  /* Are we in equivalent time (ETIM) mode? If so, the value of ACQ:SRAT will
   * be meaningless, and there's a different formula */
  vxi11_send_and_receive(clink, ":ACQ:MODE?", etim_result, 256, VXI11_READ_TIMEOUT);
  /* Equivalent time (ETIM) mode: */
  if (strncmp("ETIM",etim_result,4) == 0) {
    no_of_bytes = (long) ( 2*((time_range / hinterval) + 0.5) );
  }
  else {
    srat = vxi11_obtain_double_value(clink, ":ACQ:SRAT?");

    no_of_bytes = (long) (2*(((time_range-(1/srat)) / hinterval)+1) + 0.5);
    /* 2x because 2 bytes per data point
     * +0.5 to round up when casting as a long
     * -(1/srat) and +1 so that both raw data, and interpolated (sinx/x) data works */
  }
  return no_of_bytes;
}










