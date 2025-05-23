#!/usr/bin/env python3

import socket

HOST = '192.168.178.10'  # Standard loopback interface address (localhost)
PORT = 7        # Port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'Hello, world')
    data = s.recv(1024)

print('Received', repr(data))

