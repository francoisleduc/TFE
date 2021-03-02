import socket

import threading
import time


def lambda_f1():
	time.sleep(5)
	print("Lambda function number 1")
	# Do your thing here 
	time.sleep(1)



# Dictionnary of lambda functions ? Number(key) -> address function call 
Dict = {}
Dict[1] = lambda_f1





UDP_PORT = 10021 # random port high enough

sock = socket.socket(socket.AF_INET, # Internet
         socket.SOCK_DGRAM) # UDP
sock.bind(('', UDP_PORT))


MESSAGE = b"received lambda query"

while True:
	data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
	print("Server received: %s" % data)


	if(data == "EXEC 1"):
		t = threading.Thread(name='child procs', target=Dict[1]) # create thread to execute lambda function 
		t.start()
	
	sock.sendto(MESSAGE, addr) # Send back to client a dummy response (for now)


