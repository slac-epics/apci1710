/* apci1710ctr_ioctl.h */

#ifndef __INC_apci1710ctr_ioctl
#define __INC_apci1710ctr_ioctl

#include <linux/ioctl.h>

#define APCI1710CTR_IOC_MAGIC 'C'

#define APCI1710CTR_IOCRESET        _IO(APCI1710CTR_IOC_MAGIC, 0)
#define APCI1710CTR_IOCINTENABLE    _IO(APCI1710CTR_IOC_MAGIC, 1)
#define APCI1710CTR_IOCINTDISABLE   _IO(APCI1710CTR_IOC_MAGIC, 2)

#endif
