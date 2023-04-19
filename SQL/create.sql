create table inventory{
    slNO Integer PRIMARY KEY,
    sellerID Integer FOREIGN KEY,
    productID Integer FOREIGN KEY,
    quantity Integer
};

create table products{
    productID Integer PRIMARY KEY,
    productName VARCHAR(25),
    price Integer
};

create table sellers{
    sellerID Integer PRIMARY KEY,
    sellerName VARCHAR(25)
};

create table customers{
    customerID Integer PRIMARY KEY,
    customerName VARCHAR(25),
    customerPhno VARCHAR(25)
};

