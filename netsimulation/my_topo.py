#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI
import os

class LinuxRouter( Node ):
    "A Node with IP forwarding enabled."
    def config( self, **params ):
        super( LinuxRouter, self).config( **params )
        self.cmd( 'sysctl net.ipv4.ip_forward=1' )

    def terminate( self ):
        self.cmd( 'sysctl net.ipv4.ip_forward=0' )
        super( LinuxRouter, self ).terminate()

class NetworkTopo( Topo ):
    "A LinuxRouter connecting three IP subnets"

    # pylint: disable=arguments-differ
    def build( self, **_opts ):

        defaultIP = None  # IP address for r0-eth1

    
        h1 = self.addHost('h1', ip='10.0.0.1/24')
        h2 = self.addHost('h2', ip='10.0.0.2/24')
        h3 = self.addHost('h3', ip='10.0.0.3/24')
         
        s1 = self.addSwitch('s1')
        self.addLink(s1, h1)
        self.addLink(s1, h2)
        self.addLink(s1, h3)

       

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()


    #net["r1"].cmd("ip route add 10.0.0.100 via 10.0.0.2 dev r1-eth0")
    #net["r1"].cmd("ip route add 10.0.0.2 via 10.0.0.2 dev r1-eth0")
    #net["r1"].cmd("ip route add 192.168.0.100 via 192.168.0.100 dev r1-eth1")


    #net["r0"].cmd("ip route add 192.168.0.100 via 10.0.0.3 dev r0-eth0")
    #net["r0"].cmd("ip route add 10.0.0.3 via 10.0.0.3 dev r0-eth0")
    #net["r0"].cmd("ip route add 10.0.0.100 via 10.0.0.100 dev r0-eth1")
    

    #net["r2"].cmd("ip route add 10.0.0.100 via 10.0.1.2 dev r2-eth0")
    net.pingAll()
    net["h1"].cmd('python -m SimpleHTTPServer 999 &')
    print("Launching web server on h1 (10.0.0.1:999) ... ")
    net["h2"].cmd("wget 10.0.0.1:999 -O data")
    print("GET request sent from h2 to h1")


    net["h3"].cmd('python lambda_server.py &')
    print("Launching UDP Lambda Server on h3 (10.0.0.3:10021)")

    net["h2"].cmd('python client.py &')
    print("H2 Sending UDP packets to lambda server to test it out")
    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()


