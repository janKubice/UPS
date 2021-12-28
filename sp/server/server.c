#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 10000
#define SA struct sockaddr
   
//Funkce která bude poslouchat dotazům
void* listening(int sockfd)
{
    printf(sockfd);
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        //Přečte zprávu od klienta a uloží jí to buff
        if (strlen(buff) != 0){
            read(sockfd, buff, sizeof(buff));
            printf("from client: %s", buff);
            
            //TODO zde se bude možnost ptát co přišlo a podle toho se zařídit

            printf("To client: ");
            bzero(buff,MAX);
            n = 0;
            while ((buff[n++] == getchar()) != '\n');
            write(sockfd, buff, sizeof(buff));
        }
    }
}
   
//hlavní funkce
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    //Vytvoření soketu a verifikace 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    //Nastavení IP a portu
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    //Binding nového soketu s danou IP a verifikace
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    //server je ready na poslouchání, verifikace zda OK
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   

    // Přijme data paket od klienta a varifikuje
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    //Vytvoření vlákna a naslouchání dotazům klienta
    pthread_t thread;
    pthread_create(&thread, NULL, listening, &connfd);
    pthread_join(thread, NULL);
    
    // Nakonec zavření paketu
    close(sockfd);
}
