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

#include "halfpipe.h"

#if defined(__APPLE__) && defined(__MACH__)
    #include <ifaddrs.h>
    #include <net/if_dl.h>
#elif defined(__linux)
#error "This OS not supported at the moment"
#endif

HalfPipe::HalfPipe(std::string ifname, clock_t timeout)
    :_ifname(ifname), _timeout(timeout)
{
    _pcap = 0;
}

HalfPipe::~HalfPipe()
{
    close();
}

/*
 * HalfPipe::_get_mac()
 * --------------------
 * Get the MAC address of the interface corresponding to this half pipe. We
 * need this to identify packets intended for 'this' host
 */
int
HalfPipe::_get_mac()
{
#if defined(__APPLE__) && defined(__MACH__)
    struct ifaddrs *ifap = 0;
    if (::getifaddrs(&ifap) < 0)
        return -1;

    for (struct ifaddrs *p = ifap; p; p = p->ifa_next) {
        if (_ifname == p->ifa_name && p->ifa_addr->sa_family == AF_LINK) {
            struct sockaddr_dl *sdp = (struct sockaddr_dl*) p->ifa_addr;
            ::memcpy(&_mac, sdp->sdl_data + sdp->sdl_nlen, sizeof _mac);
            break;
        }
    }

    ::freeifaddrs(ifap);
#else
#error "Dunno how to get MAC address of an interface on this OS"
#endif
    return 0;
}

/*
 * HalfPipe::open()
 * ----------------
 * Open the half pipe (before we can read/write)
 */
int
HalfPipe::open()
{
    if (_pcap) 
        return 0;

    if (_get_mac() < 0) {
        fprintf(stderr, "bummer: can't get mac for %s\n", 
            _ifname.c_str());
        return -1;
    }

    char eb[PCAP_ERRBUF_SIZE];
    _pcap = ::pcap_open_live(_ifname.c_str(), 65535, 1, _timeout, eb);
    if (_pcap == 0) {
        fprintf(stderr, "bummer: pcap_open_live(%s) failed\n\t %s\n",
            _ifname.c_str(), eb);
        return -1;
    }

    // Ignore packets from this host (just need the incoming ones)
    if (::pcap_setdirection(_pcap, PCAP_D_IN) < 0) {
        fprintf(stderr, "bummer: pcap_set_direction(%s) failed\n",
            _ifname.c_str());
        goto bummer;
    }

    return 0;
    
bummer:
    close();
    return -1;
}

void
HalfPipe::close()
{
    if (_pcap) {
        ::pcap_close(_pcap);
        _pcap = 0;
    }
}

/*
 * HalfPipe::read()
 * ----------------
 * Read a packet from the half pipe. 
 */
int
HalfPipe::read(u_char *bytes, size_t size, Type &type)
{
    if (_pcap == 0 || bytes == 0 || size == 0)
        return -1;

    struct pcap_pkthdr phdr;
    struct pcap_pkthdr *php = &phdr;
    const u_char *bptr = 0;
    int ret = ::pcap_next_ex(_pcap, &php, &bptr);
    if (ret == 0)
        return 0;

    // Truncated read
    if (ret < 0 || php->len < sizeof _mac) 
        return -1;

    int nread = std::min(size_t(php->len), size);
    ::memcpy(bytes, bptr, nread);

    const eth_addr_t broadcast = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    eth_addr_t dst;
    ::memcpy(&dst, bytes, sizeof dst);

    // Classify the packet based on the MAC address 
    if (::memcmp(&dst, &_mac, sizeof _mac) == 0) {
        type = Host;
    } else if (::memcmp(&dst, &broadcast, sizeof dst) == 0) {
        type = Broadcast;
    } else if (dst.addr[0] == 0x1) {
        type = Multicast;
    } else {
        type = Other;
    }

    return nread;
}

/*
 * HalfPipe::write()
 * -----------------
 * Write a packet through this half pipe into the interface
 */
int
HalfPipe::write(const u_char *bytes, size_t size)
{
    if (_pcap == 0 || bytes == 0 || size == 0)
        return -1;

    if (::pcap_inject(_pcap, bytes, size) != int(size))
        return -1;

    return 0;
}

