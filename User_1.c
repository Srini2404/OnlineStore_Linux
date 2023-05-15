# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <string.h>
// Client Cod
// first run the server then run the client.
# include "items.h"
int main()
{
    int sckfd = socket(AF_INET,SOCK_STREAM,0);
    if(sckfd==-1)
    {
        perror(" ");
        return -1;
    }
    struct sockaddr_in sddr;
    sddr.sin_port = htons(8080);
    sddr.sin_family = AF_INET;
    sddr.sin_addr.s_addr = INADDR_ANY;
    int cntfd = connect(sckfd,(struct sockaddr*)&sddr,sizeof(sddr));
    if(cntfd==-1)
    {
        perror(" ");
        return -1;
    }
    // write(sckfd,"Hello there message from client1",33);

    int option;
    printf("Enter 1 to see all the products listed\n");
    printf("Enter 2 to see your cart\n");
    printf("Enter 3 to create your order(new cart)\n");
    printf("Enter 4 to edit your cart\n");
    printf("Enter 5 to confirm the order\n");
    printf("Enter 6 to close the shop and record the items in the receipt file.\n");
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
                if(p1.id!=0)
                {
                    printf("Product ID   ProductName ProductQty Price\n");
                    printf("%d   %s    %d    %d\n",p1.id,p1.name,p1.qty,p1.price);
                }
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
            // struct order ora[10];
            read(sckfd,&or,sizeof(struct order));
            // if(or.oid!=0|| or.oid!=-1)
            while(or.oid!=0)
            {
                printf("--------------------------------------------\n");
                printf("OrderID CustomerID\n");
                printf(" %d   %d\n",or.oid,or.cusid);
                if(or.cusid!=0){
                    printf("ProductID  ProductName  ProductQty  Price\n");
                }
                    // printf("%d    %s    %d  %d\n",ora[idx].cart->id,ora[idx].cart->name,ora[idx].cart->qty,ora[idx].cart);   
                for(int i=0;i<10;i++)
                {
                    if (or.cart[i].id != 0)
                    printf("%d  %s  %d   %d\n",or.cart[i].id,or.cart[i].name,or.cart[i].qty,or.cart[i].price);
                }
                read(sckfd,&or,sizeof(struct order));
                
            }
            char t[25];
            read(sckfd,t,25);
            printf("%s",t);
        }
        else if(option==3)
        {
            int val = 3;
            write(sckfd,&val,4);
            char name[100];
            int cusid;
            printf("Enter your customerID\n");
            scanf("%d",&cusid);
            if(write(sckfd,&cusid,4)==-1)
            {
                printf("Error\n");
            }
            int noord;
            printf("Enter the number of products to be ordered\n");
            scanf("%d",&noord);
            write(sckfd,&noord,4);
            char temp[50];
            read(sckfd,temp,50);
            if(strcmp(temp,"Order Invalid\n")==0)
            {
                printf("The following number of items can't be ordered\n");
            }
            else
            {
                printf("Entered Here\n");
                while(noord--){
                    printf("Enter the name of the product to be ordered:\n");
                    scanf("%s",name);
                    write(sckfd,name,100);
                    int qty;
                    printf("Enter the quantity of the product to be ordered:\n");
                    scanf("%d",&qty);
                    write(sckfd,&qty,4);
                }
            }
            printf("Out\n");
            read(sckfd,temp,50);
            printf("%s",temp);        
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

            char temp[60];
            // if(strcpy(temp,"Successfuly")==0|| strcpy(temp,"The product has been deleted\n")==0)
            // {
            //     // Here we should be getting confirmation from the server and then we should
            //     // proceed to the payment.
            //     printf("%s",temp);
            //     printf("Enter 1 to confirm the order and to proceed to payment , any other value will just save the changes in the order.:\n");
            //     scanf("%d",&val);
            //     // implement the add funcitonality of a completely new product.
            // }
            // else
            // {
            //     printf("%s",temp);
            read(sckfd,temp,60);
            printf("%s",temp);
            
        }
        else if(option==5)
        {
            write(sckfd,&option,4);
            int orderid;
            printf("Enter you orderID:\n");
            scanf("%d",&orderid);
            write(sckfd,&orderid,4);
            char temp[34];
            read(sckfd,temp,34);
            if(strcmp(temp,"Error\n")==0)
            {
                printf("Invalid OrderID\n");
                continue;
            }
            else if(strcmp(temp,"Error1\n")==0)
            {
                printf("Order Couldn't be placed due to unavailability of stock\n");
            }
            else if(strcmp(temp,"Proceed\n")==0)
            {
                printf("%s",temp);
                struct product p;
                p.id = -1;
                int totalamount=0;
                // int i=0
                // read(sckfd,&p,sizeof(struct product));
                // printf("Here id = %d\n", p.id);
                while(p.id!=0){
                    // printf("p.id before = %d\n", p.id);
                    read(sckfd,&p,sizeof(struct product));
                    // printf("p.id: %d\n", p.id);
                    if (p.id != 0){
                        printf("Product ID: %d\n",p.id);
                        printf("Product Name: %s\n",p.name);
                        printf("Product Quantity : %d\n",p.qty);
                        printf("Product Price : %d\n",p.price);
                        totalamount+=p.price*p.qty;
                    }
                    else{
                        break;
                    }   
                }
                printf("The total price for the order is:%d\n",totalamount);
                // read(sckfd,&temp,34);
                // printf("%s",temp);
                int otp;
                // read(sckfd,&otp,4);
                printf("Enter 1 to confirm the order\n");
                // printf("%d\n",otp);
                scanf("%d",&otp);
                write(sckfd,&otp,4);
                read(sckfd,&temp,34);
                printf("%s",temp);
            }            
        }
        else
        {
            printf("Invalid option try again\n");
        }

        printf("Enter 1 to see all the products listed\n");
        printf("Enter 2 to see your cart\n");
        printf("Enter 3 to create your order(new cart) or to add products to an your existing cart\n");
        printf("Enter 4 to edit your cart\n");
        printf("Enter 5 to confirm the order\n");
    }
    shutdown(sckfd,2);
    return 0;
}