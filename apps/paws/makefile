SRC = paws.c paws_mount_ext2.c paws_test.c paws_stubs.c
APP = paws
LIBS = paws

main : backing tools/backing.o all

backing :
	rm ./tools/backing.o
	cd ../../rumprun/platform/cos; make clean;

tools/backing.o :
	@ python ./tools/buildbacking.py

patch :
	# For all future patches to src-netbsd
	cp ./tools/cnic.patch ../../rumprun/src-netbsd
	cd ../../rumprun/src-netbsd; git apply --stat cnic.patch
	cd ../../rumprun/src-netbsd; git apply cnic.patch; rm cnic.patch

include ../makefile.comp
