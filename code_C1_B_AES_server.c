/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include"sha1.h"
#include"aes.h"
#include <inttypes.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

    int sockfd, new_fd,numbytes,count,i,j;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1,k=0,num;
    char s[INET6_ADDRSTRLEN];
    int rv,status = 0;
    FILE *fp, *fd;
    
    unsigned char pass[30],user[30],log_user[20],log_pass[20],hexa[50];
    unsigned char algo[100],output2[200],input2[200],msg[200];
    size_t len ;
    unsigned char input[30],output[30],buf[400];
    unsigned char key[32] = {11,13,19,20,28,30,40,59,71,84,96,102,3,4,7,10,56,78,99,100,66,76,86,46,32,13,14,15,143,153,163,'\0'};
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
  int p=0,q,size;
  size = strlen(input2);

    	
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


void dec()
{
	//output2[0]='\0' ;
  	int p=0,q,size;
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



void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    strcpy(msg,"access_granted enter the contents of the file");
    aes_init( &context);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
           // if (send(new_fd, "Hello, world!", 13, 0) == -1)
                //perror("send");
		


		if ((numbytes = recv(new_fd, buf, 200, 0)) == -1) {
        		perror("recv");
        		exit(1);
    		}

		count = 0;
		while(buf[count]!='*')
		{
			user[count] = buf[count] ;
			count += 1 ;
		}
		user[count] = '*';
   		for(i=0;user[i] != '*';i++)
			input2[i] = user[i];
   		input2[i] = user[i];		
		dec();
		strcpy(user,output2);
		//printf("%s\n",user);
		//user[count] = '\0' ;
			if (send(new_fd,"msg" ,3, 0) == -1)
               		   perror("send");
		if ((numbytes = recv(new_fd, buf, 200, 0)) == -1) {
        		perror("recv");
        		exit(1);
    		}
		count = 0;
	
		while(buf[count] != '*')
		{
			pass[count] = buf[count] ;
			//printf("pass is %02d\n",pass[j]);
			
			count += 1 ;
		}
		pass[count] = '*';
		//printf("%s\n",pass);
  for(i=0;pass[i] != '*';i++)
	input2[i] = pass[i];
   input2[i] = pass[i];
   dec();
   strcpy(pass,output2);
  // printf("%s\n",pass);
		//pass[k] = '\0' ;
		len=strlen(pass);
			
		   sha1(pass,len, algo);

		  for(i=0,j=0;i<20;i=i+1,j=j+2)
    			{	//printf("%x\n",pass[i]);
				sprintf(hexa+(j),"%02x",algo[i]);
				//printf("%s\n",hexa);	
    			}

    		fp = fopen("log_details.txt","r");

		
		while(! feof(fp))
		{
			fscanf(fp,"%s%s",log_user,log_pass);
			
			if( (strcmp(user,log_user)==0)  && (strcmp(hexa,log_pass)==0) )
			{
				status = 1;
				//printf("status\n");
				break;
			}
		}
		fclose(fp);
		

		if(status == 1)
		{
			
			strcpy(input2,msg);
			//printf("%s\n",input2);
			enc();
   			for(i=0;output2[i] != '*';i++)
				msg[i] = output2[i];
   			msg[i] = output2[i];

			if (send(new_fd,msg ,100, 0) == -1)
               		   perror("send");

			if ((numbytes = recv(new_fd, buf, 102, 0)) == -1) 
			{
        			perror("recv");
        			exit(1);
    		
			}
   			for(i=0;buf[i] != '*';i++)
				input2[i] = buf[i];
  			 input2[i] = buf[i];
			dec();
			strcpy(buf,output2);

			//buf[numbytes] = '\0' ;
			strcat(log_user,".txt");
			fd = fopen(log_user,"w+");
			fprintf(fd,"%s",buf);
			strcpy(msg,"file created") ;
			strcpy(input2,msg);
			enc();
   			for(i=0;output2[i] != '*';i++)
				msg[i] = output2[i];
   			msg[i] = output2[i];
			fclose(fd);
			if (send(new_fd,msg ,200, 0) == -1)
               		   perror("send");
				
		}

		else
		{
			strcpy(msg,"access denied") ;
			strcpy(input2,msg);
   			enc();
   			for(i=0;output2[i] != '*';i++)
				msg[i] = output2[i];
   			msg[i] = output2[i];
			if (send(new_fd, msg, 200, 0) == -1)
               		   perror("send");	
		}
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
