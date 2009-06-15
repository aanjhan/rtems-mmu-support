#include <stdio.h>
#include <string.h>

#include "libmmu.h"

rtems_libmmu_alut rtems_libmmu_alut_entry[RTEMS_LIBMMU_ALUT_SIZE];


static void
rtems_libmmu_alut_init(void)
{
  static const rtems_libmmu_alut[] = 
  {
    0x00000000, 4096, 0;
    NULL;
    NULL;
    NULL;
  };
}
    

int
rtems_libmmu_get_access_attribute(uint32_t addr)
{
  return 0;
}

int
rtems_libmmu_list_alut_access(void)
{
  return 0;
}
