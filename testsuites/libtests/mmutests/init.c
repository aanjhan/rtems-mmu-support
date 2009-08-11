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
#include <libcpu/mmu_support.h>

#define SYNC_LONGJMP_TEST(msr)				\
	asm volatile(						\
		"	mtsrr1	%0			\n\t"	\
		"	bl		1f			\n\t"	\
		"1:	mflr	3			\n\t"	\
		"	addi	3,3,1f-1b	\n\t"	\
		"	mtsrr0	3			\n\t"	\
		"	rfi					\n\t"	\
		"1:						\n\t"	\
		:								\
		:"r"(msr)						\
		:"3","lr","memory")



rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  unsigned long msr;
  int access;
  int i;
  volatile char* a1 = 0x01F00000;
  volatile char* a2 = 0x01A00008;
  void* memset_status;
  unsigned char* start = (unsigned char*)0x3000;
  rtems_libmmu_alut_entry Entry;
  rtems_libmmu_alut* pAlut;
  puts( "\n\n*** MMU ALUT TEST BEGINS ***\n\n" );
  pAlut = rtems_libmmu_alut_create(3);
  printf("ALUT created\n");
  printf("Adding entry with block size less than 4K\n");
  Entry.start_addr = 0x00000000;
  Entry.block_size = 2096;
  Entry.access_attrib = 1;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("WARNING : Invalid block size and still entry added\n");
  }
  else{
    printf("Entry addition failed\n");
  }

    
  printf("Adding entry with block size not a multiple of 4K\n");  
  Entry.start_addr = (char*)0x02000000;
  Entry.block_size = 0x00008FFF;
  Entry.access_attrib = 3;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("WARNING : Invalid block size and still entry successfully added\n");
  }
  else{
    printf("Entry adding failed\n");
  }
  
  printf("Adding valid entry into ALUT\n");
  Entry.start_addr = (char*)0x01A00000;
  Entry.block_size = 0x00008000;
  Entry.access_attrib = 2;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Entry Added\n");
  }
  else{
    printf("WARNING : Entry addition failed\n");
  }

  printf("Adding overlapping  address value\n");
  Entry.start_addr = (char*)0x02001400;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = 1;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("WARNING : Addition passed inspite of address overlap\n");
  }
  else{
    printf("Successful detection of address overlap and ignored\n");
  }

  printf("Adding entry\n");
  Entry.start_addr = (char*)0x01F00000;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = 1;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Entry successfully added\n");
  }
  else{
    printf("WARNING : Entry adding failed\n");
  }

  printf("Adding entry\n");
  Entry.start_addr = (char*)0x00008000;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = 1;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Entry successfully added\n");
  }
  else{
    printf("WARNING : Entry adding failed\n");
  }

  
  printf("Searching for access attrbute for address 0x00008111...\n");
  access = rtems_libmmu_get_access_attribute((char*)0xAA000000);
  if(access == RTEMS_UNSATISFIED){
    printf("Access Attribute not found \n");
  }
  else {
    printf("Access Attribute for the request is %d\n", access);
  }

  printf("Searching for access attrbute for address 0x00708111...\n");
  access = rtems_libmmu_get_access_attribute((char*)0x00708111);
  if(access == RTEMS_UNSATISFIED){
    printf("Unmapped address' access requested\n");
  }
  else{
    printf("WARNING : Access attribute request passed for an unmapped address\n");
  }

  printf("Checking MMU expection 1.. \n");
  for(i=0;i<128;i++){
   *a2++ = 0xaaaaaaaa;
  }
  
  printf("Checking MMU expection 2.. \n");
  for(i=0;i<128;i++){
   *a1++ = 0xaaaaaaaa;
  }

  printf("Checking MMU expection 3.. \n");
  for(i=0;i<128;i++){
   *a2++ = 0xaaaaaaaa;
  }
  puts( "\n\n*** MMU ALUT TEST ENDS ***\n\n" );
  exit(0);
}
