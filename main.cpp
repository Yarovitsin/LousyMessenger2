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
    USER sender;
    USER receiver;
    string text;
    string id;

    MESSAGE() {
        sender = USER();
        receiver = USER();
        text = "";
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
        cout << "Enter username (type 'cancel' to go back): ";
        cin >> login;
        if (login == "cancel") {
            //5 = user cancelled signup
            user.exception = 5;
            return user;
        }
    }
    cout << "Enter password: ";
    cin >> password;

    password_hash = sha256(password);

    fstream file("users.txt");
    while(getline(file, line)) {
        user_data = split(line, ' ');
        if (user_data[0] == login) {
            cout << "Username already taken. Try again? (y/n):" << endl;
            char choice;
            cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                user = signup();
                file.close();
                return user;
            } else {
                user.exception = 1;
                //1 = username already taken
                file.close();
                return user;
            }
        }
    }
    file.open("users.txt", ios::app);
    file << login << " " << password_hash << " " << sha256(login) << 0 << endl;
    file.close();
}

USER login() {
    string username;
    string password;
    string password_hash;
    string line;
    string id;
    vector<string> user_data;
    USER user;

    cout << "Enter username (type 'cancel' to abort): ";
    cin >> username;
    if (username == "cancel") {
        //6 = user cancelled login
        user.exception = 6;
        return user;
    }
    //check if username exists and offer to create new user
    ifstream user_file("users.txt");
    if(user_file.is_open()) {
        while(getline(user_file, line)) {
            user_data = split(line, ' ');
            if(user_data[0] == username) {
                cout << "Enter password: ";
                cin >> password;
                password_hash = sha256(password);
                if(user_data[1] == password_hash) {
                    user.username = user_data[0];
                    user.password_hash = user_data[1];
                    user.id = user_data[2];
                    user.exception = 0;
                    user_file.close();
                    return user;
                }
                else {
                    cout << "Incorrect password. Try again? (y/n): " << endl;
                    char choice;
                    cin >> choice;
                    if(choice == 'y' || choice == 'Y') {
                        user = login();
                        user_file.close();
                        return user;
                    }
                    else {
                        user.exception = 2;
                        //2 = incorrect password
                        user_file.close();
                        return user;
                    }
                }
            }

        }
        cout << "User does not exist" << endl;
        cout << "Would you like to create a new user? (y/n): ";
        char choice;
        cin >> choice;
        if(choice == 'y' || choice == 'Y') {
            user = signup(username);
            if (user.exception == 5) {
                user = login();
            }
            user_file.close();
            return user;
        }
        else {
            user.exception = 3;
            //3 = user does not exist
            user_file.close();
            return user;
            }
    }
    else {
        cout << "Unable to open file";
        user.exception = 4;
    }
    user_file.close();
    return user;
}

USER user_completion(USER user) {
    string line;
    vector<string> user_data;
    vector<string> friends;
    ifstream user_file("users.txt");
    if(user_file.is_open()) {
        while(getline(user_file, line)) {
            user_data = split(line, ' ');
            if(user_data.size() > 4) {
                friends = split(user_data[4], ',');
            }
            if(user.username != "" && user_data[0] == user.username) {
                user.password_hash = user_data[1];
                user.id = user_data[2];
                //store only friends' ids
                for(int i = 0; i < friends.size(); i++) {
                    user.friends[i] = new USER;
                    user.friends[i]->id = friends[i];
                }
                user_file.close();
                return user;
            }
            else if(user.id != "" && user_data[2] == user.id) {
                user.username = user_data[0];
                user.password_hash = user_data[1];
                for(int i = 0; i < friends.size(); i++) {
                    user.friends[i] = new USER;
                    user.friends[i]->id = friends[i];
                    return user;
                }
            }
        }
        cout << "User does not exist" << endl;
        user.exception = 3;
        user_file.close();
        return user;
    }
    else {
        cout << "Unable to open file";
        //4 = unable to open file
        user.exception = 4;
    }
}

USER user_find() {
    string username;
    string id;
    string line;
    USER user;

    cout << "Find user: " << endl;
    cout << "1. Search by username" << endl;
    cout << "2. Search by ID" << endl;
    cout << "3. Choose from friends" << endl;
    cout << "4. Return to main menu" << endl;
    cout << "Enter choice: ";
    int choice;
    cin >> choice;

    switch(choice) {
        case 1:
            cout << "Enter username (type 'cancel' to go back): ";
            cin >> username;
            if (username == "cancel") {
                user = user_find();
            }
            else {
                user.username = username;
                user = user_completion(user);
            }
            break;
        case 2:
            cout << "Enter ID (type 'cancel' to go back): ";
            cin >> id;
            if (id == "cancel") {
                user = user_find();
            }
            else {
                user.id = id;
                user = user_completion(user);
            }
            break;
        case 3:
            //TODO
        case 4:
            //TODO: return to main menu
        default:
            cout << "Invalid choice" << endl;
            break;

    }
    return user;
}

int send_message(USER sender, USER receiver) {
    string text;
    string line;
    string id;
    MESSAGE message;

    cout << "Enter message: ";
    //TODO: add ability to go back
    cin >> text;
    id = sha256(text + sender.id + receiver.id);
    message.id = id;
    message.sender = sender;
    message.receiver = receiver;
    message.text = text;
    ofstream message_file("messages.txt");
    if(message_file.is_open()) {
        message_file << " " << message.sender.id << " " << message.receiver.id << " " << message.text << " " << message.id << endl;
        message_file.close();
        cout << "Message sent" << endl;
    }
    else {
        cout << "Unable to open file";
        return 4;
    }
    return 0;

}

int view_messages(USER user) {
    string line;
    vector<string> message_data;
    MESSAGE message;
    ifstream message_file("messages.txt");
    if(message_file.is_open()) {
        while(getline(message_file, line)) {
            message_data = split(line, ' ');
            if(message_data[1] == user.id) {
                message.sender.id = message_data[0];
                message.receiver.id = message_data[1];
                message.text = message_data[2];
                message.id = message_data[3];
                message.sender = user_completion(message.sender);
                message.receiver = user_completion(message.receiver);
                cout << message.sender.username << ": " << message.text << endl;
            }
        }
        message_file.close();
    }
    else {
        cout << "Unable to open file";
        return 4;
    }
    return 0;
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
            return send_message(user, user_find());
        case 2:
            //view messages
            return view_messages(user);
        case 3:
            //manage friends
            break;
        case 4:
            //account settings
            break;
        case 5:
            //logout
            main_menu(login());
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

int fatal_error_handler(int exception) {
    cout << "An exception has occurred and cannot be handled. Error description is provided below" << endl;
    switch(exception) {
        case 1:
            cout << "Username already exists" << endl;
            return 1;
        case 2:
            cout << "Incorrect password" << endl;
            return 2;
        case 3:
            cout << "User does not exist" << endl;
            return 3;
        case 4:
            cout << "Unable to open file" << endl;
            return 4;
        case 5:
            cout << "User already exists" << endl;
            return 5;
        case 6:
            cout << "Aborted" << endl;
            return 6;
        default:
            cout << "Unknown error" << endl;
            return -1;
    }
}

int main() {
    USER user;
    int error;

    user = login();
    if (user.exception == 0) {
        if (main_menu(user) != 0) {
            error = fatal_error_handler(user.exception);
            return error;
        }
    }
    else {
        error = fatal_error_handler(user.exception);
        return error;
    }
}

