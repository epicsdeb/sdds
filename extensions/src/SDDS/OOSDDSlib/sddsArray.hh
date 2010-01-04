
#if !defined(_SDDS_array_)

#define _SDDS_array_ 1

#include "SDDStypes.h"

class sddsArrayDefinition {
public:
  char *name, *symbol, *units, *description, *format_string, *group_name;
  long type, field_length, dimensions;

};

class sddsArray {
public:
    sddsArrayDefinition *definition;
    long *dimension, elements;
    /* Address of array of data values, stored contiguously.
     * For STRING data, the "data values" are actually the addresses of 
     * the individual strings.
     */
    void *data;
    void *pointer;
};

#endif
