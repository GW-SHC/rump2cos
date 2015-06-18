import subprocess

# Generate a .img that is half a meg in size, populate with 0's
subprocess.call(["dd", \
        "if=/dev/zero", \
        "iflag=fullblock", \
        "of=backing.img", \
        "bs=512", \
        "count=1024"])

#
