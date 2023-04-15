#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


typedef struct {
    char *filename;
    double amount;
} Transaction;


sem_t semaphoreA, semaphoreB;


void *transfer(void *arg) {
    Transaction *t = (Transaction *)arg;

    
    if (t->filename[5] == 'A') {
        sem_wait(&semaphoreA);
        usleep(100000); 
        sem_wait(&semaphoreB);
    } else {
        sem_wait(&semaphoreB);
        usleep(100000); 
        sem_wait(&semaphoreA);
    }
   
    double sender_balance;
    FILE *sender_file = fopen(t->filename, "r");
    fscanf(sender_file, "%lf", &sender_balance);
    fclose(sender_file);

  
    if (sender_balance < t->amount) {
        printf("Insufficient balance in %s.\n", t->filename);
        if (t->filename[5] == 'A') {
            sem_post(&semaphoreB);
            sem_post(&semaphoreA);
        } else {
            sem_post(&semaphoreA);
            sem_post(&semaphoreB);
        }
        return NULL;
    }


    sender_balance -= t->amount;
    sender_file = fopen(t->filename, "w");
    fprintf(sender_file, "%.2lf", sender_balance);
    fclose(sender_file);


    char *receiver_filename = t->filename[5] == 'A' ? "saldoB.txt" : "saldoA.txt";
    double receiver_balance;
    FILE *receiver_file = fopen(receiver_filename, "r");
    fscanf(receiver_file, "%lf", &receiver_balance);
    fclose(receiver_file);

    receiver_balance += t->amount;
    receiver_file = fopen(receiver_filename, "w");
    fprintf(receiver_file, "%.2lf", receiver_balance);
    fclose(receiver_file);


    if (t->filename[5] == 'A') {
        sem_post(&semaphoreB);
        sem_post(&semaphoreA);
    } else {
        sem_post(&semaphoreA);
        sem_post(&semaphoreB);
    }


    printf("Transferred %.2lf from %s to %s.\n", t->amount, t->filename, receiver_filename);
    return NULL;
}



int main() {
    pthread_t client_A, client_B; 
    Transaction transaction_A = {"saldoA.txt", 100}; 
    Transaction transaction_B = {"saldoB.txt", 200}; 

    sem_init(&semaphoreA, 0, 1);
    sem_init(&semaphoreB, 0, 1);


    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);


    pthread_join(client_A, NULL);
    pthread_join(client_B, NULL);


    sem_destroy(&semaphoreA);
    sem_destroy(&semaphoreB);

    return 0;
}
