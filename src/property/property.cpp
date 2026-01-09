#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "property.h"
#include "../utils/screen/screen.h"
#include "../menu/menu.h"
#include "../utils/files/file.h"

using namespace std;

Property::Property()
    : properties() // Initialize properties in the initializer list
{
    strcpy(this->fileName, this->fileName);
}

void Property::readPropertiesFromFile(const char *searchTerm)
{
    PropertyDetails property;
    FILE *file = fopen(this->fileName, "r");

    if (file == nullptr)
    {
        cerr << "Error opening file for reading.\n";
        return;
    }

    cout << "\n┌─────┬──────────────┬──────────────┬───────────────┬──────────┬─────────────┐\n";
    cout << "│ ID  │     Type     │   Location   │     Price     │   Size   │    Status   │\n";
    cout << "├─────┼──────────────┼──────────────┼───────────────┼──────────┼─────────────┤\n";
    while (fscanf(file, "%d %s %s %f %d %s",
                  &property.id,
                  property.type,
                  property.location,
                  &property.price,
                  &property.size,
                  property.status) == 6)
    {
        if ((searchTerm == nullptr || string(searchTerm).empty() || string(property.location) == searchTerm) && string(property.status) == "Available")
        {
            printf("│ %-3d │ %-12s │ %-12s │ Rs. %-9.1f │ %-8d │ %-11s │\n",
                   property.id,
                   property.type,
                   property.location,
                   property.price,
                   property.size,
                   property.status);
        }
    }
    cout << "└─────┴──────────────┴──────────────┴───────────────┴──────────┴─────────────┘\n";
    // if (searchTerm == nullptr || string(searchTerm).empty() || string(property.location) == searchTerm)
    // {
    //     cout << property.id << "\t"
    //          << property.type << "\t"
    //          << property.location << "\t"
    //          << property.price << "\t"
    //          << property.size << "\t"
    //          << property.status << "\n";
    // }
    fclose(file);
}

PropertyDetails Property::getPropertyById(int id)
{
    PropertyDetails property;
    FILE *file = fopen(this->fileName, FileUtils::getFileModeString(FileUtils::FileMode::READ));

    if (file == nullptr)
    {
        cerr << "Error opening file for reading.\n";
        return property; // Return empty property on error
    }

    while (fscanf(file, "%d %s %s %f %d %s",
                  &property.id,
                  property.type,
                  property.location,
                  &property.price,
                  &property.size,
                  property.status) == 6)
    {
        if (property.id == id)
        {
            fclose(file);
            return property; // Return the found property
        }
    }
    fclose(file);
    return property; // Return empty property if not found
}

void Property::addProperty()
{
    Screen::clearScreen();
    Screen::printHeader("Add New Property");

    PropertyDetails newProperty;

    cout << "Enter Property ID: ";
    cin >> newProperty.id;
    cout << "Enter Property Type: ";
    cin >> newProperty.type;
    cout << "Enter Location: ";
    cin >> newProperty.location;
    cout << "Enter Price: ";
    cin >> newProperty.price;
    cout << "Enter Size (in sqft): ";
    cin >> newProperty.size;
    strcpy(newProperty.status, "Available");

    // FILE *file = fopen(this->fileName, "a");
    FILE *original = fopen(this->fileName, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!temp)
    {
        cerr << "Error opening temp file.\n";
        if (original)
            fclose(original);
        return;
    }

    // Write new content first in temporary file
    fprintf(temp, "%d %s %s %.2f %d %s\n",
            newProperty.id,
            newProperty.type,
            newProperty.location,
            newProperty.price,
            newProperty.size,
            newProperty.status);

    // Copy old data after
    if (original)
    {
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), original))
        {
            fputs(buffer, temp);
        }
        fclose(original);
    }

    fclose(temp);

    // Replace original file with temp file
    remove(this->fileName);
    rename("temp.txt", this->fileName);

    cout << "Property added successfully!\n";
}

void Property::viewProperties()
{
    Screen::clearScreen();
    Screen::printHeader("View Properties");

    this->readPropertiesFromFile(nullptr);
}

void Property::menu()
{
    int choice;
    Screen::clearScreen();
    Screen::printHeader("Property Menu");
    cout << "1. Add Property\n";
    cout << "2. View Properties\n";
    cout << "3. Back to Main Menu\n";
    cin >> choice;

    switch (choice)
    {
    case 1:
        this->addProperty();
        break;
    case 2:
        this->viewProperties();
        break;
    case 3:
        Menu::showMenu();
        break;
    default:
        cout << "Invalid choice. Please try again.\n";
        break;
    }
}
