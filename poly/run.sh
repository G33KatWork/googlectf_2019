#!/bin/bash

qemu-system-arm -nographic -machine virt-2.8 -net nic -net "user,net=169.254.0.0/16,host=169.254.169.253,guestfwd=tcp:169.254.169.254:80-cmd:netcat 127.0.0.1 1234" -bios lol.bin

#qemu-system-x86_64 -nographic -net nic -net "user,restrict=on,net=169.254.0.0/16,host=169.254.169.253,guestfwd=tcp:169.254.169.254:80-cmd:netcat 127.0.0.1 1234" -bios lol.bin
