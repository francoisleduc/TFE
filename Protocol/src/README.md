## General

This folder contains the source code of both the lambda server and fake_switch a.k.a client 

### Utils:
#### -- LinkedList.c_h
A double linked list implementation 

#### -- Common.c_h
A multitude of functions used both by the client and server 

#### -- Log.c_h
Essentially a log printer 

### Core:
#### -- Server.c
Lambda server code


#### -- Client.c_h
All parsing functions of the client. It was made that way (compared to server.c) because we'd need to 
integrate it with the current setup in XDP-Code/


#### -- FakeSwitch.c

This is a test client with only a main() with a loop + arguments parsing 
## License

LGPL-2.1 License 

[//]: # 
