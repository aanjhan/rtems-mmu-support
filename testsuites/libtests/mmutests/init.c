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

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  int access;
  int i;
  unsigned char* a1;
  unsigned char* a2;
  unsigned char* a3;
  unsigned char* ppteg_addr;
  unsigned char* spteg_addr;
  char * alut_search_addr1;
  char * alut_search_addr2;
  a1 = (unsigned char *)0x01A01000;
  a2 = (unsigned char *)0x01A00008;
  /* Unmapped access request test - Should panic */
  a3 = (unsigned char *)0x01A10008;
  
  alut_search_addr1 = (char*) 0x00008111;
  alut_search_addr2 = (char*) 0x00708111;
  
  ppteg_addr = (unsigned char *) 0x00FF8000;
  spteg_addr = (unsigned char *) 0x00FF7FC0;
  
  rtems_libmmu_alut_entry Entry;
  rtems_libmmu_alut* pAlut;
  

  puts( "\n\n*** MMU ALUT TEST BEGINS ***\n\n" );

  /* Write dummy entries into PTEG for testing */
  for(i=0; i<64;i++){
    *ppteg_addr = 0xFF;
    *ppteg_addr++;
  }

  for(i=0; i<64;i++){
    *spteg_addr = 0xFF;
    *spteg_addr++;
  }
  printf("Wrote Invalid entries into PTEG\n");
 
  pAlut = rtems_libmmu_alut_create(3);
  printf("ALUT created\n");
  printf("Test 1 : Adding entry with block size less than 4K\n");
  Entry.start_addr = 0x00000000;
  Entry.block_size = 2096;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_NO_PROT;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry added\n");
  }
  else{
    printf("Passed : Entry addition failed\n");
  }

    
  printf("Test 2 : Adding entry with block size not a multiple of 4K\n");  
  Entry.start_addr = (char*)0x02000000;
  Entry.block_size = 0x00008FFF;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_READ_ONLY;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Invalid block size and still entry successfully added\n");
  }
  else{
    printf("Passed : Entry adding failed\n");
  }
  
  printf("Test 3 : Adding valid entry into ALUT\n");
  Entry.start_addr = (char*)0x01A00000;
  Entry.block_size = 0x00008000;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_NO_PROT;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry Added\n");
  }
  else{
    printf("Failed : Entry addition failed\n");
  }

  printf("Test 4 : Adding overlapping  address value\n");
  Entry.start_addr = (char*)0x01A07000;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_NO_PROT;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Failed : Addition passed inspite of address overlap\n");
  }
  else{
    printf("Passed : Successful detection of address overlap and ignored\n");
  }

  printf("Test 5 : Adding valid entry\n");
  Entry.start_addr = (char*)0x01F00000;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_NO_PROT;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed: Entry successfully added\n");
  }
  else{
    printf("Failed : Entry adding failed\n");
  }

  printf("Test 6 : Adding valid entry\n");
  Entry.start_addr = (char*)0x00008000;
  Entry.block_size = 0x00004000;
  Entry.access_attrib = RTEMS_LIBMMU_ACCESS_READ_ONLY;
  status = rtems_libmmu_alut_add_entry(pAlut,&Entry);
  if(status == RTEMS_SUCCESSFUL){
    printf("Passed : Entry successfully added\n");
  }
  else{
    printf("Failed : Entry adding failed\n");
  }

  /* Now that the ALUT is created and populated, start testing for 
     search operations over particular address values */
  
  printf("Test 7 : Searching for access attrbute for address 0x%x\n", alut_search_addr1);
  access = rtems_libmmu_get_access_attribute(alut_search_addr1);
  if(access == RTEMS_UNSATISFIED){
    printf("Failed : Access Attribute not found \n");
  }
  else {
    printf("Passed : Access Attribute for the request is %d\n", access);
  }

  printf("Test 8 : Searching for access attrbute for unmapped address 0x%x\n", alut_search_addr2);
  access = rtems_libmmu_get_access_attribute(alut_search_addr2);
  if(access == RTEMS_UNSATISFIED){
    printf("Passed : Unmapped address' access requested\n");
  }
  else{
    printf("Failed : Access attribute request passed for an unmapped address\n");
  }

  printf("Checking MMU exception 1.. \n");
  for(i=0;i<128;i++){
   *a2++ = 0xCC;
  }
  
  printf("Checking MMU exception 2.. \n");
  for(i=0;i<128;i++){
   *a1++ = 0xCC;
  }

  printf("Checking MMU exception 3.. \n");
  for(i=0;i<128;i++){
   *a2++ = 0xCC;
  }
  
  printf("Checking MMU exception 4.. \n");
  for(i=0;i<128;i++){
   *a3++ = 0xCC;
  }

  puts( "\n\n*** MMU ALUT TEST ENDS ***\n\n" );
  exit(0);
}
