main: client server
	@echo "boopie!"


server: myserver.c
	@echo "Making Server Vsys 1 - myserver.c binary!"
	gcc  ./myserver.c -o myserver

client: myclient.c
	@echo "Making Client Vsys1 - myclient.c binary!"
	gcc  ./myclient.c -o myclient
