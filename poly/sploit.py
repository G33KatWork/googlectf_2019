#!/usr/bin/env python3

import socket
import base64

HOST = "poly.ctfcompetition.com"
#HOST = "127.0.0.1"
PORT = 1337

def ru(s, a):
	d = b""

	while not d.endswith(a):
		c = s.recv(4096)
		d += c
		assert(c)
		print(repr(d))

	return d

s = socket.socket()
s.connect((HOST, PORT))

#cp arm.bin lol.bin && dd if=/dev/zero of=lol.bin conv=notrunc oflag=append bs=1 count=$((1*1024*1024-$(stat --printf="%s" arm.bin))) && dd if=x86.bin of=lol.bin conv=notrunc oflag=append
code = open("lol.bin", "rb").read()
b64_code = base64.b64encode(code)

ru(s, b"Boot image (base64 encoded, limit 3MiB encoded): ")

s.sendall(b64_code + b"\n")

import telnetlib
t = telnetlib.Telnet()
t.sock = s
t.interact()
