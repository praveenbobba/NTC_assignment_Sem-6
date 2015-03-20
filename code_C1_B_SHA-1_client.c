/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include"sha1.h"
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes,i;  
    char buf[MAXDATASIZE] , *username, *password, user[20],space[5];
    struct addrinfo hints, *servinfo, *p;
    int rv,j;
    char s[INET6_ADDRSTRLEN], content[101],hexa[200];
    unsigned char pass[20];
    unsigned char algo[100];
    size_t len ;

    if (argc != 2) {
        fprintf(stderr,"usage:  hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

  /*  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

 username and password */

   printf("Enter the username: ");
   scanf("%s",user);

   //printf("%s",username);
   printf("Enter the password: ");
   scanf("%s",hexa);
   /*len = strlen(algo);
   printf("%lu\n",len);
   sha1(algo,len, pass);
  for(i=0,j=0;i<20;i=i+1,j=j+2)
    {	printf("%x\n",pass[i]);
	sprintf(hexa+(j),"%02x",pass[i]);
	printf("%s\n",hexa);	
    }*/
   //printf("\n%s",hexa);	
   strcat(user," ");
   strcat(user,hexa);
  username = &user[0];
  //password = &pass[0] ;

   if (send(sockfd, username, strlen(username), 0) == -1)
                perror("send");
   //if (send(sockfd, password, strlen(password), 0) == -1)
                //perror("send");
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
   
    printf("%s\n",buf);
    //scanf("%[^\n]s", content);
    getchar(); 
if(strcmp(buf,"access denied")!= 0)
{   
   fgets(content,101,stdin);
    if (send(sockfd, content, strlen(content), 0) == -1)
                perror("send");

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("%s\n",buf);
}

    close(sockfd);

    return 0;
}
