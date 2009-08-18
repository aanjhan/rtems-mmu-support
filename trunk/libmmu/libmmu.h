#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H
/*
*  COPYRIGHT (c) 1989-2009.
*  On-Line Applications Research Corporation (OAR).
*
*  The license and distribution terms for this file may be
*  found in the file LICENSE in this distribution or at
*  http://www.rtems.com/license/LICENSE.
*
*  $Id$
*/

#include <inttypes.h>


#define RTEMS_LIBMMU_ALUT_SIZE 16

/* Access definition macros below */
#define RTEMS_LIBMMU_ACCESS_WRITE_ONLY  1
#define RTEMS_LIBMMU_ACCESS_NO_PROT     2
#define RTEMS_LIBMMU_ACCESS_READ_ONLY   3


typedef struct
{
  char* start_addr;
  uint32_t block_size;
  int access_attrib;
} rtems_libmmu_alut_entry;

typedef struct
{
  rtems_libmmu_alut_entry entries[RTEMS_LIBMMU_ALUT_SIZE];
  uint32_t nElements;
} rtems_libmmu_alut;


void
rtems_libmmu_alut_init(void);

rtems_libmmu_alut* 
rtems_libmmu_alut_create(size_t size);

rtems_status_code
rtems_libmmu_alut_add_entry(rtems_libmmu_alut* pAlut, rtems_libmmu_alut_entry* pEntry);

extern int
rtems_libmmu_get_access_attribute(char* addr);

extern int
rtems_libmmu_list_alut_access(void);


#endif
