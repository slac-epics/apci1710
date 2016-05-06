#ifndef __INC_apci1710ctr_buf
#define __INC_apci1710ctr_buf

/**
 * ----------------------------------------------------------------------------
 * File       : apci1710ctr_buf.h
 * Author     : Chris Ford, caf@slac.stanford.edu
 * Created    : 2016-05-06
 * Last update: 2016-05-06
 * ----------------------------------------------------------------------------
 * This file is part of apci1710-asyn. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 * https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of apci1710-asyn, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#define APCI1710CTR_DEFAULT_RINGSIZE    16

typedef struct counterBuf {
    int             counter;
    unsigned int    timestamp;
    unsigned short  frameCount;
    unsigned short  flags;
} counterBuf_t;

#endif
