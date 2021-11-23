#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int main (void){
	int server_sock;
	int client_sock;
	int return_value;
	char cbuf;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	socklen_t	remote_addr_len;
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (server_sock<=0) return -1;
	
	memset(&local_addr, 0, sizeof(struct sockaddr_in));

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = 10000;
	local_addr.sin_addr.s_addr = INADDR_ANY;

	return_value = bind(server_sock, (struct sockaddr *)&local_addr,\
                sizeof(struct sockaddr_in));

	if (return_value == 0)
		printf("Bind OK\n");
	else{
		printf("Bind ER\n");
		return -1;
	}

	return_value = listen(server_sock, 5);
	if (return_value == 0)
		printf("Listen OK\n");
	else{
		printf("Listen ER\n");
	}


	while(1){
		client_sock = accept(server_sock,\
			(struct sockaddr *)&remote_addr,\
			&remote_addr_len);
		if (client_sock > 0 ) {
			return_value = fork();
			/// jsem potomek
			if (return_value==0){
				printf("(Potomek:) Huraaa nove spojeni\n");
				recv(client_sock, &cbuf, sizeof(char), 0);
				printf("(Potomek:) Dostal jsem %c\n",cbuf);
				read(client_sock, &cbuf, sizeof(char));
				printf("(Potomek:) Dostal jsem %c\n",cbuf);
				close(client_sock);
				return 0;
			}
			///jsem rodic nebo chyba forku
			close(client_sock);
		
		} else {
			printf("Trable\n");
			return -1;
		}
	}

return 0;
}
