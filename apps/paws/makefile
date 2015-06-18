include ../makefile.comp

SRC = paws.c paws_mount_ext2.c
APP = paws
LIBS = paws
MSG = PAWS REQUIRES MAKE INSTALL BE CALLED FIRST

install : backing.o
	@./tools/installbacking
backing.o :
	@./tools/buildbacking
