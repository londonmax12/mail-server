#include "mail_server.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

MailServer* CreateMailServer(int port) {
    MailServer* server = malloc(sizeof(MailServer));

    server->addr = "localhost";
    server->port = port;
    server->bufferSize = 11024;
    server->maxConnections = 1;

    return server;
}

int InitMailServer(MailServer* server) {
    server->serverFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;

    if (server->serverFd == -1)
    {
        perror("[!] Failed to create socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(server->serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("[!] Failed to set sock");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server->port);

    if (bind(server->serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("[!] Failed to bind socket");
        return 1;
    }

    if (listen(server->serverFd, server->maxConnections) < 0) {
        perror("[!] Failed to listen on socket");
        return 1;
    }

    return 0;
}

void DestroyMailServer(MailServer* server) {
    if (!server)
        return;
    
    if (server->serverFd)
        close(server->serverFd);

    free(server);
}
int WaitForClient(MailServer* server) {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    if (server->serverFd == -1) {
        perror("[!] Failed to wait for client: Server was not initialized");
        return 1;
    }

    int newClient = accept(server->serverFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    
    if (newClient < 0) {
        perror("[!] Failed to establish connection with client");
        return 1;
    }

    return newClient;
}

int ReceiveData(MailServer* server, int client, char** outReq, char** outData) {
    char reqBuf[128];
    char dataBuf[server->bufferSize];

    const char* message = "220 STMP SERVER\r\n";
    ssize_t bytesSent = send(client, message, strlen(message), 0);
    if (bytesSent == -1) {
        perror("[!] Error sending data");
        return 1;
    }

    ssize_t reqReceived = recv(client, reqBuf, sizeof(reqBuf), 0);
    if (reqReceived <= 0) {
        if (reqReceived == 0) {
            printf("[*] Connection closed by the client.\n");
        } else {
            perror("[!] Failed to receive data from client");
        }

        return 1;
    }

    bytesSent = send(client, message, strlen(message), 0);
    if (bytesSent == -1) {
        perror("Error sending data");
        exit(EXIT_FAILURE);
    }

    ssize_t dataReceived = recv(client, dataBuf, sizeof(dataBuf), 0);
    if (dataReceived <= 0) {
        if (dataReceived == 0) {
            printf("[*] Connection closed by the client.\n");
        } else {
            perror("[!] Failed to receive data from client");
        }

        return 1;
    }

    reqBuf[reqReceived] = '\0';
    dataBuf[dataReceived] = '\0';

    strcpy(*outReq, reqBuf);
    strcpy(*outData, dataBuf);

    return 0;
}