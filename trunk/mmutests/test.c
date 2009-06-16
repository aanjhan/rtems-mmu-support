#include <stdio.h>
#include "libmmu.h"

/* Memory Layout access table initialisation goes here */
//  {0x00000000, 0x00000FFF, 1},
//  {0x00008000, 0x000007FF, 2},
//  {0x02000000, 0x00000FFF, 3},
//  {0x10000000, 0x00000FFF, 2},
//  {0x11000000, 0x00000FFF, 1}
//};

int main(void)
{
  int access;
  rtems_libmmu_alut_entry Entry;
  rtems_libmmu_alut* pAlut;
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
  
  access = rtems_libmmu_get_access_attribute(pAlut, 0x00008111);
  printf("Access atribute is %d\n", access);
  return 0;
}
  
  