#if !defined(_SDDS_header_)
#define _SDDS_header_ 1
#include <stdio.h>
#include <stdlib.h>

#include "sddsString.hh"

#define SDDS_MAX_ENTITIES 20

class sddsNamelistText {
public:
  sddsNamelistText(sddsString &group);
  void addEntity(sddsString &e, sddsString &val);

private:
  long n_entities;
  sddsString group_name;
  sddsString entity[SDDS_MAX_ENTITIES];
  sddsString value[SDDS_MAX_ENTITIES];
};

inline void
sddsNamelistText::sddsNamelistText(sddsString &group) {
  n_entities = 0;
  group_name.set(group.get);
}

inline void
sddsNamelistText::addEntity(sddsString &e, sddsString &val) {
  entity[n_entities].set(e.get);
  value[n_entities].set(val.get);
  n_entities++;
}
