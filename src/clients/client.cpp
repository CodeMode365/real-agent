#include <cstdio>
#include <cstring>
#include <iostream>
#include "client.h"
#include "../menu/menu.h"
#include "../seller/seller.h"
#include "../utils/files/file.h"
#include "../utils/screen/screen.h"

using namespace std;

namespace
{
    bool readClientRecord(FILE *file, ClientDetails &client)
    {
        char line[1024] = {};
        if (!fgets(line, sizeof(line), file))
        {
            return false;
        }

        if (sscanf(line, "%d|%49[^|]|%69[^|]|%13[^|]|%69[^|]|%49[^|]|%d|%d|%f|%f|%d",
                   &client.id,
                   client.name,
                   client.address,
                   client.phone,
                   client.email,
                   client.location,
                   &client.minArea,
                   &client.maxArea,
                   &client.minPrice,
                   &client.maxPrice,
                   &client.sellerId) == 11)
        {
            return true;
        }

        return sscanf(line, "%d %49s %69s %13s %69s %49s %d %d %f %f %d",
                      &client.id,
                      client.name,
                      client.address,
                      client.phone,
                      client.email,
                      client.location,
                      &client.minArea,
                      &client.maxArea,
                      &client.minPrice,
                      &client.maxPrice,
                      &client.sellerId) == 11;
    }

    void writeClientRecord(FILE *file, const ClientDetails &client)
    {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%d|%d|%.2f|%.2f|%d\n",
                client.id,
                client.name,
                client.address,
                client.phone,
                client.email,
                client.location,
                client.minArea,
                client.maxArea,
                client.minPrice,
                client.maxPrice,
                client.sellerId);
    }

    void waitForEnter()
    {
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

Client::Client()
{
    strcpy(this->fileName, "clients.txt");
}

int Client::generateId()
{
    int id = 0;
    FILE *file = fopen("clientID.txt", FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (file)
    {
        fscanf(file, "%d", &id);
        fclose(file);
    }

    ++id;

    file = fopen("clientID.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    if (!file)
    {
        return -1;
    }

    fprintf(file, "%d", id);
    fclose(file);
    return id;
}

void Client::readClientsFromFile()
{
    ClientDetails client = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        cout << "No client records found.\n";
        return;
    }

    cout << "\nID\tName\tLocation\tAreaRange\tPriceRange\tSellerID\n";
    cout << "-----------------------------------------------------------------\n";

    while (readClientRecord(file, client))
    {
        cout << client.id << "\t"
             << client.name << "\t"
             << client.location << "\t"
             << client.minArea << "-" << client.maxArea << "\t\t"
             << client.minPrice << "-" << client.maxPrice << "\t"
             << client.sellerId << "\n";
    }

    fclose(file);
}

void Client::addClient()
{
    Screen::clearScreen();
    Screen::printHeader("Add Client");

    ClientDetails client = {};
    client.id = generateId();

    if (client.id < 0)
    {
        cout << "Unable to generate client ID.\n";
        return;
    }

    cout << "Client ID: " << client.id << "\n";
    cout << "Enter Client Name: ";
    cin >> ws;
    cin.getline(client.name, sizeof(client.name));
    cout << "Enter Client Address: ";
    cin.getline(client.address, sizeof(client.address));
    cout << "Enter Client Phone: ";
    cin.getline(client.phone, sizeof(client.phone));
    cout << "Enter Client Email: ";
    cin.getline(client.email, sizeof(client.email));
    cout << "Enter Client Requirements\n";
    cout << "Location (0 for any): ";
    cin.getline(client.location, sizeof(client.location));
    cout << "Minimum Area (0 for any): ";
    cin >> client.minArea;
    cout << "Maximum Area (0 for any): ";
    cin >> client.maxArea;
    cout << "Minimum Price (0 for any): ";
    cin >> client.minPrice;
    cout << "Maximum Price (0 for any): ";
    cin >> client.maxPrice;
    cout << "Preferred Seller ID (0 for any): ";
    cin >> client.sellerId;

    if (client.sellerId > 0)
    {
        Seller seller;
        if (seller.getSellerById(client.sellerId).id == 0)
        {
            cout << "Seller ID not found.\n";
            return;
        }
    }

    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::APPEND));
    if (!file)
    {
        cout << "Error opening file.\n";
        return;
    }

    writeClientRecord(file, client);
    fclose(file);

    cout << "Client added successfully!\n";
}

ClientDetails Client::getClientById(int id)
{
    ClientDetails client = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        return client;
    }

    while (readClientRecord(file, client))
    {
        if (client.id == id)
        {
            fclose(file);
            return client;
        }
    }

    fclose(file);
    client.id = 0;
    return client;
}

