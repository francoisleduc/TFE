## General
``` common_kern_user.h ``` 

Contains the definition of the data structure stored in BPF maps (interface statistics definition)

``` xdp_load_and_stats ``` 
This contains the source code of the user space program.
1) Loads the kernel program on an interface
2) Runs its user-space functionnalities (polling BPF maps, displaying them on screen, sending events to lambda server every now and then)

``` xdp_prog_kern ``` 
Increments values in BPF maps such as the number of bytes, the number of syn+ack, the number of tcp payload bytes, the number of packets for each arriving packet on the interface it is loaded on

#### Usage

First you have to run ``` make ``` 
Then it is advised to run ``` make copynet ``` to copy the executable and object files into the ``` /netsimulation ``` folder to test it on a small network topology.

```sh
sudo ./xdp_load_and_stats -n deviceid -a sourceipaddr -l lambdaip -p lambdaportnb --dev interface-name
```
> Note: adding `--force` after the interface name will have to be used to force the new program to be uploaded in case there is already one
> Note: The device id must be an integer and should be unique among the other devices
> Note: The ip address should have this format: x.x.x.x
> Note: lambdaportnb is the port number of the lambda server that the device will use to send the events

where **interface-name** is the interface name you want the kernel program to run on. 

## License

LGPL-2.1 License 

[//]: # 
