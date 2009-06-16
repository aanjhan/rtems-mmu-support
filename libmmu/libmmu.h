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


#define RTEMS_LIBMMU_ALUT_SIZE 5


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

typedef struct
{
  rtems_libmmu_alut_entry* entries;
  size_t size;
  uint32_t nElements;
} rtems_libmmu_alut;


void
rtems_libmmu_alut_init(void);

rtems_libmmu_alut* 
rtems_libmmu_alut_create(size_t size);

int
rtems_libmmu_alut_add_entry(rtems_libmmu_alut* pAlut, rtems_libmmu_alut_entry* pEntry);

extern int
rtems_libmmu_get_access_attribute(rtems_libmmu_alut* pAlut, uint32_t addr);

extern int
rtems_libmmu_list_alut_access(void);


#endif
