typedef struct {
    const char* addr;
    int port;
    int bufferSize;
    int maxConnections;
    int serverFd;
} MailServer;

MailServer* CreateMailServer(int port);
int InitMailServer(MailServer* server);
void DestroyMailServer(MailServer* server);
int WaitForClient(MailServer* server);
int ReceiveData(MailServer* server, int client, char** outReq, char** outData);
void HandleRequest(MailServer* server, const char* request, const char* data, const char* sender);