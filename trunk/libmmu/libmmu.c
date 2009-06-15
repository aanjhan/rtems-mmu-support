#include <stdio.h>
#include <string.h>

#include "libmmu.h"




void
rtems_libmmu_alut_init(void)
{
  /* Initialisation code goes here */
}
    

int
rtems_libmmu_get_access_attribute(uint32_t addr)
{
  return rtems_libmmu_alut[0].access_attrib;
}

int
rtems_libmmu_list_alut_access(void)
{
  return 0;
}
