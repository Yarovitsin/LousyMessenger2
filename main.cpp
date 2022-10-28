//simple messaging app that stores data in file and with network capabilities

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "sha256.h"

using namespace std;

SHA256 sha256;

//user class
class USER {
public:
    string username;
    string password_hash;
    string id;
    int exception;
    USER *friends[100];

    USER() {
        username = "";
        password_hash = "";
        id = "";
        exception = 0;
    }
};

//message class
class MESSAGE {
public:
    string sender;
    string receiver;
    string message;
    string id;

    MESSAGE() {
        sender = "";
        receiver = "";
        message = "";
        id = "";
    }
};

//split function
vector<string> split(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str);
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

USER signup(string login = "") {
    string username;
    string password;
    string password_hash;
    string line;
    string id;
    vector<string> user_data;
    USER user;

    if (login == "") {
        cout << "Enter username: ";
        cin >> login;
    }
    cout << "Enter password: ";
    cin >> password;

    password_hash = sha256(password);

    fstream file("users.txt");
    while(getline(file, line)) {
        user_data = split(line, ',');
        if (user_data[0] == login) {
            cout << "Username already taken. Try again? (y/n):" << endl;
            char choice;
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                user = signup();
                return user;
            } else {
                user.exception = 1;
                //1 = username already taken
                return user;
            }
            return user;
        }
    }
    file.close();

    file.open("users.txt", ios::app);
    file << login << " " << password_hash << " " << sha256(login) << endl;
    file.close();
}

//login function
USER login() {
    string username;
    string password;
    string password_hash;
    string line;
    string id;
    vector<string> user_data;
    USER user;

    cout << "Enter username: ";
    cin >> username;
    //check if username exists and offer to create new user
    ifstream user_file("users.txt");
    if(user_file.is_open()) {
        while(getline(user_file, line)) {
            user_data = split(line, ',');
            if(user_data[0] == username) {
                cout << "Enter password: ";
                cin >> password;
                password_hash = sha256(password);
                if(user_data[1] == password_hash) {
                    user.username = user_data[0];
                    user.password_hash = user_data[1];
                    user.id = user_data[2];
                    return user;
                }
                else {
                    cout << "Incorrect password. Try again? (y/n): " << endl;
                    char choice;
                    cin >> choice;
                    if(choice == 'y' || choice == 'Y') {
                        user = login();
                        return user;
                    }
                    else {
                        user.exception = 2;
                        //2 = incorrect password
                        return user;
                    }
                }
            }
            else {
                cout << "User does not exist" << endl;
                cout << "Would you like to create a new user? (y/n): ";
                char choice;
                cin >> choice;
                if(choice == 'y' || choice == 'Y') {
                    user = signup(username);
                    return user;
                }
                else {
                    user.exception = 3;
                    //3 = user does not exist
                    return user;
                }
            }
        }
        user_file.close();
    }
    else {
        cout << "Unable to open file";
    }

    return user;
}

int user_find() {
    string username;
    string line;
    vector<string> user_data;
    int user_index = 0;

    cout << "Find user: "; << endl;
    cout << "1. Search by username" << endl;
    cout << "2. Search by ID" << endl;
    cout << "3. Choose from friends" << endl;
}

int send_message (USER sender, USER receiver) {
    string message;
    cout << "Enter message: ";
    cin >> message;
    MESSAGE msg;
    msg.sender = sender.id;
    msg.receiver = receiver.id;
    msg.message = message;
    msg.id = sha256(msg.sender + msg.receiver + msg.message);

    fstream file("messages.txt", ios::app);
    file << msg.sender << "," << msg.receiver << "," << msg.message << "," << msg.id << endl;
    file.close();
}

int main_menu(USER user) {
    cout << "Welcome " << user.username << endl;
    cout << "1. Send message" << endl;
    cout << "2. View messages" << endl;
    cout << "3. Manage friends" << endl;
    cout << "4. Account settings" << endl;
    cout << "5. Logout" << endl;
    cout << "6. Exit" << endl;
    int choice;
    cin >> choice;
    switch (choice) {
        case 1:
            //send message
            send_message(user, user_find());
            break;
        case 2:
            //view messages
            break;
        case 3:
            //manage friends
            break;
        case 4:
            //account settings
            break;
        case 5:
            //logout
            break;
        case 6:
            //exit
            break;
        default:
            cout << "Invalid choice" << endl;
            main_menu(user);
            break;
    }
}

int main() {
    USER user;
    user = login();
    if (user.exception == 0) {
        main_menu(user);
    }
    else {
        cout << "Error: " << user.exception << endl;
    }
}

