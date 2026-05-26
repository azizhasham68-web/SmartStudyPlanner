#include <iostream>
#include "SmartStudyPlanner.h"  //including the header file for the project
#include"SmartStudyPlanner.cpp"             //including the implementation file for the project
using namespace std;
                                                            
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define NEON_PURPLE "\033[38;5;135m"
#define NEON_PINK "\033[38;5;198m"                                       //including all colors
#define NEON_CYAN "\033[38;5;51m"
#define NEON_GREEN "\033[38;5;82m"
#define NEON_YELLOW "\033[38;5;226m"
#define NEON_ORANGE "\033[38;5;208m"
#define DIM_PURPLE "\033[38;5;61m"
#define DIM_WHITE "\033[38;5;252m"

int main() {
    printBanner();                                      //to display the welcome banner with the current date
    FileManager fm;
    int choice = 0;

    while (choice != 3) {                                                               //main menu loop to allow user to login, register or exit
        cout << "\n" << NEON_PURPLE << "__________[ " << NEON_YELLOW << "MAIN MENU" << NEON_PURPLE << " ]________________________\n" << RESET;
        cout << NEON_PURPLE  << NEON_PINK << "1. Login      " << NEON_PURPLE << "                              \n" << RESET;
        cout << NEON_PURPLE  << NEON_PINK << "2. Register   " << NEON_PURPLE << "                              \n" << RESET;
        cout << NEON_PURPLE  << NEON_PINK << "3. Exit       " << NEON_PURPLE << "                              \n" << RESET;
        cout << NEON_PURPLE << "_______________________________________________\n" << RESET;
        cout << NEON_GREEN << "> " << RESET;
        cin >> choice;

        if (choice == 1) {                                  //login flow to authenticate user and load their data
            int    id = getInt("  Enter your User ID: ");
            string name;
            int    streak = 0;
            float  mins   = 0;
            if (fm.loadUser(id, name, streak, mins)) {
                cout << NEON_GREEN << "\n  Welcome back, " << NEON_CYAN << name << RESET << "\n";
                cpk_line();
                User user(id, name);
                user.loadData();
                userMenu(user);
            } else {
                cout << NEON_PINK << "  User ID not found. Please register first.\n" << RESET;
            }

        } else if (choice == 2) {                                           //to register a new user 
            string name;
            int    id = getInt("  Choose a User ID (number): ");
            cout << DIM_WHITE << "  Enter your name: " << NEON_CYAN;
            cin.ignore(); getline(cin, name); cout << RESET;
            User user(id, name);
            fm.saveUser(id, name, 0, 0);                                                            //to save the new user's profile details to a file
            cout << NEON_GREEN << "\n  Registered! Welcome, " << NEON_CYAN << name << RESET << "\n";
            cpk_line();
            userMenu(user);

        } else if (choice == 3) {                                   //to exit the program with a goodbye message
            cout << NEON_PURPLE << "\n  Goodbye! Keep studying.\n" << RESET;
            cpk_line('=');

        } else {                                                    //to handle invalid menu choice
            cout << NEON_PINK << "  Invalid choice.\n" << RESET;
        }
    }

    return 0;
}
