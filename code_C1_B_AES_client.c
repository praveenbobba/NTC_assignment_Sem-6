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
#include"aes.h"
#include <inttypes.h>
#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once

    int sockfd, numbytes,i,num,lt;  
    char buf[MAXDATASIZE] ;
    struct addrinfo hints, *servinfo, *p;
    int rv,j;
    char s[INET6_ADDRSTRLEN], content[101];
    unsigned char pass[200],input2[200],output2[200];
    unsigned char algo[100],input[16],output[16],hexa[200], *username, *password, user[20],space[5];
    unsigned char key[32] = {11,13,19,20,28,30,40,59,71,84,96,102,3,4,7,10,56,78,99,100,66,76,86,46,32,13,14,15,143,153,163,'\0'};
    size_t len ;
    unsigned int keysize=256 ;
    aes_context context;

 

void orenc()
{
  num=aes_setkey_enc (&context, key, keysize);
   num=aes_crypt_ecb (&context, AES_ENCRYPT, input, output);
}

void ordec()
{

  num=aes_setkey_dec (&context, key, keysize);
  num=aes_crypt_ecb (&context, AES_DECRYPT, input, output);
}

void enc()
{
  //output2[0] = '\0';
  int p=0,q,size,j=0,k=0;
  size = strlen(input2);
 /* if(size<=16)
  {	
	
	strcpy(input,input2);
	orenc();
	output[16] = '/';
	strcpy(output2,output);
  }*/

  //else
  //{
    	
    	if(size % 16 !=0 )
		j = 1;
    	for(p=0;p<(size/16)+j;p++)
    	{
		for(q=(16*p),i=0; q< (16 * (p+1) ) && (q<=size) ;q++,i++)
		{
			input[i] = input2[q];
			//printf("%c %c\n",input[i],input2[q]);
		}
		//input[i]='\0';
		orenc();
		for(k=0;k<16;k++)
		{
			output2[(16*p)+k] = output[k];
		}
		//strcpy(input,output);
		//ordec();
		//printf("%s",output);
        	
    	}
	output2[16*p] = '*' ;

  }



//}

void dec()
{
	//output2[0]='\0' ;
  	int p=0,q,size,j=0,k;
    	for(p=0;input2[16*p] !='*' ;p++)
    	{
		for(q=(16*p),i=0; q< (16 * (p+1) ) ;++q,++i)
		{
			input[i] = input2[q];
			//printf("%c %c\n",input[i],input2[q]);
		}
		ordec();
		//printf("%s\n",output);
		for(k=0;k<16;k++)
		{
			output2[(16*p)+k] = output[k];
		}
        	
    	}
	output2[(16*p)]='\0';	
}


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
aes_init( &context);
    //printf("%u\n",keysize);

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
//for(i=0;i<16;i++)
   //printf("%02u\n",user[i]);

   strcpy(input2,user);
   enc();
  
   for(i=0;output2[i] != '*';i++)
	user[i] = output2[i];
   user[i] = output2[i];
   lt = i+1;

   printf("Enter the password: ");
   scanf("%s",hexa);

   strcpy(input2,hexa);
   enc();
      for(i=0;output2[i] != '*';i++)
	{
		hexa[i] = output2[i];
		//printf("hexa is %02d\n",hexa[i]);
	}
      hexa[i] = output2[i];


  username = &user[0];
  

   if (send(sockfd, username, 100, 0) == -1)
                perror("send");
   //if (send(sockfd, password, strlen(password), 0) == -1)
                //perror("send");
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

   password = &hexa[0];

   if (send(sockfd, password, 100, 0) == -1)
                perror("send");
   //if (send(sockfd, password, strlen(password), 0) == -1)
                //perror("send");
    if ((numbytes = recv(sockfd, buf, 200, 0)) == -1) {
        perror("recv");
        exit(1);
    }


   for(i=0;buf[i] != '*';i++)
	input2[i] = buf[i];
   input2[i] = buf[i];
   dec();
   strcpy(buf,output2);

    //buf[numbytes] = '\0';
   
    printf("%s\n",buf);
    //scanf("%[^\n]s", content);
    getchar(); 
if(strcmp(buf,"access denied")!= 0)
{   
   fgets(content,101,stdin);
   strcpy(input2,content);
   enc();
   for(i=0;output2[i] != '*';i++)
	content[i] = output2[i];
   content[i] = output2[i];

    if (send(sockfd, content, 200, 0) == -1)
                perror("send");

    if ((numbytes = recv(sockfd, buf, 200, 0)) == -1) {
        perror("recv");
        exit(1);
    }
   for(i=0;buf[i] != '*';i++)
	input2[i] = buf[i];
   input2[i] = buf[i];
   dec();
   strcpy(buf,output2);

    //buf[numbytes] = '\0';

    printf("%s\n",buf);
}

    close(sockfd);

    return 0;
}
