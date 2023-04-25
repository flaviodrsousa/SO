#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

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
    Inicio clienteInicio=malloc(sizeof(Inicio));//Make function time (begining of the function)

    struct timeval inicio; 
    clienteInicio->tempo=gettimeofday(&inicio, NULL);
    clienteInicio->pid=getpid();
    strcpy(clienteInicio->programa,argv[3]);

    int fifo = open("./fifoInicio", O_WRONLY, 0666); //abre o fifo em modo de escrita

    if(write(fifo, clienteInicio, sizeof(Inicio))==-1){
        perror("write client error"); //PERGUNTAR AO GOSTOSO
    }

    close(fifo);

    int status;
    if (fork()==0){
        execlp(argv[3],argv[3],NULL);
        _exit(0);
    }

    wait(&status);

    fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita

    Fim clienteFinal=malloc(sizeof(Fim)); 

    struct timeval fim; 
    clienteFinal->tempo=gettimeofday(&fim, NULL);
    clienteFinal->pid=getpid();

    if(write(fifo, clienteFinal, sizeof(Fim))==-1){
        perror("write client error"); //PERGUNTAR AO GOSTOSO
    }

    close(fifo);
    return 0;
}