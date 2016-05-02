/* apci1710ctr_buf.h */

#ifndef __INC_apci1710ctr_buf
#define __INC_apci1710ctr_buf

#define APCI1710CTR_DEFAULT_RINGSIZE    16

typedef struct counterBuf {
    int             counter;
    unsigned int    timestamp;
    unsigned short  frameCount;
    unsigned short  flags;
} counterBuf_t;

#endif
