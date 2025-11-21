#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 50
#define FILENAME "mishterious_bank_data.dat"
#define TRANSACTION_HISTORY_FILE "transaction_history.dat"

// Structure definitions
typedef struct {
    char fullName[MAX_NAME_LENGTH];
    long long accountNumber;
    char password[MAX_PASSWORD_LENGTH];
    double balance;
    int isActive;
} Account;

typedef struct {
    long long accountNumber;
    char transactionType[20];
    double amount;
    double balanceAfter;
    time_t timestamp;
    long long targetAccount;
} Transaction;

// Dynamic array for accounts
Account *accounts = NULL;
int accountCount = 0;
int accountCapacity = 0;
long long currentUserAccount = -1;

// Function prototypes
void initializeSystem();
void saveDataToFile();
void loadDataFromFile();
void saveTransaction(long long accNum, const char* type, double amount, double newBalance, long long targetAcc);
void displayTransactionHistory(long long accNum);
void clearInputBuffer();

void displayWelcomeScreen();
void mainMenu();
void userRegistration();
int userLogin();
void depositFunds();
void withdrawFunds();
void transferFunds();
void changePassword();
void displayAccountDetails();
void adminMenu();

void clearScreen();
void pauseScreen();
int validatePassword(const char* password);
int validateName(const char* name);
void generateAccountNumber(char* accNum);
void encryptPassword(char* password);
int verifyPassword(const char* input, const char* stored);
void displayBalance(double balance);
void addAccount(Account newAccount);

// Utility functions
void clearScreen() {
    system("clear");
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

int validatePassword(const char* password) {
    int len = strlen(password);
    if (len < 6 || len > MAX_PASSWORD_LENGTH) {
        return 0;
    }
    
    int hasUpper = 0, hasLower = 0, hasDigit = 0;
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) hasUpper = 1;
        if (islower(password[i])) hasLower = 1;
        if (isdigit(password[i])) hasDigit = 1;
    }
    
    return (hasUpper && hasLower && hasDigit);
}

int validateName(const char* name) {
    int len = strlen(name);
    if (len < 2 || len > MAX_NAME_LENGTH) {
        return 0;
    }
    
    for (int i = 0; i < len; i++) {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '.') {
            return 0;
        }
    }
    return 1;
}

void generateAccountNumber(char* accNum) {
    srand(time(NULL));
    sprintf(accNum, "33%06d", rand() % 1000000);
}

void encryptPassword(char* password) {
    char key = 'M';
    for (int i = 0; password[i] != '\0'; i++) {
        password[i] = password[i] ^ key;
    }
}

int verifyPassword(const char* input, const char* stored) {
    char encryptedInput[MAX_PASSWORD_LENGTH];
    strcpy(encryptedInput, input);
    encryptPassword(encryptedInput);
    return strcmp(encryptedInput, stored) == 0;
}

void displayBalance(double balance) {
    printf("Current Balance: K %.2f\n", balance);
}

void addAccount(Account newAccount) {
    if (accountCount >= accountCapacity) {
        int newCapacity = (accountCapacity == 0) ? 10 : accountCapacity * 2;
        Account *newAccounts = realloc(accounts, newCapacity * sizeof(Account));
        
        if (newAccounts == NULL) {
            printf("Error: Memory allocation failed. Cannot create account.\n");
            return;
        }
        
        accounts = newAccounts;
        accountCapacity = newCapacity;
    }
    
    accounts[accountCount++] = newAccount;
}

// File handling functions
void saveDataToFile() {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("Error: Could not save data to file.\n");
        return;
    }
    
    fwrite(&accountCount, sizeof(int), 1, file);
    for (int i = 0; i < accountCount; i++) {
        fwrite(&accounts[i], sizeof(Account), 1, file);
    }
    fclose(file);
}

void loadDataFromFile() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No existing data found. Starting fresh.\n");
        return;
    }
    
    int savedCount;
    fread(&savedCount, sizeof(int), 1, file);
    
    accounts = malloc(savedCount * sizeof(Account));
    if (accounts == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return;
    }
    
    accountCapacity = savedCount;
    accountCount = 0;
    
    for (int i = 0; i < savedCount; i++) {
        Account acc;
        if (fread(&acc, sizeof(Account), 1, file) == 1) {
            addAccount(acc);
        }
    }
    
    fclose(file);
    printf("Loaded %d accounts from file.\n", accountCount);
}

