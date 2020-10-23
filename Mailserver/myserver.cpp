/* myserver.cpp */

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

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <mutex>

#include <dirent.h>
#include <pthread.h>
#include <vector>
#include <ldap.h>

#define BUF 1024
#define PORT 6543
#define MAX_CLIENTS 5


using namespace std;

std::string mail_path = "";

void receiveMsg(int new_socket, std::string user_mail_path);
void deleteMail(int new_socket, std::string user_mail_path);
void readmsg(int new_socket, std::string user_mail_path);
void listmsg(int new_socket, std::string user_mail_path);
void login(int new_socket, std::string user_mail_path);
int connectionLoop(struct sockaddr_in address, int create_socket, char * mailpath);
bool LDAPLogin(string user, string pw);

int main (int argc, char **argv)
{
    std::vector<std::thread> threads;
    int create_socket;
    struct sockaddr_in address; //makes struct with adress, port and adress_family (AF_INET)
    if( argc < 3 )
    {
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
    the address specified by addr to the socket referred to by the file descriptor sockfd.  addrlen specifies the size, in bytes, of the address structure pointed to by addr.  Traditionally, this operation is called “assigning a name to a socket”.
    */
    if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0){
        perror("bind error");
        return EXIT_FAILURE;
    }

    listen (create_socket, MAX_CLIENTS); //Socket to listen to and maximal connection Queue lenght
//THREAD STUFF
    while (true)
    {
        int new_socket = 0;
        char buffer[BUF];   //string with 1024 characters
        socklen_t addrlen = sizeof (struct sockaddr_in); //int 32 bit
        struct sockaddr_in cliaddress;
        std::cout << "Waiting for Connection on: IP: " << inet_ntoa(address.sin_addr) << " Port: " << htons(address.sin_port) << "\n";
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
        if (new_socket > 0)
        {
            printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr), ntohs(cliaddress.sin_port));
            strcpy(buffer,"Welcome to myserver, Please enter your command:\n");
            threads.push_back(std::thread(connectionLoop, address , new_socket, mailpath));
            std::cout << "Creating a thread!!!! " << std::endl;
            send(new_socket, buffer, strlen(buffer),0);
        }
      }
     for(auto &th : threads){
        th.join();
      }
//END THREAD STUFF
    close (create_socket);
    return EXIT_SUCCESS;
}

//MAIN LOOP HERE
int connectionLoop(struct sockaddr_in address, int new_socket, char * mailpath){
  std::string user_mail_path = "";
  std::string user = "";
  bool end = false;
  int setup = 0;
  int size;
  int loginCounter = 0;
  char buffer[BUF];
      do
      {

          size = recv (new_socket, buffer, BUF-1, 0);
          if( size > 0)
          {

              buffer[size] = '\0';
              if(setup == 0){
                cout << "CCC" << endl;

                  std::string pathPartOne(mailpath);
                  std::string pathpartTwo(buffer);

                  std::string path = pathPartOne + "/" + pathpartTwo;

                  mail_path = pathPartOne;
                  user = pathpartTwo;
                  std::cout << "User: "<< user << endl;
                  user_mail_path = path;


                  mkdir(pathPartOne.c_str(), 0777);

                  if(mkdir(path.c_str(), 0777) == -1)
                  {
                      std::cerr << "Error :  " << strerror(errno) << std::endl;
                  }
                  else
                  {
                      std::cout << "Directory "<< buffer <<" created";
                  }
                  setup = 1;
              }
              //Defining Stuff that has to be checked
              char SEND = 'S';
              char LIST = 'L';
              char READ = 'R';
              char DEL = 'D';
              char LOGIN = 'A';
              char QUIT = 'Q';

              if(buffer[0] == SEND)
              {
                  std::cout<<"Send received" << std::endl << std::endl;
                  //std::cout<<"THIS MESSAGE WAS IN THE receive function: "<<buffer<<std::endl;
                  receiveMsg(new_socket, user_mail_path);
              }
              else if(buffer[0] == LIST )
              {
                  std::cout<<"LIST received";
                  listmsg(new_socket, user_mail_path);
              }
              else if(buffer[0] == READ)
              {
                  std::cout<<"READ received";
                  readmsg(new_socket, user_mail_path);
              }
              else if(buffer[0] == DEL)
              {
                  std::cout<<"DEL received";
                  deleteMail(new_socket, user_mail_path);
              }
              else if(buffer[0] == LOGIN)
              {
                  std::cout<<"Login received";
                  login(new_socket, user_mail_path);
                  loginCounter ++;
              }
              else if(buffer[0] == QUIT)
              {
                  setup = 0;
                  close (new_socket);
                  end = true;
                  user = "";
                  mail_path = "";
                  user_mail_path = "";
              }
          }
          else if (size == 0)
          {
              setup = 0;
              printf("Client closed remote socket\n");
              close (new_socket);
              //close (create_socket);
              user = "";
              mail_path = "";
              user_mail_path = "";
              break;
          }
          else
          {
              setup = 0;
              perror("recv error");
              user = "";
              mail_path = "";
              user_mail_path = "";
              return EXIT_FAILURE;
          }
      }
      while (strncmp (buffer, "Q", 1)  != 0);
}

