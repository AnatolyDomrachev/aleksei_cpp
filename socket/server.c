// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define PORT 8080 

struct thread_args { int socket; struct sockaddr_in address;};

void* thread_func (void*);

int main(int argc, char const *argv[]) 
{ 

    pthread_t thread;
//	char head[2048] = "Server: Apache/2.2.11 (Win32) PHP/5.3.0\nLast-Modified: Sat, 16 Jan 2010 21:16:42 GMT\nContent-Type: text/plain; charset=windows-1251\nContent-Language: ru\n";

	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

	struct thread_args ta;

	while(new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen)) 
	{ 
		ta.socket = new_socket; 
		ta.address = address;
		pthread_create(&thread,NULL,thread_func,(void*)&ta) ;
		sleep(1);
	}

	return 0; 
} 

void* thread_func (void* s)
{		
	struct thread_args ta = *((struct thread_args*)s);
	int new_socket = ta.socket; 
	struct sockaddr_in address = ta.address;
	int valread; 
	char buffer[1024] = {0}; 
	char head[8000] = "HTTP/1.1 200 OK\r\n\n";
	char hello[2048] = "Hello from server\n\n";
	valread = read( new_socket , buffer, 1024); 
	printf("%s\n",buffer ); 
	send(new_socket , head , strlen(head) , 0 ); 
	send(new_socket , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); 
	printf("%s\n", inet_ntoa(address.sin_addr));
	shutdown(new_socket , SHUT_RDWR);
	close(new_socket);
}
