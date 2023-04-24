# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include "items.h"
int main(int argc,char*argv[])

{
    int fd = open(argv[1],O_RDWR);
    int fd1 = open(argv[2],O_RDWR);
    struct product p1;
    p1.id = -1;
    struct order o1;
    o1.oid = -1;
    lseek(fd,0,SEEK_SET);
    lseek(fd1,0,SEEK_SET);
    // for(int i=0;i<100;i++)
    // {
    //     write(fd,&p1,sizeof(struct product));
    // }
    for(int i=0;i<100;i++)
    {
        write(fd1,&o1,sizeof(struct order));
    }
    return 0;
}