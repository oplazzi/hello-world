#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>


//get address info from the server 
void get_addr_info(const char* address, const char* port, struct sockaddr_in* sock_host) {
  memset(sock_host, 0, sizeof(*sock_host));
  sock_host -> sin_family = AF_INET;
  sock_host -> sin_port = htons(atoi(port));
  inet_aton(address, &sock_host->sin_addr);
  //printf("get_addr_info OK.. AFINET : %d.. sinport :%d .. address: %s\n", sock_host->sin_family, sock_host->sin_port,address);
  printf("...Get Addr OK\n");
}

//get the socket
int do_socket(int domain, int type, int protocol) {
  int sockfd;
  int yes = 1;
  //create the socket
  sockfd = socket(domain, type, protocol);
  //check for socket validity
  if (sockfd == -1) 
    error("ERROR"); 
  // set socket option, to prevent "already in use" issue when rebooting the server right on
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    error("ERROR setting socket options");
  printf("...do_socket OK\n");
  return sockfd;
}

//connect to remote socket
void do_connect(int sockfd, const struct sockaddr_in* addr, socklen_t addrlen) {
  int res = connect(sockfd, (struct sockaddr*)addr, addrlen);
  if (res != 0) {
    error("ERROR : Connection Problem");
  }
  printf("..Connection OK\n");
}


// Communication with the server
void handle_client_message(int sock) {
  int b = 1;
  char message[1000], srv_rep[2000];
  
  while (b) {
  
    memset(message,'\0',sizeof(message));
    memset(srv_rep,'\0',sizeof(srv_rep));
    printf("Type sthg to echo:\n");
    scanf("%s", message);
    
    if (send(sock, message, strlen(message), 0) < 0) {
      error("ERROR: Pb while sending");
    }
    
    if (recv(sock, srv_rep, 2000, 0) < 0) {
      error("ERROR: Pb during reception");
    }
    
    // quit the connexion to the server
    if (!strcmp(message,"/quit")) {
      printf("message = /quit \n");
      b = 0; 
    }
    printf("Server reply :\n");
    puts(srv_rep);
  }
}

int main(int argc,char* argv[]) {

  struct sockaddr_in sock_host; 
  int sock;

  if (argc != 3) {
      fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
      return 1;
  }
   
  get_addr_info(argv[1], argv[2],&sock_host);

  sock = do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  
  do_connect(sock,&sock_host,sizeof(sock_host));
  
  handle_client_message(sock);
  
  // end connection
  int test =  close (sock) ;
  printf("valeur du test : %d",test);

  return 0;

}
