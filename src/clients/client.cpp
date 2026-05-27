#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "client.h"
#include "../menu/menu.h"
#include "../seller/seller.h"
#include "../utils/files/file.h"
#include "../utils/input/input.h"
#include "../utils/screen/screen.h"

using namespace std;

namespace
{
    bool parseClientRecord(const char *line, ClientDetails &client)
    {
        std::stringstream stream(line);
        std::string fields[11];

        for (int i = 0; i < 11; ++i)
        {
            const char delim = (i == 10) ? '\n' : '|';
            if (!std::getline(stream, fields[i], delim))
            {
                return false;
            }
        }

        client.id = std::atoi(fields[0].c_str());
        Input::copyTo(client.name, sizeof(client.name), fields[1]);
        Input::copyTo(client.address, sizeof(client.address), fields[2]);
        Input::copyTo(client.phone, sizeof(client.phone), fields[3]);
        Input::copyTo(client.email, sizeof(client.email), fields[4]);
        Input::copyTo(client.location, sizeof(client.location), fields[5]);
        client.minArea = std::atoi(fields[6].c_str());
        client.maxArea = std::atoi(fields[7].c_str());
        client.minPrice = static_cast<float>(std::atof(fields[8].c_str()));
        client.maxPrice = static_cast<float>(std::atof(fields[9].c_str()));
        client.sellerId = std::atoi(fields[10].c_str());

        return client.id > 0;
    }

