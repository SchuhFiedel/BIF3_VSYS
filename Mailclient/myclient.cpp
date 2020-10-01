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
#include <cstring>
#define BUF 1024
#define PORT 6543


void sendMessage(int socket, char * buffer);
void printMenu();
void menu(char * buffer, int socket);

void sendMail(int socket);
void listMails(int socket);
void readMail();
void deleteMail();
void logout();

std::string global_username;

int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  if( argc < 4 ){
     printf("Usage: %s ServerAdresse PortNumber Username\n", argv[0]);
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

  //std::cout << "using ip: " << inet_ntoa(address.sin_addr) << " Port " << htons(address.sin_port) << "\n";

  if (connect (create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
     std::string username(argv[3]);
     global_username = username;
       sendMessage(create_socket, argv[3]); //send Uername to Server

     size=recv(create_socket,buffer,BUF-1, 0);
     if (size>0)
     {
        buffer[size]= '\0';         //READ WHAT THE SERVER SENT US
        printf("%s",buffer);
     }

  }
  else
  {
     perror("Connect error - no server available");
     return EXIT_FAILURE;
  }

  do {
/************** MAIN LOOP ********************/

    printMenu();
    menu(buffer, create_socket);

  }
  while (strcmp (buffer, "quit\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}

/***FUNCTIONS***/

void sendMessage(int socket, char * message){
  //fgets (buffer, BUF, stdin);//save stdin into buffer with max buffer size
  //printf ("Sending message: {%s}", buffer);
  //fgets (message, BUF, stdin);
  send(socket, message, strlen (message), 0);  // send buffer
}

void printMenu(){
  std::cout << "**************************************************************************************************" << std::endl;
  std::cout << "1) SEND: Senden einer Nachricht." << std::endl;
  std::cout << "2) LIST: Auflisten der Nachrichten. Anzahl der Nachrichten die Betreff Zeile anzeigen." << std::endl;
  std::cout << "3) READ: Anzeigen einer bestimmten Nachricht." << std::endl;
  std::cout << "4) DEL : Löschen der Nachricht." << std::endl;
  std::cout << "5) QUIT: Logout." << std::endl;
  std::cout << "Please Enter the Number before the Option!" << std::endl;
  std::cout << "**************************************************************************************************" << std::endl;
}

void menu(char * buffer, int socket){
    fgets (buffer, BUF, stdin);
    //std::cout<<buffer[0];
    int input = atoi(buffer);
    //std::cout<<input;
    switch(input){
      case 1:
          system("clear");
          std::cout << "SEND" << std::endl;
          sendMail(socket);
        break;
      case 2:
          system("clear");
          std::cout << "LIST" << std::endl;
          listMails(socket);
        break;
      case 3:
          system("clear");
          std::cout << "READ" << std::endl;
          //readMail();
        break;
      case 4:
          system("clear");
          std::cout << "DEL" << std::endl;
          //deleteMail();
        break;
      case 5:
          std::cout << "QUIT" << std::endl;
          close (socket);
          exit(0);
          //logout();
        break;
      default:
          system("clear");
          std::cout << "Bäh" << std::endl;
        break;
    }
}

void sendMail(int socket){


  sendMessage(socket, "S"); // wichtig ned löschen

  char  receiver [8];
  char  sub [8];
  char  msg [BUF-85];
  std::cout << "Please enter the Receipient" << std::endl;
  fgets(receiver, 80, stdin);
  std::cout << "Please enter the Subject" << std::endl;
  fgets(sub, 80, stdin);
  std::cout << "Please enter the Message" << std::endl;
  fgets(msg, BUF, stdin);
  std::string s_receiver(receiver);
  std::string s_sub(sub);
  std::string s_msg(msg);



  std::string message = "<sender>" + global_username + "</sender>\n" +"<empfänger>" + s_receiver + "</empfänger>\n"+ "<subject>" + s_sub + "</subject>\n"+"<text>" + msg + "</text>\n";
  //std::cout<<message;
  int message_length = message.length();
  char char_Message[message_length];
  strcpy(char_Message, message.c_str());
  sendMessage(socket, char_Message);
}

void listMails(int socket){
  sendMessage(socket, "L");
  char username[8];
  strcpy(username, global_username.c_str());

  sendMessage(socket, username);


}
/*void readMail();
void deleteMail();
void logout();
*/
