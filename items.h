# include <stdio.h>
struct product{
    int id;
    char name[100];
    int price;
    int qty;
};
struct order{
    int oid;
    int cusid;
    struct product cart[10];
};