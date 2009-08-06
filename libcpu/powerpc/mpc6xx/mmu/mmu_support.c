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
#include <rtems/libmmu.h>

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
SPR_RW(SDR1);
SPR_RO(DSISR);

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
search_empty_pte_slot(libcpu_mmu_pte *pteg){
  register int i;
  for(i = 0; i < 8; i++) {
    if(~(pteg[i].ptew0 & PTEW0_VALID)) {
      /* Found invalid pte slot */
      return i;
    }
  }
  return -1; /* Failed search for empty pte slot */
}


static int
search_valid_pte(libcpu_mmu_pte *pteg, uint32_t vsid, uint32_t api){

  register int i;
  register uint32_t temp_vsid;
  register uint32_t temp_api;
    
  for(i = 0; i < 8; i++) {
    if(pteg[i].ptew0 & PTEW0_VALID) { 
		 
      temp_api  = pteg[i].ptew0 & PTEW0_API;
      temp_vsid = (pteg[i].ptew0 & PTEW0_VSID) >> 7;
		
      if(temp_api == api && temp_vsid == vsid) { /* hit */
	return i;
      }  
    }
  }
  return -1;  /* Failed search */
}


static int
BSP_ppc_add_pte(libcpu_mmu_pte *ppteg,
		libcpu_mmu_pte *spteg,
                uint32_t vsid,
                uint32_t pi,
                uint32_t access)
{
  int index = 0;
  uint32_t hash, wimg, protp, rpn, api;
  libcpu_mmu_pte* pteg;
  
  /* Search empty PTE slot in PPTEG */
  index = search_empty_pte_slot(ppteg);
  if (index != -1){
    pteg = ppteg;
    hash = 0;
  } else {
    /* Search Empty slot in  SPTEG */
    index = search_empty_pte_slot(spteg);
    if (index == -1) {
      return -1;
    } else {
      pteg = spteg;
      hash = 1;
    }
  }
    
  api = pi >> 10;
  rpn = pi;
  wimg = 8;
  /* FIX ME - Need to modify according to access data */
  protp = 2; 

  pteg[index].ptew0 |= (vsid << 7) & PTEW0_VSID;
  pteg[index].ptew0 |= (hash << 6) & PTEW0_HASHF;
  pteg[index].ptew0 |= (api & PTEW0_API);
  pteg[index].ptew1 |= (rpn << 12);
  pteg[index].ptew1 |= (wimg << 3) & (PTEW1_WIMG);
  pteg[index].ptew1 |= (protp & PTEW1_PROTP);
  pteg[index].ptew0 |= PTEW0_VALID;
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

/* THis function shall be called upon exception on the DSISR
   register. depending on the type of exception appropriate action
   will be taken in this function. Most likely parameters for this
   function are
   - Exception Type (depending on DSISR[0..n]
   - address for which the transaltion failed
   - Anything else? */


static int
mmu_handle_dsi_exception(BSP_Exception_frame *f, unsigned vector){
  volatile uint32_t  ea, sr_data, vsid, pi, hash1, hash2, pp, key, api;
  volatile int ppteg_search_status, spteg_search_status;
  int alut_access_attrb;
  libcpu_mmu_pte* ppteg;
  libcpu_mmu_pte* spteg;
  volatile unsigned long cause;
  
  /* Switch MMU and other Interrupts off */
  _write_MSR(_read_MSR() & ~ (MSR_EE | MSR_DR | MSR_IR)); 

  
 
  /* get effective address from DAR */
  ea = _read_DAR();
  cause = _read_DSISR();
 
  /* Read corresponding SR Data */
  sr_data = _read_SR((void *) ea);

  /* Extract VSID */
  vsid = sr_data & SR_VSID;

  /* get page index (PI) from EA */
  pi = (ea >> 12) & 0x0000ffff;
  api = pi >> 10;

  /* Compute HASH 1 */
  hash1 = PTE_HASH_FUNC1(vsid, pi);

  /* Compute PTEG Address from the hash 1 value */
  get_pteg_addr(ppteg, hash1);

  /* Search for PTE in group */
  ppteg_search_status = search_valid_pte(ppteg, vsid, api);
  
  if (ppteg_search_status == -1){
  
    /* PTE Not found . Search in SPTEG */
    hash2 = PTE_HASH_FUNC2(hash1);
    get_pteg_addr(spteg, hash2);
    spteg_search_status = search_valid_pte(spteg, vsid, api);
    if (spteg_search_status == -1){
      
      /* PTE not found in second PTEG also */
      alut_access_attrb = rtems_libmmu_get_access_attribute((char *)ea);
      printk("ALUT Access Attribute is %d\n", alut_access_attrb);
      BSP_ppc_add_pte(ppteg, spteg, vsid, api, alut_access_attrb);

    } else {
      /* PTE found in second group */
      /* Code for determining access attribute goes here */
      key = ((sr_data & SR_KP) && MSR_PR) || ((sr_data & SR_KS) && ~(MSR_PR));
      pp = spteg[spteg_search_status].ptew1 & PTEW1_PROTP;
      if (((key && (pp == 1 || pp == 0 || pp == 3)) ||
	  ~(key) && (pp == 3)) &&
          (cause & 0x02000000)){
	/* Write access denied */
      } else if ((key && (pp == 0)) && (cause & 0x00000000)) {
	/* Read access denied */
        
      } else {
	/* Access permitted */
      }
    }
    
  } else { 
    /* PTE found in primary group itself */
    /* Code for determining attribute goes here */
    key = ((sr_data & SR_KP) && MSR_PR) || ((sr_data & SR_KS) && ~(MSR_PR));
    pp = ppteg[ppteg_search_status].ptew1 & PTEW1_PROTP;
    if (((key && (pp == 1 || pp == 0 || pp == 3)) ||
	~(key) && (pp == 3)) && 
        (cause & 0x02000000)){
      /* Write access denied */
    } else if ((key && (pp == 0)) && (cause & 0x00000000)) {
      /* Read access denied */
    } else {
      /* Access permitted */
    }
    
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
  for (i=0; i<16; i++) _write_SR(i, (void *)(i<<28));

  /* Set up SDR1 register for page table address */
  _write_SDR1((unsigned long) 0x0000FF00);
}

/* Make a BAT entry (either IBAT or DBAT entry) Parameters to pass
   would be EA, PA, Block Length, Access Bits */
void
mmu_make_bat_entry(void){

}



/* Same thing as above but for Instruction access */
void
mmu_handle_isi_exception(void){

}
