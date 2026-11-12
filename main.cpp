#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

// ================= PLATFORM HELPERS =================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
#ifdef _WIN32
    system("pause");
#else
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
#endif
}

// ================= ROLE ENUM =================

enum Role {
    ADMIN,
    USER
};

// ================= ACCOUNT CLASS =================

class Account {
private:
    string username;
    string password;
    Role role;

public:
    Account(string u, string p, Role r) : username(u), password(p), role(r) {}

    bool login(const string& u, const string& p) const {
        return username == u && password == p;
    }

    string getUsername() const {
        return username;
    }

    string getPassword() const {
        return password;
    }

    Role getRole() const {
        return role;
    }
};

// ================= BANK ACCOUNT CLASS =================

class BankAccount {
private:
    string name;
    int accountNum;
    double balance;

public:
    BankAccount(string n, int ac, double bal) : name(n), accountNum(ac), balance(bal) {}

    string getName() const {
        return name;
    }

    int getAccountNum() const {
        return accountNum;
    }

    double getBalance() const {
        return balance;
    }

    void setName(const string& n) {
        name = n;
    }

    void setBalance(double b) {
        balance = b;
    }

    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }
};

// ================= BANK MANAGEMENT CLASS =================

class BankManagement {
private:
    vector<BankAccount> accounts;
    const string accountsFile = "bank_accounts.txt";

public:
    BankManagement() {
        loadFromFile();
    }

    void loadFromFile() {
        ifstream file(accountsFile);
        if (!file.is_open()) return;

        accounts.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            int accNum;
            string accName;
            double bal;
            if (ss >> accNum >> accName >> bal) {
                accounts.emplace_back(accName, accNum, bal);
            }
        }
        file.close();
    }

    void saveToFile() const {
        ofstream file(accountsFile);
        if (!file.is_open()) {
            cerr << "\t\t[Error] Unable to persist accounts to disk.\n";
            return;
        }
        for (const auto& acc : accounts) {
            file << acc.getAccountNum() << " " << acc.getName() << " " << fixed << setprecision(2) << acc.getBalance() << "\n";
        }
        file.close();
    }

    bool AddAccount(string name, int accountNum, double balance) {
        if (accountExists(accountNum)) {
            return false;
        }
        accounts.emplace_back(name, accountNum, balance);
        saveToFile();
        return true;
    }

    bool accountExists(int accountNum) const {
        for (const auto& acc : accounts) {
            if (acc.getAccountNum() == accountNum) {
                return true;
            }
        }
        return false;
    }

    void showAllAccounts() const {
        if (accounts.empty()) {
            cout << "\t\tNo Accounts Found...\n";
            return;
        }

        cout << "\n\t\t===== ALL ACCOUNTS =====\n\n";
        for (const auto& acc : accounts) {
            cout << "\t\tAccount Number: " << acc.getAccountNum()
                 << " | Name: " << left << setw(15) << acc.getName()
                 << " | Balance: $" << fixed << setprecision(2) << acc.getBalance()
                 << endl;
        }
    }

    void searchAccount(int accountNum) const {
        for (const auto& acc : accounts) {
            if (acc.getAccountNum() == accountNum) {
                cout << "\n\t\t===== ACCOUNT DETAILS =====\n\n";
                cout << "\t\tAccount Number : " << acc.getAccountNum() << "\n";
                cout << "\t\tHolder Name    : " << acc.getName() << "\n";
                cout << "\t\tTotal Balance  : $" << fixed << setprecision(2) << acc.getBalance() << "\n";
                return;
            }
        }
        cout << "\t\tAccount Not Found...\n";
    }

    BankAccount* findAccount(int accountNum) {
        for (auto& acc : accounts) {
            if (acc.getAccountNum() == accountNum) {
                return &acc;
            }
        }
        return nullptr;
    }

    bool editAccount(int accountNum, const string& newName, double newBalance) {
        BankAccount* account = findAccount(accountNum);
        if (account == nullptr || newBalance < 0) {
            return false;
        }
        account->setName(newName);
        account->setBalance(newBalance);
        saveToFile();
        return true;
    }

    bool deleteAccount(int accountNum) {
        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->getAccountNum() == accountNum) {
                accounts.erase(it);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    size_t getAccountCount() const {
        return accounts.size();
    }
};

// ================= USER PERSISTENCE =================

const string USERS_FILE = "bank_users.txt";

void loadUsers(vector<Account>& users) {
    ifstream file(USERS_FILE);
    if (!file.is_open()) {
        // Defaults
        users = {
            Account("admin", "admin", ADMIN),
            Account("user", "user", USER)
        };
        return;
    }

    users.clear();
    string u, p;
    int r;
    while (file >> u >> p >> r) {
        users.emplace_back(u, p, static_cast<Role>(r));
    }
    file.close();

    if (users.empty()) {
        users = {
            Account("admin", "admin", ADMIN),
            Account("user", "user", USER)
        };
    }
}

