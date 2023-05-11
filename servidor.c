#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define SizeBuffer 1000 //tamanho do buffer

typedef struct {
    struct timeval tempo; //em milisegundos
    pid_t pid;
    char programa[5];
} *Inicio,StructInicio;

typedef struct {
    struct timeval tempo; //em milisegundos
    pid_t pid;
} *Fim,StructFim;

typedef struct{ //para guardar os ficheiros das structs terminadas
    float tempo;
    char programa[5];
}ProgramaTerminado;

int main(int argc, char const *argv[]){
    if(argc!=2){
        perror("O servidor recebe como argumento uma diretoria onde guarda o seu conteudo\n");
        return 0;
    }
    StructInicio estruturaInicio;//criar a struct que guarda o estado inicial do programa
    StructFim estruturaFim;//criar a struct que guarda o estado final do programa
    ProgramaTerminado programaAcabado; //Inicializar struct que guarda conteudo do programa terminado

    int fifo;
    if((fifo = mkfifo("./fifo", 0666))<0){ //cria o fifo
        perror("Erro, o fifo já existe\n"); //Pode dar erro se o fifo ja existir
    } 
    int fd_historico = open("./historico.txt",O_CREAT|O_RDWR, 0666);//Onde o historico é guardado
    int fd_aux = open("./aux.txt",O_CREAT|O_RDWR, 0666);//Aux para fazer a atualizacao do historico
    close(fd_historico);
    close(fd_aux);
    
    int escolhe_opcao=0;
    while(1) { //ciclo infinito
        int fd_fifo = open("./fifo", O_RDONLY, 0666); //abre o fifo
        
        if((read(fd_fifo, &escolhe_opcao, sizeof(int)))>0){
            if(escolhe_opcao==0){
                int fd_historico = open("./historico.txt",O_WRONLY|O_TRUNC, 0666);
                read(fd_fifo, &estruturaInicio, sizeof(StructInicio));
                write(fd_historico, &estruturaInicio, sizeof(StructInicio));
                close(fd_historico);

            }else if(escolhe_opcao==1){
                int bytes_lidos;
                read(fd_fifo, &estruturaFim, sizeof(StructFim));
                //Quando acaba temos de remover a struct do historico
                int fd_historico = open("./historico.txt",O_RDONLY, 0666);
                int fd_aux = open("./aux.txt",O_WRONLY, 0666);
                //Faz uma copia para outro ficheiro do historico sem o que se tem de retirar
                while(bytes_lidos=(read(fd_historico,&estruturaInicio,sizeof(StructInicio)))>0){
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
                close(fd_historico);
                close(fd_aux);
                //Copia do auxiliar para o historico (todos menos o programa que terminou, pois este não foi para o aux.txt)
                fd_historico = open("./historico.txt",O_WRONLY, 0666);
                fd_aux = open("./aux.txt",O_RDONLY, 0666);
                while(bytes_lidos=(read(fd_aux,&estruturaInicio,sizeof(StructInicio)))>0){
                      write(fd_historico, &estruturaInicio, sizeof(StructInicio));
                }
                close(fd_historico);
                close(fd_aux);

            }else if(escolhe_opcao==2){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(pipe));

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
                    read(fd_fifo_novo,&programa,sizeof(programa));
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
                write(fd_fifo_novo,&tempo,sizeof(tempo));
                close(fd_fifo_novo);

            }else if(escolhe_opcao==3){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(pipe));

                //Le os varios argumentos do fifo
                int vezes_ocorre=0;
                int bytes_lidos;
                char programa[8]={};

                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(num_prog));
                //Vai buscar o programa a comparar
                char comando[8];
                read(fd_fifo_novo, &comando, sizeof(comando));
                //Percorre os varios argumentos
                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(programa));
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

            }else if(escolhe_opcao==4){
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(pipe));

                //Le os varios argumentos do fifo
                float tempo=0;
                int bytes_lidos;
                char programa[8]={};

                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(num_prog));
                //Ve varios parametros
                int programas_diferentes=0;
                int encontrado=0;
                char nome_diferentes[num_prog][8];
                //Percorre os varios argumentos
                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(programa));
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
                write(fd_fifo_novo,&programas_diferentes,sizeof(programas_diferentes));
                for(int i=0;i<programas_diferentes;i++){
                    write(fd_fifo_novo,nome_diferentes[i],sizeof(char)*8);
                }
                close(fd_fifo_novo);
            }
        }
        close(fd_fifo);
    }

    return 0;
}