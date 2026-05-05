#pragma once

struct PropertyDetails
{
    int id;
    char type[50];
    char location[100];
    float price;
    int size;
    char status[20];
    int sellerId;
};

class Property
{
private:
    PropertyDetails properties[100];
    char fileName[100];

    int generateId();
    void readPropertiesFromFile(const char *searchTerm = "");

public:
    Property();

    void menu();

    void addProperty();

    void viewProperties();

    PropertyDetails getPropertyById(int id);

    void deletePropertyById(int id);

    void searchPropertyByRequirements();

    void updatePropertyById(int id);

    // void searchProperty();

    // void deleteProperty();

    // void updateProperty();

    // void sortProperties();
};
