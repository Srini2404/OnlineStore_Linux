# include <stdio.h>
# include <stdlib.h>
# include "items.h"
# include <fcntl.h>
# include <unistd.h>
void printOrder(struct order o1)
{
    printf("The orderid is:%d\n",o1.oid);
    printf("The customerId is :%d\n",o1.cusid);
}
int main(int argc, char* argv[])
{ 
    int fd = open(argv[1],O_RDWR);
    struct order o1[100];
    lseek(fd,0,SEEK_SET);
    for(int i=0;i<100;i++)
    {
        read(fd,&o1[i],sizeof(struct order));
        if(o1[i].oid!=-1)
        {
            printf("The index where orderno !=1 %d\n",i);
            // printf("The customerID of the order is :%d\n",ord[i])
            printOrder(o1[i]);
        }
    }
}