#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>

int main(void){

int client_socket;
int return_value;
char cbuf;
int len_addr;
struct sockaddr_in my_addr;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&my_addr, 0, sizeof(struct sockaddr_in));

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10000);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	return_value = connect(client_socket, (struct sockaddr *)&my_addr,\
		 sizeof(struct sockaddr_in));
	if (return_value == 0) 
        	printf("Connect - OK\n");
	else {
        printf("Connect - ERR\n");
        	return -1;
	}

	send(client_socket, "A", sizeof(char), 0);
	printf("Poslano A\n");
	send(client_socket, "B", sizeof(char), 0);
	printf("Poslano B\n");

	return 0;
}
