/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: init.c,v 1.15 2008/01/11 22:50:10 joel Exp $
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <stdlib.h>
#include <rtems/libmmu.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  int access;
  rtems_libmmu_alut_entry Entry;
  rtems_libmmu_alut* pAlut;
  puts( "\n\n*** MMU ALUT TEST BEGINS ***\n\n" );
  pAlut = rtems_libmmu_alut_create(3);
  printf("ALUT created\n");
  Entry.start_addr = 0x00000000;
  Entry.block_size = 0x00000FFF;
  Entry.access_attrib = 1;
  rtems_libmmu_alut_add_entry(pAlut,&Entry);
  printf("1 Entry added\n");
  
  Entry.start_addr = 0x02000000;
  Entry.block_size = 0x00000FFF;
  Entry.access_attrib = 3;
  rtems_libmmu_alut_add_entry(pAlut,&Entry);
  printf("1 Entry added\n");
  
  Entry.start_addr = 0x00008000;
  Entry.block_size = 0x000007FF;
  Entry.access_attrib = 2;
  rtems_libmmu_alut_add_entry(pAlut,&Entry);
  printf("1 Entry added\n");
  
  printf("Searching for access attrbute for address 0x00008111...\n");
  access = rtems_libmmu_get_access_attribute(pAlut, 0x00008111);
  printf("Access atribute is %d\n", access);
  puts( "\n\n*** MMU ALUT TEST ENDS ***\n\n" );
  exit(0);
  //  rtems_test_exit(0);
}
