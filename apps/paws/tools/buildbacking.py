import subprocess
import os

# Generate a .img that is half a meg in size, populate with 0's
subprocess.call(["dd", \
        "if=/dev/zero", \
        "iflag=fullblock", \
        "of=backing.img", \
        "bs=512", \
        "count=1024"])

# Execute mkfs.ext2 on .img
subprocess.call(["mkfs.ext2", "backing.img"])

# objdump the new ext2 image into a .o and move it to the right directory
subprocess.call(["objcopy", "-I", "binary", "-O", "elf32-i386", "-B", "i386", "backing.img", "backing.o"])
subprocess.call(["cp", "backing.o", "../../rumprun/lib/libbmk_rumpuser/hw"])

# Compile the src
os.chdir("../../rumprun/")
os.system("./build-rr.sh hw -- -F ACFLAGS=-m32")