void login(int new_socket, std::string user_mail_path){

  char buffer[BUF]= "";   //string with 1024 characters
  int size;
  size = recv (new_socket, buffer, BUF-1, 0);

  buffer[size] = '\0';

  //std::cout<<"THIS MESSAGE WAS IN THE receive function: "<<buffer<<std::endl;
  //cut string into several strings
  std::string receivedMessage(buffer);
  //std::cout << receivedMessage << std::endl;
  std::string username, pw, token;
  std::string delimiter = "/";
  int pos = 0;
  int counter = 0;


  while ((pos = receivedMessage.find(delimiter)) != std::string::npos)
  {
      token = receivedMessage.substr(0, pos);
      //std::cout << "Token: " << counter << " "<<token << std::endl;
      if(counter == 0)
      {
          username = token;
      }
      else if(counter == 1)
      {
          pw = token;
      }
      counter ++;

      receivedMessage.erase(0, pos + delimiter.length());

  }

  char * response = "0";
  if(LDAPLogin(username, pw)){
     response = "1";
  }else{
     response = "0";
  }
   send(new_socket, response, strlen (response), 0);
}

bool LDAPLogin(string user, string pw){

      const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
      const int ldapVersion = LDAP_VERSION3;
      char ldapBindUser[256];
      char rawLdapUser[128];

      //std::cout << "USER: " << user << std::endl;
      //std::cout << "PWD: " << pw << std::endl;

      strcpy(rawLdapUser, user.c_str());
      sprintf(ldapBindUser, "uid=%s,ou=people,dc=technikum-wien,dc=at\n", rawLdapUser);
      //std::cout << "USER: " << ldapBindUser << std::endl;

      char ldapBindPassword[256];
      strcpy(ldapBindPassword, pw.c_str());
      //std::cout << "USER: " << ldapBindPassword << std::endl;

      int rc = 0;

      LDAP *ldapHandle;
      rc = ldap_initialize(&ldapHandle, ldapUri);
      if (rc != LDAP_SUCCESS)
      {
         fprintf(stderr, "ldap_init failed\n");
         return false;
      }
      printf("connected to LDAP server %s\n", ldapUri);

      rc = ldap_set_option(
          ldapHandle,
          LDAP_OPT_PROTOCOL_VERSION, // OPTION
          &ldapVersion);             // IN-Value
      if (rc != LDAP_OPT_SUCCESS)
      {
         // https://www.openldap.org/software/man.cgi?query=ldap_err2string&sektion=3&apropos=0&manpath=OpenLDAP+2.4-Release
         fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
         ldap_unbind_ext_s(ldapHandle, NULL, NULL);
         return false;
      }

      rc = ldap_start_tls_s(
          ldapHandle,
          NULL,
          NULL);
      if (rc != LDAP_SUCCESS)
      {
         fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
         ldap_unbind_ext_s(ldapHandle, NULL, NULL);
         return false;
      }

      BerValue bindCredentials;
      bindCredentials.bv_val = (char *)ldapBindPassword;
      bindCredentials.bv_len = strlen(ldapBindPassword);
      BerValue *servercredp; // server's credentials
      rc = ldap_sasl_bind_s(
          ldapHandle,
          ldapBindUser,
          LDAP_SASL_SIMPLE,
          &bindCredentials,
          NULL,
          NULL,
          &servercredp);
      if (rc != LDAP_SUCCESS)
      {
         fprintf(stderr, "LDAP bind error: %s\n", ldap_err2string(rc));
         ldap_unbind_ext_s(ldapHandle, NULL, NULL);
         return false;
      }
      return true;
}

