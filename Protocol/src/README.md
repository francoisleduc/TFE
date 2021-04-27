## General

This folder contains the source code of both the lambda server and fake_switch a.k.a client 

### Utils:
#### -- LinkedList.c_h
A double linked list implementation 

#### -- Common.h
A multitude of constants used both by the client and server 

#### -- Log.c_h
Essentially a log printer 

### -- json.hpp
Niels Lohmann JSON library for C++ under MIT License (2021)
Available @ https://github.com/nlohmann/json

### -- httplib.h
cpp-httplib is C++ library for HTTP requests under MIT License (2021)
Available @ https://github.com/yhirose/cpp-httplib

### Core:
#### -- Server.cpp_hpp
Lambda server class and related headers

### -- lambda.cpp
The lambda server implementation containing the main() function call

#### -- Client.c_h
All parsing functions of the client.
This part is integrated in XDP userspace program available in XDP-Code/ folder

#### -- FakeSwitch.c

This is a test client calling main() with argument parsing

### -- Makefile

```sh
make 
```
will generate both binaries "switch" and "lambda_server" and they will automatically be copied in the right places where it is used throughout this
repository.

It is worth noting that you can run 

```sh
make switch
```
or 

```sh
make lambda_server
```

to avoid both compilation of server and client each time

## License

LGPL-2.1 License 

[//]: # 