void saveTransaction(long long accNum, const char* type, double amount, double newBalance, long long targetAcc) {
    FILE *file = fopen(TRANSACTION_HISTORY_FILE, "ab");
    if (file == NULL) return;
    
    Transaction trans;
    trans.accountNumber = accNum;
    strcpy(trans.transactionType, type);
    trans.amount = amount;
    trans.balanceAfter = newBalance;
    trans.timestamp = time(NULL);
    trans.targetAccount = targetAcc;
    
    fwrite(&trans, sizeof(Transaction), 1, file);
    fclose(file);
}

void displayTransactionHistory(long long accNum) {
    FILE *file = fopen(TRANSACTION_HISTORY_FILE, "rb");
    if (file == NULL) {
        printf("No transaction history found.\n");
        return;
    }
    
    printf("\n=== TRANSACTION HISTORY ===\n");
    printf("Account: %lld\n\n", accNum);
    
    Transaction trans;
    int found = 0;
    
    while (fread(&trans, sizeof(Transaction), 1, file)) {
        if (trans.accountNumber == accNum) {
            found = 1;
            struct tm *timeinfo = localtime(&trans.timestamp);
            printf("Date: %s", asctime(timeinfo));
            printf("Type: %s\n", trans.transactionType);
            printf("Amount: K %.2f\n", trans.amount);
            
            if (strcmp(trans.transactionType, "TRANSFER") == 0) {
                if (trans.amount < 0) {
                    printf("Transferred to: %lld\n", trans.targetAccount);
                } else {
                    printf("Received from: %lld\n", trans.targetAccount);
                }
            }
            
            printf("Balance After: K %.2f\n", trans.balanceAfter);
            printf("---------------------------\n");
        }
    }
    
    if (!found) {
        printf("No transactions found for this account.\n");
    }
    
    fclose(file);
}

// Core banking functions
void userRegistration() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - ACCOUNT REGISTRATION ===\n\n");
    
    Account newAccount;
    
    // Get full name
    do {
        printf("Enter Full Name: ");
        fgets(newAccount.fullName, MAX_NAME_LENGTH, stdin);
        newAccount.fullName[strcspn(newAccount.fullName, "\n")] = 0;
        
        if (!validateName(newAccount.fullName)) {
            printf("Error: Invalid name. Use only letters, spaces, and dots.\n");
        }
    } while (!validateName(newAccount.fullName));
    
    // Generate account number
    char accNumStr[20];
    generateAccountNumber(accNumStr);
    newAccount.accountNumber = atoll(accNumStr);
    
    // Get password
    char password[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];
    
    do {
        printf("Create Password (min 6 chars, mix of upper/lower/digits): ");
        fgets(password, MAX_PASSWORD_LENGTH, stdin);
        password[strcspn(password, "\n")] = 0;
        
        if (!validatePassword(password)) {
            printf("Error: Password must be 6-50 characters with uppercase, lowercase, and digits.\n");
            continue;
        }
        
        printf("Confirm Password: ");
        fgets(confirmPassword, MAX_PASSWORD_LENGTH, stdin);
        confirmPassword[strcspn(confirmPassword, "\n")] = 0;
        
        if (strcmp(password, confirmPassword) != 0) {
            printf("Error: Passwords do not match.\n");
        }
    } while (strcmp(password, confirmPassword) != 0 || !validatePassword(password));
    
    // Encrypt and store password
    strcpy(newAccount.password, password);
    encryptPassword(newAccount.password);
    
    // Get initial deposit
    do {
        printf("Enter Initial Deposit (K): ");
        scanf("%lf", &newAccount.balance);
        clearInputBuffer();
        
        if (newAccount.balance < 100) {
            printf("Error: Minimum initial deposit is K 100.00\n");
        }
    } while (newAccount.balance < 100);
    
    newAccount.isActive = 1;
    addAccount(newAccount);
    saveDataToFile();
    
    printf("\n✅ ACCOUNT CREATED SUCCESSFULLY!\n");
    printf("Account Number: %lld\n", newAccount.accountNumber);
    printf("Account Holder: %s\n", newAccount.fullName);
    printf("Initial Balance: K %.2f\n", newAccount.balance);
    printf("\nPlease save your account number for future login.\n");
    
    saveTransaction(newAccount.accountNumber, "OPENING", newAccount.balance, newAccount.balance, 0);
    pauseScreen();
}

