#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include "seller.h"
#include "../menu/menu.h"
#include "../utils/files/file.h"
#include "../utils/screen/screen.h"

using namespace std;

namespace
{
    bool readSellerRecord(FILE *file, SellerDetails &seller)
    {
        char line[512] = {};
        if (!fgets(line, sizeof(line), file))
        {
            return false;
        }

        if (sscanf(line, "%d|%99[^|]|%49[^|]|%149[^\n]",
                   &seller.id,
                   seller.name,
                   seller.contact,
                   seller.address) == 4)
        {
            return true;
        }

        return sscanf(line, "%d %99s %49s %149s",
                      &seller.id,
                      seller.name,
                      seller.contact,
                      seller.address) == 4;
    }

    void writeSellerRecord(FILE *file, const SellerDetails &seller)
    {
        fprintf(file, "%d|%s|%s|%s\n",
                seller.id,
                seller.name,
                seller.contact,
                seller.address);
    }

    void waitForEnter()
    {
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

Seller::Seller()
{
    strcpy(this->fileName, "sellers.txt");
}

int Seller::generateId()
{
    int id = 0;
    FILE *file = fopen("sellerID.txt", FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (file)
    {
        fscanf(file, "%d", &id);
        fclose(file);
    }

    ++id;

    file = fopen("sellerID.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    if (!file)
    {
        return -1;
    }

    fprintf(file, "%d", id);
    fclose(file);
    return id;
}

bool Seller::sellerHasProperties(int id)
{
    FILE *file = fopen("properties.txt", FileUtils::getFileModeString(FileUtils::FileMode::READ));
    if (!file)
    {
        return false;
    }

    char line[512] = {};
    while (fgets(line, sizeof(line), file))
    {
        int propertyId = 0;
        char type[50] = {};
        char location[100] = {};
        float price = 0;
        int size = 0;
        int sellerId = 0;
        char status[20] = {};

        if (sscanf(line, "%d|%49[^|]|%99[^|]|%f|%d|%d|%19[^\n]",
                   &propertyId,
                   type,
                   location,
                   &price,
                   &size,
                   &sellerId,
                   status) != 7)
        {
            continue;
        }

        if (sellerId == id)
        {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

void Seller::readSellersFromFile()
{
    SellerDetails seller = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        cout << "No seller records found.\n";
        return;
    }

    cout << "\n"
         << left
         << setw(8) << "ID"
         << setw(24) << "Name"
         << setw(20) << "Contact"
         << setw(30) << "Address" << "\n";
    cout << string(82, '-') << "\n";

    while (readSellerRecord(file, seller))
    {
        cout << left
             << setw(8) << seller.id
             << setw(24) << seller.name
             << setw(20) << seller.contact
             << setw(30) << seller.address << "\n";
    }

    fclose(file);
}

void Seller::addSeller()
{
    Screen::clearScreen();
    Screen::printHeader("Add Seller");

    SellerDetails newSeller = {};
    newSeller.id = generateId();

    if (newSeller.id < 0)
    {
        cout << "Unable to generate seller ID.\n";
        return;
    }

    cout << "Seller ID: " << newSeller.id << "\n";
    cout << "Enter Seller Name: ";
    cin >> ws;
    cin.getline(newSeller.name, sizeof(newSeller.name));
    cout << "Enter Contact: ";
    cin.getline(newSeller.contact, sizeof(newSeller.contact));
    cout << "Enter Address: ";
    cin.getline(newSeller.address, sizeof(newSeller.address));

    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::APPEND));
    if (!file)
    {
        cout << "Error opening file.\n";
        return;
    }

    writeSellerRecord(file, newSeller);
    fclose(file);

    cout << "Seller added successfully!\n";
}

SellerDetails Seller::getSellerById(int id)
{
    SellerDetails seller = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        return seller;
    }

    while (readSellerRecord(file, seller))
    {
        if (seller.id == id)
        {
            fclose(file);
            return seller;
        }
    }

    fclose(file);
    seller.id = 0;
    return seller;
}

void Seller::deleteSellerById(int id)
{
    if (sellerHasProperties(id))
    {
        cout << "Seller is linked to one or more properties and cannot be deleted.\n";
        waitForEnter();
        return;
    }

    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    SellerDetails seller = {};
    bool found = false;

    if (!original || !temp)
    {
        cout << "Error handling files.\n";
        if (original)
        {
            fclose(original);
        }
        if (temp)
        {
            fclose(temp);
        }
        return;
    }

    while (readSellerRecord(original, seller))
    {
        if (seller.id == id)
        {
            found = true;
            continue;
        }

        writeSellerRecord(temp, seller);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Seller deleted successfully!\n" : "Seller ID not found!\n");
    waitForEnter();
}

void Seller::updateSellerById(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    SellerDetails seller = {};
    bool found = false;

    if (!original || !temp)
    {
        cout << "Error handling files.\n";
        if (original)
        {
            fclose(original);
        }
        if (temp)
        {
            fclose(temp);
        }
        return;
    }

    while (readSellerRecord(original, seller))
    {
        if (seller.id == id)
        {
            found = true;
            cout << "Enter New Name (Current: " << seller.name << "): ";
            cin >> ws;
            cin.getline(seller.name, sizeof(seller.name));
            cout << "Enter New Contact (Current: " << seller.contact << "): ";
            cin.getline(seller.contact, sizeof(seller.contact));
            cout << "Enter New Address (Current: " << seller.address << "): ";
            cin.getline(seller.address, sizeof(seller.address));
        }

        writeSellerRecord(temp, seller);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Seller updated successfully!\n" : "Seller ID not found!\n");
    waitForEnter();
}

void Seller::viewSellers()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("View Sellers");
        readSellersFromFile();

        cout << "\n1. Delete Seller by ID\n";
        cout << "2. Update Seller by ID\n";
        cout << "3. Back\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int id = 0;
            cout << "Enter Seller ID to delete: ";
            cin >> id;
            deleteSellerById(id);
        }
        else if (choice == 2)
        {
            int id = 0;
            cout << "Enter Seller ID to update: ";
            cin >> id;
            updateSellerById(id);
        }
        else if (choice != 3)
        {
            cout << "Invalid choice!\n";
            waitForEnter();
        }
    } while (choice != 3);
}

void Seller::menu()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("Seller Menu");

        cout << "1. Add Seller\n";
        cout << "2. View Sellers\n";
        cout << "3. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            addSeller();
            waitForEnter();
        }
        else if (choice == 2)
        {
            viewSellers();
        }
        else if (choice != 3)
        {
            cout << "Invalid choice.\n";
            waitForEnter();
        }
    } while (choice != 3);
}
