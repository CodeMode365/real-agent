#include <iostream>
#include <cstdio>
#include <errno.h>
#include <cstring>
#include "client.h"
#include "../utils/screen/screen.h"
#include "../utils/files/file.h"
#include "../menu/menu.h"

using namespace std;

Client::Client()
{
    strcpy(this->fileName, "clients.txt");
}

void Client::readClientsFromFile()
{
    ClientDetails client;
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        cout << "No client records found.\n";
        return;
    }

    cout << "\n---- Client List ----\n";
    cout << "ID\tName\tAddress\tPhone\tEmail\n";
    cout << "---------------------------------------------------------------\n";

    while (fscanf(file, "%d %49s %69s %13s %69s",
                  &client.id,
                  client.name,
                  client.address,
                  client.phone,
                  client.email) == 5)
    {
        cout << client.id << "\t"
             << client.name << "\t"
             << client.address << "\t"
             << client.phone << "\t"
             << client.email << "\n";
    }

    fclose(file);
}
int Client::generateId()
{
    int id = 0; // Last ID

    // Try opening file for reading
    FILE* fp = fopen("clientID.txt", FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (fp != NULL)
    {
        // File exists, read last ID
        fscanf(fp, "%d", &id);
        fclose(fp);
    }
    else
    {
        if (errno == ENOENT)
        {
            // File does not exist → first run
            std::cout << "ID file not found. Starting with ID 0.\n";
            id = 0; // starting ID
        }
        else
        {
            std::cout << "File exists but cannot be opened.\n";
            return -1; // error
        }
    }

    // Increment to get new ID
    id++;

    // Save the new last ID back to file
    fp = fopen("clientID.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    if (fp == NULL)
    {
        std::cout << "Error creating/writing ID file!\n";
        return -1;
    }
    fprintf(fp, "%d", id);
    fclose(fp);

    return id;
}

void Client::addClient()
{
    Screen::clearScreen();
    Screen::printHeader("Add Client");

    ClientDetails client;

    //cout << "Enter Client ID: ";
   // cin >> client.id;
    cout << "Enter Client Name: ";
    cin >> client.name;
    cout << "Enter Client Address: ";
    cin >> client.address;
    cout << "Enter Client Phone: ";
    cin >> client.phone;
    cout << "Enter Client Email: ";
    cin >> client.email;

    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::APPEND));
    if (!file)
    {
        cout << "Error opening file.\n";
        return;
    }else{
      client.id = Client::generateId(); // Generate a unique ID for the new client
    }

    fprintf(file, "%d %s %s %s %s\n",
            client.id,
            client.name,
            client.address,
            client.phone,
            client.email);

    fclose(file);
    cout << "Client added successfully!\n";
}

ClientDetails Client::getClientById(int id)
{
    ClientDetails client = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
        return client;

    while (fscanf(file, "%d %49s %69s %13s %69s",
                  &client.id,
                  client.name,
                  client.address,
                  client.phone,
                  client.email) == 5)
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

void Client::deleteClientById(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt",FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    ClientDetails client;
    bool found = false;

    if (!original || !temp)
    {
        cout << "Error handling files.\n";
        if (original)
            fclose(original);
        if (temp)
            fclose(temp);
        return;
    }

    while (fscanf(original, "%d %49s %69s %13s %69s",
                  &client.id,
                  client.name,
                  client.address,
                  client.phone,
                  client.email) == 5)
    {
        if (client.id != id)
        {
            fprintf(temp, "%d %s %s %s %s\n",
                    client.id,
                    client.name,
                    client.address,
                    client.phone,
                    client.email);
        }
        else
        {
            found = true;
        }
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    if (found)
    {
        cout << "Client deleted successfully!\n";
    }
    else
    {
        cout << "Client ID not found!\n";
    }
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void Client::updateClientById(int id)
{
    FILE *original = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    ClientDetails client;
    bool found = false;

    if (!original || !temp)
    {
        cout << "Error handling files.\n";
        if (original)
            fclose(original);
        if (temp)
            fclose(temp);
        return;
    }

    while (fscanf(original, "%d %49s %69s %13s %69s",
                  &client.id,
                  client.name,
                  client.address,
                  client.phone,
                  client.email) == 5)
    {
        if (client.id == id)
        {
            found = true;
            cout << "Enter New Name (Current: " << client.name << "): ";
            cin >> client.name;
            cout << "Enter New Address (Current: " << client.address << "): ";
            cin >> client.address;
            cout << "Enter New Phone (Current: " << client.phone << "): ";
            cin >> client.phone;
            cout << "Enter New Email (Current: " << client.email << "): ";
            cin >> client.email;
        }

        fprintf(temp, "%d %s %s %s %s\n",
                client.id,
                client.name,
                client.address,
                client.phone,
                client.email);
    }

    fclose(original);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    if (found)
    {
        cout << "Client updated successfully!\n";
    }
    else
    {
        cout << "Client ID not found!\n";
    }
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void Client::viewClients()
{
    int choice;
    do
    {
        Screen::clearScreen();
        Screen::printHeader("View Clients");
        readClientsFromFile();

        cout << "\nChoose an option:\n";
        cout << "1. Delete Client by ID\n";
        cout << "2. Update Client details\n";
        cout << "3. Back to Client Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 3)
            break;

        int id;
        switch (choice)
        {
        case 1:
            cout << "Enter Client ID to delete: ";
            cin >> id;
            deleteClientById(id);
            break;
        case 2:
            cout << "Enter Client ID to update: ";
            cin >> id;
            updateClientById(id);
            break;
        default:
            cout << "Invalid choice!\n";
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get();
            break;
        }
    } while (true);
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
    int choice;
    Screen::clearScreen();
    Screen::printHeader("Client Menu");
    cout << "1. Add Client\n";
    cout << "2. View Clients\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter choice: ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        addClient();
        break;
    case 2:
        viewClients();
        break;
    case 3:
        Menu::showMenu();
        break;
    default:
        cout << "Invalid choice.\n";
        break;
    }
}
