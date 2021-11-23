#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>

int main(void)
{
	int server_socket;
	int addr_len;
	struct sockaddr_un local_addr;
	int return_value;
	char ch = 'A';

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0 );
	local_addr.sun_family = AF_UNIX;

	strcpy(local_addr.sun_path, "client_socket");
	addr_len=sizeof( local_addr );

	return_value = connect( server_socket, ( struct sockaddr *)&local_addr, addr_len );
	
	if( return_value < 0 )
	{
		printf("Connect ER\n");
		return -1;;
	}

	printf( "Posilam znak = %c\n", ch );
	write(server_socket, &ch, 1 );
	read( server_socket, &ch, 1 );
	printf( "Dostavam znak = %c\n", ch );
	close(server_socket);
	return 0;
}

