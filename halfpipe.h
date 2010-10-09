// --------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// pcapr wrote this file. As long as you retain this notice you can do whatever
// you want with this stuff. If we meet some day, and you think this stuff is 
// worth it, you can buy us a beer in return. 
//
// http://www.pcapr.net
// http://twitter.com/pcapr
// http://labs.mudynamics.com
// --------------------------------------------------------------------------
#ifndef _HALFPIPE_H_
#define _HALFPIPE_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <pcap.h>

typedef struct {
    u_char addr[6];
} eth_addr_t;

class HalfPipe {
public:
    enum Type {
        Host,
        Broadcast,
        Multicast,
        Other
    };

public:
    HalfPipe(std::string ifname, clock_t timeout);
    ~HalfPipe();

    std::string ifname() const { return _ifname; }

    int open();
    int read(u_char *bytes, size_t size, Type &type);
    int write(const u_char *bytes, size_t size);
    void close();

private:
    int _get_mac();

private:
    std::string _ifname;
    clock_t _timeout;
    struct pcap *_pcap;
    eth_addr_t _mac;
};

#endif /* _HALFPIPE_H_ */

