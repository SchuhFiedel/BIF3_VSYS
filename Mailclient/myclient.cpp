/* myclient.cpp */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#define BUF 1024
#define PORT 6543

int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  if( argc < 3 ){
     printf("Usage: %s ServerAdresse PortNumber\n", argv[0]);
     exit(EXIT_FAILURE);
  }
  /*   DEBUGSTUFF
  for(int i = 0; i < argc; i++){
    std::cout << "arv" << i << ": " << argv[i] <<"\n";
  }
  */

  if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }

  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(atoi(argv[2])); //Use Port from Input (convert from char* to int)
  inet_aton (argv[1], &address.sin_addr); //convert input IP to usable IP adress

  std::cout << "using ip: " << inet_ntoa(address.sin_addr) << " Port " << htons(address.sin_port) << "\n";

  if (connect (create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
     size=recv(create_socket,buffer,BUF-1, 0);
     if (size>0)
     {
        buffer[size]= '\0';
        printf("%s",buffer);
     }
  }
  else
  {
     perror("Connect error - no server available");
     return EXIT_FAILURE;
  }

  do {
     printf ("Send message: ");
     fgets (buffer, BUF, stdin);
     send(create_socket, buffer, strlen (buffer), 0);
  }
  while (strcmp (buffer, "quit\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}
