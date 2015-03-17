Most modern processors have Memory Management Unit Hardware built into the processor whose main functions are virtual address translation, memory protection and cache control. With RTEMS primarily focused on Embedded Real Time applications, making use of these MMU features especially Memory Protection is important to meet the needs of those applications that requires such support. The goal of the project is to create an generic API structure independent of the underlying architecture to make use of the memory protection and caching features of any given architecture with MMU. The PowerPC mpc6xx has been taken up for the prototyping of the concept. The progress of the project can be tracked on http://code.google.com/p/rtems-mmu-support/


### Tasks Accomplished ###


  * A high level design of the concept. [Weblink](http://www.rtems.com/wiki/index.php/MMU_Support#Design_Notes)
  * A low level functional understanding of the PPC MMU unit. [Weblink ](http://tuxmaniac.com/work/gsoc/MMU%20Functional%20Flow.html)
  * Initial implementation of cpukit/libmmu
  * A sample testsuite for testing the libmmu api
  * Integration of the above into RTEMS infrastructure
  * Initial drafting of low level libcpu/powerpc/mpc6xx MMU handling


### Pending Tasks ###

  * Full implementation of the MMU page table search handling and abstraction to the libmmu.
  * Enabling MMU based cache control.
  * Adding POSIX compatible APIs for using features of MMU by applications
  * Updating RTEMS documents related to POSIX, BSPs