int userLogin() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - LOGIN ===\n\n");
    
    long long accountNumber;
    char password[MAX_PASSWORD_LENGTH];
    
    printf("Enter Account Number: ");
    scanf("%lld", &accountNumber);
    clearInputBuffer();
    
    printf("Enter Password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == accountNumber && accounts[i].isActive) {
            if (verifyPassword(password, accounts[i].password)) {
                currentUserAccount = accountNumber;
                printf("\n✅ LOGIN SUCCESSFUL!\n");
                printf("Welcome back, %s!\n", accounts[i].fullName);
                pauseScreen();
                return 1;
            }
        }
    }
    
    printf("\n❌ LOGIN FAILED! Invalid account number or password.\n");
    pauseScreen();
    return 0;
}

void depositFunds() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - DEPOSIT FUNDS ===\n\n");
    
    int accountIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == currentUserAccount) {
            accountIndex = i;
            break;
        }
    }
    
    if (accountIndex == -1) {
        printf("Error: Account not found.\n");
        pauseScreen();
        return;
    }
    
    double amount;
    printf("Current Balance: K %.2f\n", accounts[accountIndex].balance);
    printf("Enter amount to deposit (K): ");
    scanf("%lf", &amount);
    clearInputBuffer();
    
    if (amount <= 0) {
        printf("Error: Deposit amount must be positive.\n");
        pauseScreen();
        return;
    }
    
    accounts[accountIndex].balance += amount;
    saveDataToFile();
    saveTransaction(currentUserAccount, "DEPOSIT", amount, accounts[accountIndex].balance, 0);
    
    printf("\n✅ DEPOSIT SUCCESSFUL!\n");
    printf("Amount Deposited: K %.2f\n", amount);
    printf("New Balance: K %.2f\n", accounts[accountIndex].balance);
    pauseScreen();
}

void withdrawFunds() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - WITHDRAW FUNDS ===\n\n");
    
    int accountIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == currentUserAccount) {
            accountIndex = i;
            break;
        }
    }
    
    if (accountIndex == -1) {
        printf("Error: Account not found.\n");
        pauseScreen();
        return;
    }
    
    double amount;
    printf("Current Balance: K %.2f\n", accounts[accountIndex].balance);
    printf("Enter amount to withdraw (K): ");
    scanf("%lf", &amount);
    clearInputBuffer();
    
    if (amount <= 0) {
        printf("Error: Withdrawal amount must be positive.\n");
        pauseScreen();
        return;
    }
    
    if (amount > accounts[accountIndex].balance) {
        printf("Error: Insufficient funds. Available balance: K %.2f\n", accounts[accountIndex].balance);
        pauseScreen();
        return;
    }
    
    accounts[accountIndex].balance -= amount;
    saveDataToFile();
    saveTransaction(currentUserAccount, "WITHDRAWAL", -amount, accounts[accountIndex].balance, 0);
    
    printf("\n✅ WITHDRAWAL SUCCESSFUL!\n");
    printf("Amount Withdrawn: K %.2f\n", amount);
    printf("New Balance: K %.2f\n", accounts[accountIndex].balance);
    pauseScreen();
}

void transferFunds() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - TRANSFER FUNDS ===\n\n");
    
    int fromIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == currentUserAccount) {
            fromIndex = i;
            break;
        }
    }
    
    if (fromIndex == -1) {
        printf("Error: Your account not found.\n");
        pauseScreen();
        return;
    }
    
    long long toAccountNumber;
    printf("Your Current Balance: K %.2f\n", accounts[fromIndex].balance);
    printf("Enter recipient account number: ");
    scanf("%lld", &toAccountNumber);
    clearInputBuffer();
    
    if (toAccountNumber == currentUserAccount) {
        printf("Error: Cannot transfer to your own account.\n");
        pauseScreen();
        return;
    }
    
    int toIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == toAccountNumber && accounts[i].isActive) {
            toIndex = i;
            break;
        }
    }
    
    if (toIndex == -1) {
        printf("Error: Recipient account not found or inactive.\n");
        pauseScreen();
        return;
    }
    
    double amount;
    printf("Enter transfer amount (K): ");
    scanf("%lf", &amount);
    clearInputBuffer();
    
    if (amount <= 0) {
        printf("Error: Transfer amount must be positive.\n");
        pauseScreen();
        return;
    }
    
    if (amount > accounts[fromIndex].balance) {
        printf("Error: Insufficient funds. Available balance: K %.2f\n", accounts[fromIndex].balance);
        pauseScreen();
        return;
    }
    
    // Perform transfer
    accounts[fromIndex].balance -= amount;
    accounts[toIndex].balance += amount;
    saveDataToFile();
    
    // Save transactions for both accounts
    saveTransaction(currentUserAccount, "TRANSFER", -amount, accounts[fromIndex].balance, toAccountNumber);
    saveTransaction(toAccountNumber, "TRANSFER", amount, accounts[toIndex].balance, currentUserAccount);
    
    printf("\n✅ TRANSFER SUCCESSFUL!\n");
    printf("Amount Transferred: K %.2f\n", amount);
    printf("From: %lld (%s)\n", currentUserAccount, accounts[fromIndex].fullName);
    printf("To: %lld (%s)\n", toAccountNumber, accounts[toIndex].fullName);
    printf("Your New Balance: K %.2f\n", accounts[fromIndex].balance);
    pauseScreen();
}

