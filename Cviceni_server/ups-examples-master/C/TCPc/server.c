#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


// telo vlakna co obsluhuje prichozi spojeni
void *serve_request(void *arg){
	int client_sock;
	char cbuf='A';

	//pretypujem parametr z netypoveho ukazate na ukazatel na int
	client_sock = *(int *) arg;

        printf("(Vlakno:) Huraaa nove spojeni\n");
        recv(client_sock, &cbuf, sizeof(char), 0);
        printf("(Vlakno:) Dostal jsem %c\n",cbuf);
        read(client_sock, &cbuf, sizeof(char));
        printf("(Vlakno:) Dostal jsem %c\n",cbuf);
        close(client_sock);

	// uvolnujeme pamet
	free(arg);
	return 0;
}

int main (void){
	int server_sock;
	int client_sock;
	int return_value;
	char cbuf;
	int *th_socket;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	socklen_t	remote_addr_len;
	pthread_t thread_id;;
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (server_sock<=0) return -1;
	
	memset(&local_addr, 0, sizeof(struct sockaddr_in));

	local_addr.sin_family = AF_INET6;
	local_addr.sin_port = htons(10000);
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
			th_socket=malloc(sizeof(int));
			*th_socket=client_sock;
			pthread_create(&thread_id, NULL,\
                                  (void *)&serve_request, (void *)th_socket);
		} else {
			printf("Trable\n");
			return -1;
		}
	}

return 0;
}
