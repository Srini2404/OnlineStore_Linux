# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>

int main(int argc,char*argv[])
{
    int sellerID=1;
    int customerID=1;
    while(1)
    {
        printf("-------------------------------------------------------------------------------------------");
        printf("This is the menu\n");
        printf("Press 1 for registring as ADMIN\n");
        printf("Press 2 for registering as USER\n");
        printf("Press 3 for logging in as ADMIN\n");
        printf("Press 4 for logging in as USER\n");
        printf("Press 5 for exiting the interface\n");
        int menu;scanf("%d",&menu);
        if(menu == 1)
        {
            char name[100];
            printf("Please Enter your name\n");
            scanf("%s",name);
            sellerID++;

        }
        else if(menu==2)
        {
            char name[100];
            printf("Please Enter your name\n");
            scanf("%s",name);
            customerID++;
        }
        else if(menu == 3)
        {
            printf("Please enter your adminID\n");
            int id = -1;
            scanf("%d",&id);

        }
        else if(menu == 4)
        {
            char name[100];
            printf("Please Enter your name\n");
            scanf("%s",name);
            printf("Please enter your adminID\n");
            int id = -1;
            scanf("%d",&id);
            
        }
        else if(menu==5)
        {
            printf("Logging you out of the servers\n");
            printf("Please wait\n");
            sleep(5);
            printf("Thank you for your patience\n");
            printf("You may close the window\n");
            exit(1);
        }
    }
    return 0;
}