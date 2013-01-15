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
 Revision 1.2  2009/10/26 18:40:26  soliday
 Fixed issue on Mac computers.

 Revision 1.1  2009/08/18 19:12:42  soliday
 Agilent scope communication software based off of software from Steve Sharples
 of the Univ. of Nottingham

 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include <pthread.h>

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

#define BUF_LEN 1000000

int main(int argc,char **argv)
{
  static char   *progname;
  static char   *serverIP;

  char          comm[256];

  long          bytes_returned;
  BOOL          got_ip=FALSE;
  BOOL          got_comm=FALSE;
  int           index=1;
  CLINK         *clink; /* client link (actually a structure contining CLIENT and VXI11_LINK pointers) */

  char          outputbuf[BUF_LEN];



  clink = new CLINK; /* allocate some memory */
  progname = argv[0];

  while(index<argc){
    if(sc(argv[index],(char*)"-ip")||sc(argv[index],(char*)"-ip_address")||sc(argv[index],(char*)"-IP")){
      serverIP = argv[++index];
      got_ip = TRUE;
    }

    if(sc(argv[index],(char*)"-command")||sc(argv[index],(char*)"-c")||sc(argv[index],(char*)"-comm")){
      snprintf(comm,256,"%s",argv[++index]);
      got_comm=TRUE;
    }

    index++;
  }

  if(got_ip==FALSE||got_comm==FALSE){
    printf("%s: sends commands to an Agilent scope via ethernet\n",progname);
    printf("Run using %s [arguments]\n\n",progname);
    printf("REQUIRED ARGUMENTS:\n");
    printf("-ip    -ip_address     -IP      : IP address of scope (eg 128.243.74.232)\n");
    printf("-c     -command        -comm    : command or query to send\n\n");
    printf("DOCUMENTATION:\n");
    printf("http://cp.literature.agilent.com/litweb/pdf/54855-97017.pdf\n");
    exit(1);
  }

  if (vxi11_open_device(serverIP,clink) != 0) {
    printf("Quitting...\n");
    exit(2);
  }
  
  vxi11_send(clink, comm);
  if (strstr(comm, "?") != 0) {
    bytes_returned = vxi11_receive(clink, outputbuf, BUF_LEN);
    vxi11_close_device(serverIP,clink);

    if (bytes_returned > 0) {
      printf("%s",outputbuf);
    } else if (bytes_returned == -15) {
      printf("Error: Nothing received after sending scope command %s\n", comm);
      return(1);
    }
  } else {
    vxi11_close_device(serverIP,clink);
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








