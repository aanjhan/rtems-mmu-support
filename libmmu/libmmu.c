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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include <rtems.h>
#include <rtems/error.h>

#include "libmmu.h"

/* create a alut table instance */
rtems_libmmu_alut alut;


/* Temprary initialisation of ALUT */
//const rtems_libmmu_alut_entry rtems_libmmu_alut[RTEMS_LIBMMU_ALUT_SIZE] = 
//{
  /* Memory Layout access table initialisation goes here */
//  {0x00000000, 0x00000FFF, 1},
//  {0x00008000, 0x000007FF, 2},
//  {0x02000000, 0x00000FFF, 3},
//  {0x10000000, 0x00000FFF, 2},
//  {0x11000000, 0x00000FFF, 1}
//};

/*****************************************************
* Allocate memory for the ALUT, initialises the 
* ALUT
*****************************************************/
rtems_libmmu_alut* 
rtems_libmmu_alut_create(size_t size)
{
  
  rtems_libmmu_alut* pAlut;
  pAlut = &alut;
  
  if (pAlut != NULL)
  {
    pAlut->nElements = 0;
  }
  return pAlut;
}

/****************************************************
* Add a ALUT entry
****************************************************/
rtems_status_code
rtems_libmmu_alut_add_entry(rtems_libmmu_alut* pAlut, rtems_libmmu_alut_entry* pEntry)
{
  size_t nElements;
  nElements = pAlut->nElements;
  
  /* Check validity of the pointers */
  if (pAlut == NULL && pEntry == NULL)
  {
    return RTEMS_INVALID_ADDRESS;
  }
  
  /* Check for ALUT full condition */
  if(nElements == RTEMS_LIBMMU_ALUT_SIZE)
  {
    return RTEMS_TOO_MANY;
  }
  
  /* Check for address map overlaps???? */
  /* TODO */
  
  /* Append entry to the ALUT */
  pAlut->entries[nElements].start_addr = pEntry->start_addr;
  pAlut->entries[nElements].block_size = pEntry->block_size;
  pAlut->entries[nElements].access_attrib = pEntry->access_attrib;
  ++pAlut->nElements;
  return RTEMS_SUCCESSFUL;  
  
}  
  

/*****************************************************
* Linear search for the element emtry in the alut for*
* the address range under which it falls             *
*****************************************************/
static rtems_libmmu_alut_entry* 
rtems_libmmu_alut_search(rtems_libmmu_alut* pAlut, char* addr)
{
  rtems_libmmu_alut_entry* iter_entry;
  uint32_t nElements = pAlut->nElements;
  for ( iter_entry = pAlut->entries; iter_entry != &pAlut->entries[nElements]; ++iter_entry) {
    
    if ( iter_entry->start_addr <= addr &&
      iter_entry->start_addr + iter_entry->block_size > addr ) { 
      return iter_entry;
    } 
  } 
  return NULL;
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
rtems_libmmu_get_access_attribute(rtems_libmmu_alut* pAlut, char* addr)
{
  rtems_libmmu_alut_entry* hit_entry;
  hit_entry = rtems_libmmu_alut_search(pAlut, addr);
  if (hit_entry != NULL)
  {
    return hit_entry->access_attrib;
  }
  else
  {
    /* Error Handling code goes here */
    return RTEMS_UNSATISFIED;
  }
}


int
rtems_libmmu_list_alut_access(void)
{
  return 0;
}
