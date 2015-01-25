# rump2cos

Our development repo to explore porting rump kernels to Composite OS (COS).

Please filling the following FAQ that currently is question-heavy, and answer-light.

# Frequently Asked Questions

*What are rump kernels?*
 * Rump Kernels are pieces of the NetBSD kernel that provide access to kernel code within user space, brining only what is needed to make kernel development and debugging
	as easy as possible. Individual rump kernels provide great functionality, thus by implementing them into an OS, the OS is then given access to the same services.

*What are good references for learning about rump kernels?*
 * See "useful readings".

*What document provides the best quick(ish) intro?*
 * http://rumpkernel.org/misc/usenix-login-2014/login_1410_03_kantee.pdf

*What "components" are supported by rump kernels?*
 * Initialization, Memory allocation, Files and I/O, Clocks, Parameter retrieval, Termination, Signals, and Threads.
 
*What types of device drivers are supported?*
 *File systems, POSIX system call handlers, PCI device drivers, a SCSI protocol stack, virtio and a TCP/IP stack. 

*What is a hypercall, and how do they relate to porting to COS?*
 * A hypercall is a call from the rump kernel down to the hosting platform for resources. As the rump kernel thinks it is running on bare
 metal, there is a hypervisor which takes the hypercall and transforms the function call to something that the hosting platform can act upon.
 The hypercall is then sent back up through the hypervisor with the appropriate resources that were asked for.

You can start with http://netbsd.gw.com/cgi-bin/man-cgi?rumpuser+3+NetBSD-current.

*What are ways to start exploring rump kernel's code?*
 * To begin with, one should read the rumpuser api overview, (see link above). After that, pick a hypercall from the overview
 and dive into the source code on github. Try to understand the code base and then begin to modify it.

*How are rump kernels related to NetBSD?*
 * "Rump kernels started in 2007 as a way to make debugging and developing NETBSD kernel code easier" (http://rumpkernel.org/misc/usenix-login-2014/login_1410_03_kantee.pdf).
 NetBSD was the first BSD derived OS that implements rump kernel drivers.

*What is BSD?*
 * BSD - Berkeley Software Distribution is a Unix operating system derivative developed and distributed by the Computer Systems Research Group of the
 University of California, Berkeley.
 BSD includes:
   * The BSD kernel, which handles process scheduling, memory management, symmetric multi-processing (SMP), device drivers, etc.
   * The C library, the base API for the system.
   * Utilities such as shells, file utilities, compilers and linkers.
   * The X Window system, which handles graphical display.
  * Other versions that have branched off include: NetBSD, OpenBSD, FreeBSD

*What is Composite?*
 *Composite is a developing OS created by Gabriel Parmer and is used as a research platform at the George Washington University. Composite strives to keep the amount of Kernel
 code to a minimum, revolving around 5k lines. Composite uses components at the user level which communicative through a very fast form of IPC. These components
 provide for scalable parallel processing, reliable micro rebooting and strong security.

*What is the proposed plan to merge Composite and rump kernels, and why does it make sense?*
 * Rump kernels fit nicely with COS because both worth through the idea of components. The rump kernels are just portions of an overall kernel that function independently. Composite too has
 individual functioning parts of the kernel that are brought up to user level. Hooking up the rump kernels and the COS components would provide all the benefits of NetBSD to composite. Including
 drivers which would allow COS to run on hardware.

*What is a RAMDisk?  How do you think one is implemented?*
 *  A RAMDisk is a block of RAM (primary storage or volatile memory) that a computer's software is treating as if the memory were a disk drive. It could be implemented
 by changing the storage location of files and data to be a malloced char array of the desired size.

*...add more questions here...*
