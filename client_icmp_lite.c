#include<stdio.h>
#include<stdlib.h>              //malloc
#include<string.h>              //memset
#include<netinet/ip_icmp.h>     //icmp header
#include<netinet/ip.h>          //ip header (source,dest)
#include<sys/socket.h>          //socket
#include<arpa/inet.h>           //sokcet
#include<unistd.h>              //close

int get_cmd(char *payload, char *output, int size) {

    int i,j,len;
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
        output[j] = (unsigned char)payload[i];
        j++;
    }
    output[j] = 0x00;
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

    if((unsigned int)(icmp_head->type) == ICMP_ECHOREPLY) {
        strncpy(ip_dst, inet_ntoa(source.sin_addr), strlen(inet_ntoa(source.sin_addr)));    // return src ip
        strncpy(ip_src, inet_ntoa(dest.sin_addr), strlen(inet_ntoa(dest.sin_addr)));        // return dst ip   
    } else if((unsigned int)(icmp_head->type) == ICMP_ECHO) {
        strncpy(ip_src, inet_ntoa(source.sin_addr), strlen(inet_ntoa(source.sin_addr)));    // return src ip
        strncpy(ip_dst, inet_ntoa(dest.sin_addr), strlen(inet_ntoa(dest.sin_addr)));        // return dst ip  
    } else
        return 1;

    // return payload
    for(i=0 ; i < data_size ; i++) 
        if ((unsigned char)data[i] == 0x00)
            msg[i] = 0x01;
        else if ((unsigned char)data[i] == 0xff)
            msg[i] = 0xff;
        else if (data[i]>=32 && data[i]<=128)
            msg[i] = (unsigned char)data[i];
        else
            msg[i] = '.';
    msg[i] = 0x00; // null terminator

    return 0;
}

int main()
{
    int s_size , recv_size, st, cmd_st;
    struct sockaddr s_addr;
    struct iphdr *ip_head;
    int s;
    //char *ip_src, *ip_dst, *msg;
     
    unsigned char *buffer = (unsigned char *)malloc(128);                   // icmp packet size, usually 98
    char *ip_src = (char *) malloc(16);
    char *ip_dst = (char *) malloc(16);
    char *msg = malloc(64);
    char *cmd = malloc(64);

    s = socket(AF_INET , SOCK_RAW , IPPROTO_ICMP);                          // raw socket, ICMP only (TCP, UDP, RAW)
    if(s < 0) return 1; 
    s_size = sizeof s_addr;

    while(1)
    {
        recv_size = recvfrom(s , buffer , 128 , 0 , &s_addr , &s_size);     // receiving
        if(recv_size <0 ) return 2;

        ip_head = (struct iphdr*)buffer;
        if (ip_head->protocol == 1) {
            st = process_icmp(buffer , recv_size, ip_src, ip_dst, msg);

            if ((st == 0 ) && get_cmd(msg, cmd, strlen(msg)) == 0) {
                printf("Package found!\n");
                printf("From %s, to %s\n", ip_src, ip_dst);
                printf("Command: %s\n", cmd);
            }
        } else
            continue;

    }
    close(s);
    return 0;
}