void receiveMsg(int new_socket, std::string user_mail_path){

    char buffer[BUF]= "";   //string with 1024 characters
    int size;
    size = recv (new_socket, buffer, BUF-1, 0);

    buffer[size] = '\0';

    //std::cout<<"THIS MESSAGE WAS IN THE receive function: "<<buffer<<std::endl;
    //cut string into several strings
    std::string receivedMessage(buffer);
    std::string delimiter = "<stop>";
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string message_to_save;

    int pos = 0;
    std:string token;
    int counter = 0;
    while ((pos = receivedMessage.find(delimiter)) != std::string::npos)
    {
        token = receivedMessage.substr(0, pos);
        std::cout << "Token: " << counter << " "<<token << std::endl;
        if(counter == 0)
        {
            sender = token;
        }
        else if(counter == 1)
        {
            receiver = token;
        }
        else if(counter == 2)
        {
            subject = token;
        }
        else if(counter == 3)
        {
            message_to_save = token;
        }
        counter ++;

        receivedMessage.erase(0, pos + delimiter.length());

    }


      //std::cout << std::endl << "MESSAGE:" <<  receivedMessage << std::endl;
      //std::cout << sender << std::endl;
      //std::cout << receiver << std::endl;
      //std::cout << subject << std::endl;
      //std::cout << message_to_save << std::endl;

    fstream filept;
    char  reply [4]; //reply to the client


    std::string msg_path = mail_path+"/" + receiver +"/" +subject;
    std::cout<<"Save message to"<<msg_path<<std::endl;//DEBUGSTUFF

    filept.open( msg_path ,ios::out);
    if(filept.is_open()){
    filept<<"Sender: "<<sender<<"\nSubject: "<<subject<<"\n" <<message_to_save;
     strncpy (reply, "okk", sizeof(reply));

    filept.close();
    }else{
      std::cout<<"File could not be created"<<std::endl;
     strncpy (reply, "err", sizeof(reply));
    }

    send(new_socket, reply, strlen (reply), 0);

}

void deleteMail(int new_socket, std::string user_mail_path){
  //receives which mail to delete
  char buffer[BUF] = "";    //string with 1024 characters
  int size;
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';

  std::string file_to_delete = user_mail_path + "/" +buffer;
  int file_to_delete_length = file_to_delete.length();
  char c_file_to_delete [file_to_delete_length+1];
  strcpy(c_file_to_delete, file_to_delete.c_str());


  char  reply [4];
  if(remove(c_file_to_delete) != 0 )
    strncpy (reply, "err", sizeof(reply));
  else
    strncpy (reply, "okk", sizeof(reply));
    //SEND REPLY
  send(new_socket, reply, strlen (reply), 0);
}

void readmsg(int new_socket, std::string user_mail_path){
  //receives which mail to open
  char  reply [4];
  char buffer[BUF] = "";   //string with 1024 characters
  int size;

  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';

  std::string file_to_read = user_mail_path + "/" +buffer;

  //make file_to_read to c_string
  int file_to_read_length = file_to_read.length();
  char c_file_to_read [file_to_read_length+1];
  strcpy(c_file_to_read, file_to_read.c_str());


  ifstream f(c_file_to_read); //taking file as inputstream
  string str;
  if(f) {
     ostringstream ss;
     ss << f.rdbuf(); // reading data
     str = ss.str();
  }
  cout<<str;

  int str_length = str.length();

  char c_str[str_length];

  strcpy(c_str, str.c_str());

  strncpy(buffer,  c_str, sizeof(buffer));
  send(new_socket, buffer, strlen (buffer), 0);

}

void listmsg(int new_socket, std::string user_mail_path){
  char buffer[BUF] = "";
  //read stuff out of file
  //std::string file_to_read = user_mail_path;
  std::string all_entries = "";
  //std::cout << "User: "<< user << endl;

  struct dirent * entry = nullptr;
  DIR * dp = nullptr;

  dp = opendir(user_mail_path.c_str());
  if(dp != nullptr){
    while (entry = readdir(dp)){
      std::string s(entry->d_name);
      if(s != ".." && s != "."){
        all_entries +=  s + "\n";
      }
    }
  }
  if(all_entries.length()>0){
    std:cout << all_entries;
  }
  else{
    all_entries = "No mails found in your Inbox!";
  }

  int all_entries_length = all_entries.length();
  char c_all_entries[all_entries_length];
  strcpy(c_all_entries, all_entries.c_str());

  strncpy(buffer,  c_all_entries, sizeof(buffer));
  send(new_socket, buffer, strlen (buffer), 0);
}
