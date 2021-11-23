#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int main (void){
	int client_sock;
	int return_value;
	struct sockaddr_in remote_addr;
	
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (client_sock<=0) return -1;
	
	memset(&remote_addr, 0, sizeof(struct sockaddr_in));

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(10000);
	remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	return_value = connect(client_sock, (struct sockaddr *)&remote_addr,\
			sizeof(struct sockaddr_in));
	send(client_sock, "A", sizeof(char), 0);
	printf("Poslal jsem A\n");
	write(client_sock, "B", sizeof(char));
	printf("Poslal jsem B\n");
	close(client_sock);
	
return 0;
}
