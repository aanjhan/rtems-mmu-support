/*
*  COPYRIGHT (c) 1989-2007.
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
  return rtems_libmmu_alut[0].access_attrib;
}


int
rtems_libmmu_list_alut_access(void)
{
  return 0;
}
