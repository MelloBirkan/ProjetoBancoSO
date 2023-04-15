#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef struct {
    char *filename;
    double amount;
} Transaction;

sem_t sem_A, sem_B;

void acquire_both_semaphores() {
    while (1) {
        if (sem_trywait(&sem_A) == 0) {
            if (sem_trywait(&sem_B) == 0) {
                break;
            } else {
                sem_post(&sem_A);
            }
        }
        usleep(100); // Wait for a short time before trying again
    }
}

void release_both_semaphores() {
    sem_post(&sem_A);
    sem_post(&sem_B);
}

void *transfer(void *arg) {
    Transaction *t = (Transaction *)arg;

    // Lock semaphores for both files
    acquire_both_semaphores();

    // Read the sender's balance
    double sender_balance;
    FILE *sender_file = fopen(t->filename, "r");
    fscanf(sender_file, "%lf", &sender_balance);
    fclose(sender_file);

    // Check if the sender has enough balance
    if (sender_balance < t->amount) {
        printf("Saldo insuficiente em %s.\n", t->filename);
        release_both_semaphores();
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

    // Unlock semaphores for both files
    release_both_semaphores();

    printf("Transferiu %.2lf de %s para %s.\n", t->amount, t->filename, receiver_filename);
    return NULL;
}


void print_balances() {
    double balance_A, balance_B;
    FILE *file_A = fopen("saldoA.txt", "r");
    FILE *file_B = fopen("saldoB.txt", "r");

    fscanf(file_A, "%lf", &balance_A);
    fscanf(file_B, "%lf", &balance_B);

    fclose(file_A);
    fclose(file_B);

    printf("Saldo em A: %.2lf\n", balance_A);
    printf("Saldo em B: %.2lf\n", balance_B);
}


int main() {
    pthread_t client_A, client_B;
    Transaction transaction_A = {"saldoA.txt", 100};
    Transaction transaction_B = {"saldoB.txt", 200};

    // Initialize semaphores
    sem_init(&sem_A, 0, 1);
    sem_init(&sem_B, 0, 1);

    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);

    pthread_join(client_A, NULL);
    pthread_join(client_B, NULL);

    // Destroy semaphores
    sem_destroy(&sem_A);
    sem_destroy(&sem_B);
    print_balances();

    return 0;
}
