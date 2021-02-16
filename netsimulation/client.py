import socket

UDP_PORT = 10021
MESSAGE = b"EXEC 1" # Default message
UDP_IP = "10.0.0.3" # Lambda server must be there 

print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message sent: %s" % MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                    socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
msgFromServer = sock.recvfrom(1024)
msg = "Response from server: {}".format(msgFromServer[0])
print(msg)
