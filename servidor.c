#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SizeBuffer 1000 //tamanho do buffer

typedef struct {
    double tempo; //em milisegundos
    pid_t pid;
    char programa[15];
} *Inicio;

typedef struct {
    double tempo; //em milisegundos
    pid_t pid;
} *Fim;

int main(int argc, char const *argv[]){
    int fifoInicio = mkfifo("./fifoInicio", 0666); //cria o fifo
    int fifoFim = mkfifo("./fifoFim", 0666); //cria o fifo
    
    fifoInicio = open("./fifoInicio", O_RDONLY, 0666); //abre o fifo
    int fifonotEndofFileInicio = open("./fifoInicio", O_WRONLY, 0666); //para não termos end of file no read
    int fifonotEndofFileFim = open("./fifoFim", O_WRONLY, 0666); //para não termos end of file no read

    while(1) { //ciclo infinito
        ssize_t SizeRead = 0; //tamanho que consegui ler, pode ser menor que o SizeBuffer.
        Inicio clienteInicio;

        while((SizeRead = read(fifoInicio, clienteInicio, sizeof(Inicio))) > 0) { //le do fifo e escreve no buffer
            printf("%s\n",clienteInicio->programa);
            write(1, clienteInicio->programa, sizeof(clienteInicio->programa)); //escreve no stdout o conteudo do buffer
        }
    }
    close(fifoInicio);
    close(fifonotEndofFileInicio);

    return 0;
}