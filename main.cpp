#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>  // for setw (table formatting)
#include <cctype>   // for isdigit (check phone numbers)
using namespace std;

//  Colors for terminal text 
//  only work in terminals that support colors.//
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

//  Project Class 
class Project {
public:
    string name, status, deadline;

    Project() {}
    Project(string n, string s, string d) : name(n), status(s), deadline(d) {}
};

//  Client Class 
class Client {
public:
    int id;
    string name, email, phone;
    vector<Project> projects; // a client can have many projects

    Client() {}
    Client(int i, string n, string e, string p) : id(i), name(n), email(e), phone(p) {}

    // Show all projects of a client in a table
    void showProjects() {
        if (projects.empty()) {
            cout << YELLOW << "No projects for this client.\n" << RESET;
            return;
        }

        // Table header
        cout << left << setw(5) << "No"
             << " | " << setw(20) << "Project"
             << " | " << setw(15) << "Status"
             << " | " << setw(15) << "Deadline" << endl;
        cout << string(60, '-') << endl;

        // Print each project in nice format
        for (size_t i = 0; i < projects.size(); i++) {
            cout << CYAN << setw(5) << i + 1 << RESET
                 << " | " << setw(20) << projects[i].name
                 << " | " << setw(15) << projects[i].status
                 << " | " << setw(15) << projects[i].deadline << endl;
        }
    }
};

//  Validation Functions 

// Check if an email is valid
bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');          // find position of '@'
    size_t dotPos = email.find('.', atPos);  // find '.' after '@'

    if (atPos == string::npos || dotPos == string::npos) return false; // must contain @ and .
    if (atPos == 0 || dotPos == email.size()-1) return false;          // can't start with @ or end with .
    if (dotPos < atPos) return false;                                  // '.' must come after '@'

    return true;
}

// Checke if a phone is valid (must start with 0 and contain only digits)
bool isValidPhone(const string& phone) {
    if (phone.empty() || phone[0] != '0') return false; // must start with 0
    for (char c : phone) {
        if (!isdigit(c)) return false; // must be all numbers
    }
    return true;
}

//  Function Prototypes 
void showMenu();
int selectClient(vector<Client>& clients);
int selectProject(Client& client);
void saveClients(const vector<Client>& clients, const string& filename);
void saveProjects(const vector<Client>& clients, const string& filename);
void loadClients(vector<Client>& clients, const string& filename);
void loadProjects(vector<Client>& clients, const string& filename);

//  Main Program 
int main() {
    vector<Client> clients;  // list of all clients

    // Load existing clients/projects from file (if files exist)
    loadClients(clients, "clients.csv");
    loadProjects(clients, "projects.csv");

    int choice;
    do {
        showMenu();          // show main menu
        cin >> choice; cin.ignore();  // get user input

        if (choice == 1) { // Add Client
            int id = clients.empty() ? 1 : clients.back().id + 1; // auto-generate ID
            string name, email, phone;
            cout << "Enter client " << CYAN << "name: " << RESET; getline(cin, name);
            cout << "Enter client " << RED << "email: " << RESET; getline(cin, email);
            cout << "Enter client " << CYAN << "phone: " << RESET; getline(cin, phone);

            // Validate email & phone
            if (!isValidEmail(email) || !isValidPhone(phone)) {
                cout << RED << "Failed: Invalid email or phone.\n" << RESET;
                continue;
            }

            // Save client
            clients.push_back(Client(id, name, email, phone));
            saveClients(clients, "clients.csv");
            cout << GREEN << "Client added.\n" << RESET;
        }
        else if (choice == 2) { // Edit Client
            int idx = selectClient(clients);
            if (idx == -1) continue;
            string name, email, phone;
            cout << "New name: "; getline(cin, name);
            cout << "New email: "; getline(cin, email);
            cout << "New phone: "; getline(cin, phone);

            // Validate new info
            if (!isValidEmail(email) || !isValidPhone(phone)) {
                cout << RED << "Failed: Invalid email or phone.\n" << RESET;
                continue;
            }

            // Update
            clients[idx].name = name;
            clients[idx].email = email;
            clients[idx].phone = phone;
            saveClients(clients, "clients.csv");
            cout << GREEN << "Client updated.\n" << RESET;
        }
        else if (choice == 3) { // Delete Client
            int idx = selectClient(clients);
            if (idx == -1) continue;
            clients.erase(clients.begin() + idx);
            saveClients(clients, "clients.csv");
            saveProjects(clients, "projects.csv");
            cout << YELLOW << "Client deleted.\n" << RESET;
        }
        else if (choice == 4) { // Add Project
            int idx = selectClient(clients);
            if (idx == -1) continue;
            string pname, status, deadline;
            cout << "Project " << CYAN << "name: " << RESET; getline(cin, pname);
            cout << "Project " << CYAN << "status: " << RESET; getline(cin, status);
            cout << "Project " << CYAN << "deadline: " << RESET; getline(cin, deadline);

            clients[idx].projects.push_back(Project(pname, status, deadline));
            saveProjects(clients, "projects.csv");
            cout << GREEN << "Project added.\n" << RESET;
        }
        else if (choice == 5) { // Edit Project
            int cidx = selectClient(clients);
            if (cidx == -1) continue;
            int pidx = selectProject(clients[cidx]);
            if (pidx == -1) continue;
            string pname, status, deadline;
            cout << "Project name: "; getline(cin, pname);
            cout << "Status: "; getline(cin, status);
            cout << "Deadline: "; getline(cin, deadline);

            clients[cidx].projects[pidx] = Project(pname, status, deadline);
            saveProjects(clients, "projects.csv");
            cout << GREEN << "Project updated.\n" << RESET;
        }
        else if (choice == 6) { // Delete Project
            int cidx = selectClient(clients);
            if (cidx == -1) continue;
            int pidx = selectProject(clients[cidx]);
            if (pidx == -1) continue;
            clients[cidx].projects.erase(clients[cidx].projects.begin() + pidx);
            saveProjects(clients, "projects.csv");
            cout << YELLOW << "Project deleted.\n" << RESET;
        }
        else if (choice == 7) { // Show Projects
            int idx = selectClient(clients);
            if (idx == -1) continue;
            clients[idx].showProjects();
        }
        else if (choice == 8) {
            cout << BLUE << " Exiting.\n" << RESET;
        }
        else {
            cout << RED << " Invalid option.\n" << RESET;
        }
    } while (choice != 8);

    return 0;
}

