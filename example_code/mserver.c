/*
 * C program implementing multiplexed socket connections using select()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 256
#define MAX_CLIENTS_ALLOWED 10
#define SERVER_HOST "localhost"
#define SERVER_PORT 8889

struct sockaddr_in server_addr, client_addr;
int client_sock; // socket descriptors
fd_set readfds; // Read file descriptors set

void make_sock_nonblocking(int sock)
{
	int sock_opts;

	sock_opts = fcntl(sock, F_GETFL);
	if (sock_opts < 0) {
		perror("fcntl failed on F_GETFL");
		exit(EXIT_FAILURE);
	}

	sock_opts = (sock_opts | O_NONBLOCK);
	if (fcntl(sock, F_SETFL, sock_opts) < 0) {
		perror("fcntl failed on F_SETFL");
		exit(EXIT_FAILURE);
	}
}

int start_server()
{
	int listener_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener_sock < 0){
		perror("socket call failed\n"); 
		exit(1);
	}

	make_sock_nonblocking(listener_sock);
	
	// initialize the server_addr structure
	server_addr.sin_family = AF_INET; // for TCP/IP
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // This HOST IP
	server_addr.sin_port = htons(SERVER_PORT); // port number 1234

	int ret = bind(listener_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if (ret < 0){
		printf("bind failed\n"); exit(3);
	}

	printf("hostname = %s port = %d\n", SERVER_HOST, SERVER_PORT);
	printf("server is listening ....\n");
	listen(listener_sock, 5); // queue length = 5

	return listener_sock;
}

int main()
{
	char line[MAX];
	int listener_sock = start_server();

	fd_set readfds_master;
	FD_ZERO(&readfds_master);

	fd_set writefds_master;
	FD_ZERO(&writefds_master);	

	FD_SET(listener_sock, &readfds_master);

	int fd_set_max = listener_sock;

	while(1){
		
		fd_set readfds = readfds_master;
		
		int ready = select(fd_set_max + 1, &readfds, NULL, NULL, NULL);

		for(int fd = 0; fd <= fd_set_max && ready > 0; fd++)
		{
			if(FD_ISSET(fd, &readfds))
			{
				ready--;

				if(fd == listener_sock) {
					printf("server: accepting new connection ....\n");
					// Try to accept a client connection as descriptor newsock
					int len = sizeof(client_addr);
					client_sock = accept(listener_sock, (struct sockaddr *)&client_addr, &len);
					if (client_sock < 0){
						perror("server: accept error\n"); 
						exit(1);
					} else {
						if(client_sock > fd_set_max) {
							if (client_sock > MAX_CLIENTS_ALLOWED) {
								printf("socket fd (%d) >= MAX_CLIENTS_ALLOWED (%d)", 
										client_sock, MAX_CLIENTS_ALLOWED);
								close(client_sock);
								break;
							}

							fd_set_max = client_sock;
						}
						
						make_sock_nonblocking(client_sock);
						printf("server: accepted a client connection from\n");
						printf("---------------------------------------------–\n");
						printf("Client: IP=%s port=%d\n", 
								inet_ntoa(client_addr.sin_addr), 
								ntohs(client_addr.sin_port));
						printf("---------------------------------------------–\n");
						FD_SET(client_sock, &readfds_master);
					}
				} else {
					printf("FD = %d", fd);
					int n = read(fd, line, MAX);
					if (strncmp(line, "BYE", 3) == 0){
						printf("server: client closed connection, server loops\n");
						close(fd);
						FD_CLR(fd, &readfds_master);			
						break;
					}
					// show the line string
					printf("server: read n=%d bytes; line=%s\n", n, line);
					// echo line to client
					n = write(client_sock, line, MAX);
					printf("server: wrote n=%d bytes; ECHO=%s\n", n, line);
					printf("server: ready for next request\n");
				}
			}		
		}
	}
}
