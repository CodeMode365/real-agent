#include <iostream>
#include <cstdlib>
using namespace std;

// ================= ABSTRACTION =================
class User
{
protected:
    string name;

public:
    virtual void menu() = 0; // Pure virtual function
};

// ================= INHERITANCE =================
class Property : public User
{
public:
    void menu()
    {
        cout << "\n--- Property Menu ---\n";
        cout << "1. Add Property\n";
        cout << "2. View Property\n";
    }
};

class Seller : public User
{
public:
    void menu()
    {
        cout << "\n--- Seller Menu ---\n";
        cout << "1. Add Seller\n";
        cout << "2. View Seller\n";
    }
};

class Client : public User
{
public:
    void menu()
    {
        cout << "\n--- Client Menu ---\n";
        cout << "1. Add Client\n";
        cout << "2. View Client\n";
    }
};

// ================= ENCAPSULATION =================
class Admin
{
private:
    string username;
    string password;

public:
    Admin()
    {
        username = "admin";
        password = "1234";
    }

    bool login()
    {
        string u, p;
        cout << "\n--- Admin Login ---\n";
        system("cls");
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        if (u == username && p == password)
            return true;
        else
            return false;
    }
};

// ================= MAIN FUNCTION =================
int main()
{
    Admin admin;
    int choice;
    cout << "1111111111" << endl;
    system("cls");
    cout << "===== PROPERTY MANAGEMENT SYSTEM =====\n";

    // Login validation
    while (!admin.login())
    {
        cout << "Invalid login! Try again.\n";
    }

    cout << "Login successful!\n";

    do
    {
        cout << "\n----- MAIN MENU -----\n";
        cout << "1. Property Menu\n";
        cout << "2. Seller Menu\n";
        cout << "3. Client Menu\n";
        cout << "4. Logout\n";
        cout << "Enter choice: ";
        cin >> choice;

        User *user; // Base class pointer (Polymorphism)

        switch (choice)
        {
        case 1:
        {
            Property p;
            user = &p;
            user->menu(); // Polymorphic call
            break;
        }
        case 2:
        {
            Seller s;
            user = &s;
            user->menu();
            break;
        }
        case 3:
        {
            Client c;
            user = &c;
            user->menu();
            break;
        }
        case 4:
            cout << "Logged out successfully.\n";
            cout << "Program ended.\n";
            return 0;

        default:
            cout << "Invalid choice!\n";
        }

    } while (true);

    return 0;
}
