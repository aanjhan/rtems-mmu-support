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

#include "mmu.h"


/* Checks whether address translation is enabled */
static uint32_t
mmu_is_addr_tran_enabled(void){
  return 0x0;
}


/* Initialisation of MMU goes in here */
void
mmu_init(void){
  /* MMU initialisation code goes in here */
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
mmu_make_bat_entry(/* parameters to be decided */){

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
}

/* Same thing as above but for Instruction access */
void
mmu_handle_isi_exception(void){

}

void
mmu_make_pte(void){

}
