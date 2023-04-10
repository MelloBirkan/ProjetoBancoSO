#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char *filename;
    double amount;
    pthread_mutex_t *sender_mutex;
    pthread_mutex_t *receiver_mutex;
} Transaction;

double read_balance(char *filename) {
    double balance;
    FILE *file = fopen(filename, "r");
    fscanf(file, "%lf", &balance);
    fclose(file);
    return balance;
}

void update_balance(char *filename, double balance) {
    FILE *file = fopen(filename, "w");
    fprintf(file, "%.2lf", balance);
    fclose(file);
}

void *transfer(void *arg) {
    Transaction *t = (Transaction *)arg;

    // Lock the sender's account using mutex
    pthread_mutex_lock(t->sender_mutex);

    // Read the sender's balance
    double sender_balance = read_balance(t->filename);

    // Check if the sender has enough balance
    if (sender_balance < t->amount) {
        printf("Insufficient balance in %s.\n", t->filename);
        pthread_mutex_unlock(t->sender_mutex);
        return NULL;
    }

    // Update the sender's balance
    sender_balance -= t->amount;
    update_balance(t->filename, sender_balance);

    // Unlock the sender's account using mutex
    pthread_mutex_unlock(t->sender_mutex);

    // Lock the receiver's account using mutex
    pthread_mutex_lock(t->receiver_mutex);

    // Read the receiver's balance
    char *receiver_filename = t->filename[5] == 'A' ? "saldoB.txt" : "saldoA.txt";
    double receiver_balance = read_balance(receiver_filename);

    // Update the receiver's balance
    receiver_balance += t->amount;
    update_balance(receiver_filename, receiver_balance);

    // Unlock the receiver's account using mutex
    pthread_mutex_unlock(t->receiver_mutex);

    printf("Transferred %.2lf from %s to %s.\n", t->amount, t->filename, receiver_filename);

    return NULL;
}

int main() {
    pthread_t client_A, client_B;
    pthread_mutex_t mutex_A, mutex_B;

    // Initialize the mutexes
    pthread_mutex_init(&mutex_A, NULL);
    pthread_mutex_init(&mutex_B, NULL);

    Transaction transaction_A = {"saldoA.txt", 100, &mutex_A, &mutex_B};
    Transaction transaction_B = {"saldoB.txt", 200, &mutex_B, &mutex_A};

    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);

    pthread_join(client_A, NULL);
    pthread_join(client_B, NULL);

    // Destroy the mutexes
    pthread_mutex_destroy(&mutex_A);
    pthread_mutex_destroy(&mutex_B);

    return 0;
}
