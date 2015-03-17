# Scope of the Project #

Most modern processors have Memory Management Unit Hardware built into the processor whose main functions are virtual address translation, memory protection and cache control. With RTEMS primarily focused on Embedded Real Time applications, making use of these MMU features especially Memory Protection is important to meet the needs of those applications that requires such support. A general infrastructure for applications to use the MMU for memory protection is intended to be prototyped with a basic proof-of-concept for the PowerPC architecture and PSIM board support package.

The MMU support library intended must be generalised as much as possible, a very versatile data structure. Also since RTEMS is intended for real time systems, the library should be optimized for real time behaviour.

# Major tasks #

  * Understanding the RTEMS source tree structure and the responsibilities of various levels in its architecture.
  * Getting to terms with the PowerPC general register set, MMU hardware architecture and its functional flow.
  * Understanding what parts of the functional flow is performed by the hardware and what needs to be implemented at the Operating System level.
  * Designing a generalised architecture independent method to specify memory address blocks with protection attribute, cache control etc.
  * Low level architecture dependent implementation of MMU support for PSIM BSP with PowerpC 603e series as the base architecture.
  * Testsuite integration that tests for several illegal / dangerous map specifications and also tests low level implementation.


# Design overview #

The design keeps in place the well separated design of RTEMS. The entire implementation is segregated into 2 layers.
  * Architecture independent API implementation
  * Architecture dependent MMU support implementation

The architecture independent API implementation is placed in the **cpukit** layer of RTEMS. This is the layer where a number of _libxxx_ implementations are present. For e.g. libcsupport, librpc, posix, libnetworking are present. The MMU support lib is called **libmmu** and shall be referenced the same hereon in this document. The architecture specific implementation, here in this project the PowerPC mpc6xx series are placed in the **libcpu/powerpc/mpc6xx** of RTEMS.

![http://www.rtems.com/wiki/images/d/d0/Hld_mmu_support.png](http://www.rtems.com/wiki/images/d/d0/Hld_mmu_support.png)

### LibMMU ###
The MMU support lib or libmmu contains general APIs that enables specifying address blocks with type of memory access protection, cache control bits. This information can be used by other layers when needed. A few such scenarios are discussed in the following sections.

These information are stored in a table called **Access Look Up Table** or **ALUT**. The table is implemented as shown below. Containing some sample values

| **Start Address** | **Address Block size** | **Memory Access Attribute** |
|:------------------|:-----------------------|:----------------------------|
| 0x00001000 | 0x4000 | SUPERVISOR\_READ\_ONLY |
| 0x1A000000 | 0x8000 | NO\_PROT |
| 0x00008000 | 0x4000 | SUPERVISOR\_READ\_ONLY |

The Cache control attributes will be added as an extra field here in the ALUT. The ALUT also takes in only values satisfying specific criteria as below :

  * Block sizes of 4K and more (because min page size in general is set as 4K)
  * Only sizes that are multiples of 4K are accepted.
  * Address ranges cannot overlap. (entry wont get added into the ALUT)

**LibMMU API examples**

Some APIs that are available for accessing the ALUT are as follows

  * rtems\_libmmu\_alut\_add\_entry
  * rtems\_libmmu\_alut\_create
  * rtems\_libmmu\_alut\_list\_attrbs (yet to be made completely functional)


### LibCPU/powerpc/mpc6xx ###

In this section as briefed above, is the architecture dependent implementation of MMU support for PowerPC 603e architecture according to specifications of 1.

Some important responsibilities at this layer are listed below

  * Setting up the exception handler at the appropriate vector
  * Decoding the address at which the exception occurred by reading from appropriate register
  * Calculation of the primary and secondary page table entry group addresses using the hash functions
  * Verifying the presence of a valid PTE for that address range.
  * Updating the PTE with valid info, by **pulling** the needed info from the ALUT.
  * Also implements arbitrary PTE invalidation and updating with a recently accesses page information if all available page table entries are filled.


# Accomplished tasks #

  * Basic support APIs of libMMU implemented.
  * PowerPC MMU architecture studied and support enabled for PSIM BSP under libCPU.
  * A testsuite that executes several test scenarios for testing both the libMMU and libCPU implementation implemented and run.
  * Various checks for protected memory accesses verified in the testsuite and found functional.


# Future work / ideas #

  * Enabling Cache control by MMU
  * Increase robustness of the implementation
  * Optimisation of code for performance
  * A POSIX like API support to set the protection. The inherent design of RTEMS has pushed this further for some thought.
  * Testing on real hardware.
  * TLB Miss Exception Test (unable to test because of may be a PSIM implementation issue)
  * Task based MMU Context feasibility study.


# Challenges faced #

A brief description of major challenges faced goes in here. Also solutions used, any alternate solutions shall be mentioned.

# Test suite #

The below section describes the testsuite used to verify the implementation.

**Prologue of the testsuite** : ALUT is created using the **create\_alut** API and a set of page table entries is populated with values making sure the valid bit is set. This initial PTE setup is used in a test case which shall be described later in this section.

**Test 1-6** : Tests various valid/invalid scenarios while populating the ALUT as seen the console output below.

**Test 7-8** : Tests retrieving of access attribute from the ALUT for a couple of scenarios.

**MMU Exception Tests** : First test, writes data into the address locations starting from 0x01A00008 . Since there would be no entry for this address block, and the PTEs have been already populated with some _valid_ entry, one PTE needs to be replaced with the data for the above address accessed.

The second exception test is performed on a different page address and the update of the PTE is verified using GDB.

The third test, checks whether the first PTE update is correct by making a same access to the page accessed by the first test. Now the exception should not be hit, as the PTE search algorithm should find an appropriate valid PTE present.

The final exception test does an access to an unmapped location that is present in the memory space of PSIM but not present in the ALUT table. Now the system panics as expected.

The snapshot of the console when running the test case is provided below.

```
*** MMU ALUT TEST BEGINS ***


Wrote Invalid entries into PTEG
ALUT created
Test 1 : Adding entry with block size less than 4K
Passed : Entry addition failed
Test 2 : Adding entry with block size not a multiple of 4K
Passed : Entry adding failed
Test 3 : Adding valid entry into ALUT
Passed : Entry Added
Test 4 : Adding overlapping  address value
Passed : Successful detection of address overlap and ignored
Test 5 : Adding valid entry
Passed: Entry successfully added
Test 6 : Adding valid entry
Passed : Entry successfully added
Test 7 : Searching for access attrbute for address 0x8111
Passed : Access Attribute for the request is 1
Test 8 : Searching for access attrbute for unmapped address 0x708111
Passed : Unmapped address' access requested
Checking MMU exception 1..
Checking MMU exception 2..
Checking MMU exception 3..
Checking MMU exception 4..

Breakpoint 1, C_exception_handler (excPtr=0x4d4c8)
    at ../../../../../../../rtems/c/src/lib/libcpu/powerpc/new-exceptions/bspsupport/vectors_init.c:112
112       int               recoverable = 0;
```




# References #
  1. http://www.freescale.com/files/product/doc/MPCFPE32B.pdf