#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#define PORT 8090

using namespace std;
char * print_ip(){
    int n;
    struct ifreq ifr;
    char array[] = "eno1";  //Will we always be using this?? TODO
 
    n = socket(AF_INET, SOCK_DGRAM, 0);
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
    //display result
    //printf("IP Address is %s - %s\n" , array , inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) );
    return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}

int server() {
    int server_fd, new_socket, valread;
    struct sockaddr_in cli_addr, my_addr;
    int addrlen = sizeof(cli_addr);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(0);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(my_addr);
    getsockname(server_fd, (struct sockaddr *)&my_addr, &len);
    unsigned int myPort = ntohs(my_addr.sin_port);

    char *ip = print_ip();
    int port = (int)ntohs(cli_addr.sin_port);
    printf("Waiting for a connection on %s port %d\n", ip, port);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Found a friend! You receive first.\n");

    while (true) {
        valread = recv(new_socket, buffer, 1024, 0);
        printf("Friend: %s\n", buffer);
        char userMessage[1000];
        printf("You: ");
        scanf("%s", userMessage);
        int msgLength = strlen(userMessage);
        while (msgLength > 140) {
            printf("Error: message too long!\n");
            scanf("%s", userMessage);
            msgLength = strlen(userMessage);
        }
        // fgets(userMessage, 140, stdin); //TODO : Error checking here if the msg is over 140 characters

        send(new_socket, userMessage, strlen(userMessage), 0);
    }

    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int client(const char *ip, const int portNum) {
    printf("Connecting to server...\n");
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNum);

    // Load in correct address
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    char *ip2 = inet_ntoa(serv_addr.sin_addr);
    client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (client_fd < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected!\n");
    printf("Connected to a friend! You send first.\n");

    // Create a buffer to read user input
    while (true) {
        char userMessage[140];
        printf("You: ");
        scanf("%s", userMessage);
        int msgLength = strlen(userMessage);
        while (msgLength > 140) {
            printf("Error: message too long!\n");
            scanf("%s", userMessage);
            msgLength = strlen(userMessage);
        }
        // fgets(userMessage, 140, stdin);   //The SAFE way to do it, but not possible here??
        send(sock, userMessage, strlen(userMessage), 0);
        valread = read(sock, buffer, 1024);
        printf("Friend: %s\n", buffer);
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    const char *ipAddr;
    int portNum;
    int pFlag = 0;
    int sFlag = 0;

    if (argc == 1) {
        server();
    }
    else {
        while ((opt = getopt(argc, argv, ":hp:s:")) != -1) {
            switch (opt) {
            case 'p':
                portNum = atoi(optarg);
                pFlag = 1;
                printf("p flag %d \n", portNum);
                break;
            case 's':
                ipAddr = optarg;
                sFlag = 1;
                printf("s flag %s \n", ipAddr);
                break;
            case 'h':
                printf("Usage for server: %s\nUsage for client: %s -s ipAddress -p port\n", argv[0], argv[0]);
                return 0;
                break;
            case '?':
                printf("Error: Unknown flag %c \nUsage for server: %s\nUsage for client: %s -s ipAddress -p port\n", optopt, argv[0], argv[0]);
                return 0;
                break;
            }
        }
        if(sFlag != 1) { //Still need to sanity check IP input
            printf("Error: Missing IP address\nUsage for server: %s\nUsage for client: %s -s ipAddress -p port\n", argv[0], argv[0]);
            return 0;
        }
        if(pFlag != 1) { 
            printf("Error: Missing port\nUsage for server: %s\nUsage for client: %s -s ipAddress -p port\n", argv[0], argv[0]);
            return 0;
        }
        if(portNum == 0) { 
            printf("Error: Invalid port\nUsage for server: %s\nUsage for client: %s -s ipAddress -p port\n", argv[0], argv[0]);
            return 0;
        }
        
        client(ipAddr, portNum);
    }
    return 0;
}
