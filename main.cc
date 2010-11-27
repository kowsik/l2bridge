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

int
main(int argc, const char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: l2bridge <if1> <if2>\n");
        return 1;
    }

    if (::geteuid() != 0) {
        fprintf(stderr, "bummer: You must be root!\n");
        return 1;
    }

    // Open the pipes on the two interfaces
    std::string ifn0 = argv[1];
    std::string ifn1 = argv[2];
    if (ifn0 == ifn1) {
        fprintf(stderr, "bummer: You are kidding right?\n");
        return 1;
    }

    const char *ifns[2] = { ifn0.c_str(), ifn1.c_str() };
    HalfPipe p1(ifns[0], 1);
    HalfPipe p2(ifns[1], 1);
    if (p1.open() < 0 || p2.open() < 0)
        return 1;

    HalfPipe *pipes[2] = { &p1, &p2 };
    uint8_t bytes[64*1024];
    uint64_t counts[2] = { 0, 0 };

    printf("bridging %s <=> %s\n", p1.ifname().c_str(), p2.ifname().c_str());

    // And bridge them. Packet in on 0, out on 1 and vice-versa
    while (true) {
        for (size_t n=0; n<2; ++n) {
            HalfPipe::Type type;
            int nread = pipes[n]->read(bytes, sizeof bytes, type);
            if (nread <= 0 || type == HalfPipe::Host)
                continue;
            (void) pipes[1-n]->write(bytes, nread);
            counts[n] += nread;

            printf("%s: %-40lld %lld: %s\r", ifn0.c_str(), counts[0], 
                counts[1], ifn1.c_str());
        }
    }

    return 0;
}

