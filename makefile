main: server client
	@echo "boopie!"


server: Mailserver/myserver.c
	@echo "Making Server Vsys 1 - myserver.c binary!"
	gcc  Mailserver/myserver.c -o Mailserver/myserver

client: Mailclient/myclient.c
	@echo "Making Client Vsys1 - myclient.c binary!"
	gcc  Mailclient/myclient.c -o Mailclient/myclient
