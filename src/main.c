#include <stdio.h>

#include "mail_server.h"

int main() {
    int port = 587;
    MailServer* mailServer = CreateMailServer(port);

    printf("[*] Initializing mail server\n");
    InitMailServer(mailServer);

    printf("[*] Listening on port %d...\n", port);
    while (1) {
        printf("[*] Waiting for connection...\n");
        int client = WaitForClient(mailServer);
        printf("[*] New connection established\n");
        char req[128];
        char data[mailServer->bufferSize];

        char* reqPtr = req;
        char* dataPtr = data;
        ReceiveData(mailServer, client, &reqPtr, &dataPtr);
        printf("%s, %s", req, data);
    }
    printf("[*] Destroying mail server\n");
    DestroyMailServer(mailServer);
    return 0;
}