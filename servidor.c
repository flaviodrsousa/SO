#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#define SizeBuffer 1000 //tamanho do buffer

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
    StructInicio estruturaInicio;//criar a struct
    StructFim estruturaFim;//criar a struct

    int fifo = mkfifo("./fifo", 0666); //cria o fifo
    int fd_txt = open("./txt.txt",O_CREAT|O_TRUNC|O_RDWR);//Onde o historico é guardado
    int fd_aux = open("./aux.txt",O_CREAT|O_TRUNC|O_RDWR);//Aux para fazer a atualizacao do historcio
    close(fd_txt);
    close(fd_aux);
    
    while(1) { //ciclo infinito
        int fd_fifo = open("./fifo", O_RDONLY, 0666); //abre o fifo

        int Switch;
        read(fd_fifo, &Switch, sizeof(int));
            if(Switch==0){
                int fd_txt = open("./txt.txt",O_WRONLY);
                read(fd_fifo, &estruturaInicio, sizeof(StructInicio));
                write(fd_txt, &estruturaInicio, sizeof(StructInicio));
                close(fd_txt);
            }else if(Switch==1){
                int bytes_lidos;
                read(fd_fifo, &estruturaFim, sizeof(StructFim));
                //Quando acaba temos de remover a struct do historico
                int fd_txt = open("./txt.txt",O_RDONLY);
                int fd_aux = open("./aux.txt",O_TRUNC|O_WRONLY);
                //Faz uma copia para outro ficheiro do historico sem o que se tem de retirar
                while(bytes_lidos=(read(fd_txt,&estruturaInicio,sizeof(StructInicio)))>0){
                    if(estruturaFim.pid!=estruturaInicio.pid){
                        write(fd_aux, &estruturaInicio, sizeof(StructInicio));
                    }
                }
                close(fd_txt);
                close(fd_aux);
                //Copia do auxiliar para o historico
                fd_txt = open("./txt.txt",O_TRUNC|O_WRONLY);
                fd_aux = open("./aux.txt",O_RDONLY);
                while(bytes_lidos=(read(fd_aux,&estruturaInicio,sizeof(StructInicio)))>0){
                      write(fd_txt, &estruturaInicio, sizeof(StructInicio));
                }
                close(fd_txt);
                close(fd_aux);
            }else{
                int fd_txt = open("./txt.txt",O_RDONLY);
                int bytes_lidos;
                while(bytes_lidos=(read(fd_txt,&estruturaInicio,sizeof(StructInicio)))>0){
                    char output[20]={};
                    sprintf(output,"Pid:%d ",estruturaInicio.pid);//Formata a string num buffer
                    write(1,output,20);

                    struct timeval fim; 
                    gettimeofday(&fim, NULL);
                    int secs = (double)(fim.tv_usec - estruturaInicio.tempo.tv_usec) / 1000000 + (double)(fim.tv_sec - estruturaInicio.tempo.tv_sec);
                    char output2[40]={};
                    sprintf(output2,"Tempo em execucao:%d ", secs);
                    write(1,output2,20);

                    char output3[20]={};
                    sprintf(output3,"Programa:%s ",estruturaInicio.programa);//Formata a string num buffer
                    write(1,output3,20);

                    char paragrafro='\n';
                    write(1,&paragrafro,sizeof(char));
                }
                close(fd_txt);

            }
        close(fd_fifo);
    }
    

    return 0;
}