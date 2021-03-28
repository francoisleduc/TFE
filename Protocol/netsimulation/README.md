## General
In the architecture of the system routers and switches must be able to send events (formatted) to lambda servers on the network. These events must be then read by the server and eventually run lambda functions associated to the events received. 

In order to test the protocol you will find here the two executables "switch" and "lambda_server" that act as either a client or a server and exchange messages.

The idea is to first test this protocol here to then deploy this protocol on the switch (located in XDP-Code/http-counter folder) once it is approved and working

## Usage 

#### Switch

To launch the client that will communicate with a server **@ip\_addr\_of\_server** which is listening on port **@server_port**
```sh
./switch ip_addr_of_server server_port 
```
In our case we are using 127.0.0.1 for the address and port number 8080

The following command is used during simulations

```sh
./switch 127.0.0.1 8080
```

#### Lambda server

The lambda server exectuable will be the lambda server code deployed in the network for our lambda server(s) and can be launched via the following command 


```sh
./lambda_server port packet_drop_rate
```

where the packet\_drop\_rate is a custom dev parameter to test the protocol when packets are dropped on the network. This way we can look for loopholes or simply check the behavior of the server according to the number of packets dropped.

For a standard usage the following command is used:

```sh
./lambda_server 8080 0.05
```

> Note: `./lambda_server 8080 0.0` will act as if there was absolutely no packet loss on the network 


## License

LGPL-2.1 License 

[//]: # 
