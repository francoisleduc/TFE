import socket

UDP_PORT = 10021

sock = socket.socket(socket.AF_INET, # Internet
         socket.SOCK_DGRAM) # UDP
sock.bind(('', UDP_PORT))


MESSAGE = b"received lambda query"

while True:
	data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
	print("received message: %s" % data)
	sock.sendto(MESSAGE, addr)
