#!/bin/bash

winname=$(powershell.exe '$env:Username')
winname=${winname%$'\r'}

rm -f /mnt/c/Users/$winname/vmlinux

cd

# installa bc e pahole per step successivo
sudo apt -y install pahole bc

# the old update routine
sudo apt update -y
# add tools to build kernel, apologies if i missed anything as i already have a bunch of dev stuff setup
sudo apt install -y autoconf  bison build-essential flex libelf-dev libncurses-dev libssl-dev libtool libudev-dev

# get kernel version
version=$(uname -r)

# extract substring
version=${version%%-*}

# get matching version
wget https://github.com/microsoft/WSL2-Linux-Kernel/archive/refs/tags/linux-msft-wsl-$version.tar.gz

# extract it (change to your kernel version)
tar -xf linux-msft-wsl-$version.tar.gz

# build linux/modules

# change to the folder tar extracted to
cd  WSL2-Linux-Kernel-linux-msft-wsl-$version/
cat /proc/config.gz | gunzip > .config
yes "" | make prepare modules_prepare -j $(expr $(nproc) - 1)

# select vcan and can subsystems here
./scripts/config -m CONFIG_CAN
./scripts/config -m CONFIG_CAN_DEV
./scripts/config -m CONFIG_CAN_VCAN
./scripts/config -m CONFIG_CAN_RAW
yes "" | make modules -j $(expr $(nproc) - 1)
sudo make modules_install
make -j $(expr $(nproc) - 1)
sudo make install

# copy it to windows drive
cp vmlinux /mnt/c/Users/$winname/

# cleanup
cd ..
rm -rf WSL2-Linux-Kernel-linux-msft-wsl-$version
rm linux-msft-wsl-$version.tar.gz

# create this file, change /<yourwindowsloginname> to your windows users directory name
sudo touch /mnt/c/Users/$winname/.wslconfig

# with these contents
echo "[wsl2]" >> /mnt/c/Users/$winname/.wslconfig
echo "kernel=C:\\\\Users\\\\$winname\\\\vmlinux" >> /mnt/c/Users/$winname/.wslconfig

# Make relevant CAN modules autoloading
can_modules="
vcan
can
can-raw
can_dev
"
sudo sh -c "echo ${can_modules} > /etc/modules-load.d/socketcan.conf"

# exit wsl
exit


