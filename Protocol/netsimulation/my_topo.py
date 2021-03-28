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

    
        h1 = self.addHost('h1', ip='10.0.0.1/24', mac='00:00:00:00:00:01')
        h2 = self.addHost('h2', ip='10.0.0.2/24', mac='00:00:00:00:00:02')
         
        s1 = self.addSwitch('s1')
        self.addLink(s1, h1)
        self.addLink(s1, h2)


       

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()

    net.pingAll()
    

    net["h1"].cmd('python -m SimpleHTTPServer 999 &')
    print("Launching web server on h1 (10.0.0.1:999) ... ")
    #net["h2"].cmd("wget 10.0.0.1:999 -O data")
    print("Use $> h2 wget 10.0.0.1:999 -O data")


    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()


