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
#include <bsp/ppc_exc_bspsupp.h>
#include "mmu_support.h"
#include <rtems/powerpc/powerpc.h>
#include <rtems/irq.h>
#include <rtems/asm.h>

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


SPR_RO(DAR);
SPR_RO(SDR1);

#define KEY_SUP			(1<<30) /* supervisor mode key */
#define KEY_USR			(1<<29) /* user mode key */
#define LD_PG_SIZE              12 /* In logarithm base */
#define LD_PI_SIZE              16
#define LD_VSID_SIZE            24
#define LD_HASH_SIZE            19

/* Primary and secondary PTE hash functions */

/* Compute the primary hash from a VSID and a PI */
#define PTE_HASH_FUNC1(vsid, pi) (((vsid)^(pi))&(0x0007FFFF))

/* Compute the secondary hash from a primary hash */
#define PTE_HASH_FUNC2(hash1) ((~(hash1))&(0x0007FFFF))

/* HTABORG and HTABMASK values */


static int
search_empty_pte_slot(libcpu_mmu_pte *pte){
  register int i;
  for(i = 0; i < 8; i++) {
    if(~(pte[i].ptew0 & PTEW0_VALID)) {
      /* Found invalid pte slot */
      return i;
    }
  }
  return -1; /* Failed search for empty pte slot */
}


static int
search_valid_pte(libcpu_mmu_pte *pte, uint32_t vsid, uint32_t api){

  register int i;
  register uint32_t temp_vsid;
  register uint32_t temp_api;
    
  for(i = 0; i < 8; i++) {
    if(pte[i].ptew0 & PTEW0_VALID) { 
		 
      temp_api  = pte[i].ptew0 & PTEW0_API;
      temp_vsid = (pte[i].ptew0 & PTEW0_VSID) >> 7;
		
      if(temp_api == api && temp_vsid == vsid) { /* hit */
	return i;
      }  
    }
  }
  return -1;  /* Failed search */
}


static int
add_pte(libcpu_mmu_pte *pte,
        uint32_t vsid,
        uint32_t hashfunc,
        uint32_t api,
        uint32_t rpn,
        uint32_t wimg,
        uint32_t protp)
{
  pte->ptew0 |= (vsid << 7) & PTEW0_VSID;
  pte->ptew0 |= (hashfunc << 6) & PTEW0_HASHF;
  pte->ptew0 |= (api & PTEW0_API);
  pte->ptew1 |= (rpn << 12);
  pte->ptew1 |= wimg & (PTEW1_WIMG << 3);
  pte->ptew1 |= (protp & PTEW1_PROTP);
  pte->ptew0 |= PTEW0_VALID;
  return 0;
}

static void
get_pteg_addr(libcpu_mmu_pte* pteg, uint32_t hash){
  uint32_t masked_hash;
  uint32_t htaborg, htabmask;
  htabmask = _read_SDR1() & 0x1ff;
  htaborg = _read_SDR1() & 0xffff0000;
  masked_hash = ((htaborg >> 16) & 0x000001ff) | ((hash >> 10) & htabmask);
  pteg = (libcpu_mmu_pte *)(htaborg | (masked_hash << 16) | (hash & 0x000003ff) << 6);
}




/* Checks whether address translation is enabled */
static uint32_t
mmu_is_addr_tran_enabled(void){
  return 0x0;
}



/* THis function shall be called upon exception on the DSISR
   register. depending on the type of exception appropriate action
   will be taken in this function. Most likely parameters for this
   function are
   - Exception Type (depending on DSISR[0..n]
   - address for which the transaltion failed
   - Anything else? */


static int
mmu_handle_dsi_exception(BSP_Exception_frame *f, unsigned vector){
  uint32_t ea, sr_data, vsid, pi, hash1, hash2;
  int ppteg_search_status, spteg_search_status;
  libcpu_mmu_pte* ppteg;
  libcpu_mmu_pte* spteg;
  printk("DSI Exception hit\n");
  
  /* get effective address from DAR */
  ea = _read_DAR();

  /* Read corresponding SR Data */
  sr_data = _read_SR((void *) ea);

  /* Extract VSID */
  vsid = sr_data & SR_VSID;

  /* get page index (PI) from EA */
  pi = (ea >> 12) & 0x0000ffff;

  /* Compute HASH 1 */
  hash1 = PTE_HASH_FUNC1(vsid, pi);

  /* Compute PTEG Address from the hash 1 value */
  get_pteg_addr(ppteg, hash1);

  /* Search for PTE in group */
  ppteg_search_status = search_valid_pte(ppteg, vsid, pi);
  
  if (ppteg_search_status == -1){
  
    /* PTE Not found . Search in SPTEG */
    hash2 = PTE_HASH_FUNC2(hash1);
    get_pteg_addr(spteg, hash2);
    spteg_search_status = search_valid_pte(spteg, vsid, pi);
    if (spteg_search_status == -1){
      /* PTE not found in second PTEG also */
    } else {
      /* PTE found in second group */
      /* Code for determining access attribute goes here */
    }
    
  } else { 
    /* PTE found in primary group itself */
    /* Code for determining attribute goes here */
  }
  return 0;

}

static int
mmu_handle_tlb_dlmiss_exception(BSP_Exception_frame *f, unsigned vector){
  printk("DL TLB MISS Exception hit\n");
  return 0;
}


static int
mmu_handle_tlb_dsmiss_exception(BSP_Exception_frame *f, unsigned vector){
  printk("DS TLB MISS Exception hit\n");
  return 0;
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
  uint32_t i;
  ppc_exc_set_handler(ASM_PROT_VECTOR, mmu_handle_dsi_exception);
  ppc_exc_set_handler(ASM_60X_DLMISS_VECTOR, mmu_handle_tlb_dlmiss_exception);
  ppc_exc_set_handler(ASM_60X_DSMISS_VECTOR, mmu_handle_tlb_dsmiss_exception);
  

  /* Initialise segment registers */
  for (i = 0;i < 16;i++) {
    asm volatile( "mtsrin %0, %1\n" : : "r" (i * 0x1000), "r" (i << (31 - 3)));
  }

  /* Set up SDR1 register for page table address */
  /*asm volatile ("mtspr 25,%0"::"r" (pt));*/
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



/* Same thing as above but for Instruction access */
void
mmu_handle_isi_exception(void){

}
