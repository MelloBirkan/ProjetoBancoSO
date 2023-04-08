#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int account_number;
    double balance;
} BankAccount;

typedef struct {
    BankAccount* from;
    BankAccount* to;
    double amount;
} Transaction;

void* transfer(void* arg) {
    Transaction* trans = (Transaction*) arg;

    if (trans->from->balance >= trans->amount) {
        trans->from->balance -= trans->amount;
        usleep(100);
        trans->to->balance += trans->amount;
    } else {
        printf("Insufficient funds in account %d\n", trans->from->account_number);
    }

    return NULL;
}

int main() {
    BankAccount account1 = {1, 1000.0};
    BankAccount account2 = {2, 1000.0};

    Transaction transaction1 = {&account1, &account2, 300.0};
    Transaction transaction2 = {&account2, &account1, 500.0};

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, transfer, (void*) &transaction1);
    pthread_create(&thread2, NULL, transfer, (void*) &transaction2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Account 1 balance: %.2f\n", account1.balance);
    printf("Account 2 balance: %.2f\n", account2.balance);

    return 0;
}
