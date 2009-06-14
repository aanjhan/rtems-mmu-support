#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H

#include <inttypes.h>

extern int
rtems_libmmu_get_access_attribute(uint32_t addr);

extern int
rtems_libmmu_list_alut_access(void);


#endif
