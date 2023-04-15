#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define a estrutura Transaction para armazenar informações sobre a transferência
typedef struct {
    char *filename;
    double amount;
} Transaction;

// Função a ser executada pelas threads
void *transfer(void *arg) {
    Transaction *t = (Transaction *)arg;

    // Ler o saldo do remetente
    double sender_balance;
    FILE *sender_file = fopen(t->filename, "r");
    fscanf(sender_file, "%lf", &sender_balance);
    fclose(sender_file);

    // Verificar se o remetente tem saldo suficiente
    if (sender_balance < t->amount) {
        printf("Insufficient balance in %s.\n", t->filename);
        return NULL;
    }

    // Atualizar o saldo do remetente
    sender_balance -= t->amount;
    sender_file = fopen(t->filename, "w");
    fprintf(sender_file, "%.2lf", sender_balance);
    fclose(sender_file);

    // Ler e atualizar o saldo do destinatário
    char *receiver_filename = t->filename[5] == 'A' ? "saldoB.txt" : "saldoA.txt";
    double receiver_balance;
    FILE *receiver_file = fopen(receiver_filename, "r");
    fscanf(receiver_file, "%lf", &receiver_balance);
    fclose(receiver_file);

    receiver_balance += t->amount;
    receiver_file = fopen(receiver_filename, "w");
    fprintf(receiver_file, "%.2lf", receiver_balance);
    fclose(receiver_file);

    // Imprimir mensagem informando o sucesso da transferência
    printf("Transferred %.2lf from %s to %s.\n", t->amount, t->filename, receiver_filename);
    return NULL;
}

// Função principal do programa
int main() {
    pthread_t client_A, client_B; // Declara duas threads, uma para cada cliente
    Transaction transaction_A = {"saldoA.txt", 100}; // Cria a transação do cliente A
    Transaction transaction_B = {"saldoB.txt", 200}; // Cria a transação do cliente B

    // Cria as threads e passa a função transfer e as transações como argumentos
    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);

    // Aguarda a conclusão das threads
    pthread_join(client_A, NULL);
    pthread_join(client_B, NULL);

    return 0;
}
