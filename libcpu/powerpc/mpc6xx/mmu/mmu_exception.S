/*
 * (c) 2009
 *
 *
 *  This file contains the assembly code for the PowerPC
 *  exceptions handling related to MMU 
 *
 * $Id: mmu_exception.S, $
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <bsp/vectors.h>
#include <libcpu/raw_exception.h>

PUBLIC_VAR(dsi_exception_vector_prolog_code)
SYM (dsi_exception_vector_prolog_code):
    bl mmu_handle_dsi_exception
	/*bla	push_normalized_frame*/
PUBLIC_VAR (dsi_exception_vector_prolog_code_size)
dsi_exception_vector_code_prolog_size = . - dsi_exception_vector_prolog_code