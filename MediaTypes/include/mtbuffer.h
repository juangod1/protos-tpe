#ifndef MTBUFFER_H
#define MTBUFFER_H


#include "../../Shared/include/buffer.h"

int mt_buffer_starts_with_boundary(char *boundary, buffer_p buffer);

void mt_buffer_discard(buffer_p buffer);

char *mt_buffer_get_mime(buffer_p buffer);

char *mt_buffer_get_boundary(buffer_p buffer);

#endif
