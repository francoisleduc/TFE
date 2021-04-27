## General
This directory is dedicated to my master thesis on "Lambda functions for network control and monitoring". 

Until now you can find three folders 


```sh
/XDP-Code 
```

and
```sh
/Protocol
```

and 

```sh
/Kubeless 
```


The first one focuses on a program that counts packets, bytes at a network interface of a programmable device in an efficient way. The data collected is then sent in the form of events to a lambda server.

The second one focuses on the development of a (programmable device) client -> (lambda) server protocol. The client wants to send a multitude of events to the server, each with a unique identifier. 

Kubeless is a Kubernetes-native serverless framework that lets you deploy small bits of code (functions) without having to worry about the underlying infrastructure. This folder will contain the functions we want to be deployed in the cluster.

## License

LGPL-2.1 License 

[//]: # 
