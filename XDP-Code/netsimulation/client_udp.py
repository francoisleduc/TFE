import socket

msgFromClient       = ""
bytesToSend         = str.encode(msgFromClient)
serverAddressPort   = ("10.0.0.1", 12345)
bufferSize          = 1024
# Create a UDP socket at client side

UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# Send to server using created UDP socket

UDPClientSocket.sendto(bytesToSend, serverAddressPort)

msgFromServer = UDPClientSocket.recvfrom(bufferSize)

msg = "Message from Server {}".format(msgFromServer[0])

print(msg)
