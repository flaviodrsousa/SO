#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include "estruturas.h"

int main(int argc, char const *argv[]){
    if(argc!=2){
        perror("Erro\n");
        return 0;
    }
    StructInicio estruturaInicio;//criar a struct
    StructFim estruturaFim;//criar a struct
    ProgramaTerminado programaAcabado; //Inicializar struct que guarda conteudo do programa terminado
    int fifo;
    if((fifo = mkfifo("./fifo", 0666))<0){
        perror("Erro\n"); //Pode dar erro se o fifo ja existir
    } //cria o fifo
    int fd_txt = open("./txt.txt",O_CREAT|O_RDWR, 0666);//Onde o historico é guardado
    int fd_aux = open("./aux.txt",O_CREAT|O_RDWR, 0666);//Aux para fazer a atualizacao do historcio
    close(fd_txt);
    close(fd_aux);
    
    int escolhe_struct=0;
    while(1) { //ciclo infinito
        int fd_fifo = open("./fifo", O_RDONLY, 0666); //abre o fifo
        
        if((read(fd_fifo, &escolhe_struct, sizeof(int)))>0){
            if(escolhe_struct==0){
                int fd_txt = open("./txt.txt",O_WRONLY|O_TRUNC, 0666);
                read(fd_fifo, &estruturaInicio, sizeof(StructInicio));
                write(fd_txt, &estruturaInicio, sizeof(StructInicio));
                close(fd_txt);
            }else if(escolhe_struct==1){
                int bytes_lidos;
                read(fd_fifo, &estruturaFim, sizeof(StructFim));
                //Quando acaba temos de remover a struct do historico
                int fd_txt = open("./txt.txt",O_RDONLY, 0666);
                int fd_aux = open("./aux.txt",O_WRONLY, 0666);
                //Faz uma copia para outro ficheiro do historico sem o que se tem de retirar
                while(bytes_lidos=(read(fd_txt,&estruturaInicio,sizeof(StructInicio)))>0){
                    if(estruturaFim.pid!=estruturaInicio.pid){
                        write(fd_aux, &estruturaInicio, sizeof(StructInicio));
                    }else{
                        char prog_acabado[30]={};
                        sprintf(prog_acabado,"%s/%d.txt",argv[1],estruturaInicio.pid);//Cria o nome do ficheiro
                        int fd_prog_acabado=open(prog_acabado,O_CREAT|O_WRONLY, 0666);//Cria o ficheiro onde se vai guardar o programa acabado
                        double secs = (double)(estruturaFim.tempo.tv_usec - estruturaInicio.tempo.tv_usec) / 1000000 + (double)(estruturaFim.tempo.tv_sec - estruturaInicio.tempo.tv_sec);
                        float miliseg = secs * 1000;
                        strcpy(programaAcabado.programa,estruturaInicio.programa);
                        programaAcabado.tempo=miliseg;
                        write(fd_prog_acabado, &programaAcabado, sizeof(ProgramaTerminado));
                        close(fd_prog_acabado);
                    }
                }
                close(fd_txt);
                close(fd_aux);
                //Copia do auxiliar para o historico
                fd_txt = open("./txt.txt",O_WRONLY, 0666);
                fd_aux = open("./aux.txt",O_RDONLY, 0666);
                while(bytes_lidos=(read(fd_aux,&estruturaInicio,sizeof(StructInicio)))>0){
                      write(fd_txt, &estruturaInicio, sizeof(StructInicio));
                }
                close(fd_txt);
                close(fd_aux);
            }else if(escolhe_struct==2){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(char)*7);
                //Le os varios argumentos do fifo
                float tempo=0;
                int bytes_lidos;
                char programa[8]={};
                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(int));
                //Percorre os varios argumentos
                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(char)*8);
                    //Ve qual o ficheiro que tem de abrir
                    char prog_acabado[30]={};
                    sprintf(prog_acabado,"%s/%s.txt",argv[1],programa);
                    //Abre o ficheiro e mete na struct
                    int fd_prog_acabado=open(prog_acabado,O_RDONLY, 0666);
                    read(fd_prog_acabado, &programaAcabado, sizeof(ProgramaTerminado));
                    close(fd_prog_acabado);
                    //soma o tempo ao tempo total
                    tempo+=programaAcabado.tempo; 
                    //para ir reduzindo o numero de programas a somar
                    num_prog--;
                }
                close(fd_fifo_novo);
                    
                //Para mandar o tempo final para o cliente
                fd_fifo_novo = open(pipe, O_WRONLY, 0666);
                write(fd_fifo_novo,&tempo,sizeof(float));
                close(fd_fifo_novo);
            }else if(escolhe_struct==3){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(char)*7);
                //Le os varios argumentos do fifo
                int vezes_ocorre=0;
                int bytes_lidos;
                char programa[8]={};
                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(int));
                //Vai buscar o programa a comparar
                char comando[8];
                read(fd_fifo_novo, &comando, sizeof(char)*8);
                //Percorre os varios argumentos
                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(char)*8);
                    //Ve qual o ficheiro que tem de abrir
                    char prog_acabado[30]={};
                    sprintf(prog_acabado,"%s/%s.txt",argv[1],programa);
                    //Abre o ficheiro e mete na struct
                    int fd_prog_acabado=open(prog_acabado,O_RDONLY, 0666);
                    read(fd_prog_acabado, &programaAcabado, sizeof(ProgramaTerminado));
                    close(fd_prog_acabado);
                    //soma o tempo ao tempo total
                    if(!strcmp(programaAcabado.programa,comando)) vezes_ocorre++;
                    //para ir reduzindo o numero de programas a somar
                    num_prog--;
                }
                close(fd_fifo_novo);
                    
                //Para mandar o tempo final para o cliente
                fd_fifo_novo = open(pipe, O_WRONLY, 0666);
                write(fd_fifo_novo,&vezes_ocorre,sizeof(int));
                close(fd_fifo_novo);

            }else if(escolhe_struct==4){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(char)*7);

                //Le os varios argumentos do fifo
                float tempo=0;
                int bytes_lidos;
                char programa[8]={};

                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(int));
                //Ve varios parametros
                int programas_diferentes=0;
                int encontrado=0;
                char nome_diferentes[num_prog][8];
                //Percorre os varios argumentos
                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(char)*8);
                    //Ve qual o ficheiro que tem de abrir
                    char prog_acabado[30]={};
                    sprintf(prog_acabado,"%s/%s.txt",argv[1],programa);
                    //Abre o ficheiro e mete na struct
                    int fd_prog_acabado=open(prog_acabado,O_RDONLY, 0666);
                    read(fd_prog_acabado, &programaAcabado, sizeof(ProgramaTerminado));
                    close(fd_prog_acabado);
                    //ve se o programa passado como argumento ja existe na lista dos programas existente
                    for(int i=0;i<programas_diferentes && encontrado==0;i++){
                        if(!strcmp(programaAcabado.programa,nome_diferentes[i])) encontrado=1;
                    }
                    if(encontrado==0){
                        strcpy(nome_diferentes[programas_diferentes],programaAcabado.programa);
                        programas_diferentes++;
                    }else{
                        encontrado=0;
                    }
                    //para ir reduzindo o numero de programas a somar
                    num_prog--;
                }
                close(fd_fifo_novo);

                //Para mandar o tempo final para o cliente
                fd_fifo_novo = open(pipe, O_WRONLY, 0666);
                write(fd_fifo_novo,&programas_diferentes,sizeof(int));
                for(int i=0;i<programas_diferentes;i++){
                    write(fd_fifo_novo,nome_diferentes[i],sizeof(char)*8);
                }
                close(fd_fifo_novo);
            }

        }
    }
}