import subprocess
import os

# Start in the right directory
os.chdir("./tools")

# Copy data.iso into this directory
subprocess.call(["cp", "../images/data.iso", "."])

# Objdump the new ext2 image into a .o and move it to the right directory
subprocess.call(["objcopy", "-I", "binary", "-O", "elf32-i386", "-B", "i386", "data.iso", "backing.o"])
subprocess.call(["cp", "backing.o", "../../../rumprun/lib/libbmk_rumpuser/cos"])

# Compile the src
os.chdir("../../../rumprun/")
os.system("./build-rr.sh cos -- -F ACFLAGS=-m32")
#os.system("./build-rr.sh cos")
