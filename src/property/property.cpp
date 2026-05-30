#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "property.h"
#include "../clients/client.h"
#include "../menu/menu.h"
#include "../seller/seller.h"
#include "../utils/input/input.h"
#include "../utils/screen/screen.h"

using namespace std;

bool parsePropertyRecord(const char *line, PropertyDetails &property)
{
    stringstream stream(line);
    string id, type, location, price, size, sellerId, status;

    if (!getline(stream, id, '|') ||
        !getline(stream, type, '|') ||
        !getline(stream, location, '|') ||
        !getline(stream, price, '|') ||
        !getline(stream, size, '|') ||
        !getline(stream, sellerId, '|') ||
        !getline(stream, status))
    {
        return false;
    }

    property.id = atoi(id.c_str());
    Input::copyTo(property.type, sizeof(property.type), type);
    Input::copyTo(property.location, sizeof(property.location), location);
    property.price = (float)atof(price.c_str());
    property.size = atoi(size.c_str());
    property.sellerId = atoi(sellerId.c_str());
    Input::copyTo(property.status, sizeof(property.status), status);

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

    if (line[0] == '\0')
    {
        property = {};
        return true;
    }

    return parsePropertyRecord(line, property);
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

void printProperty(const PropertyDetails &property)
{
    Screen::beginRecord();
    Screen::printKeyValue("ID", to_string(property.id));
    Screen::printKeyValue("Type", property.type);
    Screen::printKeyValue("Location", property.location);

    ostringstream price;
    price.setf(ios::fixed);
    price.precision(2);
    price << property.price;
    Screen::printKeyValue("Price", price.str());

    Screen::printKeyValue("Size (sqft)", to_string(property.size));
    Screen::printKeyValue("Seller ID", to_string(property.sellerId));
    Screen::printKeyValue("Status", property.status);
    Screen::endRecord();
}

Property::Property()
{
    strcpy(this->fileName, "properties.txt");
}

int Property::generateId()
{
    int id = 0;
    FILE *file = fopen("propertyID.txt", "r");

    if (file)
    {
        if (fscanf(file, "%d", &id) != 1)
        {
            id = 0;
        }
        fclose(file);
    }

    ++id;

    file = fopen("propertyID.txt", "w");
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
    FILE *file = fopen(this->fileName, "r");

    if (!file)
    {
        cout << "No property records found.\n";
        return;
    }

    bool found = false;

    while (readPropertyRecord(file, property))
    {
        if (property.id == 0)
        {
            continue;
        }
        if (searchTerm == nullptr ||
            searchTerm[0] == '\0' ||
            strcmp(searchTerm, "0") == 0 ||
            strcmp(searchTerm, Input::NA.c_str()) == 0 ||
            strcmp(property.location, searchTerm) == 0)
        {
            found = true;
            printProperty(property);
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
    FILE *file = fopen(this->fileName, "r");

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

    Input::copyTo(property.type, sizeof(property.type),
                  Input::readString("Enter Property Type: "));
    Input::copyTo(property.location, sizeof(property.location),
                  Input::readString("Enter Location: "));
    property.price = Input::readFloat("Enter Price: ");
    property.size = Input::readInt("Enter Size (in sqft): ");
    property.sellerId = Input::readInt("Enter Seller ID: ");

    Seller seller;
    if (property.sellerId <= 0 || seller.getSellerById(property.sellerId).id == 0)
    {
        cout << "Seller ID not found. Add the seller first.\n";
        return;
    }

    Input::copyTo(property.status, sizeof(property.status), "Available");

    FILE *file = fopen(this->fileName, "a");
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
    FILE *file = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

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
        if (property.id == 0)
        {
            continue;
        }
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
    Input::waitForEnter();
}

void Property::updatePropertyById(int id)
{
    FILE *file = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

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
        if (property.id == 0)
        {
            continue;
        }
        if (property.id == id)
        {
            found = true;
            int currentSellerId = property.sellerId;

            string type = Input::readString(
                string("Enter New Property Type (Current: ") + property.type + "): ",
                property.type);
            Input::copyTo(property.type, sizeof(property.type), type);

            string location = Input::readString(
                string("Enter New Location (Current: ") + property.location + "): ",
                property.location);
            Input::copyTo(property.location, sizeof(property.location), location);

            property.price = Input::readFloat(
                string("Enter New Price (Current: ") + to_string(property.price) + "): ",
                property.price);
            property.size = Input::readInt(
                string("Enter New Size (Current: ") + to_string(property.size) + "): ",
                property.size);
            property.sellerId = Input::readInt(
                string("Enter New Seller ID (Current: ") + to_string(property.sellerId) + "): ",
                property.sellerId);

            Seller seller;
            if (property.sellerId <= 0 || seller.getSellerById(property.sellerId).id == 0)
            {
                cout << "Seller ID not found. Keeping the existing seller link.\n";
                property.sellerId = currentSellerId;
            }

            string status = Input::readString(
                string("Enter New Status (Available/Sold, Current: ") + property.status + "): ",
                property.status);
            Input::copyTo(property.status, sizeof(property.status), status);
        }

        writePropertyRecord(temp, property);
    }

    fclose(file);
    fclose(temp);

    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << (found ? "Property updated successfully!\n" : "Property ID not found!\n");
    Input::waitForEnter();
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
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            int id = Input::readInt("Enter Property ID to delete: ");
            deletePropertyById(id);
        }
        else if (choice == 2)
        {
            int id = Input::readInt("Enter Property ID to update: ");
            updatePropertyById(id);
        }
        else if (choice != 3)
        {
            cout << "Invalid choice!\n";
            Input::waitForEnter();
        }
    } while (choice != 3);
}

void Property::searchPropertyByRequirements()
{
    Client clientManager;

    Screen::clearScreen();
    Screen::printHeader("Search Property For Client");
    int clientId = Input::readInt("Enter Client ID: ");

    ClientDetails client = clientManager.getClientById(clientId);
    if (client.id == 0)
    {
        cout << "Client not found.\n";
        Input::waitForEnter();
        return;
    }

    FILE *file = fopen(this->fileName, "r");
    if (!file)
    {
        cout << "No property records found.\n";
        Input::waitForEnter();
        return;
    }

    PropertyDetails property = {};
    bool found = false;

    while (readPropertyRecord(file, property))
    {
        if (property.id == 0)
        {
            continue;
        }
        bool matches = strcmp(property.status, "Available") == 0;

        bool anyLocation = strcmp(client.location, "0") == 0 ||
                           strcmp(client.location, Input::NA.c_str()) == 0;
        if (matches && !anyLocation && strcmp(client.location, property.location) != 0)
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
            printProperty(property);
        }
    }

    fclose(file);

    if (!found)
    {
        cout << "No available properties matched the client requirements.\n";
    }

    Input::waitForEnter();
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
        // cout << "3. Search by Client Requirements\n";
        cout << "3. Back to Main Menu\n";
        choice = Input::readChoice("Enter choice: ");

        if (choice == 1)
        {
            addProperty();
            Input::waitForEnter();
        }
        else if (choice == 2)
        {
            viewProperties();
        }
        // else if (choice == 3)
        // {
        //     searchPropertyByRequirements();
        // }
        else if (choice != 3)
        {
            cout << "Invalid choice. Please try again.\n";
            Input::waitForEnter();
        }
    } while (choice != 3);
}
