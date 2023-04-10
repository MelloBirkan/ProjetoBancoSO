#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char *filename;
    double amount;
} Transaction;

pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;

void *transfer(void *arg) {
    Transaction *t = (Transaction *)arg;

    if (t->filename[5] == 'A') {
        pthread_mutex_lock(&mutexA);
        printf("Locked mutex A for %s\n", t->filename);
        sleep(1);
        pthread_mutex_lock(&mutexB);
        printf("Locked mutex B for %s\n", t->filename);
    } else {
        pthread_mutex_lock(&mutexB);
        printf("Locked mutex B for %s\n", t->filename);
        sleep(1);
        pthread_mutex_lock(&mutexA);
        printf("Locked mutex A for %s\n", t->filename);
    }

    // Read the sender's balance
    double sender_balance;
    FILE *sender_file = fopen(t->filename, "r");
    fscanf(sender_file, "%lf", &sender_balance);
    fclose(sender_file);

    // Check if the sender has enough balance
    if (sender_balance < t->amount) {
        printf("Insufficient balance in %s.\n", t->filename);
        pthread_mutex_unlock(&mutexA);
        pthread_mutex_unlock(&mutexB);
        return NULL;
    }

    // Update the sender's balance
    sender_balance -= t->amount;
    sender_file = fopen(t->filename, "w");
    fprintf(sender_file, "%.2lf", sender_balance);
    fclose(sender_file);

    // Read and update the receiver's balance
    char *receiver_filename = t->filename[5] == 'A' ? "saldoB.txt" : "saldoA.txt";
    double receiver_balance;
    FILE *receiver_file = fopen(receiver_filename, "r");
    fscanf(receiver_file, "%lf", &receiver_balance);
    fclose(receiver_file);

    receiver_balance += t->amount;
    receiver_file = fopen(receiver_filename, "w");
    fprintf(receiver_file, "%.2lf", receiver_balance);
    fclose(receiver_file);

    printf("Transferred %.2lf from %s to %s.\n", t->amount, t->filename, receiver_filename);

    // Unlock the mutexes
    pthread_mutex_unlock(&mutexA);
    pthread_mutex_unlock(&mutexB);

    return NULL;
}

int main() {
    pthread_t client_A, client_B;
    Transaction transaction_A = {"saldoA.txt", 100};
    Transaction transaction_B = {"saldoB.txt", 200};

    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);

    pthread_join(client_A, NULL);
    pthread_join(client_B, NULL);

    return 0;
}
