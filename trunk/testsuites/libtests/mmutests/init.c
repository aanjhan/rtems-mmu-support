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
  rtems_status_code status;
  int access;
  rtems_libmmu_alut_entry Entry;
  rtems_libmmu_alut* pAlut;
  puts( "\n\n*** MMU ALUT TEST BEGINS ***\n\n" );

  pAlut = rtems_libmmu_alut_create(3);
  printf("ALUT created\n");
  printf("Adding entry\n");
  Entry.start_addr = 0x00000000;
  Entry.block_size = 0x00000FFF;
  Entry.access_attrib = 1;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("1 Entry added\n");
  }
  else{
    printf("Entry addition failed\n");
  }

    
  printf("Adding entry\n");  
  Entry.start_addr = (char*)0x02000000;
  Entry.block_size = 0x00000FFF;
  Entry.access_attrib = 3;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Entry successfully added\n");
  }
  else{
    printf("Entry adding failed\n");
  }
  

  printf("Adding overlapping  address value\n");
  Entry.start_addr = (char*)0x02000400;
  Entry.block_size = 0x000007FF;
  Entry.access_attrib = 2;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("WARNING : Addition passed inspite of address overlap\n");
  }
  else{
    printf("Successful detection of address overlap and ignored\n");
  }

  printf("Adding entry\n");
  Entry.start_addr = (char*)0x00008000;
  Entry.block_size = 0x000007FF;
  Entry.access_attrib = 2;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Entry successfully added\n");
  }
  else{
    printf("Entry adding failed\n");
  }
  
  printf("Searching for access attrbute for address 0x00008111...\n");
  access = rtems_libmmu_get_access_attribute(pAlut, (char*)0x00008111);
  printf("Access atribute is %d\n", access);

  printf("Searching for access attrbute for address 0x00708111...\n");
  access = rtems_libmmu_get_access_attribute(pAlut, (char*)0x00708111);
  if(access == RTEMS_UNSATISFIED){
    printf("Unmapped address' access requested\n");
  }
  else{
    printf("WARNING : Access attribute request passed for an unmapped address\n");
  }

  puts( "\n\n*** MMU ALUT TEST ENDS ***\n\n" );
  exit(0);
  //  rtems_test_exit(0);
}
