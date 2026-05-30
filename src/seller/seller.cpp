#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "seller.h"
#include "../menu/menu.h"
#include "../utils/input/input.h"
#include "../utils/screen/screen.h"

using namespace std;

// turn one "id|name|contact|address" line back into a seller
bool parseSellerRecord(const char *line, SellerDetails &seller)
{
    stringstream stream(line);
    string id, name, contact, address;

    if (!getline(stream, id, '|') ||
        !getline(stream, name, '|') ||
        !getline(stream, contact, '|') ||
        !getline(stream, address))
    {
        return false;
    }

    seller.id = atoi(id.c_str());
    Input::copyTo(seller.name, sizeof(seller.name), name);
    Input::copyTo(seller.contact, sizeof(seller.contact), contact);
    Input::copyTo(seller.address, sizeof(seller.address), address);

    return seller.id > 0;
}

bool readSellerRecord(FILE *file, SellerDetails &seller)
{
    char line[512] = {};
    if (!fgets(line, sizeof(line), file))
    {
        return false;
    }

    line[strcspn(line, "\r\n")] = '\0';

    if (line[0] == '\0')
    {
        seller = {};
        return true;
    }

    return parseSellerRecord(line, seller);
}

void writeSellerRecord(FILE *file, const SellerDetails &seller)
{
    fprintf(file, "%d|%s|%s|%s\n",
            seller.id,
            seller.name,
            seller.contact,
            seller.address);
}

void printSeller(const SellerDetails &seller)
{
    Screen::beginRecord();
    Screen::printKeyValue("ID", to_string(seller.id));
    Screen::printKeyValue("Name", seller.name);
    Screen::printKeyValue("Contact", seller.contact);
    Screen::printKeyValue("Address", seller.address);
    Screen::endRecord();
}

Seller::Seller()
{
    strcpy(this->fileName, "sellers.txt");
}

int Seller::generateId()
{
    int id = 0;
    FILE *file = fopen("sellerID.txt", "r");

    if (file)
    {
        if (fscanf(file, "%d", &id) != 1)
        {
            id = 0;
        }
        fclose(file);
    }

    ++id;

    file = fopen("sellerID.txt", "w");
    if (!file)
    {
        return -1;
    }

    fprintf(file, "%d", id);
    fclose(file);
    return id;
}

// don't allow deleting a seller that still owns properties.
// in a property line the seller id is field number 5 (counting from 0)
bool Seller::sellerHasProperties(int id)
{
    FILE *file = fopen("properties.txt", "r");
    if (!file)
    {
        return false;
    }

    char line[512] = {};
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
        {
            continue;
        }

        stringstream stream(line);
        string field;
        int column = 0;
        int sellerId = 0;
        bool parsed = false;

        while (getline(stream, field, '|'))
        {
            if (column == 5) // reached the seller id column
            {
                sellerId = atoi(field.c_str());
                parsed = true;
                break;
            }
            ++column;
        }

        if (parsed && sellerId == id)
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
    FILE *file = fopen(this->fileName, "r");

    if (!file)
    {
        cout << "No seller records found.\n";
        return;
    }

    bool found = false;
    while (readSellerRecord(file, seller))
    {
        if (seller.id == 0)
        {
            continue;
        }
        found = true;
        printSeller(seller);
    }

    if (!found)
    {
        cout << "No seller records found.\n";
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

    Input::copyTo(newSeller.name, sizeof(newSeller.name),
                  Input::readString("Enter Seller Name: "));
    Input::copyTo(newSeller.contact, sizeof(newSeller.contact),
                  Input::readString("Enter Contact: "));
    Input::copyTo(newSeller.address, sizeof(newSeller.address),
                  Input::readString("Enter Address: "));

    FILE *file = fopen(this->fileName, "a");
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
    FILE *file = fopen(this->fileName, "r");

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
        Input::waitForEnter();
        return;
    }

    FILE *original = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

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
        if (seller.id == 0)
        {
            continue;
        }
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
    Input::waitForEnter();
}

void Seller::updateSellerById(int id)
{
    FILE *original = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

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
        if (seller.id == 0)
        {
            continue;
        }
        if (seller.id == id)
        {
            found = true;

            string name = Input::readString(
                string("Enter New Name (Current: ") + seller.name + "): ",
                seller.name);
            Input::copyTo(seller.name, sizeof(seller.name), name);

            string contact = Input::readString(
                string("Enter New Contact (Current: ") + seller.contact + "): ",
                seller.contact);
            Input::copyTo(seller.contact, sizeof(seller.contact), contact);

            string address = Input::readString(
                string("Enter New Address (Current: ") + seller.address + "): ",
                seller.address);
            Input::copyTo(seller.address, sizeof(seller.address), address);
        }

        writeSellerRecord(temp, seller);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Seller updated successfully!\n" : "Seller ID not found!\n");
    Input::waitForEnter();
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
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            int id = Input::readInt("Enter Seller ID to delete: ");
            deleteSellerById(id);
        }
        else if (choice == 2)
        {
            int id = Input::readInt("Enter Seller ID to update: ");
            updateSellerById(id);
        }
        else if (choice != 3)
        {
            cout << "Invalid choice!\n";
            Input::waitForEnter();
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
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            addSeller();
            Input::waitForEnter();
        }
        else if (choice == 2)
        {
            viewSellers();
        }
        else if (choice != 3)
        {
            cout << "Invalid choice.\n";
            Input::waitForEnter();
        }
    } while (choice != 3);
}