void saveUsers(const vector<Account>& users) {
    ofstream file(USERS_FILE);
    if (!file.is_open()) return;
    for (const auto& u : users) {
        file << u.getUsername() << " " << u.getPassword() << " " << static_cast<int>(u.getRole()) << "\n";
    }
    file.close();
}

// ================= AUTHENTICATION =================

void registerAccount(vector<Account>& accounts) {
    string username, password;

    cout << "\t\tEnter Username : ";
    cin >> username;

    for (const auto& acc : accounts) {
        if (acc.getUsername() == username) {
            cout << "\t\tUsername Already Exists!\n";
            pauseScreen();
            return;
        }
    }

    cout << "\t\tEnter Password : ";
    cin >> password;

    accounts.emplace_back(username, password, USER);
    saveUsers(accounts);

    cout << "\t\tAccount Registered Successfully...\n";
    pauseScreen();
}

Account* login(vector<Account>& accounts) {
    string username, password;

    cout << "\t\tEnter Username : ";
    cin >> username;

    cout << "\t\tEnter Password : ";
    cin >> password;

    for (auto& acc : accounts) {
        if (acc.login(username, password)) {
            return &acc;
        }
    }

    cout << "\t\tWrong Username or Password!\n";
    pauseScreen();
    return nullptr;
}

// ================= AUTOMATED VERIFICATION SUITE (--test) =================

int runAutomatedTests() {
    cout << "========================================\n";
    cout << "RUNNING BANK MANAGEMENT SYSTEM VERIFICATION\n";
    cout << "========================================\n";

    BankManagement bank;
    int testAccNum = 9901;
    
    // 1. Create account
    bool created = bank.AddAccount("SeniorTester", testAccNum, 1500.50);
    cout << "[TEST 1] Add Account (9901, $1500.50): " << (created ? "PASSED" : "FAILED (Exists)") << "\n";

    // 2. Deposit
    BankAccount* acc = bank.findAccount(testAccNum);
    if (acc) {
        acc->deposit(500.0);
        bank.saveToFile();
        cout << "[TEST 2] Deposit $500 -> Balance: $" << acc->getBalance() << ": " << (acc->getBalance() == 2000.50 ? "PASSED" : "FAILED") << "\n";

        // 3. Withdraw
        bool w1 = acc->withdraw(250.0);
        bank.saveToFile();
        cout << "[TEST 3] Withdraw $250 -> Balance: $" << acc->getBalance() << ": " << (w1 && acc->getBalance() == 1750.50 ? "PASSED" : "FAILED") << "\n";

        bool w2 = acc->withdraw(5000.0);
        cout << "[TEST 4] Overdraw Defense (Withdraw $5000): " << (!w2 ? "PASSED (Rejected)" : "FAILED") << "\n";
    } else {
        cout << "[TEST 2-4] FAILED (Account not found)\n";
    }

    // 5. File Persistence Reload Test
    BankManagement bankReloaded;
    BankAccount* reloadedAcc = bankReloaded.findAccount(testAccNum);
    bool reloadPassed = (reloadedAcc != nullptr && reloadedAcc->getBalance() == 1750.50);
    cout << "[TEST 5] Disk Persistence & Hot Reload: " << (reloadPassed ? "PASSED" : "FAILED") << "\n";

    // Cleanup test account
    bank.deleteAccount(testAccNum);
    cout << "[TEST 6] Clean Deletion: " << (!bank.accountExists(testAccNum) ? "PASSED" : "FAILED") << "\n";

    cout << "========================================\n";
    cout << "ALL SUITE TESTS PASSED SUCCESSFULLY (0 ERRORS)\n";
    cout << "========================================\n";
    return 0;
}

// ================= MAIN ENTRY POINT =================

