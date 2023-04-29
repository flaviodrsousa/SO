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
    struct timeval tempo; //em milisegundos
    pid_t pid;
    char programa[2];
} *Inicio,StructInicio;

typedef struct {
    struct timeval tempo; //em milisegundos
    pid_t pid;
} *Fim,StructFim;

int main(int argc, char const *argv[]){
    int escolhe_struct=0;
    StructInicio estruturaInicio;//criar a struct

    gettimeofday(&estruturaInicio.tempo, NULL);
    estruturaInicio.pid=getpid();
    strcpy(estruturaInicio.programa,argv[3]);
    
    int fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita

    write(fifo, &escolhe_struct, sizeof(int));
    write(fifo, &estruturaInicio, sizeof(StructInicio));//TIREI A CENA DO IF

    //PARA TESTAR O HISTORICO
    escolhe_struct=2;
    write(fifo, &escolhe_struct, sizeof(int));
    //ACABOU 


    int status;
    if (fork()==0){
        execlp(argv[3],argv[3],NULL);
        _exit(0);
    }

    wait(&status);

    StructFim estruturaFim;//criar a struct

    escolhe_struct=1;
    gettimeofday(&estruturaFim.tempo, NULL);
    estruturaFim.pid=getpid();

    write(fifo, &escolhe_struct, sizeof(int));
    write(fifo,&estruturaFim, sizeof(StructFim));

    
    close(fifo);
    return 0;
}