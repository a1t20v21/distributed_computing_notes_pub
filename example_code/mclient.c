/*
 * C program - TCP Client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 256
#define SERVER_HOST "localhost"
#define SERVER_PORT 8889

struct sockaddr_in server_addr;
int sock, r;

int client_start()
{
	/*
	 * create a TCP socket
	 */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock<0){
		printf("socket call failed\n"); exit(1);
	}

	/*
	 * fill server_addr with server’s IP and PORT
	 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // localhost
	server_addr.sin_port = htons(SERVER_PORT); // server port number

	/*
	 * connecting to server
	 */
	r = connect(sock,(struct sockaddr*)&server_addr, sizeof(server_addr));
	if (r < 0){
		printf("connect failed\n"); exit(3);
	}

	printf("connected OK to\n");
	printf("-------------------------------------------------------\n");
	printf("Server hostname=%s PORT=%d\n", SERVER_HOST, SERVER_PORT);
	printf("-------------------------------------------------------\n");
	printf("========= client started ==========\n");
}

int main()
{
	int n;
	char line[MAX], ans[MAX];
	client_start();
	printf("******** processing loop *********\n");
	while (1){
		printf("input a line : ");
		bzero(line, MAX); // zero out line[ ]
		fgets(line, MAX, stdin); // get a line from stdin
		
		// Send line to server
		n = write(sock, line, MAX);
		printf("client: wrote n=%d bytes; line=%s\n", n, line);

		if (strncmp(line, "BYE", 3) == 0) // exit if NULL line
			exit(0);
		
		// Read a line from sock and show it
		n = read(sock, ans, MAX);
		printf("client: read n=%d bytes; echo=%s\n", n, ans);
	}
}
