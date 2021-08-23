## General
This directory is dedicated to my master thesis on "Lambda functions for network control and monitoring". 

Is is structured in three folders representing the 3 modules 

```sh
/XDP-Code 
```

and
```sh
/Protocol
```

and 

```sh
/Kubernetes
```


The first one contains eBPF programs that aim to collect information on flows. It makes the use of BPF maps.

The second one focuses on the development of a (programmable device) client -> (lambda) server protocol. The client wants to send a multitude of events to the server, each with a unique identifier.

Kubeless is a Kubernetes-native serverless framework that lets you deploy small bits of code (functions) without having to worry about the underlying infrastructure. This folder will contain the functions we want to be deployed in the cluster as well as all Kubernetes services deployed.

## General Installation Steps

For individual modification and development please follow specific readme instructions provided in each folder.


...


## License

LGPL-2.1 License 

[//]: # 
