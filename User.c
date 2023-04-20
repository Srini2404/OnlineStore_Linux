# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
// Client Cod
// first run the server then run the client.
# include "items.h"
int main()
{
    int sckfd = socket(AF_INET,SOCK_STREAM,0);
    if(sckfd==-1)
    {
        perror(" ");
    }
    struct sockaddr_in sddr;
    sddr.sin_port = htons(8080);
    sddr.sin_family = AF_INET;
    sddr.sin_addr.s_addr = INADDR_ANY;
    int cntfd = connect(sckfd,(struct sockaddr*)&sddr,sizeof(sddr));
    if(cntfd==-1)
    {
        perror(" ");
    }
    // write(sckfd,"Hello there message from client1",33);

    int option;
    printf("Enter 1 to see all the products listed\n");
    printf("Enter 2 to see your cart\n");
    printf("Enter 3 to place your order\n");
    printf("Enter 4 to edit your cart\n");
    printf("Enter 5 to exit");
    while(1)
    {
        scanf("%d",&option);
        if(option==1)
        {
            int val = 1;
            write(sckfd,&val,4);
            // here we have to retrieve the data sent from the server and then print it.
            struct product p1;
            char temp;
            read(sckfd,&p1,sizeof(struct product));
            if(p1.id!=0)
            {
                printf("--------------------------------------------------------------------------\n");
                printf("\n");
            }
            while(p1.id!=0)
            {
                printf()
            }
        }
        else if(option==2)
        {
            int val = 2;
            write(sckfd,&val,4);
            int id;
            scanf("Please enter you customerID: %d",&id);
            write(sckfd,&id,4);
            // here we have to retrieve the data sent from the server and then print it.
        }
        else if(option==3)
        {
            int val = 3;
            write(sckfd,&val,4);
            char name[100];
            scanf("Enter the name of the product to be ordered:%s",name);
            write(sckfd,name,100);
            // think of what can be done here.
        }
        else if(option==4)
        {
            int val = 4;
            write(sckfd,&val,4);
            char name[100];
            scanf("Enter the name of the product which you wish to modify:%s",name);
            int qty;
            scanf("Enter the quantity you wish to order or enter -1 if you want to delete the product from your cart:%d",&qty);
            write(sckfd,&qty,4);
            // Here we should be getting confirmation from the server and then we should
            // proceed to the payment.
            scanf("Enter 1 to confirm the order and to proceed to payment: %d",&val);

        }
        else if(option==5)
        {
            break;
        }
    // getchar();
    }
    shutdown(sckfd,2);
    return 0;
}