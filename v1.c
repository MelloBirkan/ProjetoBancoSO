#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int account_number;
    char filename[64];
} BankAccount;

typedef struct {
    BankAccount* from;
    BankAccount* to;
    double amount;
} Transaction;

double read_balance(BankAccount* account) {
    double balance;

    FILE* file = fopen(account->filename, "r");
    if (file) {
        fscanf(file, "%lf", &balance);
        fclose(file);
    } else {
        printf("Error opening file %s\n", account->filename);
        balance = 0;
    }

    return balance;
}

void write_balance(BankAccount* account, double balance) {
    FILE* file = fopen(account->filename, "w");
    if (file) {
        fprintf(file, "%.2f", balance);
        fclose(file);
    } else {
        printf("Error opening file %s\n", account->filename);
    }
}

void* transfer(void* arg) {
    Transaction* trans = (Transaction*)arg;

    double from_balance = read_balance(trans->from);
    double to_balance = read_balance(trans->to);

    if (from_balance >= trans->amount) {
        from_balance -= trans->amount;
        to_balance += trans->amount;

        write_balance(trans->from, from_balance);
        write_balance(trans->to, to_balance);
    } else {
        printf("Insufficient funds in account %d\n", trans->from->account_number);
    }

    return NULL;
}

int main() {
    BankAccount account1 = {1, "saldoA.txt"};
    BankAccount account2 = {2, "saldoB.txt"};

    // Initialize account files
    write_balance(&account1, 1000.0);
    write_balance(&account2, 1000.0);

    Transaction transaction1 = {&account1, &account2, 300.0};
    Transaction transaction2 = {&account2, &account1, 500.0};

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, transfer, (void*)&transaction1);
    pthread_create(&thread2, NULL, transfer, (void*)&transaction2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Account 1 balance: %.2f\n", read_balance(&account1));
    printf("Account 2 balance: %.2f\n", read_balance(&account2));

    return 0;
}
