### Rump2Cos Installation and usage instructions

##### Setting up Rump2Cos Local Repository

1. Clone rump2cos repository
```
git clone https://github.com/GW-SHC/rump2cos.git
```
2. Initialize rump2cos submodules - Only needs to be done once
```
git submodule init
```
3. Update rump2cos submodules - Everytime rump2cos is pulled
```
git submodule update --recursive
```
This should clone `Composite` and `rumprun` repos in `rump2cos`.

4. Repeat steps 2 & 3 in `rumprun` directory

All set! Go to build steps now.
**Note** If you'd like to have separate Composite working directory, you might want to update your `Makefile`s from `Composite` main repository, if it's not already up-to-date here.

##### Compile rump2cos

There are 2 different ways of compiling `rumpkernel`!

1. The RK is configured currently to expect a backing.o file used for the paws application. Going into `apps/paws` and doing a `make` will build `rumpkernel`, and `paws` application and place the backing.o into the appropirate location.

This can and should be fixed eventually so that the RK can be compiled
without being interfiered by application dependencies.

2. Going into `rumprun` and executing `build-rr.sh [platform]` script: This will build rumpkernel alone. Possible platforms to pass are cos, hw, and xen.
3. 
**Note** RK is not currently designed to be able to change platforms
after the initial build-rr.sh, if one wishes to make a RK with platform
hw after using cos, it is advised to make a separate rumprun direcotry.
**Note** If you're compiling this on a 64bit machine you'll need to run `build-rr.sh cos -- -F ACLFLAGS=-m32` to compile `rumpkernel` to run on 32bit. Compiling without the m32 flag will cause all sorts of bugs and compilation errors, if problems arrise try a fresh install of rump2cos or just rumprun and make sure to use m32.

To compile the rest of the system,
1. Go into `Composite/src` and execute a series of commands: `make config-i386` -> `make init` -> `make` -> `make cp`. Further instruction can be found on Composite repo.
2. Once you've either `hello` or `paws` program compiled, by running make in their respective /app directories, you'll need to go into `Composite/src/components/implementation/no_interface/rumpbooter` and execute `./buildRumpCos.sh <app>`. This will link the (application & RK libs) + Cos into a single entity and transer the object to the transfer directory designated by your Composite setup.

##### Run rump2cos

Go into `Composite/transfer` directory and execute:
```
./qemu.sh rumpkernboot.sh
```

All done!

