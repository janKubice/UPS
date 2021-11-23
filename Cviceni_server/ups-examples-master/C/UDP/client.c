#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void)
{
	int client_sock;
	int addr_len;
	struct sockaddr_in remote_addr;
	int return_value;
	char ch = 'A';

	client_sock = socket(AF_INET, SOCK_DGRAM, 0 );
	if (client_sock<=0){
		return -1;
	}

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	remote_addr.sin_port = htons( 1000 );
	addr_len=sizeof( remote_addr );


	printf( "Posilam %c\n", ch );
	sendto(client_sock, &ch, 1,0,(struct sockaddr*)&remote_addr, addr_len );

	printf( "Cekam na znak od serveru:\n" );

	recvfrom( client_sock, &ch, 1,0,(struct sockaddr*)&remote_addr, &addr_len );
	printf( "Dostavam = %c\n", ch );

	close(client_sock);
	return 0 ;
}

