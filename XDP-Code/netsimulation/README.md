## Usage

First make sure you have execute the ``` make ``` command in either /flow-stats or /port-scan-stats folder depending on what XDP program you want to test.
Then ``` make copynet ``` to copy the executable into the ``` /netsimulation ``` folder to make them available to the simulation environment.

After that all the other commands will be run from the current ``` /netsimulation ``` folder.

By running on a first terminal
```sh
$ sudo ./my_topo.py
```
It creates a simple mininet network topology with one switch and two hosts.
Links: (s1, h1) (s1, h2)

On host h1, there is a web server running on 10.0.0.1:999
From h2 you can make requests for data file by running 

```sh
$ h2 wget 10.0.0.1:999 -O data
```
in mininet

In second terminal you want to launch the lambda_server

```sh
$ ./lambda_server 127.0.0.1 8000 0.0 1000
```

And in a third terminal 

```sh
sudo ./xdp_load_and_stats -n deviceid -s sourceipaddr -l lambdaip -p lambdaportnb --dev interface-name
```
> Note: The device id must be an integer and should be unique among the other devices
> Note: The ip address should have this format: x.x.x.x
> Note: lambdaportnb is the port number of the lambda server that the device will use to send the events
> Note: adding `--force` after the interface name will have to be used to force the new XDP program to be uploaded in case there is already one

where **s1-eth1** is the interface name you want the kernel program to run on.

From now on, the output of the userspace xdp_load_and_stats_X should 
update every 2seconds with a refresh of what's been captured on that interface.

Sending pings or getting data from h1 will be captured

You can also access bpf_printk() logs used for debugging or extra information by using the 

```sh
sudo cat /sys/kernel/debug/tracing/trace_pipe 
```

command

## License

LGPL-2.1 License 

[//]: # 