void Client::updateClientRequirements(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    ClientDetails client = {};
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

    while (readClientRecord(original, client))
    {
        if (client.id == id)
        {
            found = true;
            cout << "Location (Current: " << client.location << ", 0 for any): ";
            cin >> ws;
            cin.getline(client.location, sizeof(client.location));
            cout << "Minimum Area (Current: " << client.minArea << "): ";
            cin >> client.minArea;
            cout << "Maximum Area (Current: " << client.maxArea << "): ";
            cin >> client.maxArea;
            cout << "Minimum Price (Current: " << client.minPrice << "): ";
            cin >> client.minPrice;
            cout << "Maximum Price (Current: " << client.maxPrice << "): ";
            cin >> client.maxPrice;
            cout << "Preferred Seller ID (Current: " << client.sellerId << ", 0 for any): ";
            cin >> client.sellerId;

            if (client.sellerId > 0)
            {
                Seller seller;
                if (seller.getSellerById(client.sellerId).id == 0)
                {
                    cout << "Seller ID not found. Resetting preferred seller to any.\n";
                    client.sellerId = 0;
                }
            }
        }

        writeClientRecord(temp, client);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Client requirements updated successfully!\n" : "Client ID not found!\n");
    waitForEnter();
}

void Client::deleteClientById(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    ClientDetails client = {};
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

    while (readClientRecord(original, client))
    {
        if (client.id == id)
        {
            found = true;
            continue;
        }

        writeClientRecord(temp, client);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Client deleted successfully!\n" : "Client ID not found!\n");
    waitForEnter();
}

void Client::updateClientById(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    ClientDetails client = {};
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

    while (readClientRecord(original, client))
    {
        if (client.id == id)
        {
            found = true;
            cout << "Enter New Name (Current: " << client.name << "): ";
            cin >> ws;
            cin.getline(client.name, sizeof(client.name));
            cout << "Enter New Address (Current: " << client.address << "): ";
            cin.getline(client.address, sizeof(client.address));
            cout << "Enter New Phone (Current: " << client.phone << "): ";
            cin.getline(client.phone, sizeof(client.phone));
            cout << "Enter New Email (Current: " << client.email << "): ";
            cin.getline(client.email, sizeof(client.email));
            cout << "Update requirements as well.\n";
            cout << "Location (Current: " << client.location << ", 0 for any): ";
            cin.getline(client.location, sizeof(client.location));
            cout << "Minimum Area (Current: " << client.minArea << "): ";
            cin >> client.minArea;
            cout << "Maximum Area (Current: " << client.maxArea << "): ";
            cin >> client.maxArea;
            cout << "Minimum Price (Current: " << client.minPrice << "): ";
            cin >> client.minPrice;
            cout << "Maximum Price (Current: " << client.maxPrice << "): ";
            cin >> client.maxPrice;
            cout << "Preferred Seller ID (Current: " << client.sellerId << ", 0 for any): ";
            cin >> client.sellerId;

            if (client.sellerId > 0)
            {
                Seller seller;
                if (seller.getSellerById(client.sellerId).id == 0)
                {
                    cout << "Seller ID not found. Resetting preferred seller to any.\n";
                    client.sellerId = 0;
                }
            }
        }

        writeClientRecord(temp, client);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Client updated successfully!\n" : "Client ID not found!\n");
    waitForEnter();
}

void Client::viewClients()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("View Clients");
        readClientsFromFile();

        cout << "\n1. Delete Client by ID\n";
        cout << "2. Update Client\n";
        cout << "3. Update Client Requirements\n";
        cout << "4. Back\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int id = 0;
            cout << "Enter Client ID to delete: ";
            cin >> id;
            deleteClientById(id);
        }
        else if (choice == 2)
        {
            int id = 0;
            cout << "Enter Client ID to update: ";
            cin >> id;
            updateClientById(id);
        }
        else if (choice == 3)
        {
            int id = 0;
            cout << "Enter Client ID to update requirements: ";
            cin >> id;
            updateClientRequirements(id);
        }
        else if (choice != 4)
        {
            cout << "Invalid choice!\n";
            waitForEnter();
        }
    } while (choice != 4);
}

void Client::enterDetails()
{
    addClient();
}

void Client::viewDetails()
{
    viewClients();
}

void Client::menu()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("Client Menu");
        cout << "1. Add Client\n";
        cout << "2. View Clients\n";
        cout << "3. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            addClient();
            waitForEnter();
        }
        else if (choice == 2)
        {
            viewClients();
        }
        else if (choice != 3)
        {
            cout << "Invalid choice.\n";
            waitForEnter();
        }
    } while (choice != 3);
}
