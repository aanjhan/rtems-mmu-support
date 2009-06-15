#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H

#include <inttypes.h>


#define RTEMS_LIBMMU_ALUT_SIZE 32


/* Access definition macros below */
#define RTEMS_LIBMMU_ACCESS_PROT_NONE      0
#define RTEMS_LIBMMU_ACCESS_PROT_READ      1
#define RTEMS_LIBMMU_ACCESS_PROT_WRITE     2
#define RTEMS_LIBMMU_ACCESS_PROT_EXECUTE   3


typedef struct
{
  uint32_t start_addr;
  uint32_t block_size;
  unsigned char access_attrib;
} rtems_libmmu_alut_entry;



extern int
rtems_libmmu_get_access_attribute(uint32_t addr);

extern int
rtems_libmmu_list_alut_access(void);


#endif
