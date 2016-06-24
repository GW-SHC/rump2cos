SRC = paws.c paws_mount_ext2.c paws_test.c paws_stubs.c
APP = paws
LIBS = paws

main : backing tools/backing.o all

backing :
	rm ./tools/backing.o
	cd ../../rumprun/platform/cos; make clean;

tools/backing.o :
	@ python ./tools/buildbacking.py

include ../makefile.comp
