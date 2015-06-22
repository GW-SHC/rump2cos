SRC = paws.c paws_mount_ext2.c
APP = paws
LIBS = paws
DEPS = backing.o

main : backing.o
	@$(MAKE) all

backing.o :
	python ./tools/buildbacking.py

include ../makefile.comp
