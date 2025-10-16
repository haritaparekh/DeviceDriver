#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>



int main() {


   int fd;
   const char *w_buff = "Hello PCD Driver!!";
   char *r_buff;
   ssize_t w_bytes;
   ssize_t r_bytes;
   fd = open("/dev/pcd", O_RDWR);
   if(fd == -1) {
       printf("Error opening device : %s\n", strerror(errno));
   } else{
       w_bytes = write(fd, w_buff, strlen(w_buff));

       if(w_bytes == strlen(w_buff)){
            printf("All data written successfully\n");

            if(EINVAL != (lseek(fd, 0, SEEK_SET))){
                r_bytes = read(fd, r_buff, w_bytes);
                printf("Reading the written data: %s\n", r_buff);
                    }
       }else if(w_bytes < strlen(w_buff)){
        printf("Not all bytes written\n");

        if(EINVAL != (lseek(fd, 0, SEEK_SET))){
            r_bytes = read(fd, r_buff, w_bytes);
            printf("Reading the written data: %s\n", r_buff);
                    }
       }else{
        printf("Error while write\n %s",strerror(errno));
       }
       
       close(fd);
       while(1){
       }
   }
       return 0;
}

