#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "property.h"
#include "../clients/client.h"
#include "../menu/menu.h"
#include "../seller/seller.h"
#include "../utils/files/file.h"
#include "../utils/screen/screen.h"

using namespace std;

namespace
{
    bool parsePropertyPipeRecord(const char *line, PropertyDetails &property)
    {
        std::stringstream stream(line);
        std::string id;
        std::string type;
        std::string location;
        std::string price;
        std::string size;
        std::string sellerId;
        std::string status;

        if (!std::getline(stream, id, '|') ||
            !std::getline(stream, type, '|') ||
            !std::getline(stream, location, '|') ||
            !std::getline(stream, price, '|') ||
            !std::getline(stream, size, '|') ||
            !std::getline(stream, sellerId, '|') ||
            !std::getline(stream, status))
        {
            return false;
        }

        property.id = std::atoi(id.c_str());
        std::strncpy(property.type, type.c_str(), sizeof(property.type) - 1);
        property.type[sizeof(property.type) - 1] = '\0';
        std::strncpy(property.location, location.c_str(), sizeof(property.location) - 1);
        property.location[sizeof(property.location) - 1] = '\0';
        property.price = std::atof(price.c_str());
        property.size = std::atoi(size.c_str());
        property.sellerId = std::atoi(sellerId.c_str());
        std::strncpy(property.status, status.c_str(), sizeof(property.status) - 1);
        property.status[sizeof(property.status) - 1] = '\0';

        return property.id > 0;
    }

    bool readPropertyRecord(FILE *file, PropertyDetails &property)
    {
        char line[512] = {};
        if (!fgets(line, sizeof(line), file))
        {
            return false;
        }

        line[strcspn(line, "\r\n")] = '\0';

        if (strchr(line, '|') != nullptr && parsePropertyPipeRecord(line, property))
        {
            return true;
        }

        return sscanf(line, "%d %49s %99s %f %d %d %19s",
                      &property.id,
                      property.type,
                      property.location,
                      &property.price,
                      &property.size,
                      &property.sellerId,
                      property.status) == 7;
    }

    void writePropertyRecord(FILE *file, const PropertyDetails &property)
    {
        fprintf(file, "%d|%s|%s|%.2f|%d|%d|%s\n",
                property.id,
                property.type,
                property.location,
                property.price,
                property.size,
                property.sellerId,
                property.status);
    }

    void printPropertyHeader()
    {
        cout << "\n"
             << left
             << setw(8) << "ID"
             << setw(18) << "Type"
             << setw(20) << "Location"
             << setw(14) << "Price"
             << setw(10) << "Size"
             << setw(12) << "Seller ID"
             << setw(12) << "Status" << "\n";
        cout << string(94, '-') << "\n";
    }

    void printPropertyRow(const PropertyDetails &property)
    {
        cout << left
             << setw(8) << property.id
             << setw(18) << property.type
             << setw(20) << property.location
             << setw(14) << fixed << setprecision(2) << property.price
             << setw(10) << property.size
             << setw(12) << property.sellerId
             << setw(12) << property.status << "\n";
    }

    void waitForEnter()
    {
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }
}

Property::Property()
{
    strcpy(this->fileName, "properties.txt");
}

int Property::generateId()
{
    int id = 0;
    FILE *file = fopen("propertyID.txt", FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (file)
    {
        fscanf(file, "%d", &id);
        fclose(file);
    }

    ++id;

    file = fopen("propertyID.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));
    if (!file)
    {
        return -1;
    }

    fprintf(file, "%d", id);
    fclose(file);
    return id;
}

void Property::readPropertiesFromFile(const char *searchTerm)
{
    PropertyDetails property = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        cout << "No property records found.\n";
        return;
    }

    printPropertyHeader();
    bool found = false;

    while (readPropertyRecord(file, property))
    {
        if (searchTerm == nullptr ||
            searchTerm[0] == '\0' ||
            strcmp(searchTerm, "0") == 0 ||
            strcmp(property.location, searchTerm) == 0)
        {
            found = true;
            printPropertyRow(property);
        }
    }

    if (!found)
    {
        cout << "No properties matched the current view.\n";
    }

    fclose(file);
}

PropertyDetails Property::getPropertyById(int id)
{
    PropertyDetails property = {};
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (!file)
    {
        return property;
    }

    while (readPropertyRecord(file, property))
    {
        if (property.id == id)
        {
            fclose(file);
            return property;
        }
    }

    fclose(file);
    property.id = 0;
    return property;
}

void Property::addProperty()
{
    Screen::clearScreen();
    Screen::printHeader("Add New Property");

    PropertyDetails property = {};
    property.id = generateId();

    if (property.id < 0)
    {
        cout << "Unable to generate property ID.\n";
        return;
    }

    cout << "Property ID: " << property.id << "\n";
    cout << "Enter Property Type: ";
    cin >> ws;
    cin.getline(property.type, sizeof(property.type));
    cout << "Enter Location: ";
    cin.getline(property.location, sizeof(property.location));
    cout << "Enter Price: ";
    cin >> property.price;
    cout << "Enter Size (in sqft): ";
    cin >> property.size;
    cout << "Enter Seller ID: ";
    cin >> property.sellerId;

    Seller seller;
    if (seller.getSellerById(property.sellerId).id == 0)
    {
        cout << "Seller ID not found. Add the seller first.\n";
        return;
    }

    strcpy(property.status, "Available");

    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::APPEND));
    if (!file)
    {
        cout << "Error opening file.\n";
        return;
    }

    writePropertyRecord(file, property);
    fclose(file);

    cout << "Property added successfully!\n";
}

