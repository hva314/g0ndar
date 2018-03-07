#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int call(int port) {
    struct sockaddr_in cc , client;
    char *m = "burn";

    int r = socket(AF_INET , SOCK_STREAM , 0);
    if (r == -1) {
        fprintf(stderr, "Cannot create socket");
        close(r);
        return 1;
    }

    cc.sin_family = AF_INET;
    cc.sin_addr.s_addr = INADDR_ANY;
    cc.sin_port = htons(port);

    int st_r = bind(r ,(struct sockaddr *)&cc , sizeof(cc));
    if (st_r < 0) {
        fprintf(stderr, "Cannot connect");
        return 1;
    }

    listen(r , 3);

    int sock_size = sizeof(struct sockaddr_in);

    int cl = accept(r, (struct sockaddr *)&client, (socklen_t*)&sock_size);
    if (cl < 0) {
        fprintf(stderr,"accept failed");
        return 1;
    }

    write(cl , m , strlen(m));
    return 0;
}

int main(int argc , char *argv[])
{
    call(4444);
    return 0;
}
