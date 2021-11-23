#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>


int main (void){

int server_socket=0;
int client_socket=0;
int return_value=0;

char cbuf[51];
int len_addr;
struct sockaddr_in my_addr, peer_addr;

server_socket = socket(AF_INET, SOCK_STREAM, 0);

memset(&my_addr, 0, sizeof(struct sockaddr_in));

my_addr.sin_family = AF_INET;
my_addr.sin_port = htons(10000);
my_addr.sin_addr.s_addr = INADDR_ANY;

return_value = bind(server_socket, (struct sockaddr *) &my_addr, \
	sizeof(struct sockaddr_in));

if (return_value == 0) 
	printf("Bind - OK\n");
else {
	printf("Bind - ERR\n");
	return -1;
}

listen(server_socket, 5);

for (;;){
	client_socket = accept(server_socket, (struct sockaddr *) &peer_addr, &len_addr);
	if (client_socket>0) {
		return_value = fork ();
		if (return_value==0){
			printf("Hura nove spojeni\n");
			do {
				return_value=recv(client_socket, &cbuf, 50, 0);
				printf("Prijato %c\n",cbuf);
			} while (return_value > 0);
			close(client_socket);
			return 0;
		} else {
			close(client_socket);
		}
	}
	else {
		printf ("Brutal Fatal ERROR\n");
		return -1;
	}
}

return 0;
}

