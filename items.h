# include <stdio.h>
struct product{
    int id;
    char name[100];
    int price;
    int qty;
};
struct order{
    int oid;
    struct product cart[10];
};