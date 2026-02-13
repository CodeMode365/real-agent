#pragma once

struct SellerDetails
{
    int id;
    char name[100];
    char contact[50];
    char address[150];
};

class Seller
{
private:
    SellerDetails sellers[100];
    char fileName[100];

    void readSellersFromFile();
    void writeSellerToFile(SellerDetails seller);

public:
    Seller();

    void menu();

    void addSeller();

    void viewSellers();

    SellerDetails getSellerById(int id);

    void deleteSellerById(int id);

    void updateSellerById(int id);
};
