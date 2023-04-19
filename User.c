# include <stdio.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
// Client Cod
// first run the server then run the client.
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
        }
        else if(option==2)
        {

        }
        else if(option==3)
        {

        }
        else if(option==4)
        {

        }
        else if(option==5)
        {

        }
    // getchar();
    shutdown(sckfd,2);
    return 0;
}