void changePassword() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - CHANGE PASSWORD ===\n\n");
    
    int accountIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == currentUserAccount) {
            accountIndex = i;
            break;
        }
    }
    
    if (accountIndex == -1) {
        printf("Error: Account not found.\n");
        pauseScreen();
        return;
    }
    
    char currentPassword[MAX_PASSWORD_LENGTH];
    char newPassword[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];
    
    printf("Enter current password: ");
    fgets(currentPassword, MAX_PASSWORD_LENGTH, stdin);
    currentPassword[strcspn(currentPassword, "\n")] = 0;
    
    if (!verifyPassword(currentPassword, accounts[accountIndex].password)) {
        printf("Error: Current password is incorrect.\n");
        pauseScreen();
        return;
    }
    
    do {
        printf("Enter new password (min 6 chars, mix of upper/lower/digits): ");
        fgets(newPassword, MAX_PASSWORD_LENGTH, stdin);
        newPassword[strcspn(newPassword, "\n")] = 0;
        
        if (!validatePassword(newPassword)) {
            printf("Error: Password must be 6-50 characters with uppercase, lowercase, and digits.\n");
            continue;
        }
        
        printf("Confirm new password: ");
        fgets(confirmPassword, MAX_PASSWORD_LENGTH, stdin);
        confirmPassword[strcspn(confirmPassword, "\n")] = 0;
        
        if (strcmp(newPassword, confirmPassword) != 0) {
            printf("Error: New passwords do not match.\n");
        }
    } while (strcmp(newPassword, confirmPassword) != 0 || !validatePassword(newPassword));
    
    // Encrypt and store new password
    strcpy(accounts[accountIndex].password, newPassword);
    encryptPassword(accounts[accountIndex].password);
    saveDataToFile();
    
    printf("\n✅ PASSWORD CHANGED SUCCESSFULLY!\n");
    pauseScreen();
}

void displayAccountDetails() {
    clearScreen();
    printf("=== MISHTERIOUS BANK - ACCOUNT DETAILS ===\n\n");
    
    int accountIndex = -1;
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].accountNumber == currentUserAccount) {
            accountIndex = i;
            break;
        }
    }
    
    if (accountIndex == -1) {
        printf("Error: Account not found.\n");
        pauseScreen();
        return;
    }
    
    printf("Account Holder: %s\n", accounts[accountIndex].fullName);
    printf("Account Number: %lld\n", accounts[accountIndex].accountNumber);
    printf("Account Status: %s\n", accounts[accountIndex].isActive ? "Active" : "Inactive");
    displayBalance(accounts[accountIndex].balance);
    printf("Password: ******** (hidden for security)\n");
    
    printf("\nRecent Transactions:\n");
    displayTransactionHistory(currentUserAccount);
    
    pauseScreen();
}

