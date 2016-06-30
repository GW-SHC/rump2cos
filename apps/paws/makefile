SRC = paws.c paws_mount_ext2.c paws_test.c paws_stubs.c
APP = paws
LIBS = paws

main : backing tools/backing.o all

backing :
	rm -f ./tools/backing.o
	cd ../../rumprun/platform/cos; make clean;

tools/backing.o :
	@ python ./tools/buildbacking.py

patch :
	# For all future patches to src-netbsd
	cp ./tools/*.patch ../../rumprun/src-netbsd
	cd ../../rumprun/src-netbsd; git apply *.patch

include ../makefile.comp