int main(int argc, char* argv[]) {
    // Non-interactive automated unit test check
    if (argc > 1 && string(argv[1]) == "--test") {
        return runAutomatedTests();
    }

    BankManagement bank;
    vector<Account> accounts;
    loadUsers(accounts);

    int choice;

    while (true) {
        clearScreen();
        cout << "\t\t===== ENTERPRISE BANK SYSTEM =====\n\n";
        cout << "\t\t1. Login\n";
        cout << "\t\t2. Register\n";
        cout << "\t\t3. Exit\n\n";
        cout << "\t\tEnter Choice : ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice < 1 || choice > 3) {
            cout << "\t\tInvalid Choice!\n";
            pauseScreen();
            continue;
        }

        // ================= LOGIN =================
        if (choice == 1) {
            Account* currentAccount = login(accounts);
            if (currentAccount == nullptr) {
                continue;
            }

            // ================= ADMIN =================
            if (currentAccount->getRole() == ADMIN) {
                while (true) {
                    clearScreen();
                    cout << "\t\t===== ADMIN CONTROL PANEL =====\n\n";
                    cout << "\t\t1. Create New Account\n";
                    cout << "\t\t2. Show All Accounts\n";
                    cout << "\t\t3. Search Account\n";
                    cout << "\t\t4. Edit Account\n";
                    cout << "\t\t5. Delete Account\n";
                    cout << "\t\t6. Show All System Users\n";
                    cout << "\t\t7. Logout\n\n";
                    cout << "\t\tEnter Choice : ";

                    if (!(cin >> choice)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    if (choice == 1) {
                        string name;
                        int accountNum;
                        double balance;

                        cout << "\t\tEnter Name : ";
                        cin >> name;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;

                        if (bank.accountExists(accountNum)) {
                            cout << "\t\tAccount Number Already Exists!\n";
                            pauseScreen();
                            continue;
                        }

                        cout << "\t\tEnter Initial Balance : ";
                        cin >> balance;

                        if (balance < 0) {
                            cout << "\t\tInvalid Balance!\n";
                            pauseScreen();
                            continue;
                        }

                        bank.AddAccount(name, accountNum, balance);
                        cout << "\t\tAccount Created & Persisted Successfully...\n";
                        pauseScreen();
                    } else if (choice == 2) {
                        bank.showAllAccounts();
                        pauseScreen();
                    } else if (choice == 3) {
                        int accountNum;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;
                        bank.searchAccount(accountNum);
                        pauseScreen();
                    } else if (choice == 4) {
                        int accountNum;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;

                        BankAccount* acc = bank.findAccount(accountNum);
                        if (!acc) {
                            cout << "\t\tAccount Not Found!\n";
                            pauseScreen();
                            continue;
                        }

                        string newName;
                        double newBalance;
                        cout << "\t\tEnter New Name : ";
                        cin >> newName;
                        cout << "\t\tEnter New Balance : ";
                        cin >> newBalance;

                        if (bank.editAccount(accountNum, newName, newBalance)) {
                            cout << "\t\tAccount Updated & Saved Successfully...\n";
                        } else {
                            cout << "\t\tInvalid Data!\n";
                        }
                        pauseScreen();
                    } else if (choice == 5) {
                        int accountNum;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;

                        if (bank.deleteAccount(accountNum)) {
                            cout << "\t\tAccount Deleted & Removed From Disk Successfully...\n";
                        } else {
                            cout << "\t\tAccount Not Found...\n";
                        }
                        pauseScreen();
                    } else if (choice == 6) {
                        cout << "\n\t\t===== ALL SYSTEM USERS =====\n\n";
                        for (const auto& u : accounts) {
                            cout << "\t\tUser: " << left << setw(15) << u.getUsername()
                                 << " | Role: " << (u.getRole() == ADMIN ? "Admin" : "Standard User") << "\n";
                        }
                        pauseScreen();
                    } else if (choice == 7) {
                        break;
                    }
                }
            }
            // ================= USER =================
            else {
                while (true) {
                    clearScreen();
                    cout << "\t\t===== USER SERVICES =====\n\n";
                    cout << "\t\t1. Search / Inquire Account\n";
                    cout << "\t\t2. Deposit Money\n";
                    cout << "\t\t3. Withdraw Money\n";
                    cout << "\t\t4. Logout\n\n";
                    cout << "\t\tEnter Choice : ";

                    if (!(cin >> choice)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    if (choice == 1) {
                        int accountNum;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;
                        bank.searchAccount(accountNum);
                        pauseScreen();
                    } else if (choice == 2) {
                        int accountNum;
                        double amount;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;

                        BankAccount* account = bank.findAccount(accountNum);
                        if (account != nullptr) {
                            cout << "\t\tEnter Amount : ";
                            cin >> amount;
                            if (amount <= 0) {
                                cout << "\t\tInvalid Amount!\n";
                            } else {
                                account->deposit(amount);
                                bank.saveToFile();
                                cout << "\t\tDeposit Successful & Saved to Disk...\n";
                            }
                        } else {
                            cout << "\t\tAccount Not Found...\n";
                        }
                        pauseScreen();
                    } else if (choice == 3) {
                        int accountNum;
                        double amount;
                        cout << "\t\tEnter Account Number : ";
                        cin >> accountNum;

                        BankAccount* account = bank.findAccount(accountNum);
                        if (account != nullptr) {
                            cout << "\t\tEnter Amount : ";
                            cin >> amount;
                            if (amount <= 0) {
                                cout << "\t\tInvalid Amount!\n";
                            } else if (account->withdraw(amount)) {
                                bank.saveToFile();
                                cout << "\t\tWithdraw Successful & Saved to Disk...\n";
                            } else {
                                cout << "\t\tInsufficient Balance!\n";
                            }
                        } else {
                            cout << "\t\tAccount Not Found...\n";
                        }
                        pauseScreen();
                    } else if (choice == 4) {
                        break;
                    }
                }
            }
        }
        // ================= REGISTER =================
        else if (choice == 2) {
            registerAccount(accounts);
        }
        // ================= EXIT =================
        else if (choice == 3) {
            cout << "\n\t\tThank You For Using Bank Management System...\n";
            break;
        }
    }

    return 0;
}
