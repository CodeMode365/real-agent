#pragma once

struct PropertyDetails
{
    int id;
    char type[50];
    char location[100];
    float price;
    int size;
    char status[20];
};

class Property
{
private:
    // Private member variables can be declared here
    PropertyDetails properties[100];
    char fileName[100] = "property.txt";

    // PropertyDetails *readPropertiesFromFile(const char *searchTerm = "");
    void readPropertiesFromFile(const char *searchTerm = "");

    void writePropertiesToFile(PropertyDetails property);

public:
    Property();

    void menu();

    void addProperty();

    void viewProperties();

    PropertyDetails getPropertyById(int id);

    void deletePropertyById(int id);

    void updatePropertyById(int id);

    // void searchProperty();

    // void deleteProperty();

    // void updateProperty();

    // void sortProperties();
};
