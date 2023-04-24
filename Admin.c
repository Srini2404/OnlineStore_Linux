# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <string.h>
# include <netinet/in.h>
# include<time.h>
# include "items.h"
int main(int argc,char*argv[])
{
    int fd = open(argv[1],O_RDWR); // this shall store the products of that shop owner.
    int fd2 = open(argv[2],O_RDWR);
    struct product inven[100]; // this shall store the products that are present in the store.
    struct order ord[100]; // This is used to keep track of the orders of the seller.
    int idx = 0; // this is used for keeping track of the last product added(index).
    int odno = -1;
    for(int i=0;i<100;i++)
    {
        read(fd,&inven[i],sizeof(struct product));    
    }
    // printf("The first value is:%d\n",inven[0].id);
    for(int i=0;i<100;i++)
    {
        read(fd2,&ord[i],sizeof(struct order));
        if(ord[i].oid!=-1)
        {
            printf("The index where orderno !=1 %d\n",i);
            odno++;
        }
    }
    for(int i=0;i<100;i++)
    {
        if(inven[i].id!=-1){
            printf("The value where i!=-1 is\n");
            printf("%d\n",i);
        }
    }

    char menu='a';
    printf("Enter c for receiving the requests from client\n");
    printf("Enter s for receving the requests from terminal(seller side functions)\n");
    scanf("%c",&menu);
    if(menu=='c')
    {
        int sckfd = socket(AF_INET,SOCK_STREAM,0);
        if(sckfd==0)
        {
            perror(" ");
        }
        struct sockaddr_in sckd,cli;
        sckd.sin_addr.s_addr = INADDR_ANY;
        sckd.sin_family = AF_INET;
        sckd.sin_port = htons(8080);
        if(bind(sckfd,(struct sockaddr*)&sckd,sizeof(sckd))==-1)
        {
            perror(" ");
        }
        int newsd;
        listen(sckfd,5);
        int counter = 0;
        char temp[100];
        int val;
        while(1)
        {
            int clisize = sizeof(cli); 
            if((newsd=accept(sckfd,(struct sockaddr*)&cli,&clisize))==-1)
            {
                perror(" ");
            }
            // counter++;
            if(!fork())
            {
                read(newsd,&val,4);
                if(val==1)
                {
                    for(int i=0;i<100;i++)
                    {
                        if(inven[i].id!=-1)
                        {
                            if(inven[i].qty!=0)
                            {
                                write(newsd,&inven[i],sizeof(struct product));
                            }
                        }
                    }
                    struct product p1;
                    write(newsd,&p1,12);
                }
                else if(val==2)
                {
                    
                    int cusid;
                    read(newsd,&cusid,4);
                    int flg = 0;
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].cusid==cusid)
                        {
                            write(newsd,&ord[i],sizeof(struct order));
                            flg = 1;
                            break;
                        }
                    }
                    struct order o1;
                    write(newsd,&o1,sizeof(struct order)); 
                    if(flg)
                    {
                        printf("End of cart\n");      
                        write(newsd,"End of the cart\n",17);       
                    }
                    else
                    {
                        printf("Invalid CustomerID\n");
                        write(newsd,"Invalid customerID\n",20);
                    }
                }
                else if(val==3)
                {
                    int cusid;
                    struct order o1;
                    read(newsd,&cusid,4);
                    int noord;
                    read(newsd,&noord,4);
                    // Here we should first check if the number of products mentioned
                    // plus the number of items in the cart is less than or equal to 10;
                    int flg = 0;
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].cusid==cusid)
                        {
                            if(ord[i].status==0)
                            {
                                o1=ord[i];
                                flg = 1;
                                break;
                            }
                        }
                    }
                    if(flg==1)
                    {
                        if(noord<0)
                        {
                            printf("Error\n");
                            write(newsd,"Order Invalid\n",15);
                        }
                        else
                        {
                            if(o1.status!=1)
                            {
                                if(o1.pidx+noord>10)
                                {
                                    printf("Error\n");
                                    write(newsd,"Order Invalid\n",15);
                                }
                                else
                                {
                                    write(newsd,"1",1); // 1 means proceed forward
                                    int chk=0;
                                    // o1.pidx+=noord;
                                    while(noord--)
                                    {
                                        // struct product p1;
                                        char name[100];
                                        read(sckfd,name,100);
                                        int qty;
                                        read(sckfd,&qty,4);
                                        for(int i=0;i<100;i++)
                                        {
                                            if(inven[i].id!=-1)
                                            {
                                                if((strcpy(inven[i].name,name))==0)
                                                {
                                                    if(inven[i].qty>=qty)
                                                    {
                                                        o1.cart[o1.pidx++] = inven[i];
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    printf("Error Couldn't place the order\n");
                                                    chk = 1;
                                                    break;
                                                }
                                            }
                                        }
                                        if(chk)
                                        {
                                            break;
                                        }
                                        
                                    }
                                    if(!chk)
                                    {
                                        write(newsd,"Order Successfully Placed\n",27);
                                        printf("Order added to cart Successfully\n");
                                    }
                                    else
                                    {
                                        write(newsd,"Error Placing Order\n",21);
                                    }
                                }
                            }
                            else
                            {
                                write(newsd,"Order Invalid\n",15);
                                printf("Invalid Order\n");
                            }
                        }
                    }
                    else if(flg==0)
                    {
                        if(noord>10|| noord <=0)
                        {
                            printf("Error with data\n");
                            write(newsd,"Order Invalid\n",15);
                        }
                        else
                        {
                            write(newsd,"1",1);
                            char temp[100];
                            while(noord--)
                            {
                                read(sckfd,temp,100);
                                int qty;
                                read(sckfd,&qty,4);
                                
                                for(int i=0;i<100;i++)
                                {
                                    if(inven[i].id!=-1)
                                    {
                                        if((strcpy(inven[i].name,temp))==0)
                                        {
                                            if(inven[i].qty>=qty)
                                            {
                                                o1.cart[o1.pidx++] = inven[i];
                                                
                                                break;   
                                            }
                                        }
                                    }
                                }
                            }
                            struct flock fl;
                            fl.l_len = sizeof(struct order);
                            fl.l_whence = SEEK_SET;
                            fl.l_type = F_WRLCK;
                            
                            for(int i=0;i<100;i++)
                            {
                                if(ord[i].oid==-1)
                                {
                                    fl.l_start = (i)*(sizeof(struct order));
                                    lseek(fd2,(i)*sizeof(struct order),SEEK_SET);
                                    ord[i] = o1;
                                    write(fd2,&ord[i],sizeof(struct order));
                                    fl.l_type = F_UNLCK;
                                    sleep(3);
                                    fcntl(fd2,F_UNLCK,&fl); 
                                    write(newsd,"Order Successful\n",18);
                                    printf("A new order has been created successfully\n");
                                    break;
                                }
                            }
                        }
                    }
                    
                }
                else if(val==4)
                {
                    int cusid;
                    read(newsd,&cusid,4);
                    // char temp[100];
                    int oid;
                    read(newsd,&oid,4);
                    int pid;
                    read(newsd,&pid,4);
                    int qty;
                    read(newsd,&qty,4);
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].cusid==cusid)
                        {
                            if(ord[i].oid==oid)
                            {
                                for(int j=0;j<10;j++)
                                {
                                    if(ord[i].cart[j].id==pid)
                                    {
                                        if(qty==0)
                                        {
                                            ord[i].cart[j].qty= 0;
                                            write(newsd,"The product has been deleted\n",30);
                                            printf("The specified product has been sucessfully removed\n");
                                        }
                                        else if(qty<0)
                                        {
                                            printf("Invalid transaction\n");
                                            write(newsd,"Invalid transaction\n",21);
                                        }
                                        else 
                                        {
                                            if(ord[i].cart[j].qty>=qty)
                                            {
                                                ord[i].cart[j].qty = qty;
                                            }
                                            else
                                            {
                                                for(int i=0;i<100;i++)
                                                {
                                                    if(inven[i].id==pid)
                                                    {
                                                        if(inven[i].qty>=qty)
                                                        {
                                                            ord[i].cart[j].qty = qty;
                                                            printf("Successful\n");
                                                            write(newsd,"Successful\n",12);
                                                            break;
                                                        }
                                                        else
                                                        {
                                                            write(newsd,"Not possible\n",14);
                                                            printf("Sorry not possible\n");
                                                            break;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        break;
                                   }
                                }
                                break;
                            }
                            break;
                        }
                        break;
                    }
                }
                else if(val==5)
                {        
                    int orderid;
                    read(newsd,&orderid,4);
                    if(orderid==0)
                    {
                        // return ;
                    }
                    struct product ps[10];
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].oid == orderid)
                        {
                            for(int j=0;j<10;j++)
                            {
                                ps[j] = ord[i].cart[i];
                            }
                            break;
                        }
                    }
                    struct flock fl;
                    // fl.l_start = SEEK_SET;
                    fl.l_whence = SEEK_SET;
                    struct product p1;
                    // fl.l_len = 0;
                    fl.l_type = F_WRLCK;
                    fl.l_len = sizeof(struct product);
                    // fcntl(fd,F_SETLKW,&fl);
                    int arr[10];
                    int idx = 0;
                    int flg = 0;
                    int curidx[10];
                    struct product prod[10];
                    while(read(fd,&p1,sizeof(struct product)!=EOF))
                    {
                        for(int i=0;i<10;i++)
                        {
                            if(p1.id==ps[i].id)
                            {
                                if(p1.qty>=ps[i].qty)
                                {
                                    prod[idx] = ps[i];
                                    prod[idx].qty = p1.qty - ps[i].qty; 
                                    arr[idx] = p1.id;
                                    curidx[idx++] = lseek(fd,-sizeof(struct product),SEEK_CUR);
                                }
                                else
                                {
                                    flg = 1;
                                    break;
                                }
                            }
                        }
                        if(flg)
                        {
                            break;
                        }
                    }
                    if(flg)
                    {
                        printf("Order Couldn't be placed due to unavailability of stock\n");
                    }
                    else
                    {
                        srand(time(0));
                        write(newsd,"Enter the OTP shown on the screen",34);
                        int otp = rand();
                        write(newsd,&otp,4);
                        int cotp;
                        read(newsd,&cotp,4);
                        if(cotp==otp)
                        {
                            for(int i=0;i<10;i++)
                            {
                                fl.l_start = curidx[i];
                                fcntl(fd,F_SETLKW,&fl);
                                sleep(2);
                                write(fd,&prod[i],sizeof(struct product));
                                fl.l_type = F_UNLCK;
                                fcntl(fd,F_UNLCK);
                            }
                            for(int i=0;i<10;i++)
                            {
                                for(int j=0;j<100;j++)
                                {
                                    if(inven[j].id==arr[i])
                                    {
                                        inven[j].qty = prod[i].qty;
                                    }
                                }
                            }
                            printf("Transaction Successful\n");
                            write(newsd,"Transaction Successful",23);
                            
                        }
                        else
                        {
                            printf("Transaction Failed\n");
                            write(sckfd,"Transaction Failed\n",20);
                        }
                    }
                }
            }
            else
            {
                close(newsd);
            }
        }
    }
    else if(menu=='s')
    {
        int option;
        printf("The menu for seller side data updation \n");
        printf("---------------------------------------------------------------------------------\n");
        printf("Enter 1 to add a product\n");
        printf("Enter 2 to delete a product\n");
        printf("Enter 3 to update the price in the inventory\n");
        printf("Enter 4 to update the quantity of a product\n");
        printf("Enter 5 to exit\n");

        scanf("%d",&option);
        while(1)
        {
            if(option==1)
            {
                struct product p1;
                printf("Please enter the name of the product that you wish to be added\n");
                scanf("%s",p1.name);
                printf("Please enter the id, quantity and the price respectively for the above mentioned product\n");
                scanf("%d %d %d",&p1.id,&p1.qty,&p1.price);
                int flg = 0;
                int flg1 = 0;
                struct flock fl;
                fl.l_len = sizeof(struct product);
                fl.l_type = F_WRLCK;
                fl.l_whence = SEEK_SET;
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==p1.id)
                    {
                        printf("This is a redundant product\n");
                        flg1 = 1;
                        break;
                    }
                }
                if(!flg1){
                    for(int i=0;i<100;i++)
                    {
                        if(inven[i].id==-1)
                        {
                            fl.l_start = (i)*(sizeof(struct product));
                            // here try writing the record locking functionality and then update the following.
                            int k = lseek(fd,(i)*sizeof(struct product),SEEK_SET);
                            fcntl(fd,F_SETLKW,&fl);
                            printf("The file descrip is in :%d",k);
                            write(fd,&p1,sizeof(struct product));
                            lseek(fd,-sizeof(struct product),SEEK_CUR);
                            read(fd,&p1,sizeof(struct product));
                            printf("The updated value when we have finished writing is:%d\n",p1.id);
                            sleep(5);
                            inven[i] = p1;
                            fl.l_type = F_UNLCK;
                            fcntl(fd,F_UNLCK,&fl);
                            flg = 1;
                            break;
                        }
                    }
                }
                if(flg==0&&flg1==0)
                {
                    printf("Inventory full! couldn't add the new item.");
                }
                else if(flg==1 && flg1==0)
                {
                    printf("The added has been added Successfully\n");
                }
                // inven[idx++]=p1;
            }
            else if(option==2)
            {
                int id;
                struct flock fl;
                fl.l_len = sizeof(struct product);
                fl.l_whence = SEEK_SET;
                fl.l_type = F_WRLCK;
                printf("Enter the id of the product that has to be deleted\n");
                scanf("%d",&id);
                int flg = 0;
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        fl.l_start = (i)*(sizeof(struct product));
                        lseek(fd,(i)*sizeof(struct product),SEEK_SET);
                        fcntl(fd,F_SETLKW,&fl);
                        inven[i].id=-1;
                        write(fd,&inven[i],sizeof(struct product));
                        sleep(2);
                        flg = 1;
                        fl.l_type = F_UNLCK;
                        fcntl(fd,F_UNLCK,&fl);
                    }
                }
                if(!flg)
                {
                    printf("The given productID wasn't found\n");
                }
                else
                {
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].oid!=0)
                        {
                            for(int j=0;j<10;j++)
                            {
                                if(ord[i].cart[j].id==id)
                                {
                                    ord[i].cart[j].id==0;
                                    // invalid productID;
                                }
                            }
                        }
                    }
                    printf("The given item has been deleted successfully\n");
                }    
            }
            else if(option==3)
            {
                // use record locking here also.
                int id;
                printf("Enter the id of the product that has to be updated:\n");
                scanf("%d",&id);
                int price;
                int flg=0;
                struct flock fl;
                fl.l_len = sizeof(struct product);
                fl.l_whence = SEEK_SET;
                fl.l_type = F_WRLCK;
                printf("Enter the new price of the product:");
                scanf("%d",&price);
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        fl.l_start = (i)*(sizeof(struct product));
                        lseek(fd,(i)*sizeof(struct product),SEEK_SET);
                        fcntl(fd,F_SETLKW,&fl);
                        inven[i].price = price;
                        write(fd,&inven[i],sizeof(struct product));
                        sleep(2);
                        flg = 1;
                        fl.l_start = F_UNLCK;
                        fcntl(fd,F_UNLCK,&fl);
                        break;
                    }
                }
                if(!flg)
                {
                    printf("Invalid productID!");
                }
                else
                {
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].oid!=0)
                        {
                            for(int j=0;j<10;j++)
                            {
                                if(ord[i].cart[j].id==id)
                                {
                                    ord[i].cart[j].price = price;
                                }
                            }
                        }
                    }
                    printf("Price for the products has been updated\n");
                }
            }
            else if(option==4)
            {
                // use record locking here also.
                int id;
                printf("Enter the id of the product that has to be updated\n");
                scanf("%d",&id);
                int qty;
                int flg=0;
                struct flock fl;
                fl.l_len = sizeof(struct product);
                fl.l_whence = SEEK_SET;
                fl.l_type = F_WRLCK;
                printf("Enter the new qty of the product");
                scanf("%d",&qty);
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        fl.l_start = (i)*(sizeof(struct product));
                        lseek(fd,(i)*sizeof(struct product),SEEK_SET);
                        fcntl(fd,F_SETLKW,&fl);
                        inven[i].qty = qty;
                        sleep(2);
                        flg = 1;
                        fl.l_type = F_UNLCK;
                        fcntl(fd,F_UNLCK,&fl);
                        break;
                    }
                }
                if(!flg)
                {
                    printf("Invalid productID!");
                }
                else
                {
                    printf("The quantity for the products have been updated\n");
                }
            }
            else if(option==5)
            {
                printf("The final inventory array is:\n");
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id!=-1){
                        printf("The value where i!=-1 is\n");
                        printf("%d\n",i);
                    }
                }
                struct product p1;
                lseek(fd,0,SEEK_SET);
                int i=-1;
                while(read(fd,&p1,sizeof(struct product)))
                {
                    i++;
                    if(p1.id!=-1)
                    {
                        printf("%d\t%s\n",p1.id,p1.name);
                        printf("%d\t%d\n", inven[i].id,i);
                    }
                }   
                break;
            }
            else
            {
                printf("Invalid entry, try again later\n");
            }
            printf("The menu for server side data updation is\n");
            printf("---------------------------------------------------------------------------------\n");
            printf("Enter 1 to add a product\n");
            printf("Enter 2 to delete a product\n");
            printf("Enter 3 to update the price in the inventory\n");
            printf("Enter 4 to update the quantity of a product\n");
            printf("Enter 5 to exit the program\n");
            scanf("%d",&option);    
        }
    }
    else {
        printf("Invalid Input\n");
    }
    return 0;
}
