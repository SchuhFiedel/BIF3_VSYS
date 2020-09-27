/* myserver.cpp */
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




#define BUF 1024
#define PORT 6543

using namespace std;




int main (int argc, char **argv) {
  int create_socket, new_socket;
  socklen_t addrlen; //int 32 bit
  char buffer[BUF];   //string with 1024 characters
  int size;
  struct sockaddr_in address, cliaddress; //makes two structs with adress, port and adress_family (AF_INET)

  if( argc < 2 ){
     printf("Usage: %s PortNumber MailFolder (PATH)\n", argv[0]);
     exit(EXIT_FAILURE);
  }
  char * mailpath = argv[2];

  create_socket = socket (AF_INET, SOCK_STREAM, 0);

  memset(&address,0,sizeof(address)); //fill with 0
  address.sin_family = AF_INET;           // I HAVE NO FUCKIN IDEA WHAT IS HaPPENING HERE
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(argv[1]));


/*     bind() assigns
       the address specified by addr to the socket referred to by the file
       descriptor sockfd.  addrlen specifies the size, in bytes, of the
       address structure pointed to by addr.  Traditionally, this operation
       is called “assigning a name to a socket”.*/

  if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
     perror("bind error");
     return EXIT_FAILURE;
  }
  listen (create_socket, 5); //Socket to listen to and maximal connection Queue lenght

  addrlen = sizeof (struct sockaddr_in);

  bool end = false;

  while (end == false) {
     std::cout << "Waiting for Connection on: IP: " << inet_ntoa(address.sin_addr) << " Port: " << htons(address.sin_port) << "\n";
     new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
     if (new_socket > 0)
     {
        printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr), ntohs(cliaddress.sin_port));
        strcpy(buffer,"Welcome to myserver, Please enter your command:\n");
        send(new_socket, buffer, strlen(buffer),0);
     }
     do {
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size > 0)
        {
           buffer[size] = '\0';

//Defining Stuff that has to be checked
           char SEND = 'S';
           char LIST = 'L';
           char READ = 'R';
           char DEL = 'D';
           char QUIT = 'Q';
           if(buffer[0] == SEND)
           {
                std::cout<<"Send received";
           }else if(buffer[0] == LIST)
           {
                std::cout<<"LIST received";
           }else if(buffer[0] == READ)
           {
                std::cout<<"READ received";
           }else if(buffer[0] == DEL)
           {
                std::cout<<"DEL received";
           }else if(buffer[0] == QUIT)
           {
                close (new_socket);
                end = true;
           }


           printf ("Message received: %s\n", buffer);


//Create Direcotry with buffer name -- later uSERNAME
            std::string pathPartOne(mailpath);
            std::string pathpartTwo(buffer);
            std::string path = pathPartOne + "/" + pathpartTwo;
           if(mkdir(path.c_str(), 0777) == -1){
             std::cerr << "Error :  " << strerror(errno) << std::endl;
           }
           else{
             std::cout << "Directory "<< buffer <<" created";
           }


        }
        else if (size == 0)
        {
           printf("Client closed remote socket\n");
           break;
        }
        else
        {
           perror("recv error");
           return EXIT_FAILURE;
        }
     } while (strncmp (buffer, "quit", 4)  != 0);
     close (new_socket);
  }
  close (create_socket);
  return EXIT_SUCCESS;
}
