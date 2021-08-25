## General
This directory is dedicated to my master thesis on "Lambda functions for network control and monitoring". 

It is structured in three folders representing the 3 modules 

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

# General Installation Steps

For individual modification and development please follow specific readme instructions provided in each folder.
Since the code was only tested on Ubuntu 18.04 here are the dependencies and installation steps required.

## Ubuntu 18.04

```sh
$ sudo apt install clang llvm libelf-dev libpcap-dev gcc-multilib build-essential
$ sudo apt install g++ gcc make
$ sudo apt install curl
```
to install clang and llvm dependencies as well as C and C++ compiler (gcc and g++) if not already installed by default.


##### Mininet
Then make sure that mininet 2.3.0 is installed


```sh
$ git clone git://github.com/mininet/mininet
```

The above git command will check out the latest version. If you want to use any other version you may check that version out explicitly

```sh
$ cd mininet
$ git tag
$ git checkout -b mininet-2.3.0 2.3.0
$ cd ..
```

and install the full package 

```sh
$ mininet/util/install.sh -a
```

##### Compilation

For the following steps you will need to go in either /XDP-Code/flow-stats or /XDP-Code/port-scan-stats depending on the eBPF program that is of interest. Compilations steps are very similar

Once in the right directory execute
```sh
$ make 
```
to compile both user space programs and XDP/eBPF programs. It must be followed by either
 ``` $ make flow ``` or  ``` $ make port ```
to push the executable in the /netsimulation environement in which these are going to be used. If this last command is not used only the previous version of the executable will be used during the network simulation test environement.


As for the lambda server compilation, go in the /TFE/Protocol/src/ and execute
```sh
$ make 
```

followed by 
```sh
$ make push
```

##### Netsimulation

Launch the mininet network topology in a first terminal 

```sh
$ sudo ./my_topo.py
```
It creates a simple mininet network topology with one switch and two hosts. Links: (s1, h1) (s1, h2)

In second terminal you want to launch the lambda_server
```sh
$ ./lambda_server 127.0.0.1 8000 0.0 1000
```

The following steps require the user to change the _X varible by either "flow" or "port" depending what program you're focusing on. 
In a third terminal,
```sh
$ sudo ./xdp_load_and_stats_X -n 200 -s 127.0.0.1 -l 127.0.0.1 -p 8000 --dev s1-eth1
```
> Note: The device id (-n) must be an integer and should be unique among the other devices
> Note: The ip address should have this format: x.x.x.x and does not really matter in this context
> Note: lambdaportnb (-p) is the port number of the lambda server that the device will use to send the events
> Note: adding `--force` after the interface name will have to be used to force the new XDP program to be uploaded in case there is already one or if you decided to recompile and re-upload the program.

where **s1-eth1** is the interface name you want the kernel program to run on.

From now on, the output of the userspace xdp_load_and_stats_X should 
update every 2seconds with a refresh of what's been captured on that interface.


Back to the mininet terminal
On host h1, there is a web server running on 10.0.0.1:999 . From h2 you can make requests for data file by running
```sh
$ h2 wget 10.0.0.1:999 -O data
```
this will have for effect to create new BPF map entries (new flows caught by the program). These events are printed out in terminal 3. They should also appear on terminal 2 at the server end.

For additional details you might look at debug logs by accessing the debug file:

```sh
$ sudo cat /sys/kernel/debug/tracing/trace_pipe 
```

##### Kubernetes

For now the lambda server is limited to receiving events and parsing them (lambda function http triggers are commented lines in the code ). This was made so that any user that would find trouble to run Kubernetes could at least test the first part of the pipeline of this project.

If you wish to use the full project you need to uncomment line 204 and line 227 of Server.cpp in /XDP/Protocol/src
and follow compilation step regarding the lambda server and reloading it.

-  You will need to install docker 20.10.7 see https://www.docker.com
-  You will need to install kubectl 1.16

```sh
$ curl -LO https://dl.k8s.io/release/v1.16.0/bin/linux/amd64/kubectl
$ sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl
```
- Install Minikube 1.22

```sh
$ curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
$ sudo install minikube-linux-amd64 /usr/local/bin/minikube
```

- Run Minikube

```sh
$ minikube start
```
Then you must install the dashboard UI which is not installed by default.
After that you might need to create an admin account for your cluster and login to acces it.

```sh
$ kubectl proxy
```

- Install Kubeless

```sh
$ kubectl create ns kubeless
```

```sh
$ kubectl create -f https://github.com/kubeless/kubeless/releases/download/v1.0.8/kubeless-v1.0.8.yaml
```

Deploy both functions located in /TFE/Kubernetes/flow-heavy-hitter-detection and /TFE/Kubernetes/port-scan-kubeless respectively

```sh
$ kubeless function deploy heavyhitter --runtime nodejs10 --handler heavyhitter.heavyhitter --dependencies package.json --from-file heavyhitter.js

$ kubeless function deploy port-scan --runtime nodejs10 --handler port-scan.portscan --dependencies package.json --from-file port-scan.js
```

Make sure they are correctly deployed
```sh
$ kubeless function ls
```


- Install MongoDB

These steps must be done from /TFE/Kubernetes/mongodb-kubernetes

```sh
$ kubectl apply -f mongod-service.yaml
$ kubectl apply -f mongo-statefulset.yaml
```

and wait for all pods to be running.

Now replicas must be configured:


```sh
kubectl exec -it mongod-0 -- mongo
```
(inside in interractive mode)

```sh
rs.initiate()
var cfg = rs.conf()
cfg.members[0].host="mongod-0.mongodb-service:27017"
rs.reconfig(cfg)
rs.status() 
```

(should be alone no sync to others)

Then,
```sh
rs.add("mongod-1.mongodb-service:27017")
rs.add("mongod-2.mongodb-service:27017")
rs.status()

```

mongo replicas should be up and running.




## License

LGPL-2.1 License 

[//]: # 