    bool readClientRecord(FILE *file, ClientDetails &client)
    {
        char line[1024] = {};
        if (!fgets(line, sizeof(line), file))
        {
            return false;
        }

        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0')
        {
            client = {};
            return true;
        }

        return parseClientRecord(line, client);
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

    std::string formatFloat(float value)
    {
        std::ostringstream out;
        out.setf(std::ios::fixed);
        out.precision(2);
        out << value;
        return out.str();
    }

    void printClient(const ClientDetails &client)
    {
        Screen::beginRecord();
        Screen::printKeyValue("ID", std::to_string(client.id));
        Screen::printKeyValue("Name", client.name);
        Screen::printKeyValue("Address", client.address);
        Screen::printKeyValue("Phone", client.phone);
        Screen::printKeyValue("Email", client.email);
        Screen::printKeyValue("Pref Location", client.location);
        Screen::printKeyValue("Area Range",
                              std::to_string(client.minArea) + " - " + std::to_string(client.maxArea));
        Screen::printKeyValue("Price Range",
                              formatFloat(client.minPrice) + " - " + formatFloat(client.maxPrice));
        Screen::printKeyValue("Pref Seller ID", std::to_string(client.sellerId));
        Screen::endRecord();
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
        if (fscanf(file, "%d", &id) != 1)
        {
            id = 0;
        }
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

    bool found = false;
    while (readClientRecord(file, client))
    {
        if (client.id == 0)
        {
            continue;
        }
        found = true;
        printClient(client);
    }

    if (!found)
    {
        cout << "No client records found.\n";
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

    Input::copyTo(client.name, sizeof(client.name),
                  Input::readString("Enter Client Name: "));
    Input::copyTo(client.address, sizeof(client.address),
                  Input::readString("Enter Client Address: "));
    Input::copyTo(client.phone, sizeof(client.phone),
                  Input::readString("Enter Client Phone: "));
    Input::copyTo(client.email, sizeof(client.email),
                  Input::readString("Enter Client Email: "));

    cout << "Enter Client Requirements\n";
    Input::copyTo(client.location, sizeof(client.location),
                  Input::readString("Location (blank/0 for any): "));
    client.minArea = Input::readInt("Minimum Area (0 for any): ");
    client.maxArea = Input::readInt("Maximum Area (0 for any): ");
    client.minPrice = Input::readFloat("Minimum Price (0 for any): ");
    client.maxPrice = Input::readFloat("Maximum Price (0 for any): ");
    client.sellerId = Input::readInt("Preferred Seller ID (0 for any): ");

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
        if (client.id == 0)
        {
            continue;
        }
        if (client.id == id)
        {
            found = true;
            const int previousSellerId = client.sellerId;

            const std::string location = Input::readString(
                std::string("Location (Current: ") + client.location + ", blank/0 for any): ",
                client.location);
            Input::copyTo(client.location, sizeof(client.location), location);

            client.minArea = Input::readInt(
                std::string("Minimum Area (Current: ") + std::to_string(client.minArea) + "): ",
                client.minArea);
            client.maxArea = Input::readInt(
                std::string("Maximum Area (Current: ") + std::to_string(client.maxArea) + "): ",
                client.maxArea);
            client.minPrice = Input::readFloat(
                std::string("Minimum Price (Current: ") + formatFloat(client.minPrice) + "): ",
                client.minPrice);
            client.maxPrice = Input::readFloat(
                std::string("Maximum Price (Current: ") + formatFloat(client.maxPrice) + "): ",
                client.maxPrice);
            client.sellerId = Input::readInt(
                std::string("Preferred Seller ID (Current: ") + std::to_string(client.sellerId) + ", 0 for any): ",
                client.sellerId);

            if (client.sellerId > 0)
            {
                Seller seller;
                if (seller.getSellerById(client.sellerId).id == 0)
                {
                    cout << "Seller ID not found. Keeping previous preferred seller.\n";
                    client.sellerId = previousSellerId;
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
    Input::waitForEnter();
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
        if (client.id == 0)
        {
            continue;
        }
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
    Input::waitForEnter();
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
        if (client.id == 0)
        {
            continue;
        }
        if (client.id == id)
        {
            found = true;
            const int previousSellerId = client.sellerId;

            const std::string newName = Input::readString(
                std::string("Enter New Name (Current: ") + client.name + "): ",
                client.name);
            Input::copyTo(client.name, sizeof(client.name), newName);

            const std::string newAddress = Input::readString(
                std::string("Enter New Address (Current: ") + client.address + "): ",
                client.address);
            Input::copyTo(client.address, sizeof(client.address), newAddress);

            const std::string newPhone = Input::readString(
                std::string("Enter New Phone (Current: ") + client.phone + "): ",
                client.phone);
            Input::copyTo(client.phone, sizeof(client.phone), newPhone);

            const std::string newEmail = Input::readString(
                std::string("Enter New Email (Current: ") + client.email + "): ",
                client.email);
            Input::copyTo(client.email, sizeof(client.email), newEmail);

            cout << "Update requirements as well.\n";

            const std::string location = Input::readString(
                std::string("Location (Current: ") + client.location + ", blank/0 for any): ",
                client.location);
            Input::copyTo(client.location, sizeof(client.location), location);

            client.minArea = Input::readInt(
                std::string("Minimum Area (Current: ") + std::to_string(client.minArea) + "): ",
                client.minArea);
            client.maxArea = Input::readInt(
                std::string("Maximum Area (Current: ") + std::to_string(client.maxArea) + "): ",
                client.maxArea);
            client.minPrice = Input::readFloat(
                std::string("Minimum Price (Current: ") + formatFloat(client.minPrice) + "): ",
                client.minPrice);
            client.maxPrice = Input::readFloat(
                std::string("Maximum Price (Current: ") + formatFloat(client.maxPrice) + "): ",
                client.maxPrice);
            client.sellerId = Input::readInt(
                std::string("Preferred Seller ID (Current: ") + std::to_string(client.sellerId) + ", 0 for any): ",
                client.sellerId);

            if (client.sellerId > 0)
            {
                Seller seller;
                if (seller.getSellerById(client.sellerId).id == 0)
                {
                    cout << "Seller ID not found. Keeping previous preferred seller.\n";
                    client.sellerId = previousSellerId;
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
    Input::waitForEnter();
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
        // cout << "3. Update Client Requirements\n";
        cout << "3. Back\n";
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            const int id = Input::readInt("Enter Client ID to delete: ");
            deleteClientById(id);
        }
        else if (choice == 2)
        {
            const int id = Input::readInt("Enter Client ID to update: ");
            updateClientById(id);
        }
        // else if (choice == 3)
        // {
        //     const int id = Input::readInt("Enter Client ID to update requirements: ");
        //     updateClientRequirements(id);
        // }
        else if (choice != 3)
        {
            cout << "Invalid choice!\n";
            Input::waitForEnter();
        }
    } while (choice != 3);
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
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            addClient();
            Input::waitForEnter();
        }
        else if (choice == 2)
        {
            viewClients();
        }
        else if (choice != 3)
        {
            cout << "Invalid choice.\n";
            Input::waitForEnter();
        }
    } while (choice != 3);
}
