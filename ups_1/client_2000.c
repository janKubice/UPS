#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

int client_socket;
char recv_buff[BUFF_SIZE];
char resp_buff[BUFF_SIZE];
int return_value;

/**
 * @brief Odesle zpravu serveru
 * 
 * @param line text který má být odeslaný na server
 * @return int -1 pokud se neco nepovedlo
 */
int send_message(char *line) {

		memset(resp_buff, '\0', sizeof(resp_buff));
		sprintf(resp_buff, "%s\n", line);
		return_value = send(client_socket, resp_buff, sizeof(resp_buff), 0);

		if (return_value <= 0) {
				printf("Sending ERROR\n");
				return -1;
		}
}

/**
 * @brief do recv_buff ulozi obsah prijmute zpravy ze serveru
 * 
 * @return int -1 pokud se neco nepovedlo
 */
int rcv_message() {
		char temp[BUFF_SIZE];

		memset(temp, '\0', sizeof(temp));
		return_value = recv(client_socket, temp, sizeof(temp), 0);

		if (return_value <= 0) {
				printf("Receiving ERR\n");
				return -1;
		}

		memset(recv_buff, '\0', sizeof(recv_buff));
		sprintf(recv_buff, "%s", temp);
}

int main() {
		struct sockaddr_in my_address;

		// Vytvoreni socketu
		client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (client_socket <= 0) {
			printf("Socket ERROR\n");
			return -1;
		}

		memset(&my_address, 0, sizeof(struct sockaddr_in));

		// Nastaveni adresy
		my_address.sin_family = AF_INET;
		my_address.sin_port = htons(2000);
		my_address.sin_addr.s_addr = inet_addr("34.122.204.187");

		// Pripojeni k serveru
		return_value = connect(client_socket, (struct sockaddr *)&my_address, sizeof(struct sockaddr_in));
		if (return_value == 0)
				printf("Connect  OK\n");
		else {
	      printf("Connect  ERROR\n");
				return -1;
		}

		
		rcv_message();
		printf("%s",recv_buff);
		rcv_message();
		printf("%s",recv_buff);
		
		char input_math[BUFF_SIZE];
		
		printf("Enter expession in format: operand|number|number -> example: plus|10|10 \n");
		scanf("%s", input_math);
		
		printf("Response to server: %s\n", input_math);
		send_message(input_math);
		rcv_message();
		printf("%s",recv_buff);
		
		close(client_socket);

		return 0;
}
