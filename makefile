all: server client
	@echo "boopie!"


server: Mailserver/myserver.cpp
	@echo "Making Server Vsys 1 - myserver.c binary!"
	g++  -std=c++11 -Wall -pthread Mailserver/myserver.cpp -o Mailserver/myserver.o -lldap -llber

client: Mailclient/myclient.cpp
	@echo "Making Client Vsys1 - myclient.c binary!"
	g++  Mailclient/myclient.cpp -o Mailclient/myclient.o

clean:
	rm Mailclient/*.o
	rm Mailserver/*.o
