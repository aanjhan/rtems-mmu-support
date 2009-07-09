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


int dsi_exception_vector_prolog_code_size;

/* Function prototypes that can be shared with high
   level modules go in here */
void
mmu_init(void);

extern void
mmu_handle_dsi_exception(void);

extern void dsi_exception_vector_prolog_code(void);

void
mmu_irq_init(void);

#endif
