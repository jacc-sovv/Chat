#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8090

using namespace std;

int server(){
    int server_fd, new_socket, valread;
    struct sockaddr_in cli_addr;
    int addrlen = sizeof(cli_addr);
    char buffer[1024] = {0};


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    //can potentially remove this to use random ports and addresses and stuff
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    printf("Hostname: %s\n", hostname);

    struct hostent *gethostbyname(const char *name);

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(PORT);

    


    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&cli_addr,sizeof(cli_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }




    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    struct in_addr ipAddr = cli_addr.sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
    printf("%s\n", str);

    char *ip = inet_ntoa(cli_addr.sin_addr);
    int port = (int) ntohs(cli_addr.sin_port);
    printf("Waiting for a connection on %s port %d\n", ip, port);

    if ((new_socket= accept(server_fd, (struct sockaddr*)&cli_addr, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Found a friend! You receive first.\n");


    while (true){
        valread = recv(new_socket, buffer, 1024, 0);
        printf("Friend: %s\n", buffer);
        char userMessage[1000];
        printf("You: ");
        scanf("%s", userMessage);
        int msgLength = strlen(userMessage);
        while (msgLength > 140){
            printf("Error: message too long!\n");
            scanf("%s", userMessage);
            msgLength = strlen(userMessage);
        }
        //fgets(userMessage, 140, stdin); //TODO : Error checking here if the msg is over 140 characters


        send(new_socket, userMessage, strlen(userMessage), 0);
    }
 
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int client(const char* ip){
    printf("Connecting to server...\n");
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //Load in correct address
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    char *ip2 = inet_ntoa(serv_addr.sin_addr);
    client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (client_fd < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected!\n");
    printf("Connected to a friend! You send first.\n");

    //Create a buffer to read user input
    while (true){
        char userMessage[140];
        printf("You: ");
        scanf("%s", userMessage);
        int msgLength = strlen(userMessage);
        while (msgLength > 140){
            printf("Error: message too long!\n");
            scanf("%s", userMessage);
            msgLength = strlen(userMessage);
        }
        //fgets(userMessage, 140, stdin);   //The SAFE way to do it, but not possible here??
        send(sock, userMessage, strlen(userMessage), 0);
        valread = read(sock, buffer, 1024);
        printf("Friend: %s\n", buffer);
    }
 
    // closing the connected socket
    close(client_fd);
    return 0;
}

int main(int argc, char const* argv[]){

    
   if(argc == 1){
        
        server();
    }
    else{
        client(argv[1]);
    }
}
