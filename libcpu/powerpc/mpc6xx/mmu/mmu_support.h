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
#ifndef _LIBCPU_MMU_SUPPORT_H
#define _LIBCPU_MMU_SUPPORT_H

/* Defining masks for the various bits of the PTE
   Referenced from OEA PowerPC Manual */

#define PTEW0_VALID            0x80000000
#define PTEW0_VSID             0x7fffff80
#define PTEW0_HASHF            0x00000040
#define PTEW0_API              0x0000003f
#define PTEW1_RPN              0xfffff000
#define PTEW1_REF              0x00000100
#define PTEW1_CHG              0x00000080
#define PTEW1_WIMG             0x00000078 
#define PTEW1_PROTP            0x00000003

#define SR_VSID                0x00ffffff
#define SR_KS                  0x40000000                                           
#define SR_KP                  0x20000000  

/* Splitting the 64 bit PTE into two 32 bit words. As shown in the OEA
   Manual of PowerPC */

typedef struct
{
  uint32_t ptew0; /* Word 0 */
  uint32_t ptew1; /* Word 1 */
} libcpu_mmu_pte;






/* Function prototypes that can be shared with high
   level modules go in here */
void
mmu_init(void);

void
mmu_irq_init(void);

#endif
