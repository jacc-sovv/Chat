/*******************************************
* Group Name  : xXXXXx

* Member1 Name: John McFall
* Member1 SIS ID: 832435619
* Member1 Login ID: jmcf521

* Member2 Name: Jack Sovereign
* Member2 SIS ID: 832430250
* Member2 Login ID: jacc
********************************************/

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

using namespace std;
char * print_ip(){
    int n;
    struct ifreq ifr;
    char array[] = "eno1"; 
 
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

void packi16(unsigned char *buf, unsigned int i)
{
    *buf++ = i>>8; *buf++ = i;
}

int unpacki16(unsigned char *buf)
{
    unsigned int i2 = ((unsigned int)buf[0]<<8) | buf[1];
    int i;

    // change unsigned numbers to signed
    if (i2 <= 0x7fffu) { i = i2; }
    else { i = -1 - (unsigned int)(0xffffu - i2); }

    return i;
}

void msg_as_packet(unsigned char * buffer, char * msg){

    //Host to network
    packi16(buffer, 457);
    buffer += 2;
    packi16(buffer, strlen(msg));
    buffer += 2;


    memcpy(buffer, msg, strlen(msg));

    buffer -= 4 + strlen(msg);
}

unsigned char * unpack(unsigned char* message){
    //Network to host
    int version = unpacki16(message);
    if(version != 457){
        printf("ERROR : VERSION NOT 457, EXITTING");
        exit(1);
    }
    message += 4;
    return message;
}

int server() {
    int server_fd, new_socket;
    struct sockaddr_in cli_addr, my_addr;
    int addrlen = sizeof(cli_addr);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(0);

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
    printf("Welcome to Chat!\nWaiting for a connection on %s port %d\n", ip, myPort);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&cli_addr, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Found a friend! You receive first.\n");

    while (true) {
        unsigned char buffer[1024] = {0};
        char *userMessage = NULL;
        size_t inputLen = 0;
        int msgLength = 0;
        recv(new_socket, buffer, 1024, 0);
        unsigned char* msg = unpack(buffer);
        printf("Friend: %s", msg);
        printf("\nYou: "); 
        msgLength = getline(&userMessage, &inputLen, stdin);
        userMessage[msgLength - 1] = '\0';
        msgLength--;
        while (msgLength > 140) {
            printf("Error: Input too long.\nYou: ");
            msgLength = getline(&userMessage, &inputLen, stdin);
            userMessage[msgLength - 1] = '\0';
            msgLength--;
        }
        unsigned char * packet = (unsigned char *) malloc(sizeof(unsigned char) * 144);
        msg_as_packet(packet, userMessage);
        send(new_socket, packet, strlen(userMessage)+4, 0);
        free(userMessage);
        free(packet);
    }
    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

int client(const char *ip, const int portNum) {
    printf("Connecting to server...\n");
    int sock = 0, client_fd;
    struct sockaddr_in serv_addr;
    
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
    inet_ntoa(serv_addr.sin_addr);
    client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (client_fd < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected!\n");
    printf("Connected to a friend! You send first.");

    // Create a buffer to read user input
    
    while (true) {
        unsigned char buffer[1024] = {0};
        char *userMessage = NULL;
        size_t inputLen = 0;
        int msgLength = 0;
        printf("\nYou: ");
        msgLength = getline(&userMessage, &inputLen, stdin);
        userMessage[msgLength - 1] = '\0';
        msgLength--;
        while (msgLength > 140) {
            printf("Error: Input too long.\nYou: ");
            msgLength = getline(&userMessage, &inputLen, stdin);
            userMessage[msgLength - 1] = '\0';
            msgLength--;
        }
        unsigned char * packet = (unsigned char *) malloc(sizeof(unsigned char) * 144);
        msg_as_packet(packet, userMessage);
        send(sock, packet, strlen(userMessage) + 4, 0);
        free(userMessage);
        free(packet);
        recv(sock, buffer, 1024, 0);
        unsigned char* msg = unpack(buffer);
        printf("Friend: %s", msg);
    }

    // closing the connected socket
    close(client_fd);
    return 0;
}

int ipValidator(const char* ipToCheck) {
    char validChars[11] = {'0','1','2','3','4','5','6','7','8','9','.'};
    bool flag = false;
    int pCount = 0;
    for(int i = 0; i < (int)sizeof(ipToCheck); i++){
        for(int j = 0; j < (int)sizeof(validChars); j++){
            if(&ipToCheck[i] == &validChars[j]){
                flag = true;
                if(&ipToCheck[i] == &validChars[10]){
                    pCount++;
                }
            } 
        }
        if(flag){
            flag = false;
        } else {
            return false;
        }
    }
    if(pCount != 3) return false;
    return true;
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
                break;
            case 's':
                ipAddr = optarg;
                sFlag = 1;
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
        if(sFlag != 1) { 
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
        if(ipValidator(ipAddr)) { 
            printf("Error: Invalid IP address\nUsage for server: %s\nUsage for client: %s -s ipAddress -p port\n", argv[0], argv[0]);
            return 0;
        }
        client(ipAddr, portNum);
    }
    return 0;
}
