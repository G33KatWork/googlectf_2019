#!/bin/bash
git clone https://gitlab.denx.de/u-boot/u-boot.git
cd u-boot
git checkout bdf97b5d393fc94666a847e9bac1c358b2c63c59
git am ../patches/*.patch

git clean -xdf && ARCH=arm make qemu_arm_defconfig && ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make -j32
cp u-boot.bin ../arm.bin

git clean -xdf && make qemu-x86_64_defconfig && make -j32
cp u-boot.rom ../x86.bin

cd ..
cp arm.bin lol.bin && dd if=/dev/zero of=lol.bin conv=notrunc oflag=append bs=1 count=$((1*1024*1024-$(stat --printf="%s" arm.bin))) && dd if=x86.bin of=lol.bin conv=notrunc oflag=append
