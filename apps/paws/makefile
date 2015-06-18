include ../makefile.comp

SRC = paws.c paws_mount_ext2.c
APP = paws
LIBS = paws
DEPS = backing.o

backing.o :
	python ./tools/buildbacking.py
