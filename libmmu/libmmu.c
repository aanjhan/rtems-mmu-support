#include <stdio.h>
#include <string.h>

#include "libmmu.h"

int
rtems_libmmu_get_access_attribute(uint32_t addr)
{
  return 1;
}

int
rtems_libmmu_list_alut_access(void)
{
  return 1;
}
