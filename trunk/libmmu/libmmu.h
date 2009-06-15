#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H

#include <inttypes.h>


#define RTEMS_LIBMMU_ALUT_SIZE 1


/* Access definition macros below */
#define RTEMS_LIBMMU_ACCESS_PROT_NONE      0
#define RTEMS_LIBMMU_ACCESS_PROT_READ      1
#define RTEMS_LIBMMU_ACCESS_PROT_WRITE     2
#define RTEMS_LIBMMU_ACCESS_PROT_EXECUTE   3


typedef struct
{
  uint32_t start_addr;
  uint32_t block_size;
  int access_attrib;
} rtems_libmmu_alut_entry;


const rtems_libmmu_alut_entry rtems_libmmu_alut[RTEMS_LIBMMU_ALUT_SIZE] = 
{
  /* Memory Layout access table initialisation goes here */
  {0x00000000, 4096, 1}
};




void
rtems_libmmu_alut_init(void);

extern int
rtems_libmmu_get_access_attribute(uint32_t addr);

extern int
rtems_libmmu_list_alut_access(void);


#endif