void adminMenu() {
    int choice;
    do {
        clearScreen();
        printf("=== MISHTERIOUS BANK - ADMIN PANEL ===\n\n");
        printf("1. View All Accounts\n");
        printf("2. View Total Bank Balance\n");
        printf("3. Search Account by Number\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                clearScreen();
                printf("=== ALL ACCOUNTS (%d total) ===\n\n", accountCount);
                printf("%-20s %-15s %-15s\n", "Account Holder", "Account Number", "Balance (K)");
                printf("-------------------------------------------------\n");
                for (int i = 0; i < accountCount; i++) {
                    if (accounts[i].isActive) {
                        printf("%-20s %-15lld %-15.2f\n", 
                               accounts[i].fullName, 
                               accounts[i].accountNumber, 
                               accounts[i].balance);
                    }
                }
                pauseScreen();
                break;
                
            case 2:
                clearScreen();
                printf("=== TOTAL BANK BALANCE ===\n\n");
                double totalBalance = 0;
                int activeAccounts = 0;
                for (int i = 0; i < accountCount; i++) {
                    if (accounts[i].isActive) {
                        totalBalance += accounts[i].balance;
                        activeAccounts++;
                    }
                }
                printf("Total Bank Assets: K %.2f\n", totalBalance);
                printf("Total Active Accounts: %d\n", activeAccounts);
                printf("Total Registered Accounts: %d\n", accountCount);
                pauseScreen();
                break;
                
            case 3:
                {
                    clearScreen();
                    printf("=== SEARCH ACCOUNT ===\n\n");
                    long long searchAcc;
                    printf("Enter account number to search: ");
                    scanf("%lld", &searchAcc);
                    clearInputBuffer();
                    
                    int found = 0;
                    for (int i = 0; i < accountCount; i++) {
                        if (accounts[i].accountNumber == searchAcc) {
                            printf("\nAccount Found:\n");
                            printf("Holder: %s\n", accounts[i].fullName);
                            printf("Account Number: %lld\n", accounts[i].accountNumber);
                            printf("Balance: K %.2f\n", accounts[i].balance);
                            printf("Status: %s\n", accounts[i].isActive ? "Active" : "Inactive");
                            found = 1;
                            break;
                        }
                    }
                    
                    if (!found) {
                        printf("Account not found.\n");
                    }
                    pauseScreen();
                }
                break;
                
            case 4:
                break;
                
            default:
                printf("Invalid choice. Please try again.\n");
                pauseScreen();
        }
    } while (choice != 4);
}

void userMenu() {
    int choice;
    do {
        clearScreen();
        printf("=== MISHTERIOUS BANK - CUSTOMER PANEL ===\n\n");
        printf("1. Deposit Funds\n");
        printf("2. Withdraw Funds\n");
        printf("3. Transfer Funds\n");
        printf("4. Change Password\n");
        printf("5. View Account Details\n");
        printf("6. View Transaction History\n");
        printf("7. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                depositFunds();
                break;
            case 2:
                withdrawFunds();
                break;
            case 3:
                transferFunds();
                break;
            case 4:
                changePassword();
                break;
            case 5:
                displayAccountDetails();
                break;
            case 6:
                clearScreen();
                printf("=== TRANSACTION HISTORY ===\n\n");
                displayTransactionHistory(currentUserAccount);
                pauseScreen();
                break;
            case 7:
                currentUserAccount = -1;
                printf("Logged out successfully.\n");
                pauseScreen();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                pauseScreen();
        }
    } while (choice != 7 && currentUserAccount != -1);
}

void mainMenu() {
    int choice;
    do {
        clearScreen();
        printf("=========================================\n");
        printf("      WELCOME TO MISHTERIOUS BANK       \n");
        printf("         Banking Made Mysterious        \n");
        printf("=========================================\n\n");
        printf("1. Register New Account\n");
        printf("2. Login to Existing Account\n");
        printf("3. Admin Panel\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:
                userRegistration();
                break;
            case 2:
                if (userLogin()) {
                    userMenu();
                }
                break;
            case 3:
                printf("Enter Admin Password: ");
                char adminPass[50];
                fgets(adminPass, 50, stdin);
                adminPass[strcspn(adminPass, "\n")] = 0;
                
                if (strcmp(adminPass, "admin123") == 0) {
                    adminMenu();
                } else {
                    printf("Invalid admin password!\n");
                    pauseScreen();
                }
                break;
            case 4:
                printf("Thank you for banking with MISHTERIOUS BANK!\n");
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                pauseScreen();
        }
    } while (choice != 4);
}

void initializeSystem() {
    printf("Initializing MISHTERIOUS BANK System...\n");
    loadDataFromFile();
    printf("System ready!\n");
    sleep(1);
}

void cleanup() {
    if (accounts != NULL) {
        free(accounts);
        accounts = NULL;
    }
}

int main() {
    initializeSystem();
    mainMenu();
    cleanup();
    return 0;
}
