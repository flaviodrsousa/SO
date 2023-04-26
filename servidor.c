#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SizeBuffer 1000 //tamanho do buffer

typedef struct {
    double tempo; //em milisegundos
    pid_t pid;
    char programa[2];
} *Inicio,StructInicio;

typedef struct {
    double tempo; //em milisegundos
    pid_t pid;
} *Fim,StructFim;

int main(int argc, char const *argv[]){
    StructInicio estruturaInicio;//criar a struct
    StructFim estruturaFim;//criar a struct

    int fifo = mkfifo("./fifo", 0666); //cria o fifo
    
    while(1) { //ciclo infinito
        int fd_fifo = open("./fifo", O_RDONLY, 0666); //abre o fifo

        int escolhe_strcut;
        read(fd_fifo, &escolhe_strcut, sizeof(int));
            if(escolhe_strcut==0){
                read(fd_fifo, &estruturaInicio, sizeof(StructInicio));
                write(1, &estruturaInicio.programa, sizeof(estruturaInicio.programa));
            }else{
                read(fd_fifo, &estruturaFim, sizeof(StructFim));
            }
        
        close(fd_fifo);
    }

    return 0;
}