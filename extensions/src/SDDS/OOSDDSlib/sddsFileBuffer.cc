
#include <stdio.h>
#include "sddsFileBuffer.hh"

sddsFileBuffer::sddsFileBuffer(void) {
  data = NULL;
  buffer = NULL;
  bytesLeft = 0;
  bufferSize = 0;
}
