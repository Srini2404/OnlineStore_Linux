# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <string.h>
# include <netinet/in.h>
# include<time.h>
# include "items.h"
// void printOrder(struct order o1)
// {
//     printf("The orderid is:%d\n",o1.oid);
//     printf("The customerId is :%d\n",o1.cusid);
//     for(int i=0;i<10;i++)
//     {
//         printf("The productID of the cart is:%d\n",o1.cart[i].id);
//     }    
// }
int main(int argc,char*argv[])
{
    int fd = open(argv[1],O_RDWR); // this shall store the products of that shop owner.
    int fd2 = open(argv[2],O_RDWR);
    struct product inven[100]; // this shall store the products that are present in the store.
    struct order ord[100]; // This is used to keep track of the orders of the seller.
    int idx = 0; // this is used for keeping track of the last product added(index).
    int odno = 1;
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
            // printf("The index where orderno !=1 %d\n",i);
            // printOrder(ord[i]);
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
        int opt=1;
        if(setsockopt(sckfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1)
        {
            perror(" ");
        }
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
                while (1){

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
                        printf("The customerID received:%d\n",cusid);
                        int flg = 0;
                        for(int i=0;i<100;i++)
                        {
                            if(ord[i].cusid==cusid)
                            {
                                
                                printf("Here\n");
                                // for(int i=0;i<100;i++)
                                // {
                                    // if(ord[i].oid!=-1)
                                    // {
                                    //     printf("index:%d\n",i);
                                    //     printf("orderID:%d\n",ord[i].oid);
                                    // }
                                    // if(ord[i].cusid==cusid)
                                    // {
                                        for(int j=0;j<10;j++)
                                        {
                                            // if(ord[i].cart[j].id)
                                            printf("The productIDs are :%d\n",ord[i].cart[j].id);
                                            printf("The productname is: %s\n",ord[i].cart[j].name);
                                        }
                                    // }
                                
                                write(newsd,&ord[i],sizeof(struct order));
                                flg = 1;
                                // break;
                            }
                        }
                        struct order o1;
                        o1.oid=0;
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
                        printf("Hello there\n");
                        int cusid;
                        struct order o1;
                        printf("Reading customerID\n");
                        int c = read(newsd,&cusid,4);
                        // printf("The number of bytes read is:%d",c);
                        printf("The customerID %d\n",cusid);
                        int noord;
                        read(newsd,&noord,4);
                        printf("The number of orders is:%d\n",noord);
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
                                                    if((strcmp(inven[i].name,name))==0)
                                                    {
                                                        if(inven[i].qty>=qty)
                                                        {
                                                            strcpy(o1.cart[o1.pidx].name,inven[i].name);
                                                            o1.cart[o1.pidx].id = inven[i].id;
                                                            o1.cart[o1.pidx].price = inven[i].price;
                                                            o1.cart[o1.pidx].qty = qty;
                                                            o1.pidx++;
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
                            if(noord>10 || noord <=0)
                            {
                                printf("Error with data\n");
                                write(newsd,"Order Invalid\n",15);
                            }
                            else
                            {
                                write(newsd,"1",1);
                                printf("Creating new cart\n");
                                char temp[100];
                                while(noord--)
                                {
                                    printf("Noord value:%d\n",noord);
                                    read(newsd,temp,100);
                                    printf("%s\n",temp);
                                    int qty;
                                    read(newsd,&qty,4);
                                    printf("The quantity is:%d\n",qty);
                                    int check=0;
                                    for(int i=0;i<100;i++)
                                    {
                                        if(inven[i].id!=-1)
                                        {
                                            if((strcmp(inven[i].name,temp))==0)
                                            {
                                                printf("The names are matching\n");
                                                if(inven[i].qty>=qty)
                                                {
                                                    // o1.cart[o1.pidx++] = inven[i];
                                                    printf("The product %s is being added\n",inven[i].name);
                                                    strcpy(o1.cart[o1.pidx].name,inven[i].name);
                                                    o1.cart[o1.pidx].id = inven[i].id;
                                                    o1.cart[o1.pidx].price = inven[i].price;
                                                    o1.cart[o1.pidx].qty = qty;
                                                    o1.pidx++;
                                                    o1.cusid = cusid;
                                                    o1.status =0;
                                                    if(!check)
                                                    {
                                                        o1.oid=odno++;
                                                    }
                                                    printf("The ordernumber is :%d\n",o1.oid);
                                                    check = 1;
                                                    break;   
                                                }
                                            }
                                        }
                                    }
                                    if(check)
                                    {
                                        printf("The item has been added\n");
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
                                        printf("Inside\n");
                                        fl.l_start = (i)*(sizeof(struct order));
                                        lseek(fd2,(i)*sizeof(struct order),SEEK_SET);
                                        ord[i] = o1;
                                        fcntl(fd2,F_SETLKW,&fl);
                                        write(fd2,&ord[i],sizeof(struct order));
                                        fl.l_type = F_UNLCK;
                                        sleep(3);
                                        fcntl(fd2,F_SETLKW,&fl); 
                                        write(newsd,"Order Successful\n",18);
                                        printf("A new order has been created successfully\n");
                                        break;
                                    }
                                }
                                for(int i=0;i<100;i++)
                                {
                                    if(ord[i].oid!=-1)
                                    {
                                        printf("index:%d\n",i);
                                        printf("orderID:%d\n",ord[i].oid);
                                    }
                                    if(ord[i].cusid==cusid)
                                    {
                                        for(int j=0;j<10;j++)
                                        {
                                            // if(ord[i].cart[j].id)
                                            printf("The productIDs are :%d\n",ord[i].cart[j].id);
                                            printf("The productname is: %s\n",ord[i].cart[j].name);
                                        }
                                    }
                                }

                            }
                        }
                        
                    }
                    else if(val==4)
                    {
                        int cusid;
                        read(newsd,&cusid,4);
                        printf("CUSID:%d\n",cusid);
                        // char temp[100];
                        int oid;
                        read(newsd,&oid,4);
                        printf("OID:%d\n",oid);
                        int pid;
                        read(newsd,&pid,4);
                        int qty;
                        printf("PID:%d\n",pid);
                        read(newsd,&qty,4);
                        for(int i=0;i<100;i++)
                        {
                            // printf("customerid%d\n",ord[i].cusid);
                            if(ord[i].cusid==cusid)
                            {
                                printf("orderid%d\n",ord[i].oid);
                                if(ord[i].oid==oid)
                                {
                                    printf("CHECK\n");
                                    for(int j=0;j<10;j++)
                                    {
                                        if(ord[i].cart[j].id==pid)
                                        {
                                            struct flock fl;
                                            fl.l_len = sizeof(struct order);
                                            fl.l_whence = SEEK_SET;
                                            fl.l_type = F_WRLCK;
                                            printf("in\n");
                                            if(qty==0)
                                            {
                                                ord[i].cart[j].qty= 0;
                                                fl.l_start = (i)*(sizeof(struct order));
                                                lseek(fd2,(i)*sizeof(struct order),SEEK_SET);
                                                fcntl(fd2,F_SETLKW,&fl);
                                                write(fd2,&ord[i],sizeof(struct order));
                                                fl.l_type = F_UNLCK;
                                                sleep(3);
                                                fcntl(fd2,F_SETLKW,&fl); 
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
                                                    // printf("DEI\n");
                                                    ord[i].cart[j].qty = qty;
                                                    fl.l_start = (i)*(sizeof(struct order));
                                                    lseek(fd2,(i)*sizeof(struct order),SEEK_SET);
                                                    fcntl(fd2,F_SETLKW,&fl);
                                                    write(fd2,&ord[i],sizeof(struct order));
                                                    fl.l_type = F_UNLCK;
                                                    sleep(3);
                                                    fcntl(fd2,F_SETLKW,&fl);
                                                    write(newsd,"Successful\n",12);
                                                    break;
                                                }
                                                else
                                                {
                                                    for(int k=0;k<100;k++)
                                                    {
                                                        if(inven[k].id==pid)
                                                        {
                                                            if(inven[k].qty>=qty)
                                                            {
                                                                ord[i].cart[j].qty = qty;
                                                                fl.l_start = (i)*(sizeof(struct order));
                                                                lseek(fd2,(i)*sizeof(struct order),SEEK_SET);
                                                                fcntl(fd2,F_SETLKW,&fl);
                                                                write(fd2,&ord[i],sizeof(struct order));
                                                                fl.l_type = F_UNLCK;
                                                                sleep(3);
                                                                fcntl(fd2,F_SETLKW,&fl);
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
                                            
                                        }
                                    }
                                    // break;
                                }
                                // break;
                            }
                            // break;
                        }
                    }
                    else if(val==5)
                    {        
                        int orderid;
                        read(newsd,&orderid,4);
                        if(orderid==0)
                        {
                            write(newsd,"Error\n",7);
                            //return ;
                        }
                        else
                        {
                            struct product ps[10];
                            for(int i=0;i<100;i++)
                            {
                                if(ord[i].oid == orderid)
                                {
                                    for(int j=0;j<10;j++)
                                    {
                                        ps[j] = ord[i].cart[j];
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
                            while(read(fd,&p1,sizeof(struct product)))
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
                                write(newsd,"Error1\n",8);
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
                                        fcntl(fd,F_SETLKW,&fl);
                                    }
                                    for(int i=0;i<10;i++)
                                    {
                                        for(int j=0;j<100;j++)
                                        {
                                            if(inven[j].id==prod[i].id)
                                            {
                                                inven[j] = prod[i];
                                            }
                                        }
                                    }
                                    // create a log file.
                                    // make the orderid -1.
                                    
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
