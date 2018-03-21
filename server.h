#define RED     "\x1B[31m"
#define GRN     "\x1B[32m"
#define YEL     "\x1B[33m"
#define BLU     "\x1B[34m"
#define MAG     "\x1B[35m"
#define CYN     "\x1B[36m"
#define WHT     "\x1B[37m"
#define RESET   "\x1B[0m"
#define PASS    "\x1B[32m [+] \x1B[0m"
#define FAIL    "\x1B[31m [-] \x1B[0m"
#define CHCK    "\x1B[33m[*] \x1B[0m"

#define IP4_HDRLEN 20                   // IPv4 header length
#define ICMP_HDRLEN 8                   // ICMP header length for echo request, excludes data

uint16_t checksum (uint16_t *, int);
char *allocate_strmem (int);
uint8_t *allocate_ustrmem (int);
int *allocate_intmem (int);
int ping(char *msg, char *src, char *dst, char *iface);