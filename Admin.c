# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <string.h>
# include <netinet/in.h>
# include "items.h"
int main(int argc,char*argv[])
{
    int fd = open(argv[1],O_CREAT|O_EXCL|O_RDWR);
    struct product inven[100]; // this shall store the products that are present in the store.
    struct order ord[100]; // This is used to keep track of the orders of the seller.
    int idx = 0; // this is used for keeping track of the last product added(index).
    int odno = 0;
    for(int i=0;i<100;i++)
    {
        read(fd,&inven[i],sizeof(struct product));    
    }
    for(int i=0;i<100;i++)
    {
        read(fd,&ord[i],sizeof(struct order));
    }
    char menu='a';
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
            counter++;
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
                    int value;
                    read(newsd,&value,4);
                    for(int i=0;i<100;i++)
                    {
                        if(ord[i].cusid==value)
                        {
                            write(newsd,&ord[i],sizeof(struct order));
                            break;
                        }
                    }                
                }
                else if(val==3)
                {
                    int cusid;
                    read(newsd,&cusid,4);
                    char temp[100];
                    read(newsd,temp,100);
                    int qty;
                    read(newsd,&qty,4);
                    int flg =0;
                    for(int i=0;i<100;i++)
                    {
                        if(strcmp(inven[i].name,temp)==0)
                        {
                            struct order o1;
                            o1.oid = ++odno;
                            o1.cusid = cusid;
                        }
                    }
                    if(!flg)
                    {
                        printf("Order not processed\n");
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
                    read(newsd,&pid,100);
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
                                            printf("The specified product has been sucessfully removed\n");
                                        }
                                        else if(qty<0)
                                        {
                                            printf("Invalid transaction\n");
                                        }
                                        else 
                                        {
                                            ord[i].cart[j].qty = qty;
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
                    int tid;
                    read(newsd,&tid,4);
                    if(tid==1)
                    {
                        // the transaction code must be written.
                    }
                    // else


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
        printf("The menu for server side data updation is");
        printf("---------------------------------------------------------------------------------");
        printf("Enter 1 to add a product\n");
        printf("Enter 2 to delete a product\n");
        printf("Enter 3 to update the quantity in the inventory\n");
        printf("Enter 4 to update the price of a product\n");
        scanf("%d",&option);
        while(1)
        {
            if(option==1)
            {
                struct product p1;
                printf("Please enter the name of the product that you wish to be added\n");
                scanf("%s",&p1.name);
                printf("Please enter the id, quantity and the price respectively for the above mentioned product\n");
                scanf("%d %d %d",&p1.id,&p1.qty,&p1.price);
                int flg = 0;
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==-1)
                    {
                        // here try writing the record locking functionality and then update the following.
                        inven[i] = p1;
                        flg = 1;
                        break;
                    }
                }
                if(flg==0)
                {
                    printf("Inventory full!,couldn't add the new item.");
                }
                // inven[idx++]=p1;
            }
            else if(option==2)
            {
                int id;
                scanf("Enter the id of the product that has to be deleted :%d",&id);
                int flg = 0;
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        inven[i].id=-1;
                        flg = 1;
                    }
                }
                if(!flg)
                {
                    printf("The given productID wasn't found\n");
                }
            }
            else if(option==3)
            {
                // use record locking here also.
                int id;
                scanf("Enter the id of the product that has to be updated:%d",&id);
                int price;
                int flg=0;
                scanf("Enter the new price of the product:%d",&price);
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        inven[i].price = price;
                        flg = 1;
                        break;
                    }
                }
                if(!flg)
                {
                    printf("Invalid productID!");
                }
            }
            else if(option==4)
            {
                // use record locking here also.
                int id;
                scanf("Enter the id of the product that has to be updated:%d",&id);
                int qty;
                int flg=0;
                scanf("Enter the new qty of the product:%d",&qty);
                for(int i=0;i<100;i++)
                {
                    if(inven[i].id==id)
                    {
                        inven[i].price = qty;
                        flg = 1;
                        break;
                    }
                }
                if(!flg)
                {
                    printf("Invalid productID!");
                }
            }
            else
            {
                printf("Invalid entry, try again later\n");
            }
            printf("The menu for server side data updation is");
            printf("---------------------------------------------------------------------------------");
            printf("Enter 1 to add a product\n");
            printf("Enter 2 to delete a product\n");
            printf("Enter 3 to update the quantity in the inventory\n");
            printf("Enter 4 to update the price of a product\n");
            printf("Enter 5 to exit the program\n");
            scanf("%d",&option);    
        }
    }
    else {
        printf("Invalid Input\n");
    }
    return 0;
}
