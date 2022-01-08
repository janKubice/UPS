/**
 * Server pro hru
 * prelozi se prikazem gcc server.c -lpthread -o server
 * 
 * Author: Jan Kubice
 * Date: 27.12.2021
 * Version: Fakt obri cislo
 */ 

#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h> 
#include<pthread.h>
#define CLIENT_MSG_SIZE 512

#define CLIENT_GET_GAMES "1"
#define CLIENT_SET_PIXEL "2"
#define CLIENT_GET_ITEM "3"
#define CLIENT_SEND_QUESS "4"
#define CLIENT_CREATE_GAME "5"
#define CLIENT_CONNECT_TO_GAME "6"
#define CLIENT_RECONNECT "7"
#define CLIENT_LEAVE "8"
#define CLIENT_GET_POINTS "9"
#define CLIENT_GET_ID "10"
#define CLIENT_CANCEL_GAME "11"

#define OK "100"
#define ERR "-1"


void *connection_handler(void *);

struct Player
{
    /* data */
} player;

struct Game
{
    /* data */
} game;


/**
 * @brief Vstupni bod programu
 * 
 * @param argc pocet argumentu
 * @param argv pole argumentu
 * @return int navratova hodnota programu
 */
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //vytvoreni socketu
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 10000 );
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("bind failed. Error\n");
        return 1;
    }
    printf("bind done\n");

    listen(socket_desc , 3);

    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted\n");
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            printf("could not create thread\n");
        }
        printf("Handler assigned\n");
    }
     
    if (client_sock < 0)
    {
        printf("accept failed\n");
        return 1;
    }
     
    return 0;
}
 
/**
 * @brief Funkce je spustena ve vlastnim vlakne a obstarava jednoho klienta, zpracovava prijmute zpravy
 * 
 * @param socket_desc socket uzivatele
 * @return void* 
 */
void *connection_handler(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[CLIENT_MSG_SIZE];
 
    //prijimani zpravy
    while( (read_size = recv(sock , client_message , CLIENT_MSG_SIZE , 0)) > 0 )
    {
        //end of string marker
        client_message[read_size] = '\0';
        printf("server prijmul: %s\n", client_message);	
		
        int i = 0;
        char *p = strtok(client_message, ";");
        char *params[3];

        while (p != NULL)
        {
            params[i++] = p;
            p = strtok(NULL, ";");
        }

        if (strcmp(params[1], CLIENT_SET_PIXEL) == 0){

        }
        else if (strcmp(params[1], CLIENT_GET_ITEM) == 0){

        }
        else if (strcmp(params[1], CLIENT_SEND_QUESS) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_CREATE_GAME) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_CONNECT_TO_GAME) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_RECONNECT) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_LEAVE) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_GET_POINTS) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_GET_ID) == 0){
            
        }
        else if (strcmp(params[1], CLIENT_CANCEL_GAME) == 0){
            
        }

	    memset(client_message, 0, CLIENT_MSG_SIZE);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 