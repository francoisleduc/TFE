## General
This folder is dedicated to both kernel and user space programs that will run on a programmable switch. The idea is to have a program that will "run on" an interface of the NIC and that will count and save a number of measures in kernel memory about the network traffic entering on that interface. The user space program will run at the same time on top of it and is dedicated to polling the measures stored in a repetitive manneer to decide whether or not it should trigger a lambda and send its ID to a remote lambda server for further execution.

You will find three directories:

```sh
/dependencies
```
Contains all linux headers needed, libbpf library, and a set of common functions/parameters/constants used for the programs themselves

```sh
/http-counter
```
Contains user & kernel programs as well as the XDP loader source codes 

```sh
/netsimulation
```
Is essentially the network environement in which we test basic functionnalities with a simple network topology

## License

LGPL-2.1 License 

[//]: # 
