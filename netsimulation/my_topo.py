#!/usr/bin/python

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI

print("Hello world")
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
        r0 = self.addNode('r0', cls=LinuxRouter, ip=None)
        r1 = self.addNode('r1', cls=LinuxRouter, ip=None)

        self.addLink(r0, r1, params1={'ip': '10.0.0.2/24'},
                      params2={ 'ip' : '10.0.0.3/24' })  

        h1 = self.addHost('h1', ip='10.0.0.100/24',
                          defaultRoute='via 10.0.0.1')
        h2 = self.addHost('h2', ip='192.168.0.100/24',
                          defaultRoute='via 192.168.0.1')
        


        
        self.addLink(h1, r0 , params2={ 'ip' : '10.0.0.1/24' }, params1={ 'ip' : '10.0.0.100/24' })
        
        
        self.addLink(h2, r1, params2={ 'ip' : '192.168.0.1/24' }, params1={ 'ip' : '192.168.0.100/24' })

       

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo,
                   waitConnected=True )  # controller is used by s1-s3
    net.start()


    net["r1"].cmd("ip route add 10.0.0.100 via 10.0.0.2 dev r1-eth0")
    net["r1"].cmd("ip route add 10.0.0.2 via 10.0.0.2 dev r1-eth0")
    net["r1"].cmd("ip route add 192.168.0.100 via 192.168.0.100 dev r1-eth1")


    net["r0"].cmd("ip route add 192.168.0.100 via 10.0.0.3 dev r0-eth0")
    net["r0"].cmd("ip route add 10.0.0.3 via 10.0.0.3 dev r0-eth0")
    net["r0"].cmd("ip route add 10.0.0.100 via 10.0.0.100 dev r0-eth1")
    

    #net["r2"].cmd("ip route add 10.0.0.100 via 10.0.1.2 dev r2-eth0")


    CLI( net )
    net.stop()


if __name__ == '__main__':
    setLogLevel( 'info' )
    run()



