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

//unsigned char packet[144];

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

typedef struct packet{
    uint16_t version;
    uint16_t length;
    char* msg;
} packet;

void msg_as_packet(unsigned char * buffer, char * msg){
    printf("STARTING MSG AS PACKET\n");
    printf("msg is what? %s\n", msg);
    //memcpy(packet, (void *)htons(457), 2);


    // char length[2];
    // sprintf(length,  "%d", htons(strlen(msg)));
    //Host to network
    packi16(buffer, 457);
    buffer += 2;
    packi16(buffer, strlen(msg));
    buffer += 2;


    memcpy(buffer, msg, strlen(msg));
    printf("in function, final packet is %s\n", buffer);
    buffer -= 4 + strlen(msg);

    // packet[0] = version[0];
    // packet[1] = version[1];

    // printf("about to hit for loop\n");
    // printf("user messag eis %s", msg);
    // printf("packet now looks like %s\n", packet);
    // for(int i = 0; i < strlen(msg); i++){
    //     printf("putting %c into buffer at index %d\n", msg[i], i);
    //     packet[i + 4] = msg[i];
    //     printf("%c\n", packet[i+4]);
        
    // }
    // printf("final packet is %s\n", packet);
    // for(int i = 0; i < sizeof(packet); i++){
    //     printf("%c", packet[i]);
    // }

}

packet pack_msg(char* msg){
    packet my_packet;
    my_packet.version = htons(457);
    my_packet.length = htons(strlen(msg));
    my_packet.msg = msg;
    return my_packet;
}

unsigned char * unpack(unsigned char* message){
    printf("Message is %s\n", message);
    int version = unpacki16(message);
    printf("version is %d\n", version);
    message += 2;
    printf("Message is %s\n", message);
    int length = unpacki16(message);
    message += 2;
    
    printf("Message is %s\n", message);
    return message;

}

int server() {
    int server_fd, new_socket, valread;
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
    printf("Waiting for a connection on %s port %d\n", ip, myPort);

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
        valread = recv(new_socket, buffer, 1024, 0);
        unsigned char* msg = unpack(buffer);
        printf("Friend: %s\n", msg);
        printf("You: ");
        // scanf("%[^\n]", userMessage);    
        msgLength = getline(&userMessage, &inputLen, stdin);
        userMessage[msgLength - 1] = '\0';
        msgLength--;
        // printf("SERVER: You entered %s, which has %d chars.\n", userMessage, msgLength);
        while (msgLength > 140) {
            printf("Error: message too long!\nYou: ");
            msgLength = getline(&userMessage, &inputLen, stdin);
            userMessage[msgLength - 1] = '\0';
            msgLength--;
        }
        // fgets(userMessage, 140, stdin); //TODO : Error checking here if the msg is over 140 characters

        unsigned char * packet = (unsigned char *) malloc(sizeof(unsigned char) * 144);
        msg_as_packet(packet, userMessage);

        send(new_socket, userMessage, strlen(userMessage), 0);
        free(userMessage);
        free(packet);
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
        unsigned char buffer[1024] = {0};
        char *userMessage = NULL;
        size_t inputLen = 0;
        int msgLength = 0;
        printf("You: ");
        msgLength = getline(&userMessage, &inputLen, stdin);
        userMessage[msgLength - 1] = '\0';
        msgLength--;

        // printf("CLIENT: You entered %s, which has %d chars.\n", userMessage, msgLength);
        while (msgLength > 140) {
            printf("Error: message too long!\nYou: ");
            msgLength = getline(&userMessage, &inputLen, stdin);
            userMessage[msgLength - 1] = '\0';
            msgLength--;
        }
        // fgets(userMessage, 140, stdin);   //The SAFE way to do it, but not possible here??
        unsigned char * packet = (unsigned char *) malloc(sizeof(unsigned char) * 144);
        printf("about to call msg as packet\n");
        msg_as_packet(packet, userMessage);
        printf("Packet looks like %s\n", packet);
        //packet my_packet = pack_msg(userMessage);
        send(sock, packet, strlen(userMessage) + 4, 0);
        free(userMessage);
        free(packet);
        valread = recv(sock, buffer, 1024, 0);
        unsigned char* msg = unpack(buffer);
        printf("Friend: %s\n", msg);
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