###### Sample Execution Log:
```
phani@phani-thinkpad:~/research/rump2cos/Composite/transfer$ ./qemu.sh rumpkernboot.sh 
/usr/bin/ld: error in /tmp/llboot.o.2678.o(.eh_frame); no .eh_frame_hdr table will be created.
/usr/bin/ld: error in /tmp/llboot.o.2678.o(.eh_frame); no .eh_frame_hdr table will be created.
Couldn't map the configuration info into the address space: Invalid argument
System memory info from multiboot (end 0xc1257000):
	Modules:
	- 0: [01258000, 01536800) : 40800000-4092aa40 @ virtual address 40800000, _start = 4092aa40.
	Memory regions:
	- 0 (Available): [00000000, 0009fc00)
	- 1 (Reserved ): [0009fc00, 000a0000)
	- 2 (Reserved ): [000f0000, 00100000)
	- 3 (Available): [00100000, 07ffe000)
	- 4 (Reserved ): [07ffe000, 08000000)
	- 5 (Reserved ): [fffc0000, 100000000)
	Amount of wasted memory due to layout is 13 MB + 0xa8800 B
Initializing virtual memory
	Setting up initial page directory.
Found good RSDP @ c00f0990
Found good HPET @ c37ffb89
Initiliazing HPET @ c37ffb89
-- Checksum is OK
Addr: 00000000fed00000
hpet: fed00000
Set HPET @ c3900000
Setting up the booter component.
	Capability table and page-table created.
	Creating 1 booter VM PTEs for PGD @ 0xc0110000 from [c1258000,c1536800) to [40800000,40ade800).
	Mapping in booter VM.
	Creating 8 user-typed memory PTEs for PGD @ 0xc0110000 from [c4000000,c6000000) to [80000000,82000000).
	Mapping in user-typed memory.
	Creating 8 untyped memory PTEs for PGD @ 0xc0110000 from [c154e000,c3258000) to [60000000,61d0a000).
	Mapping in untyped memory.
	Created boot component structure from page-table and capability-table.
	Creating initial threads, tcaps, and rcv end-points in boot-component.
	Boot component initialization complete.
Enabling timer @ c3900000
Upcall into boot component at ip 0x4092aa40
------------------[ Kernel boot complete ]------------------
p
Micro Booter started.
---------------------------
switchto 36
switchto 36 @ 4092aa40
	New thread 2 with argument 0
switchto 37
switchto 37 @ 4092aa40
	New thread 3 with argument 1
switchto 38
switchto 38 @ 4092aa40
	New thread 4 with argument 2
switchto 39
switchto 39 @ 4092aa40
	New thread 5 with argument 3
switchto 40
switchto 40 @ 4092aa40
	New thread 6 with argument 4
test done
---------------------------
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppAverage THD SWTCH (Total: 1123805452 / Iterations: 1000000 ): 1123
---------------------------
p---------------------------
Starting timer test.
.p.p.p.p.p.p.p.p.p.pTimer test completed.
Success.
---------------------------
---------------------------
---------------------------
---------------------------
---------------------------
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppAverage ASND/ARCV (Total: 2371786073 / Iterations: 1000000 ): 2371
---------------------------
---------------------------
---------------------------
---------------------------

Micro Booter done.

Rump Sched Test Start
In rumptest_thd_fn
thdid: 28504
fetching thd id
thdid, is now: 12
switching back to old thread
switched back to old thread, thdid: 12

Rump Sched Test End

RumpKernel Boot Start.
Rump Kernel bootstrap on platform Composite
bmk_sched_init done
bmk_memalloc_init done
first page: 0
ppmax: 1fff000
alloc_bitmap: 0x1000, 1
bitmap_size: 4096
returned from bmk_pgalloc_loadmem
rumprun_boot


bmk_current: 0x1ffd010
Entering rump_init
rumpuser_init
rumprun_platform_rumpuser_init is being called.
rumpuser_clock_gettime
which: 0
bmk_platform_clock_epochoffset is being called.
pbmk_intr_init done
Copyright (c) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
    2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015
    The NetBSD Foundation, Inc.  All rights reserved.
Copyright (c) 1982, 1986, 1989, 1991, 1993
    The Regents of the University of California.  All rights reserved.

NetBSD 7.99.17 (RUMP-ROAST)
total memory = 15232 KB
ppptimecounter: Timecounters tick every 10.000 msec
timecounter: Timecounter "clockinterrupt" frequency 100 Hz quality 0
cpu0 at thinair0: rump virtual cpu
proot file system type: rumpfs
kern.module.path=/stand/i386/7.99.17/modules
rumpuser_getfileinfo
pmainbus0 (root)
Exit rump_init
Mounting filesystem
pDone Mounting filesystem
rumprun_lwp_init
exiting rumprun_lwp_init
netbsd_initfini.c: _netbsd_userlevel_init
Calling runinit
netbsd_initfini.c: runinit
Calling _libc_init
Exiting _libc_init
__init_array_start: 0x4095e8f4
__init_array_end: 0x4095e8f8
fn: 0x4095e8f4
Exiting runinit
Exiting runinitExiting _netbsd_userlevel_init
mounted tmpfs on /tmp
skipping over _rumprun_config(cmdline). TODO - see if this has an impact
sched_yield is called
bmk_sched_yield is being called 
rumpuser_clock_gettime
which: 1
sched_yield is exiting
Rumprun.c: 146
Rumprun.c: 148
Rumprun.c: 150
pFIX PROGNAME

=== calling "hello" main() ===

rumpuser_getfileinfo
rumpuser_getfileinfo
/dev/paws: hostpath paws (512 KB)
fd for /dev/paws: 3
phello hobbes back for more

directory /mnt created
Printing /
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory //mnt
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory //tmp
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory //dev

Printing /dev
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /dev/zero
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /dev/null

fd for /dev/paws is: 5
fstat info: st_dev 8
fstat info: st_size 524288

first print:
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/.
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/..
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCuprrent directory /mnt/lost+found

Making files...

Second print: 
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/.
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/..
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/lost+found
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file1
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file2

Unmounted, reprinting /mnt

Remounting and printing /mnt

Third print:
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/.
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/..
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/lost+found
pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file1
ppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppCurrent directory /mnt/file2
Writting to file1: done

Reading from file 1:
abcpdabcdabcabcdabcabcdabcdabcdabcabcdabcdabcdabcabcdabcabcdabcabcdabcabcdabcabcdabcddddddddd
efgefgefgefgefgefefefefefgefgefgefgefefefefgefefggefefgefefefgefgefefgefgefgefgefggefgefggggggggggg


=== main() of "hello" returned 0 ===

=== _exit(0) called ===
netbsd_initfini.c: _netbsd_userlevel_fini
netbsd_initfini.c: runfini
rump kernel halting...
syncing disks... done
unmounting file systems...
unmounted /dev/paws on /mnt type ext2fs
unmounted tmpfs on /tmp type tmpfs
unmounted rumpfs on / type rumpfs
unmounting done
rumpuser_exit
bmk_platform_halt is being called.
It's message is: PppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppQEMU 2.0.0 monitor - type 'help' for more information
(qemu) ppppppppppppppppppppppppppppppppppppppppppqppppppppppppquppppppppppquippppppppppquitpppppppppppp
phani@phani-thinkpad:~/research/rump2cos/Composite/transfer$ 
```

### Appendix

##### Pushing your changes

The rump2cos repository is comprised of an app/ direcotry as well as two
git submodules or sub repositories.

Editing any of the code within the subrepositories (Cos or rumprun) will
requre a commit and a push within those repos individually.
For example, switching to Composite/ directory will switch ones git status to report information from the Composite repository. Any changes made in the Composite/ directory will need to be commited and pushed to the Composite repo.
Changing back to the higher rump2cos directory, one can now add the new
commit number of the Composite submodle as if it was a file difference or
any other git recognizable change. The same applies to rumprun.

Once as all new commit numbers have been added to rump2cos, one can push
the main repository.

**Note** If you commit the submodules and add their commit number to
push rump2cos *but* forget to push the submodules themselves, others will have errors when pulling down the new code as the rump2cos submodules will be pointing to commits that do not exist on github.
the remote repositories.

##### Pulling others changes
When pulling, follow the instructions marked at the beginning, skipping the submodule init. One only has to run git submodule update --recursive inorder to pull down the new code for the submodules.
