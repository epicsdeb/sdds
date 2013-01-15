
#include "sddsDefinition.hh"
#include "SDDS.h"
#include "mdb.h"

gdd_NEWDEL_STAT(sddsParameterDefinition);
gdd_NEWDEL_DEL(sddsParameterDefinition);
gdd_NEWDEL_NEW(sddsParameterDefinition);


int
sddsParameterDefinition::setType(char *typeString) {
  long t;
  if ((t=match_string(typeString, SDDS_type_name, 
		      SDDS_NUM_TYPES, EXACT_MATCH))<0) {
    return -1;
  } else {
    type = t + 1;
    return 0;
  }
}
