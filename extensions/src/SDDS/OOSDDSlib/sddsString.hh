
#if !defined(_SDDS_string_)
#define _SDDS_string_ 1
#define SDDS_MAX_STRING_SIZE 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gddNewDel.h"

class sddsString {
public:
  int set(char *string);
  char *get(void) {return str;}
  gdd_NEWDEL_FUNC(sddsString);

private:
  char str[SDDS_MAX_STRING_SIZE];
  gdd_NEWDEL_DATA(sddsString)

};

inline int
sddsString::set(char* string) {
  if (string == NULL) {
    str[0] = '\0';
  } else if (strlen(string) >= SDDS_MAX_STRING_SIZE) {
    strncpy(str,string,SDDS_MAX_STRING_SIZE);
    str[SDDS_MAX_STRING_SIZE-1] = '\0';
    return(-1);
  } else {
    strcpy(str,string);
  }
  return(0);
}


#endif


