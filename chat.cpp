#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

using namespace std;

int server(){
//printf("in server\n");
    int server_fd, new_socket, valread;
    struct sockaddr_in cli_addr;
    int opt = 1;
    int addrlen = sizeof(cli_addr);
    char buffer[1024];
    char* hello = "Hello from server";




    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
//can potentially remove this to use random ports and addresses and stuff
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(PORT);





 
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&cli_addr,
             sizeof(cli_addr))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }




    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char *ip = inet_ntoa(cli_addr.sin_addr);
    int port = (int) ntohs(cli_addr.sin_port);
    printf("Waiting for a connection on %s port %d\n", ip, port);

    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&cli_addr,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }



    valread = recv(new_socket, buffer, 1024, 0);
    printf("%s\n", buffer);

    char userMessage[140];
    printf("What would you like to say?\n");
    fgets(userMessage, 140, stdin);


    send(new_socket, userMessage, strlen(userMessage), 0);
    printf("Hello message sent\n");
 
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int client(const char* ip){
    printf("test\n");
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
    printf("%s\n", ip);
    printf("1\n");
    //Load in correct address
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    char *ip2 = inet_ntoa(serv_addr.sin_addr);
    printf("server_addr is %s\n", ip2);
    printf("about to connect\n");
    client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("connected\n");

    if (client_fd < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    //Create a buffer to read user input
    char userMessage[140];
    printf("What would you like to say?\n");
    fgets(userMessage, 140, stdin);


    send(sock, userMessage, strlen(userMessage), 0);
    printf("Hello message sent:\n");



    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
 
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
