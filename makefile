all: server client
	@echo "boopie!"


server: Mailserver/myserver.cpp
	@echo "Making Server Vsys 1 - myserver.c binary!"
	g++  -pthread Mailserver/myserver.cpp -o Mailserver/myserver.o

client: Mailclient/myclient.cpp
	@echo "Making Client Vsys1 - myclient.c binary!"
	g++  -pthread Mailclient/myclient.cpp -o Mailclient/myclient.o

clean:
	rm Mailclient/*.o 
	rm Mailserver/*.o
