#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ifaddrs.h>            // getifaddrs
#include "server.h"
#include "ping.c"

void banner() {
    printf(GRN"        __________________________________________\n"RESET);
    printf(GRN"       /"MAG"                                         "GRN"/\n"RESET);
    printf(GRN"      /"MAG"          ___           __               "GRN"/\n"RESET);
    printf(GRN"     /"MAG"    ___ _ / _ \\ ___  ___/ /___ _ ____    "GRN"/\n"RESET);
    printf(GRN"    /"MAG"    / _ `// // // _ \\/ _  // _ `// __/   "GRN"/\n"RESET);
    printf(GRN"   /"MAG"     \\_, / \\___//_//_/\\_,_/ \\_,_//_/     "GRN"/\n"RESET);
    printf(GRN"  /"MAG"     /___/                               "GRN"/\n"RESET);
    //printf(GRN" /"MAG"                                  "BLU "Blue.  "GRN"/\n"RESET);
    printf(GRN" /_________________________________________"GRN"/\n"RESET);
    printf("                                          \n");
}

int get_ip(char *ip, char *ifname) {
    struct ifaddrs *addrs, *iface;
    getifaddrs(&addrs);
    iface = addrs;

    printf(CHCK"Interfaces:\n\n");
    while (iface) {
        if (iface->ifa_addr && iface->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)iface->ifa_addr;

            // exclude virtual NIC, loopback, and tunneling
            if (iface->ifa_name[0] != 'v' && iface->ifa_name[0] != 'l' && iface->ifa_name[0] != 't') {
                strcpy(ip, inet_ntoa(pAddr->sin_addr));
                strcpy(ifname, iface->ifa_name);
                printf(PASS);
            } else {
                printf(FAIL);
            }
            printf("%15s: \t%s\n", iface->ifa_name, inet_ntoa(pAddr->sin_addr));
        }

        iface = iface->ifa_next;
    }
    freeifaddrs(addrs);
}

void set_up(char *source, char *dest, char *iface, char *cmd)
{
    memset(&source[0], 0x00, sizeof(source));
    memset(&dest[0], 0x00, sizeof(dest));
    memset(&iface[0], 0x00, sizeof(iface));
    memset(&cmd[0], 0x00, sizeof(cmd));

    //strcpy(dest, "8.8.8.8");
    printf(CHCK "Client IP: ");
    fgets(dest, 64, stdin);
    fflush(stdin);

    get_ip(source,iface);

    printf("\n");
    printf(CHCK"Setting:\n\n");

    if (strlen(source) == 0)
        printf(FAIL);
    else
        printf(PASS);
    printf("%5s: %s\n", "Server", source);

    if (strlen(dest) == 0)
        printf(FAIL);
    else
        printf(PASS);
    printf("%5s: %s\n", "Client", dest);

    printf("\n");

}

int main(int argc , char *argv[])
{ 
    banner();
    char source[64];
    char dest[64];
    char iface[64];
    char cmd[512];
    int i;

    set_up(source, dest, iface, cmd);

    while (1) {
        printf(CYN "msg> " RESET);

        cmd[0] = 0xff;
        fgets(cmd + 1, sizeof cmd - 1, stdin);
        if (strlen(cmd) <= 2)
            continue;
        cmd[strlen(cmd)-1] = 0xff;

        // swap nibbles
        for (i=0; i<strlen(cmd); i++)
            cmd[i] = ((unsigned char)cmd[i] & 0x0F)<<4 | (unsigned char)(cmd[i] & 0xF0)>>4;

        if (ping(cmd, source, dest, iface) == 0)
            printf(PASS "Package sent!\n");
        else   
            printf(FAIL "Failed to sent package!\n");
    }
}