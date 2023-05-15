# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <string.h>
# include <netinet/in.h>
# include<time.h>
# include "items.h"

void server_send_prod(int fd,int newsd)
{
    struct product p;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id != -1 && p.qty > 0){
            write(newsd, &p, sizeof(struct product));
        }
    }
    struct product p1;
    p1.id = 0;
    write(newsd,&p1,12);
}
void server_send_cart(int fd2,int newsd)
{
    int cusid;
    read(newsd,&cusid,4);
    printf("The customerID received:%d\n",cusid);
    int flg = 0;
    struct order o;
    while (read(fd2, &o, sizeof(struct order))){
        if (o.cusid == cusid){
            for (int j=0; j<10; j++){
                printf("The productIDs are :%d\n",o.cart[j].id);
                printf("The productname is: %s\n",o.cart[j].name);
            }
            if(o.oid!=-1){
                write(newsd, &o, sizeof(struct order));
                flg = 1;
            }
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
void server_create_order(int fd,int fd2,int newsd,int *odno)
{
    // printf("Hello there\n");
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
    struct order o;
    while(read(fd2, &o, sizeof(struct order))){
        // This is first checking if the order cart is present and is non empty.
        if (o.cusid == cusid && o.oid != -1){
            
            o1 = o;
            flg = 1;
            break;
            //the cart for the customer is found
        }
    }
    if(flg==1)
    {
        // printf("YOOOOOOOO\n");
        if(noord<0)
        {
            printf("Error\n");
            write(newsd,"Order Invalid\n",15);
        }
        else
        {
            
            if(o1.pidx+noord>10)
            {
                printf("Error\n");
                write(newsd,"Order Invalid\n",15);
            }
            else
            {
                write(newsd,"1",1); // 1 means proceed forward
                // int chk=0;
                int found = 0;
                // this is used to check the number of products that are matched correctly with the inventory.
                // Only if found is equal to number of items in that order we are allowed to proceed.
                for (int q=0; q<noord; q++)
                {
                        // printf("kokok\n");
                        int chk = 0;
                        char name[100];
                        read(newsd,name,100);
                        int qty;
                        read(newsd,&qty,4);
                        lseek(fd, 0, SEEK_SET);
                        struct product p;
                        int ok=1;
                        for (int i=0; i<10; i++){
                        // printf("%s\n", o1.cart[i].name);
                            if (strcmp(o1.cart[i].name, name) == 0){
                                ok = 0;
                                break;
                            }
                    }
                    if(ok){
                        while (read(fd, &p, sizeof(struct product))){
                            if (p.id != -1){
                                if((strcmp(p.name,name))==0)
                                {
                                    if(p.qty>=qty)
                                    {
                                        strcpy(o1.cart[o1.pidx].name,p.name);
                                        o1.cart[o1.pidx].id = p.id;
                                        o1.cart[o1.pidx].price = p.price;
                                        o1.cart[o1.pidx].qty = qty;
                                        o1.pidx++;
                                        found += 1;
                                        chk = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if(!chk)
                    {
                        break;
                    }    
                }
                if(found == noord)
                {
                    // write(newsd,"Order Successfully Placed\n",27);
                    printf("Order added to cart Successfully\n");

                    struct flock fl;
                    fl.l_len = sizeof(struct order);
                    fl.l_whence = SEEK_SET;
                    fl.l_type = F_WRLCK;

                    fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                    fcntl(fd2, F_SETLKW, &fl);
                    write(fd2, &o1, sizeof(struct order));
                    fl.l_type = F_UNLCK;
                    sleep(3);
                    fcntl(fd2, F_SETLKW, &fl);
                    write(newsd,"Order Successful\n",18);
                    printf("A new order has been created successfully\n");
                }
                else
                {
                    write(newsd,"Error Placing Order\n",21);
                }
            }
        }
    }
    else if(flg==0)
    {
        //this is the condition to create a new cart
        if(noord>10 || noord <=0)
        {
            printf("Error with data\n");
            write(newsd,"Order Invalid\n",15);
        }
        else
        {

            for (int i=0; i<10; i++){
                o1.cart[i].id = 0;
                strcpy(o1.cart[i].name, "");
                o1.cart[i].qty = 0;
                o1.cart[i].price = 0;
            }
            o1.pidx = 0;

            write(newsd,"1",1);
            printf("Creating new cart\n");
            char temp[100];

            int found = 0;
            int check=0;
            for (int q=0; q<noord; q++)
            {

                read(newsd,temp,100);
                printf("%s\n",temp);
                int qty;
                read(newsd,&qty,4);
                printf("The quantity is:%d\n",qty);
                struct product p;
                lseek(fd, 0, SEEK_SET);
                int chk = 0;

                int ok = 1;
                for (int i=0; i<10; i++){
                    // printf("%s\n", o1.cart[i].name);
                    if (strcmp(o1.cart[i].name, temp) == 0){
                        ok = 0;
                        break;
                    }
                }

                if (ok){
                    while (read(fd, &p, sizeof(struct product))){
                        if (p.id != -1){
                            if (strcmp(p.name, temp) == 0){
                                // printf("Yo\n");
                                if (p.qty >= qty){

                                    // printf("Gone\n");
                                    strcpy(o1.cart[o1.pidx].name,p.name);
                                    o1.cart[o1.pidx].id = p.id;
                                    o1.cart[o1.pidx].price = p.price;
                                    o1.cart[o1.pidx].qty = qty;
                                    o1.pidx++;
                                    o1.cusid = cusid;
                                    found++;

                                    if (!check){
                                        o1.oid = *(odno)++;
                                    }
                                    check = 1;
                                    chk = 1;
                                    // printf("Here\n");
                                    break;
                                }
                            }
                        }
                    }
                }
                if(chk)
                {
                    printf("The item has been added\n");
                }
            }

            if (found != noord){
                write(newsd, "Error placing order\n", 21);
                // odno--;
                
            }else{

                struct flock fl;
                fl.l_len = sizeof(struct order);
                fl.l_whence = SEEK_SET;
                fl.l_type = F_WRLCK;

                lseek(fd2, 0, SEEK_SET);
                // printf("Hi\n");
                
                while (read(fd2, &o, sizeof(struct order))){
                    printf("%d\n", o.oid);
                    if (o.oid == -1){
                        // printf("Here1\n");
                        fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                        fcntl(fd2, F_SETLKW, &fl);
                        write(fd2, &o1, sizeof(struct order));
                        fl.l_type = F_UNLCK;
                        sleep(3);
                        fcntl(fd2, F_SETLKW, &fl);
                        write(newsd,"Order Successful\n",18);
                        printf("A new order has been created successfully\n");
                        break;
                    }
                }
            }                                
        }
    }
}
void server_cart_update(int fd,int fd2,int newsd)
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

    struct order o;
    while (read(fd2, &o, sizeof(struct order))){
        if (o.cusid == cusid){
            if (o.oid == oid){
                for (int j=0; j<10; j++){
                    if(o.cart[j].id == pid){

                        struct flock fl;
                        fl.l_len = sizeof(struct order);
                        fl.l_whence = SEEK_SET;
                        fl.l_type = F_WRLCK;

                        if (qty == 0){
                            o.cart[j].qty = 0;
                            fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                            fcntl(fd2, F_SETLKW, &fl);
                            write(fd2,&o,sizeof(struct order));

                            fl.l_type = F_UNLCK;
                            sleep(3);
                            fcntl(fd2,F_SETLKW,&fl); 
                            write(newsd,"The product has been deleted\n",30);
                            printf("The specified product has been sucessfully removed\n");
                        }else if (qty < 0){
                            printf("Invalid transaction\n");
                            write(newsd,"Invalid transaction\n",21);
                        }else{

                            if(o.cart[j].qty>=qty)
                            {
                                // printf("DEI\n");
                                o.cart[j].qty = qty;
                                fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                                fcntl(fd2,F_SETLKW,&fl);
                                write(fd2,&o,sizeof(struct order));
                                fl.l_type = F_UNLCK;
                                sleep(3);
                                fcntl(fd2,F_SETLKW,&fl);
                                write(newsd,"Successful\n",12);
                                break;
                            }

                            else
                            {
                                struct product p;
                                while (read(fd, &p, sizeof(struct product))){
                                    if (p.id == pid){
                                        if (p.qty >= qty){

                                            o.cart[j].qty = qty;
                                            fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                                            fcntl(fd2,F_SETLKW,&fl);

                                            write(fd2,&o,sizeof(struct order));
                                            fl.l_type = F_UNLCK;
                                            sleep(3);
                                            fcntl(fd2,F_SETLKW,&fl);
                                            printf("Successful\n");
                                            write(newsd,"Successful\n",12);
                                            break;

                                        }else{
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
            }
        }
    }
    write(newsd, "Invalid data\n", sizeof("Invalid data\n"));
}

void server_confirm_order(char arg[],int newsd,int fd2,int fd)
{
    int fd3 = open(arg,O_TRUNC|O_RDWR|O_CREAT,0777);
    int orderid;
    read(newsd,&orderid,4);
    printf("orderID %d\n",orderid);
    if(orderid==0)
    {
        write(newsd,"Error\n",7);
    }
    else
    {
        struct product ps[10];
        struct order o;
        lseek(fd2, 0, SEEK_SET);
        int flg2 = 0;
        while (read(fd2, &o, sizeof(struct order))){
            // printf("%d\n", o.oid);
            if (o.oid == orderid){
                flg2 = 1;
                for (int j=0; j<10; j++){
                    ps[j] = o.cart[j];
                }
                break;
            }
        }

        if (!flg2){
            write(newsd,"Error\n",7);
            return ;
        }

        struct product p1;
        int idx = 0;
        int flg = 0;
        int curidx[10];
        struct product prod[10];
        // printf("2\n");
        lseek(fd,0,SEEK_SET);

        for (int i=0; i<10; i++){
            if (ps[i].id == 0){
                continue;
            }
            lseek(fd, 0, SEEK_SET);
            // int found = 0;
            while (read(fd, &p1, sizeof(struct product))){
                if (p1.id == ps[i].id)
                {
                    if (p1.qty >= ps[i].qty)
                    {
                        prod[idx] = ps[i];
                        prod[idx].qty = p1.qty - ps[i].qty;
                        // arr[idx] = p1.id;
                        curidx[idx] = lseek(fd, (-1)*sizeof(struct product), SEEK_CUR);
                        lseek(fd, sizeof(struct product), SEEK_CUR);
                        idx++;
                        // printf("Yo\n");
                        break;
                    }
                    else{
                        // printf("The quantity present in the file is %d\n", p1.qty);
                        printf("The quantity present in the file is %d\n",p1.qty);
                        break;
                    }
                }
            }

            if (flg) break;

            // if (!found){
            //     flg = 1;
            //     break;
            // } 
            
        }

        if (flg){
            write(newsd, "Error1\n", 8);
            printf("Order couldn't be placed due to unavailability of stock\n");
        }

        else
        {
            write(newsd,"Proceed\n",9);
            sleep(5);

            for(int i=0;i<10;i++)
            {
                // printf("Yo %d\n", ps[i].id);
                struct product temp1 = ps[i];
                if(ps[i].id!=0){
                    write(newsd,&temp1,sizeof(struct product));
                    printf("The value is:%d\n",ps[i].id);
                }
            }
            // printf("Outside\n");
            struct product p3;
            p3.id = 0;
            write(newsd,&p3,sizeof(struct product));
            struct flock fl;
            fl.l_whence = SEEK_SET;
            fl.l_type = F_WRLCK;
            fl.l_len = sizeof(struct product);

            int cotp = 1;
            int otp = 0;
            read(newsd,&otp,4);
            if(cotp==otp)
            {
                for(int i=0;i<idx;i++)
                {
                    fl.l_start = curidx[i];
                    lseek(fd, curidx[i], SEEK_SET);
                    fcntl(fd,F_SETLKW,&fl);
                    // sleep(2);
                    write(fd,&prod[i],sizeof(struct product));
                    fl.l_type = F_UNLCK;
                    fcntl(fd,F_SETLKW,&fl);
                }
                
                
                int chk = 0;

                // lseek(fd2, 0, SEEK_SET);
                
                // while (read(fd2, &o, sizeof(struct product))){
                printf("%d\n", o.oid);
                
                o.oid = -1;
                write(newsd, "Transaction successful\n", 24);
                chk = 1;

                struct flock fl;
                fl.l_whence = SEEK_SET;
                fl.l_start = lseek(fd2, (-1)*sizeof(struct order), SEEK_CUR);
                fl.l_len = sizeof(struct order);
                fl.l_type = F_WRLCK;

                fcntl(fd2, F_SETLKW, &fl);
                write(fd2, &o, sizeof(struct order));
                
                fl.l_type = F_UNLCK;
                fcntl(fd2, F_SETLKW, &fl);
                // break;
                
                
                if(chk){
                    printf("Transaction Successful\n");
                    int totalamount=0;
                    char buff[150];
                    for(int n=0;n<10;n++)
                    {
                        if(strcmp(ps[n].name," ")!=0)
                        {
                            sprintf(buff,"Product Name: %s\n",ps[n].name);
                            write(fd3,buff,strlen(buff));
                            sprintf(buff,"Product Price: %d\n",ps[n].price);
                            write(fd3,buff,strlen(buff));
                            totalamount+=ps[n].price*ps[n].qty;
                            sprintf(buff,"Product Quantity:%d\n",ps[n].qty);
                            write(fd3,buff,strlen(buff));
                        }
                    }
                    sprintf(buff,"Total Amount:%d\n",totalamount);
                    write(fd3,buff,strlen(buff));
                    close(fd3);

                }
                else
                {
                    write(newsd,"Transaction Failure\n",21);
                    printf("Transaction failure\n");
                }
            }
            else
            {
                printf("Transaction Failed\n");
                write(newsd,"Transaction Failed\n",20);
            }
        }        
    }
}
void seller_add_prod(int fd)
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
    
    struct product p;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id == p1.id){
            printf("This is a redundant product\n");
            flg1 = 1;
            break;
        }
    }
    lseek(fd, 0, SEEK_SET);
    if(!flg1){
        
        int i=0;
        while (read(fd, &p, sizeof(struct product))){
            
            if (p.id == -1){
                fl.l_start = i*sizeof(struct product);
                lseek(fd, i*sizeof(struct product), SEEK_SET);
                fcntl(fd, F_SETLKW, &fl);

                write(fd, &p1, sizeof(struct product));
                sleep(5);
                fl.l_type = F_UNLCK;
                fcntl(fd, F_UNLCK, &fl);
                flg = 1;
                break;
            }
            i++;
        }
    }
    if(flg==0&&flg1==0)
    {
        printf("Inventory full! couldn't add the new item.");
    }
    else if(flg==1 && flg1==0)
    {
        printf("The product has been added Successfully\n");
    }
}
void seller_del_prod(int fd,int fd2)
{
    int id;
    struct flock fl;
    fl.l_len = sizeof(struct product);
    fl.l_whence = SEEK_SET;
    fl.l_type = F_WRLCK;
    printf("Enter the id of the product that has to be deleted\n");
    scanf("%d",&id);
    int flg = 0;
    
    struct product p;
    // int i=0;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id == id){
            fl.l_start = lseek(fd,(-1)*sizeof(struct product),SEEK_CUR);
            // lseek(fd,(i)*sizeof(struct product),SEEK_SET);
            // lseek(fd,(-1)*sizeof(struct product),SEEK_CUR);
            fcntl(fd,F_SETLKW,&fl);
            p.id = -1;
            write(fd,&p,sizeof(struct product));
            sleep(2);
            flg = 1;
            fl.l_type = F_UNLCK;
            fcntl(fd,F_UNLCK,&fl);
            break;
        }
        // i++;
    }
    if(!flg)
    {
        printf("The given productID wasn't found\n");
    }
    else
    {
        struct order o;
        lseek(fd2, 0, SEEK_SET);
        while (read(fd2, &o, sizeof(struct order))){
            if (o.oid != 0){
                for (int j=0; j<10; j++){
                    if(o.cart[j].id==id)
                    {
                        o.cart[j].id=-1;
                        // invalid productID;
                    }
                }
                lseek(fd2, -(1)*sizeof(struct order), SEEK_CUR);
                write(fd2, &o, sizeof(struct order));
            }
        }
        
        printf("The given item has been deleted successfully\n");
    }    
}

void seller_prodprice_update(int fd,int fd2)
{
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

    struct product p;
    int i=0;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id == id){

            fl.l_start = (i)*(sizeof(struct product));
            lseek(fd,(i)*sizeof(struct product),SEEK_SET);
            fcntl(fd,F_SETLKW,&fl);
            p.price = price;
            write(fd,&p,sizeof(struct product));
            sleep(2);
            flg = 1;
            fl.l_start = F_UNLCK;
            fcntl(fd,F_UNLCK,&fl);
            break;

        }
        i++;
    }
    
    if(!flg)
    {
        printf("Invalid productID!");
    }
    else
    {
        
        struct order o;
        lseek(fd2, 0, SEEK_SET);
        while (read(fd2, &o, sizeof(struct order))){
            if (o.oid != 0){
                for (int j=0; j<10; j++){
                    if(o.cart[j].id==id)
                    {
                        o.cart[j].price=price;
                        // invalid productID;
                    }
                }
                lseek(fd2, -(1)*sizeof(struct order), SEEK_CUR);
                write(fd2, &o, sizeof(struct order));
            }
        }
        printf("Price for the products has been updated\n");
    }
}

void seller_prodqty_update(int fd,int fd2)
{
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
    struct product p;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id == id){

            fl.l_start = lseek(fd,(-1)*sizeof(struct product),SEEK_CUR);
            fcntl(fd,F_SETLKW,&fl);
            p.qty = qty;
            write(fd,&p,sizeof(struct product));
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
        printf("The quantity for the products have been updated\n");
    }
}

void seller_close(char *argv,int fd)
{
    printf("Shutting Down\n");
    int fd3;
    fd3 = open(argv,O_TRUNC| O_RDWR | O_CREAT, 0777); // this is the log file that is generated.
    struct product p;
    while (read(fd, &p, sizeof(struct product))){
        if (p.id != -1){

            printf("New Product\n");
            char temp[110];
            sprintf(temp, "%d %s %d %d\n", p.id, p.name, p.qty, p.price);
            write(fd3, temp, strlen(temp));
        }
    }
    
    printf("Done and thank you!\n");
    close(fd3);
}
int main(int argc,char*argv[])
{
    int fd = open(argv[1],O_RDWR); // this shall store the products of that shop owner.
    int fd2 = open(argv[2],O_RDWR);
    // struct product inven[100]; // this shall store the products that are present in the store.
    // struct order ord[100]; // This is used to keep track of the orders of the seller.
    int idx = 0; // this is used for keeping track of the last product added(index).
    int odno = 1;

    struct order od;
    while (read(fd2, &od, sizeof(struct order))){
        if (od.oid != -1){
            odno++;
        }
    }

    char menu='a';
    printf("Enter c for receiving the requests from client\n");
    printf("Enter s for receving the requests from terminal(seller side functions)\n");
    scanf("%c",&menu);
    int option;
    if(menu=='s')
    {
        printf("The menu for server side data updation is\n");
        printf("---------------------------------------------------------------------------------\n");
        printf("Enter 1 to add a product\n");
        printf("Enter 2 to delete a product\n");
        printf("Enter 3 to update the price in the inventory\n");
        printf("Enter 4 to update the quantity of a product\n");
        printf("Enter 5 to exit the program and record the changes in the log file\n");
        scanf("%d",&option);
        while(1)
        {
            lseek(fd, 0, SEEK_SET);
            lseek(fd2, 0, SEEK_SET);
            if(option==1)
            {
                seller_add_prod(fd);
            }
            else if(option == 2)
            {
                seller_del_prod(fd,fd2);
            }
            else if(option == 3)
            {
                seller_prodprice_update(fd,fd2);
            }
            else if(option == 4)
            {
                seller_prodqty_update(fd,fd2);
            }
            else if(option == 5)
            {
                char t[100];
                strcpy(t,argv[3]);
                seller_close(t,fd);
                break;
            }
            else
            {
                printf("Invalid Option, please try again\n");
            }
            printf("The menu for server side data updation is\n");
            printf("---------------------------------------------------------------------------------\n");
            printf("Enter 1 to add a product\n");
            printf("Enter 2 to delete a product\n");
            printf("Enter 3 to update the price in the inventory\n");
            printf("Enter 4 to update the quantity of a product\n");
            printf("Enter 5 to exit the program and record the changes in the log file\n");
            scanf("%d",&option);
        }

    }
    // use option 4 if we want to change the quantity of an existing product.
    else if(menu=='c')
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
                    lseek(fd, 0, SEEK_SET);
                    lseek(fd2, 0, SEEK_SET);
                    read(newsd,&val,4);
                    printf("val %d\n", val);
                    if(val==1)
                    {
                        server_send_prod(fd,newsd);
                    }
                    else if(val==2)
                    {
                        server_send_cart(fd2,newsd);
                    }
                    else if(val==3)
                    {
                        server_create_order(fd,fd2,newsd,&odno);
                    }
                    else if(val==4)
                    {
                        server_cart_update(fd,fd2,newsd);
                    }
                    else if(val==5)
                    {
                        char temp1[100];
                        strcpy(temp,argv[4]);
                        server_confirm_order(temp,newsd,fd2,fd);
                    }
                }
            }
            else
            {
                close(newsd);
            }

        }
    }
}