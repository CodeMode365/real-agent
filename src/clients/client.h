#pragma once
#include "../utils/user/user.h"

struct ClientDetails
{
    int id;
    char name[50];
    char address[70];
    char phone[14];
    char email[70];
    char location[50];
    int minArea;
    int maxArea;
    float minPrice;
    float maxPrice;
    int sellerId;
};

class Client : public User
{
private:
    char fileName[100];
    int generateId();
    void readClientsFromFile();

public:
    Client();

    void menu() override;
    void enterDetails() override;
    void viewDetails() override;

    void addClient();
    void viewClients();
    ClientDetails getClientById(int id);
    void updateClientById(int id);
    void updateClientRequirements(int id);
    void deleteClientById(int id);

    ~Client() {}
};
