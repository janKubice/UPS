#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>

int main()
{
	int server_sockfd, client_sock;
	int server_len, client_len;
	struct sockaddr_un server_address;
	struct sockaddr_un client_address;
	char ch = 'A';;

	unlink( "server_socket" );

	server_sockfd = socket( AF_UNIX, SOCK_STREAM, 0 );
	server_address.sun_family = AF_UNIX;

	strcpy( server_address.sun_path, "server_socket" );
	server_len = sizeof( server_address );

	bind( server_sockfd, ( struct sockaddr *)&server_address, server_len );
	
	listen( server_sockfd, 5 );

	while( 1 )
	{
		printf( "Server ceka\n" );
		
		client_len = sizeof( client_address );
		client_sock = accept( server_sockfd, ( struct sockaddr *)&client_address, &client_len );

		read( client_sock, &ch, 1 );
		printf( "Klient poslal = %c\n", ch );

		ch++;

		printf( "Server odesila = %c\n", ch );
		write( client_sock, &ch, 1 );

		close( client_sock );

	}
}

