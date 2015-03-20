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

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

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
    int sockfd, new_fd,numbytes,count,i,j;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1,k=0;
    char s[INET6_ADDRSTRLEN],buf[20],user[20],log_user[20],log_pass[20],hexa[50];
    int rv,status = 0;
    FILE *fp, *fd;
    char *msg = "access granted\nEnter the content to make a file" ;
    unsigned char pass[20];
    unsigned char algo[100];
    size_t len ;
    
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
		while(buf[count]!=' ')
		{
			user[count] = buf[count] ;
			count += 1 ;
		}
		user[count] = '\0' ;
		count += 1;
		//printf("%s\n",user);
		while(count < numbytes)
		{
			pass[k] = buf[count] ;
			k = k+ 1;
			count += 1 ;
		}
		pass[k] = '\0' ;
		len=strlen(pass);
			
		   sha1(pass,len, algo);

		  for(i=0,j=0;i<20;i=i+1,j=j+2)
    			{	//printf("%x\n",pass[i]);
				sprintf(hexa+(j),"%02x",algo[i]);
				//printf("%s\n",hexa);	
    			}
		//printf("%s\n",hexa);
    		fp = fopen("log_details.txt","r");
		while(! feof(fp))
		{
			fscanf(fp,"%s%s",log_user,log_pass);
			if( strcmp(user,log_user)==0  && strcmp(hexa,log_pass)==0 )
			{
				status = 1;
				break;
			}
		}
		fclose(fp);

    		//printf("username '%s'\n",user);
		//printf("password '%s'\n",pass);
    		/*buf[numbytes] = '\0';

    		printf("password '%s'\n",buf);


		if ((numbytes = recv(new_fd, buf, 20, 0)) == -1) {
        		perror("recv");
        		exit(1);
    		}

    		buf[numbytes] = '\0';

    		printf("password '%s'\n",buf);*/

		if(status == 1)
		{
			if (send(new_fd,msg ,strlen(msg), 0) == -1)
               		   perror("send");
	
			if ((numbytes = recv(new_fd, buf, 102, 0)) == -1) 
			{
        			perror("recv");
        			exit(1);
    		
			}
			buf[numbytes] = '\0' ;
			strcat(log_user,".txt");
			fd = fopen(log_user,"w+");
			fprintf(fd,"%s",buf);
			msg = "Your file has been created" ;
			fclose(fd);
			if (send(new_fd,msg ,strlen(msg), 0) == -1)
               		   perror("send");
				
		}

		else
		{
			if (send(new_fd, "access denied", 13, 0) == -1)
               		   perror("send");	
		}
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}