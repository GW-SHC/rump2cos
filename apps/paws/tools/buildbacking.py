import subprocess
import os

# Start in the right directory
os.chdir("./tools")

# Generate a .img that is half a meg in size, populate with 0's
subprocess.call(["dd", \
        "if=/dev/zero", \
        "iflag=fullblock", \
        "of=data.iso", \
        "bs=512", \
        "count=1024"])

# Execute mkfs.ext2 on .img
subprocess.call(["mkfs.ext2", "data.iso"])

# objdump the new ext2 image into a .o and move it to the right directory
subprocess.call(["objcopy", "-I", "binary", "-O", "elf32-i386", "-B", "i386", "data.iso", "backing.o"])
subprocess.call(["cp", "backing.o", "../../../rumprun/lib/libbmk_rumpuser/cos"])

# Compile the src
os.chdir("../../../rumprun/")
#os.system("./build-rr.sh cos -- -F ACFLAGS=-m32")
os.system("./build-rr.sh cos")
