echo "In order to proceed building Free95 for i686, please enable sudo mode:"
sudo echo "Sudo mode enabled! Proceeding with build."

export PATH="/home/kap/Downloads/bin:$PATH"
make all
qemu-system-i386 -drive format=raw,file=./bin/os.bin -serial stdio
