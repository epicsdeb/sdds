
#if !defined(_SDDS_error_)
#define _SDDS_error_ 1
#define MAX_ERROR_SIZE 256
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gddNewDel.h"

class sddsErrorMessage {
public:
  void set(char *error_text);
  char *get(void) {return message;}
  void setPrev(sddsErrorMessage* p) {prev = p;}
  sddsErrorMessage* getPrev(void) {return prev;}
  gdd_NEWDEL_FUNC(sddsErrorMessage);

private:
  char message[MAX_ERROR_SIZE];
  sddsErrorMessage *prev;
  gdd_NEWDEL_DATA(sddsErrorMessage)

};


class sddsError {

public:
  sddsError(void);
  ~sddsError(void);
  long SDDS_NumberOfErrors(void);
  void SDDS_ClearErrors(void);
  void SDDS_SetError(char* error_text);
  void SDDS_PrintErrors(FILE* fp, long mode);
  void SDDS_RegisterProgramName(char *name);
  void SDDS_Bomb(char *message);
  void SDDS_Warning(char *message);

private:
  char registeredProgramName[MAX_ERROR_SIZE];
  sddsErrorMessage *currentError;
  long n_errors;

};

inline long
sddsError::SDDS_NumberOfErrors(void) {
  return(n_errors);
}

inline void
sddsError::SDDS_RegisterProgramName(char *name) {
  if (name == NULL) {
    registeredProgramName[0] = '\0';
  } else if (strlen(name) >= MAX_ERROR_SIZE) {
    strncpy(registeredProgramName,name,MAX_ERROR_SIZE);
    registeredProgramName[MAX_ERROR_SIZE-1] = '\0';
  } else {
    strcpy(registeredProgramName,name);
  }
}

inline void
sddsError::SDDS_Bomb(char *message) {
  if (strlen(registeredProgramName)) {
    fprintf(stderr,"Error (%s): %s\n",
	    registeredProgramName, message?message:"?");
  } else {
    fprintf(stderr,"Error: %s\n",message?message:"?");
  }
  exit(1);
}

inline void
sddsError::SDDS_Warning(char *message) {
  if (strlen(registeredProgramName)) {
    fprintf(stderr,"Warning (%s): %s\n",
	    registeredProgramName, message?message:"?");
  } else {
    fprintf(stderr,"Warning: %s\n",message?message:"?");
  }
}

#endif


