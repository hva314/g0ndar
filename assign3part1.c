#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[]) {

   int page_table[] = {2, 4, 1, 7, 3, 5, 6};
   int fd_in, fd_out;

   unsigned long physical_addr, virtual_addr;                  // physical and virtual address
   unsigned long offset, page;                                 // offset of address and page number


   // error checking
   if (argc != 2) {
      perror("Usage: ./%s <sequence file>\n", argv[0]);
      exit(1);
   }
   if ((fd_in = open(argv[1], O_RDONLY)) == -1) {
      perror("Cannot open input file!");
      exit(1);
   }
   if ((fd_out = open("output-part1", O_WRONLY | O_CREAT, 0600)) == -1) {
      perror("Cannot open output file!");
      close(fd_in);
      exit(1);
   }
   
   // read in the input from file
   while (read(fd_in, &virtual_addr, sizeof(virtual_addr)) > 0) {
      
      page = page_table[virtual_addr >> 7];     // get the page number

      offset = virtual_addr & 0x7F;
      physical_addr = (page << 7) + offset;     // calculate the address in physical frames
      
      if (write(fd_out, &physical_addr, sizeof(physical_addr)) == -1) {
         perror("Cannot write to output file!");
         close(fd_out);
         exit(1);
      }
   }
   
   close(fd_in);
   close(fd_out);
   
   return 0;
}