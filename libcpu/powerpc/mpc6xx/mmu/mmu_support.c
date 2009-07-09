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

#include <rtems.h>
#include <libcpu/bat.h>
#include <libcpu/spr.h>
#include "mmu_support.h"
#include <rtems/powerpc/powerpc.h>
#include <rtems/irq.h>
#include <libcpu/raw_exception.h>
#include <rtems/asm.h>

// FIX ME
#include <stdio.h>

/* Macro for clearing BAT Arrays in ASM */

#define CLRBAT_ASM(batu,r)			\
	"	sync                 \n"	\
	"	isync                \n"	\
	"	li      "#r",    0   \n"	\
	"	mtspr	"#batu", "#r"\n"	\
	"	sync                 \n"	\
	"	isync                \n"

#define	CLRBAT(bat)				\
	asm volatile(				\
		CLRBAT_ASM(%0, 0)		\
		:				\
		:"i"(bat##U)			\
		:"0")


/* Checks whether address translation is enabled */
static uint32_t
mmu_is_addr_tran_enabled(void){
  return 0x0;
}

static void
tlbmisshdl(void){
}

/* Initialisation of MMU goes in here */
void
mmu_init(void){
  /* Clear BAT registers */
  CLRBAT (DBAT0);
  CLRBAT (DBAT1);
  CLRBAT (DBAT2);
  CLRBAT (DBAT3);  
  CLRBAT (DBAT4);
  CLRBAT (DBAT5);
  CLRBAT (DBAT6);
  CLRBAT (DBAT7);
  
}

void
mmu_irq_init(void){
  rtems_raw_except_connect_data dsiVector;
  dsiVector.exceptIndex	= ASM_PROT_VECTOR;
  dsiVector.hdl.vector	= ASM_PROT_VECTOR;
  dsiVector.hdl.raw_hdl	= dsi_exception_vector_prolog_code;
  dsiVector.hdl.raw_hdl_size = 	(unsigned) &dsi_exception_vector_prolog_code_size;
  ppc_set_exception(&dsiVector);
}

/* Turn on the MMU - Best done in assembly (Try to use  'rfi'
   instruction) */
void
mmu_on(void){
}

/* Turn off the MMU - Best done in assembly (Try to use  'rfi'
   instruction) */
void
mmu_off(void){
}

/* Make a BAT entry (either IBAT or DBAT entry) Parameters to pass
   would be EA, PA, Block Length, Access Bits */
void
mmu_make_bat_entry(void){

}

/* Search BAT array for a Hit or Miss - Actually the same function
   that is used to convert EA to PA can be used to return something
   indicating a BAT (Data or Instruction) miss 
   - Parameters to be decided */

uint32_t
mmu_dbat_generate_pa_from_ea(void){
  return 0x0;

  /* The same function can be used to continue in case of a BAT Array
  Hit to perform further Page protection checks and continuing access
  to the memory subsystem */

}


/* For Instruction BATs - May be this can be put in one single
   function */
uint32_t
mmu_ibat_generate_pa_from_ea(void){
  return 0x0;
  /* Same as above but for Instruction BAT array? */
}


/* THis function shall be called upon exception on the DSISR
   register. depending on the type of exception appropriate action
   will be taken in this function. Most likely parameters for this
   function are
   - Exception Type (depending on DSISR[0..n]
   - address for which the transaltion failed
   - Anything else? */
void
mmu_handle_dsi_exception(void){
  /* This would be the place where a new PTE can be
     fetched from the ALUT at the higher level */
  printf("Hit\n");

}

/* Same thing as above but for Instruction access */
void
mmu_handle_isi_exception(void){

}

void
mmu_make_pte(void){

}
