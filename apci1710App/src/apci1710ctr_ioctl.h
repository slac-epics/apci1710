#ifndef __INC_apci1710ctr_ioctl
#define __INC_apci1710ctr_ioctl

/**
 * ----------------------------------------------------------------------------
 * File       : apci1710ctr_ioctl.h
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

#include <linux/ioctl.h>

#define APCI1710CTR_IOC_MAGIC 'C'

#define APCI1710CTR_IOCRESET        _IO(APCI1710CTR_IOC_MAGIC, 0)
#define APCI1710CTR_IOCINTENABLE    _IO(APCI1710CTR_IOC_MAGIC, 1)
#define APCI1710CTR_IOCINTDISABLE   _IO(APCI1710CTR_IOC_MAGIC, 2)

#endif
