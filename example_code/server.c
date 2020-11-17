/*
 * This C program depicts the basic steps required
 * to write a TCP/IP server.
 */ 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

struct sockaddr_in server_addr, client_addr;
int server_sock, client_sock;


int main()
{
	char buf[100];
	int len; 

	/*
	 * Step 1 -> Create the main server socket
	 * 
	 *	a. main server socket is responsible for connection initiation requests from new clients
	 *
	 */ 
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	/*
	 * Once we have the main server socket, we can bind that socket to an ethernet interface
	 * and a port. For that we use a C structure of type `sockaddr_in`. This structure is 
	 * defined as follows: (man 7 ip)
	 *
	 *	    struct sockaddr_in {
     *          sa_family_t    sin_family; 
     *          in_port_t      sin_port;
     *          struct in_addr sin_addr;
     *      };
	 *
     *      struct in_addr {
     *          uint32_t       s_addr;
     *      };
	 *
	 */

	server_addr.sin_family = AF_INET; 
	/* Address Family: For a TCP server, it's always AF_INET - `man 2 socket`*/
	
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* IP Address of the local interface which we want to `bind` our socket*/

	server_addr.sin_port = htons(30001);
	/*port where the server process listens for incoming connections*/


	/* 
	 * Step 2 -> Once sockaddr_in structure is initialised, 
	 * we can use it with the bind() system call to bind 
	 * the server socker to an IP, port and protocol.
	 */
	bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));

	/*
	 * Step 3 -> listen() system call enables the server 
	 * socket to passively wait for incoming connections. 
	 */
	listen(server_sock, 5);
	
	len = sizeof(client_addr);

	/*
	 * Step 4 -> accept() system call accepts a new connection and creates a 
	 * handle (communications file descriptor) for the client. All data exchange
	 * between the client and the server will be using this `handle`. The main server
	 * socket is used only for handling new connections.
	 *
	 * client_addr: it's an empty structure. This structure will be initialised
	 *				by the accept() system call using the Source IP Address and Source
	 *				Port obtained from the TCP packet header sent by the client. When the 
	 *				server responds back, it will use this information.
	 */ 
	client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &len);
	printf("New Client: \n");
	printf("---------------------------------------------–\n");
	printf("Client: IP=%s port=%d\n", inet_ntoa(client_addr.sin_addr), 
			ntohs(client_addr.sin_port));
	printf("---------------------------------------------–\n");

	/* 
	 * Step 5 ->
	 * Get any data available on the client_sock file descriptor to the character buffer
	 */
	recvfrom(client_sock, buf, 100, 0, NULL, 0);
	printf("%s\n", buf);
	close(client_sock);
	close(server_sock);

	return 0;
}