void Property::deletePropertyById(int id)
{
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    if (!file || !temp)
    {
        cerr << "Error opening file.\n";
        if (file)
        {
            fclose(file);
        }
        if (temp)
        {
            fclose(temp);
        }
        return;
    }

    PropertyDetails property = {};
    bool found = false;

    while (readPropertyRecord(file, property))
    {
        if (property.id == id)
        {
            found = true;
            continue;
        }

        writePropertyRecord(temp, property);
    }

    fclose(file);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Property deleted successfully!\n" : "Property ID not found!\n");
    waitForEnter();
}

void Property::updatePropertyById(int id)
{
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    FILE *temp = fopen("temp.txt", FileUtils::getFileModeString(FileUtils::FileMode::WRITE));

    if (!file || !temp)
    {
        cerr << "Error opening file.\n";
        if (file)
        {
            fclose(file);
        }
        if (temp)
        {
            fclose(temp);
        }
        return;
    }

    PropertyDetails property = {};
    bool found = false;

    while (readPropertyRecord(file, property))
    {
        if (property.id == id)
        {
            found = true;
            const int currentSellerId = property.sellerId;
            cout << "Enter New Property Type (Current: " << property.type << "): ";
            cin >> ws;
            cin.getline(property.type, sizeof(property.type));
            cout << "Enter New Location (Current: " << property.location << "): ";
            cin.getline(property.location, sizeof(property.location));
            cout << "Enter New Price (Current: " << property.price << "): ";
            cin >> property.price;
            cout << "Enter New Size (Current: " << property.size << "): ";
            cin >> property.size;
            cout << "Enter New Seller ID (Current: " << property.sellerId << "): ";
            cin >> property.sellerId;

            Seller seller;
            if (seller.getSellerById(property.sellerId).id == 0)
            {
                cout << "Seller ID not found. Keeping the existing seller link.\n";
                property.sellerId = currentSellerId;
            }

            cout << "Enter New Status (Available/Sold, Current: " << property.status << "): ";
            cin >> ws;
            cin.getline(property.status, sizeof(property.status));
        }

        writePropertyRecord(temp, property);
    }

    fclose(file);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Property updated successfully!\n" : "Property ID not found!\n");
    waitForEnter();
}

void Property::viewProperties()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("View Properties");
        readPropertiesFromFile();

        cout << "\n1. Delete Property by ID\n";
        cout << "2. Update Property by ID\n";
        cout << "3. Back\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int id = 0;
            cout << "Enter Property ID to delete: ";
            cin >> id;
            deletePropertyById(id);
        }
        else if (choice == 2)
        {
            int id = 0;
            cout << "Enter Property ID to update: ";
            cin >> id;
            updatePropertyById(id);
        }
        else if (choice != 3)
        {
            cout << "Invalid choice!\n";
            waitForEnter();
        }
    } while (choice != 3);
}

void Property::searchPropertyByRequirements()
{
    Client clientManager;
    int clientId = 0;

    Screen::clearScreen();
    Screen::printHeader("Search Property For Client");
    cout << "Enter Client ID: ";
    cin >> clientId;

    ClientDetails client = clientManager.getClientById(clientId);
    if (client.id == 0)
    {
        cout << "Client not found.\n";
        waitForEnter();
        return;
    }

    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));
    if (!file)
    {
        cout << "No property records found.\n";
        waitForEnter();
        return;
    }

    PropertyDetails property = {};
    bool found = false;

    printPropertyHeader();

    while (readPropertyRecord(file, property))
    {
        bool matches = strcmp(property.status, "Available") == 0;

        if (matches && strcmp(client.location, "0") != 0 && strcmp(client.location, property.location) != 0)
        {
            matches = false;
        }
        if (matches && client.minArea > 0 && property.size < client.minArea)
        {
            matches = false;
        }
        if (matches && client.maxArea > 0 && property.size > client.maxArea)
        {
            matches = false;
        }
        if (matches && client.minPrice > 0 && property.price < client.minPrice)
        {
            matches = false;
        }
        if (matches && client.maxPrice > 0 && property.price > client.maxPrice)
        {
            matches = false;
        }
        if (matches && client.sellerId > 0 && property.sellerId != client.sellerId)
        {
            matches = false;
        }

        if (matches)
        {
            found = true;
            printPropertyRow(property);
        }
    }

    fclose(file);

    if (!found)
    {
        cout << "No available properties matched the client requirements.\n";
    }

    waitForEnter();
}

void Property::menu()
{
    int choice = 0;

    do
    {
        Screen::clearScreen();
        Screen::printHeader("Property Menu");
        cout << "1. Add Property\n";
        cout << "2. View Properties\n";
        cout << "3. Search by Client Requirements\n";
        cout << "4. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            addProperty();
            waitForEnter();
        }
        else if (choice == 2)
        {
            viewProperties();
        }
        else if (choice == 3)
        {
            searchPropertyByRequirements();
        }
        else if (choice != 4)
        {
            cout << "Invalid choice. Please try again.\n";
            waitForEnter();
        }
    } while (choice != 4);
}
