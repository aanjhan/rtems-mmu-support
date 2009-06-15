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

#include <stdio.h>
#include <string.h>
#include "libmmu.h"

/*****************************************************
* Linear search for the element emtry in the alut for*
* the address range under which it falls             *
*****************************************************/
static int
rtems_libmmu_alut_search(uint32_t addr)
{
  int index;
  for ( index = 0; index < RTEMS_LIBMMU_ALUT_SIZE; index++ ) {
    
    if ( rtems_libmmu_alut[index].start_addr <= addr &&
         rtems_libmmu_alut[index].start_addr + rtems_libmmu_alut[index].block_size > addr ) { 
      return index;
    } 
  } 
  return -1;
}


/*****************************************************
* Initialisation of libmmu things goes here          *
*****************************************************/
void
rtems_libmmu_alut_init(void)
{
  /* Initialisation code goes here */
}
    
/*****************************************************
* Returns the access attribute for a particular addr *
* location                                           *
*****************************************************/
int
rtems_libmmu_get_access_attribute(uint32_t addr)
{
  int element_index;
  element_index = rtems_libmmu_alut_search(addr);
  if (element_index != -1)
  {
    return rtems_libmmu_alut[element_index].access_attrib;
  }
  else
  {
    /* Error Handling code goes here */
  }
}


int
rtems_libmmu_list_alut_access(void)
{
  return 0;
}
