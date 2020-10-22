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
#include <iomanip>
#define BUF 1024
#define PORT 6543


void sendMessage(int socket, char * buffer);
void printMenu();
void menu(char * buffer, int socket);


void sendMail(int socket);
void listMails(int socket);
void readMail(int socket);
void deleteMail(int socket);
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
  std::cout << "5) Login:Login into the Ldap Server." << std::end
  std::cout << "6) QUIT: Logout." << std::endl;
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
          sendMessage(socket, "S"); // wichtig ned löschen
          sendMail(socket);
        break;
      case 2:
          system("clear");
          std::cout << "LIST" << std::endl;
          sendMessage(socket, "L");
          listMails(socket);
        break;
      case 3:
          system("clear");
          std::cout << "READ" << std::endl;
          std::cout << "Your Inbox: " << std::endl << std::endl;
          sendMessage(socket, "L");
          listMails(socket);
          sendMessage(socket, "R");
          readMail( socket);
        break;
      case 4:
          system("clear");
          std::cout << "DEL" << std::endl;
          sendMessage(socket, "L");
          listMails(socket);
          sendMessage(socket, "D");
          deleteMail(socket);
        break;
      case 5:
          system("clear");
          std::cout << "LIST" << std::endl;
          loginMail(socket, "A");
          listMails(socket);
        break;
        case 6:
            std::cout << "QUIT" << std::endl;
            close (socket);
            exit(0);
            //logout();
          break;
      default:
          system("clear");
          std::cout << "Please Enter one of the Numbers above!" << std::endl;
        break;
    }
}

void loginMail(int socket){
  std::string username = global_username;
  std::string pw = "";
  /*while(mail==""|| mail.length() > 8){
    std::cout << "Please enter which mail to read" << std::endl;
    std::getline (std::cin,mail);
    mail = mail.substr(0,7);
    if(mail==""){
      std::cout << "plase enter the subject of the mail you want to read!" << std::endl << std::endl;
    }
    if(mail.length() > 8){
      std::cout << "The subject can only be 8 chars long!" << std::endl << std::endl;
    }
  }*/

  //cnvert string to char* for sending
  int message_length = username.length();
  char char_Message[message_length];
  strcpy(char_Message, username.c_str());
  sendMessage(socket, char_Message);


  int size;
  char answer [BUF];
  size = recv (socket, answer, BUF-1, 0);
  std::cout<<"This is server answer: "<< std::endl<<std::endl<< answer << std::endl<<std::endl;

}

void sendMail(int socket){
  //int boop = 8;
  std::string  receiver = "";
  std::string  sub = "";
  std::string  msg = "";

  while(receiver == "" || receiver == global_username || receiver.length() > 8){
    std::cout << "Please enter the Receipient" << std::endl;
    std::getline (std::cin,receiver);
    receiver = receiver.substr(0,7);
    if(receiver == global_username){
      std::cout << "You can not send messages to yourself sorry!" << std::endl<< std::endl;
    }
  }

  while(sub == "" || sub.length() > 8){
    std::cout << "Please enter the Subject" << std::endl;
    std::getline (std::cin,sub);
    sub = sub.substr(0,7);
    if(sub == ""){
      std::cout << "The Subject can not be left empty!" << std::endl << std::endl;
    }
  }

  while(msg == ""){
    std::cout << "Please enter the Message" << std::endl;
    std::getline (std::cin,msg);
    if(msg == ""){
      std::cout << "The Message can not be left empty!" << std::endl << std::endl;
    }
  }

  msg = msg.substr(0,900);

  std::string message = global_username + "<stop>" + receiver + "<stop>" + sub + "<stop>" + msg + "<stop>";

  //cnvert string to char* for sending
  int message_length = message.length();
  char char_Message[message_length];
  strcpy(char_Message, message.c_str());
  sendMessage(socket, char_Message);
  //system("clear");


  int size;
  char answer [BUF];
  size = recv (socket, answer, BUF-1, 0);
  std::cout<<"This is server answer: "<< answer<< std::endl<<std::endl;


}

void listMails(int new_socket){
  char buffer[BUF] = "";   //string with 1024 characters
  int size;

  size = recv (new_socket, buffer, BUF-1, 0);
  std::cout<<"This is server answer: "<< std::endl<<std::endl<< buffer << std::endl<<std::endl;

}


void readMail(int socket){
  std::string mail = "";
  while(mail==""|| mail.length() > 8){
    std::cout << "Please enter which mail to read" << std::endl;
    std::getline (std::cin,mail);
    mail = mail.substr(0,7);
    if(mail==""){
      std::cout << "plase enter the subject of the mail you want to read!" << std::endl << std::endl;
    }
    if(mail.length() > 8){
      std::cout << "The subject can only be 8 chars long!" << std::endl << std::endl;
    }
  }

  //cnvert string to char* for sending
  int message_length = mail.length();
  char char_Message[message_length];
  strcpy(char_Message, mail.c_str());
  sendMessage(socket, char_Message);


  int size;
  char answer [BUF];
  size = recv (socket, answer, BUF-1, 0);
  std::cout<<"This is server answer: "<< std::endl<<std::endl<< answer << std::endl<<std::endl;

}



void deleteMail(int socket){

  std::string mail = "";
  while(mail==""|| mail.length() > 8){
    std::cout << "Please enter which mail to delete" << std::endl;
    std::getline (std::cin,mail);
    mail = mail.substr(0,7);
    if(mail==""){
      std::cout << "plase enter the subject of the mail you want to read!" << std::endl << std::endl;
    }
    if(mail.length() > 8){
      std::cout << "The subject can only be 8 chars long!" << std::endl << std::endl;
    }
  }

  //cnvert string to char* for sending
  int message_length = mail.length();
  char char_Message[message_length];
  strcpy(char_Message, mail.c_str());
  sendMessage(socket, char_Message);

  int size;
  char answer [BUF];
  size = recv (socket, answer, BUF-1, 0);
  std::cout<<"This is server answer: "<< answer<< std::endl<<std::endl;

}