//  Menu Function 
void showMenu() {
    cout << BOLD << "\n--- " << BLUE << "Freelancer Project Tracker" << RESET << BOLD << " ---\n" << RESET;
    cout << CYAN << "1. Add Client\n2. Edit Client\n3. Delete Client\n";
    cout << "4. Add Project\n5. Edit Project\n6. Delete Project\n";
    cout << "7. Show Client Projects\n8. Exit\n" << RESET;
    cout << YELLOW << "Choose: " << RESET;
}

//  Select Client 
int selectClient(vector<Client>& clients) {
    if (clients.empty()) { cout << RED << "No clients.\n" << RESET; return -1; }
    cout << BOLD << "Select client:\n" << RESET;
    for (size_t i=0; i<clients.size(); i++) 
        cout << CYAN << i+1 << ". " << clients[i].name << RESET << endl;
    int c; cin >> c; cin.ignore();
    if (c<1 || c>(int)clients.size()) return -1;
    return c-1;
}

//  Select Project 
int selectProject(Client& client) {
    if (client.projects.empty()) { cout << RED << "No projects.\n" << RESET; return -1; }
    cout << BOLD << "Select project:\n" << RESET; client.showProjects();
    int p; cin >> p; cin.ignore();
    if (p<1 || p>(int)client.projects.size()) return -1;
    return p-1;
}

//  Save Clients to File 
// Format: Table with aligned columns
void saveClients(const vector<Client>& clients, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;

    // Header
    file << left << setw(5) << "ID" 
         << " | " << setw(15) << "Name" 
         << " | " << setw(25) << "Email" 
         << " | " << setw(15) << "Phone" << "\n";
    file << string(70, '-') << "\n";

    // Rows
    for (auto& c : clients) {
        file << left << setw(5) << c.id
             << " | " << setw(15) << c.name
             << " | " << setw(25) << c.email
             << " | " << setw(15) << c.phone << "\n";
    }
    file.close();
}

//  Save Projects to File 
void saveProjects(const vector<Client>& clients, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return;

    // Header
    file << left << setw(10) << "ClientID"
         << " | " << setw(20) << "Project"
         << " | " << setw(15) << "Status"
         << " | " << setw(15) << "Deadline" << "\n";
    file << string(70, '-') << "\n";

    // Rows
    for (auto& c : clients) {
        for (auto& p : c.projects) {
            file << left << setw(10) << c.id
                 << " | " << setw(20) << p.name
                 << " | " << setw(15) << p.status
                 << " | " << setw(15) << p.deadline << "\n";
        }
    }
    file.close();
}

//  Load Clients 
// NOTE: This still loads from old semicolon (;) format for safety
void loadClients(vector<Client>& clients, const string& filename) {
    clients.clear();
    ifstream file(filename); if(!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.find(';') == string::npos) continue;
        stringstream ss(line); string idStr,name,email,phone;
        getline(ss,idStr,';'); getline(ss,name,';'); getline(ss,email,';'); getline(ss,phone,';');
        clients.push_back(Client(stoi(idStr), name, email, phone));
    }
    file.close();
}

//  Load Projects 
void loadProjects(vector<Client>& clients, const string& filename) {
    ifstream file(filename); if(!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        if (line.find(';') == string::npos) continue;
        stringstream ss(line); string idStr,name,status,deadline;
        getline(ss,idStr,';'); getline(ss,name,';'); getline(ss,status,';'); getline(ss,deadline,';');
        int cid = stoi(idStr);
        for(auto& c:clients) {
            if(c.id == cid) {
                c.projects.push_back(Project(name,status,deadline));
                break;
            }
        }
    }
    file.close();
}

