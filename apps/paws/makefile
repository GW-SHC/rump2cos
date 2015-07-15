SRC = paws.c paws_mount_ext2.c paws_test.c
APP = paws
LIBS = paws

main : tools/backing.o all

tools/backing.o :
	@ python ./tools/buildbacking.py

include ../makefile.comp
