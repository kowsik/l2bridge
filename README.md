# l2bridge
It is a libpcap based user-mode layer2 bridge. You need the latest version of
[libpcap](http://www.tcpdump.org/#latest-release) to compile this.

# What do I use this for?
I originally wrote it so I can bridge en1 and en0 on my MacBook to capture
iPhone traffic without jail-breaking, installing access points, hubs and other
complex setups.

# How do I build this
Just type `make` and you are done.

# And running?
	sudo ./l2bridge en1 en0

# How does it work?
It capture packets on both en1 and en0 and copies them back and forth. Any
packets destined to the interface MAC address (i.e. to the host running
l2bridge) are skipped.

# What OS does this work on?
Currently just OSX. The only OS dependency is in the `HalfPipe::_get_mac`
method which extracts the MAC address from the interface. Linux has an
`ioctl(SIOCGIFHWADDR)` call that we could use, but haven't done it yet.

# License
"THE BEER-WARE LICENSE" (Revision 42):

pcapr wrote this file. As long as you retain this notice you can do whatever
you want with this stuff. If we meet some day, and you think this stuff is 
worth it, you can buy us a beer in return. 

[http://www.pcapr.net](http://www.pcapr.net)
[http://twitter.com/pcapr](http://twitter.com/pcapr)
[http://labs.mudynamics.com](http://labs.mudynamics.com)
