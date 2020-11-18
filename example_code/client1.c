/*
 * This C program depicts the basic steps required
 * to write a TCP/IP client.
 *
 * The debug macros are copied from the book "Learn C The Hard Way" by Zed Shaw.
 */ 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define clean_error() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, \
		"[ERROR] (%s:%d: errno: %s) " M "\n", \
		__FILE__, __LINE__, clean_error(), ##__VA_ARGS__)

#define check(A, M, ...) if(!(A)) { \
		log_err(M, ##__VA_ARGS__); errno = 0; goto error; }

struct sockaddr_in server_addr;

int main()
{
	/*
	 * Step 1 -> create the TCP socket
	 */
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	/*
	 * Server IP and Server listening port
	 *
	 * For any TCP/IP connection, following 4 parameters are mandatory:
	 *		- Source IP Address (Client IP)
	 *		- Source Port Number (Client Port)
	 *		- Destination IP Address
	 *		- Destination Port Number
	 *
	 * Client IP is decided by the routing information table
	 * and Client port randomly assigned by the Operating System
	 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(30001);

	/*
	 * Step 2 -> connect() system call initiates a connection to the server
	 */
	check(connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) != -1,
			"Client connection initiation failed!!!");

	/*
	 * Once the connection is successfully established, client can write data
	 * to the socket.
	 */
	send(sock, "hello", strlen("hello"), 0);
	close(sock);
	return 0;

error:
	close(sock);
	return -1;
}
