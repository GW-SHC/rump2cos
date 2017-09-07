SRC = udpserv.c udpserv_stub.c
APP = udpserv
LIBS = paws 

#TODO: MOVE TO makefile.comp and tools too.
main : backing tools/backing.o all

backing :
	rm -f ./tools/backing.o
	cd ../../rumprun/platform/cos; make clean;

tools/backing.o :
	@ python ./tools/buildbacking.py

patch :
	# For all future patches to src-netbsd
	cp ./tools/full.patch ../../rumprun/src-netbsd
	cd ../../rumprun/src-netbsd; git apply --stat full.patch


include ../makefile.comp
