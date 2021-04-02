## Usage

First you have to run ``` make ``` in the ``` /http-counter``` folder.
Then ``` make copynet ``` to copy the executable into the ``` /netsimulation ``` folder

After that all the other commands will be run from the ``` /netsimulation ```

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
$ ./lambda_server 8080 0.0
```

And in a third terminal 

```sh
sudo ./xdp_load_and_stats --dev s1-eth1 
```
> Note: adding `--force` after the interface name will have to be used to force the new program to be uploaded in case there is already one

where **s1-eth1** is the interface name you want the kernel program to run on.

From now on, the output of the userspace xdp_load_and_stats should 
update every 2seconds with a refresh of what's been captured on that interface.

Sending pings or getting data from h1 will be captured

![Example](https://imgur.com/Hik4EeC.jpg)


You can also access bpf_printk() logs used for debugging or extra information

```sh
sudo cat /sys/kernel/debug/tracing/trace_pipe 
```

## License

LGPL-2.1 License 

[//]: # 
