
#if !defined(_SDDS_layout_)

#define _SDDS_layout_ 1

#include <stdio.h>
#include <limits.h>
#include "gddNewDel.h"
#include "SDDS.h"
#include "SDDStypes.h"
#include "sddsArray.hh"
#include "sddsError.hh"
#include "sddsSortedIndex.hh"
#include "sddsString.hh"
#include "sddsDefinition.hh"

// from flex/bison generated parser
class sddsLayout;
int headerParse(FILE *fp, sddsLayout *layout);

class sddsLayout {

public:
  sddsLayout(sddsError &errorStack);
  long SDDS_DeferSavingLayout(long mode);
  long SDDS_ReadLayout(FILE *fp);
  void setVersion(long version);
  void setDescription(char *description);
  void setContents(char *contents);
  void commitParameterDefinition(sddsParameterDefinition *parameterDef);

  sddsError &err;  // make error stack public for parser

private:
  long deferSavingLayout;  // static taken from SDDS_copy.c
  long n_columns, n_parameters, n_associates, n_arrays;
  
  // for description group
  sddsString description;
  sddsString contents;

  long version;
  short layout_written;

  sddsDataMode data_mode;
  sddsColumnDefinition *column_definition;

  // Converting from array to hash table
  //  tsHash<sddsParameterDefinition> parameter_definition_table;

  sddsParameterDefinition *parameter_definition;

  sddsArrayDefinition *array_definition;
  sddsAssociateDefinition *associate_definition;
  sddsSortedIndex **column_index, **parameter_index, **array_index;
};

inline long
sddsLayout::SDDS_DeferSavingLayout(long mode) {
  deferSavingLayout = mode; 
  return mode;
}

inline void
sddsLayout::setVersion(long v) {
  version = v;
}

inline void
sddsLayout::setDescription(char *desc) {
  description.set(desc);
}

inline void
sddsLayout::setContents(char *cont) {
  contents.set(cont);
}

#endif
