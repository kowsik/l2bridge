# --------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
#  pcapr wrote this file. As long as you retain this notice you can do whatever
#  you want with this stuff. If we meet some day, and you think this stuff is 
#  worth it, you can buy us a beer in return. 
# 
#  http://www.pcapr.net
#  http://twitter.com/pcapr
#  http://labs.mudynamics.com
#  --------------------------------------------------------------------------
default: l2bridge
	@true

l2bridge: halfpipe.o main.o
	g++ -Wall -Werror -g -O2 halfpipe.o main.o -o l2bridge -lpcap

clean:
	rm ./*.o
	rm ./l2bridge

halfpipe.o: halfpipe.cc halfpipe.h
	g++ -Wall -Werror -g -O2 -c halfpipe.cc -o halfpipe.o

main.o: main.cc halfpipe.h
	g++ -Wall -Werror -g -O2 -c main.cc -o main.o
