
#include "SDDS.h"
#include "sddsError.hh"

// sddsErrorMessage implementation
gdd_NEWDEL_STAT(sddsErrorMessage);
gdd_NEWDEL_DEL(sddsErrorMessage);
gdd_NEWDEL_NEW(sddsErrorMessage);

void
sddsErrorMessage::set(char* error_text) {
  if (error_text == NULL) {
    message[0] = '\0';
  } else if (strlen(error_text) >= MAX_ERROR_SIZE) {
    strncpy(message,error_text,MAX_ERROR_SIZE);
    message[MAX_ERROR_SIZE-1] = '\0';
  } else {
    strcpy(message,error_text);
  }
}


// sddsError implementation
sddsError::sddsError(void) {
  printf("sddsError: constructing\n");
  registeredProgramName[0] = '\0';
  n_errors = 0;
  currentError = NULL;

}

sddsError::~sddsError(void) {
  printf("sddsError: destructing\n");
  SDDS_ClearErrors();
}

void
sddsError::SDDS_ClearErrors(void) {
  sddsErrorMessage *pmsg1, *pmsg2;
  n_errors = 0;
  pmsg1 = currentError;
  while (pmsg1 != NULL) {
    pmsg2 = pmsg1->getPrev();
    delete pmsg1;
    pmsg1 = pmsg2;
  }
  currentError = NULL;
}

void
sddsError::SDDS_SetError(char* error_text) {
  // Allocate new error message and point it to any previous error message
  sddsErrorMessage* pmsg = new sddsErrorMessage;
  pmsg->setPrev(currentError);

  currentError = pmsg;
  pmsg->set(error_text);
  n_errors++;
}

void 
sddsError::SDDS_PrintErrors(FILE* fp, long mode) {
  long depth;
  sddsErrorMessage *pmsg;
  if (!n_errors)
    return;
  if (!fp) {
    this->SDDS_ClearErrors();
    return;
  }

  if (mode&SDDS_VERBOSE_PrintErrors)
    depth = n_errors;
  else
    depth = 1;

  if (strlen(registeredProgramName))
    fprintf(fp,"Error for %s:\n", registeredProgramName);
  else 
    fprintf(fp,"Error:\n");

  pmsg = currentError;
  if (depth==1) {
    fprintf(fp,"%s\n",pmsg->get());
  } else {
    while (pmsg != NULL) {
      fprintf(fp,"%s\n",pmsg->get());
      pmsg = pmsg->getPrev();
    }
  }
  fflush(fp);
  this->SDDS_ClearErrors();
  if (mode&SDDS_EXIT_PrintErrors) 
    exit(1);

}
