#include <iostream>
#include <cstdio>
#include <cstring>
#include "seller.h"
#include "../utils/screen/screen.h"
#include "../menu/menu.h"

using namespace std;

Seller::Seller()
{
    strcpy(this->fileName, "sellers.txt");
}

void Seller::readSellersFromFile()
{
    SellerDetails seller;
    FILE *file = fopen(this->fileName, "r");

    if (!file)
    {
        cout << "No seller records found.\n";
        return;
    }

    cout << "\n---- Seller List ----\n";
    cout << "ID\tName\tContact\tAddress\n";
    cout << "------------------------------------------\n";

    while (fscanf(file, "%d %s %s %s",
                  &seller.id,
                  seller.name,
                  seller.contact,
                  seller.address) == 4)
    {
        cout << seller.id << "\t"
             << seller.name << "\t"
             << seller.contact << "\t"
             << seller.address << "\n";
    }

    fclose(file);
}

void Seller::addSeller()
{
    Screen::clearScreen();
    Screen::printHeader("Add Seller");

    SellerDetails newSeller;

    cout << "Enter Seller ID: ";
    cin >> newSeller.id;

    cout << "Enter Seller Name: ";
    cin >> newSeller.name;

    cout << "Enter Contact: ";
    cin >> newSeller.contact;

    cout << "Enter Address: ";
    cin >> newSeller.address;

    FILE *file = fopen(this->fileName, "a");

    if (!file)
    {
        cout << "Error opening file.\n";
        return;
    }

    fprintf(file, "%d %s %s %s\n",
            newSeller.id,
            newSeller.name,
            newSeller.contact,
            newSeller.address);

    fclose(file);

    cout << "Seller added successfully!\n";
}

SellerDetails Seller::getSellerById(int id)
{
    SellerDetails seller;
    FILE *file = fopen(this->fileName, "r");

    if (!file)
        return seller;

    while (fscanf(file, "%d %s %s %s",
                  &seller.id,
                  seller.name,
                  seller.contact,
                  seller.address) == 4)
    {
        if (seller.id == id)
        {
            fclose(file);
            return seller;
        }
    }

    fclose(file);
    return seller;
}

void Seller::deleteSellerById(int id)
{
    FILE *original = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

    SellerDetails seller;

    if (!original || !temp)
    {
        cout << "Error handling files.\n";
        return;
    }

    while (fscanf(original, "%d %s %s %s",
                  &seller.id,
                  seller.name,
                  seller.contact,
                  seller.address) == 4)
    {
        if (seller.id != id)
        {
            fprintf(temp, "%d %s %s %s\n",
                    seller.id,
                    seller.name,
                    seller.contact,
                    seller.address);
        }
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << "Seller deleted successfully!\n";
}

void Seller::viewSellers()
{
    Screen::clearScreen();
    Screen::printHeader("View Sellers");
    readSellersFromFile();
}

void Seller::menu()
{
    int choice;

    Screen::clearScreen();
    Screen::printHeader("Seller Menu");

    cout << "1. Add Seller\n";
    cout << "2. View Sellers\n";
    cout << "3. Delete Seller\n";
    cout << "4. Back to Main Menu\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        addSeller();
        break;

    case 2:
        viewSellers();
        break;

    case 3:
    {
        int id;
        cout << "Enter Seller ID to delete: ";
        cin >> id;
        deleteSellerById(id);
        break;
    }

    case 4:
        Menu::showMenu();
        break;

    default:
        cout << "Invalid choice.\n";
    }
}
