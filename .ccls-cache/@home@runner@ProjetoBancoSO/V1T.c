#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define a estrutura Transaction para armazenar informações sobre a transferência,
// incluindo o nome do arquivo do saldo e o valor da transferência
typedef struct {

    char *filename;
    double amount;
} Transaction;

// Função a ser executada pelas threads que realiza a transferência entre contas
void *transfer(void *arg) {
  
    Transaction *t = (Transaction *)arg;// Converte o ponteiro para o tipo correto (Transaction)
    double sender_balance;
    FILE *sender_file = fopen(t->filename, "r");// Abre o arquivo com o saldo do remetente em modo de leitura
    fscanf(sender_file, "%lf", &sender_balance);// Lê o saldo do remetente do arquivo e armazena na variável sender_balance
    fclose(sender_file);

    // Verifica se o remetente possui saldo suficiente para a transferência
    if (sender_balance < t->amount) {
        printf("Saldo insuficiente em %s.\n", t->filename);
        return NULL;
    }

    sender_balance -= t->amount;// Atualiza o saldo do remetente, subtraindo o valor da transferência
    sender_file = fopen(t->filename, "w"); // Abre novamente o arquivo do remetente, agora em modo de escrita
    fprintf(sender_file, "%.2lf", sender_balance);// Escreve o novo saldo do remetente no arquivo
    fclose(sender_file);

    char *receiver_filename = t->filename[5] == 'A' ? "saldoB.txt" : "saldoA.txt";// Determina o nome do arquivo do destinatário baseado no nome do arquivo do remetente
    double receiver_balance;
    FILE *receiver_file = fopen(receiver_filename, "r"); // Abre o arquivo com o saldo do destinatário em modo de leitura
    fscanf(receiver_file, "%lf", &receiver_balance);// Lê o saldo do destinatário do arquivo e armazena na variável receiver_balance
    fclose(receiver_file);

    receiver_balance += t->amount;// Atualiza o saldo do destinatário, adicionando o valor da transferência
    receiver_file = fopen(receiver_filename, "w");// Abre novamente o arquivo do destinatário, agora em modo de escrita
    fprintf(receiver_file, "%.2lf", receiver_balance); // Escreve o novo saldo do destinatário no arquivo
    fclose(receiver_file);

    // Imprime uma mensagem informando o sucesso da transferência
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
    
    pthread_t client_A, client_B;// Declara duas threads, uma para cada cliente (A e B)
    Transaction transaction_A = {"saldoA.txt", 100};// Cria a transação do cliente A (conta A evalor de 100)
    Transaction transaction_B = {"saldoB.txt", 200}; // Cria a transação do cliente B (conta B e valor de 200)

    // Cria as threads e passa a função transfer e as transações como argumentos
    pthread_create(&client_A, NULL, transfer, (void *)&transaction_A);// A thread client_A executará a função transfer com a transação do cliente A
    pthread_create(&client_B, NULL, transfer, (void *)&transaction_B);// A thread client_B executará a função transfer com a transação do cliente B

    // Aguarda a conclusão das threads antes de prosseguir com a execução do programa
    pthread_join(client_A, NULL);    
    pthread_join(client_B, NULL);
    print_balances();
return 0;
}

