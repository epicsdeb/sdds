
#if !defined(_SDDS_definition_)

#define _SDDS_definition_ 1

#include <stdio.h>
#include <limits.h>
#include "gddNewDel.h"
#include "sddsString.hh"

class sddsDataMode {

public:
  long mode, lines_per_row, no_row_counts;
  long additional_header_lines;
  
};

class sddsColumnDefinition {

public:
  char *name, *symbol, *units, *description, *format_string;
  long type, field_length;
  /* these are used internally and are not set by the user: */
  long definition_mode, memory_number, pointer_number;
  
};

class sddsParameterDefinition {
  
public:
  gdd_NEWDEL_FUNC(sddsParameterDefinition)
  void setName(char *n) {name.set(n);}
  char *getName() {return name.get();}
  void setSymbol(char *s) {symbol.set(s);}
  void setUnits(char *u) {units.set(u);}
  void setDescription(char *desc) {description.set(desc);}
  void setFormatString(char *format) {format_string.set(format);}
  int setType(char *typeString);
  void setFixedValue(char *fixed) {fixed_value.set(fixed);}

private:
  gdd_NEWDEL_DATA(sddsParameterDefinition)
  sddsString name, symbol, units, description, format_string;
  long type;
  sddsString fixed_value;
  /* these are used internally and are not set by the user: */
  long definition_mode, memory_number;
  
};

class sddsAssociateDefinition {

public:
  char *name, *filename, *path, *description, *contents;
  long sdds;

};


#endif
