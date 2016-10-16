SRC =
APP = nginx
LIBS = paws
RUNCONFS =  -T ../temp qemu -M 128 -i -b images/data.iso,/data -I if,vioif,'-net tap,script=no,ifname=tap0' -W if,inet,static,10.0.120.101/24 --
RUNPOST  = -c /data/conf/nginx.conf

main : copy backing run

include ../makefile.comp

copy : compile
	cp -r ../../rumprun-packages/nginx/images ./
	cp ../../rumprun-packages/nginx/bin/nginx nginx

compile :
	./../../rumprun-packages/nginx/runmake.sh

backing :
	-rm tools/data.iso
	-rm tools/backing.o
	cd ../../rumprun/platform/cos; make clean;
	python ./tools/buildbacking.py

run : bake
	$(BAKE) $(LIBS) $(APP).bin $(APP).out
	$(RUN) $(RUNCONFS) $(APP).bin

bake :
	cp nginx nginx.out

