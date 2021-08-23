## General
This folder is dedicated to both kernel and user space programs that will run on a programmable switch.
You will find 5 directories:

```sh
/dependencies
```
Contains all linux headers needed, libbpf library, and a set of common functions/parameters/constants used for the eBPF/BPF programs themselves

```sh
/flow-stats
```
Contains user & kernel programs as well as the XDP loader source codes for the detection of heavy-hitters since it is dedicated to gather flow related data

```sh
/port-scan-stats
```

Contains user & kernel programs as well as the XDP loader source codes for the detection of port scanning attacks since it is dedicated to gather tcp syn,rst flags and empty udp datagrams data

```sh
/Protocol
```
Contains the protocol pushed from the original /Protocol folder at the base of the branch.

```sh
/netsimulation
```
Is essentially everything related to the network environment in which we test basic functionnalities with a Mininet network topology

## License

LGPL-2.1 License 

[//]: # 
