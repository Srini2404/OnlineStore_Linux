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
    printf("Enter 5 to confirm the order\n");
    printf("Enter  6to exit");
    // repeat the above in the end of the while loop.
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
                printf("Product ID   ProductName ProductQty Price\n");
                printf("%d   %s    %d    %d\n",p1.id,p1.name,p1.qty,p1.price);
            }
            while(p1.id!=0)
            {
                read(sckfd,&p1,sizeof(struct product));
                printf("Product ID   ProductName ProductQty Price\n");
                printf("%d   %s    %d    %d\n",p1.id,p1.name,p1.qty,p1.price);
            }
        }
        else if(option==2)
        {
            int val = 2;
            write(sckfd,&val,4);
            int id;
            printf("Please enter you customerID:\n");
            scanf("%d",&id);
            write(sckfd,&id,4);
            struct order or;
            int idx = 0;
            struct order ora[10];
            read(sckfd,&or,sizeof(struct order));
            // if(or.oid!=0|| or.oid!=-1)
            while(ora[idx].oid!=0)
            {
                printf("--------------------------------------------\n");
                printf("OrderID CustomerID\n");
                printf(" %d   %d\n",or.oid,or.cusid);
                if(ora[0].cusid!=0){
                    printf("ProductID  ProductName  ProductQty  Price\n");
                }
                    // printf("%d    %s    %d  %d\n",ora[idx].cart->id,ora[idx].cart->name,ora[idx].cart->qty,ora[idx].cart);   
                for(int i=0;i<10;i++)
                {
                    printf("%d  %s  %d   %d\n",ora[idx].cart[i].id,ora[idx].cart[i].name,ora[idx].cart[i].qty,ora[idx].cart[i].price);
                }
                read(sckfd,&or,sizeof(struct order));
                    
            }
        }
        else if(option==3)
        {
            int val = 3;
            write(sckfd,&val,4);
            char name[100];
            int cusid;
            printf("Enter your customerID\n");
            scanf("%d",&cusid);
            write(sckfd,&cusid,4);
            int noord;
            printf("Enter the number of products to be ordered\n");
            scanf("%d",&noord);
            write(sckfd,&noord,4);
            while(noord--){
                printf("Enter the name of the product to be ordered:\n");
                scanf("%s",name);
                write(sckfd,name,100);
                int qty;
                printf("Enter the quantity of the product to be ordered:\n");
                scanf("%d",&qty);
                write(sckfd,&qty,4);
            }        
            // think of what can be done here.
        }
        else if(option==4)
        {
            int val = 4;
            write(sckfd,&val,4);
            // char name[100];
            // scanf("Enter the name of the product which you wish to modify:%s",name);
            int cusid;
            printf("Enter the customerID:\n");
            scanf("%d",&cusid);
            write(sckfd,&cusid,4);
            int oid;
            printf("Enter the orderID:\n");
            scanf("%d",&oid);
            write(sckfd,&oid,4);
            int pid;
            printf("Enter the productID:\n");
            scanf("%d",&pid);
            write(sckfd,&pid,4);
            int qty;
            printf("Enter the quantity you wish to order or enter 0 if you want to delete the product from your cart:\n");
            scanf("%d",&qty);
            write(sckfd,&qty,4);
            // Here we should be getting confirmation from the server and then we should
            // proceed to the payment.
            printf("Enter 1 to confirm the order and to proceed to payment , any other value will just save the changes in the order.:\n");
            scanf("%d",&val);
            // implement the add funcitonality of a completely new product.

        }
        else if(option==5)
        {
            int orderid;
            printf("Enter you orderID:\n");
            scanf("%d",&orderid);
            char temp[34];
            read(sckfd,temp,34);
            printf("%s",temp);
            int otp;
            read(sckfd,&otp,4);
            scanf("OTP:%d",&otp);
            write(sckfd,&otp,4);
            read(sckfd,&temp,23);
            printf("%s",temp);            
        }
        else if(option==6)
        {
            break;
        }
    // getchar();
    }
    shutdown(sckfd,2);
    return 0;
}