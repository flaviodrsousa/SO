#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "estruturas.h"

int main(int argc, char *argv[]){
    int escolhe_struct=0;
    if((argc>3 && !strcmp(argv[1],"execute")) && !strcmp(argv[2],"-u")){ //So faz no caso de ser execute -u ls l por exemplo
        StructInicio estruturaInicio;//criar a struct
        gettimeofday(&estruturaInicio.tempo, NULL);
        estruturaInicio.pid=getpid();
        strcpy(estruturaInicio.programa,argv[3]);
        
        int fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fifo, &escolhe_struct, sizeof(int));
        write(fifo, &estruturaInicio, sizeof(StructInicio));//TIREI A CENA DO IF
        close(fifo);
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
        fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fifo, &escolhe_struct, sizeof(int));
        write(fifo,&estruturaFim, sizeof(StructFim));
        close(fifo);
    }else if(argc==2 && !strcmp(argv[1],"status")){
        //PARA TESTAR O HISTORICO
        StructInicio estruturaInicio;//criar a struct
        int fd_txt = open("./txt.txt",O_RDONLY,0666);
   
        while((read(fd_txt,&estruturaInicio,sizeof(StructInicio)))>0){
            char output1[20]={};
            sprintf(output1,"pid:%d ",estruturaInicio.pid);//Formata a string num buffer
            write(1,output1,sizeof(char)*20);
            struct timeval fim; 
            char output2[40]={};
            gettimeofday(&fim, NULL);
            double secs = (double)(fim.tv_usec - estruturaInicio.tempo.tv_usec) / 1000000 + (double)(fim.tv_sec - estruturaInicio.tempo.tv_sec);
            float miliseg = secs*1000;
            sprintf(output2,"Tempo em execussao:%.0f ", miliseg);
            write(1,output2,sizeof(char)*40);
            char output3[20]={};
            sprintf(output3,"programa:%s\n",estruturaInicio.programa);//Formata a string num buffer
            write(1,output3,sizeof(char)*20);
        }
        close(fd_txt);
    }else if(argc>1 && !strcmp(argv[1],"stats-time")){
        //No servidor vamos usar a opcao 2
        escolhe_struct=2;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_struct, sizeof(int));
        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe
        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro\n"); //Pode dar erro se o fifo ja existir
        } 
        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(char)*5);
        close(fd_fifo);
        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe
        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a somar
        int numero_progs=argc-2;
        write(fd_fifo_novo, &numero_progs, sizeof(int)); 
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+1]);
            write(fd_fifo_novo, nome, sizeof(char)*8); //+1 = -2+1
            numero_progs--;
        }
        close(fd_fifo_novo);
        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        float tempo;
        read(fd_fifo_novo,&tempo,sizeof(float));
        close(fd_fifo_novo);
        //Formata a string que da o tempo
        char string_tempo[20]={};
        sprintf(string_tempo,"Tempo:%f\n",tempo);
        write(1, &string_tempo, sizeof(char)*20); 
    }else if(argc>1 && !strcmp(argv[1],"stats-command")){
        //No servidor vamos usar a opcao 3
        escolhe_struct=3;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_struct, sizeof(int));
        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe
        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro\n"); //Pode dar erro se o fifo ja existir
        } 
        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(char)*5);
        close(fd_fifo);
        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe
        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a ver se sao do tipo
        int numero_progs=argc-3;
        write(fd_fifo_novo, &numero_progs, sizeof(int)); 
        //Manda o programa a comparar
        char comando[8];
        strcpy(comando,argv[2]);
        write(fd_fifo_novo, comando, sizeof(char)*8);
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+2]);
            write(fd_fifo_novo, nome, sizeof(char)*8); //+1 = -2+1
            numero_progs--;
        }
        close(fd_fifo_novo);
        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        int vezes;
        read(fd_fifo_novo,&vezes,sizeof(int));
        close(fd_fifo_novo);
        //Formata a string que da o tempo
        char string_tempo[20]={};
        sprintf(string_tempo,"Vezes:%d\n",vezes);
        write(1, &string_tempo, sizeof(char)*20); 

    }else if(argc>1 && !strcmp(argv[1],"stats-uniq")){
        //No servidor vamos usar a opcao 4
        escolhe_struct=4;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_struct, sizeof(int));

        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe

        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro\n"); //Pode dar erro se o fifo ja existir
        } 

        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(char)*5);
        close(fd_fifo);

        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe

        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a ver se sao diferentes
        int numero_progs=argc-2;
        write(fd_fifo_novo, &numero_progs, sizeof(int)); 
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+1]);
            write(fd_fifo_novo, nome, sizeof(char)*8); //+1 = -2+1
            numero_progs--;
        }
        close(fd_fifo_novo);

        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        int num_progs_difs;
        read(fd_fifo_novo,&num_progs_difs,sizeof(int));

        for(int i=0;i<num_progs_difs;i++){
            char programa_dif[8]={};
            read(fd_fifo_novo,&programa_dif,sizeof(char)*8);
            char string_formatada[19];
            sprintf(string_formatada,"Programa: %s\n",programa_dif);
            write(1, &string_formatada, sizeof(char)*14); 
        }
        close(fd_fifo_novo);
    
    }else if((argc>3 && !strcmp(argv[1],"execute")) && !strcmp(argv[2],"-p")){

        int numProcessos = 0; // Número de comandos na pipeline

        // Conta o número de comandos
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "/") == 0) {
                numProcessos++;
            }
        }
        numProcessos++;

        int pipes[numProcessos - 1][2]; // Array de pipes
        int comandoInicio = 3; // Índice do primeiro comando na pipeline
        int comandoFim=comandoInicio;
        // Cria os pipes
        for (int i = 0; i < numProcessos; i++) {
            comandoFim = comandoInicio;

            // Encontra o índice do próximo comando
            while (comandoFim < argc && strcmp(argv[comandoFim], "/")) {
                comandoFim++;
            }

            // No caso de ser o primeiro programa, apenas temos de direcionar a saida
            if (i == 0) {
                // Cria o pipe
                if (pipe(pipes[i]) < 0) {
                    perror("pipe");
                    _exit(1);
                }
                if (fork() == 0) {
                    // CÓDIGO DO FILHO
                    close(pipes[i][0]); // fechar o descritor de leitura
                    dup2(pipes[i][1], 1); // direcionar o output para o descritor de escrita
                    close(pipes[i][1]); // uma vez que não vai ser usado

                    // Criar o array com os argumentos
                    int tamanho = comandoFim - comandoInicio;
                    char *argumentos[tamanho + 1];
                    for (int i = 0; i < tamanho; i++) {
                        argumentos[i] = argv[comandoInicio + i];
                    }
                    argumentos[tamanho] = NULL;
                    execvp(argv[comandoInicio], argumentos);
                }
                close(pipes[i][1]); // fechar o descritor de escrita do pai
                int status;
                wait(&status);
            } else if (i > 0 && i <= numProcessos - 2) {
                // Cria o pipe
                if (pipe(pipes[i]) < 0) {
                    perror("pipe");
                    _exit(1);
                }
                if (fork() == 0) {
                    // CÓDIGO DO FILHO
                    close(pipes[i][0]); // fechar o descritor de leitura do pipe atual

                    dup2(pipes[i - 1][0], 0); // direcionar o input para pipe anterior
                    close(pipes[i - 1][0]); // uma vez que não vai ser usado

                    dup2(pipes[i][1], 1); // direcionar o output para o descritor de escrita
                    close(pipes[i][1]); // uma vez que não vai ser usado

                    // Criar o array com os argumentos
                    int tamanho = comandoFim - comandoInicio;
                    char *argumentos[tamanho + 1];
                    for (int i = 0; i < tamanho; i++) {
                        argumentos[i] = argv[comandoInicio + i];
                    }
                    argumentos[tamanho] = NULL;
                    execvp(argv[comandoInicio], argumentos);

                    _exit(1);
                }
                close(pipes[i - 1][0]); // fechar o descritor de leitura do pipe anterior
                close(pipes[i][1]); // fechar o descritor de escrita do pipe atual
                int status;
                wait(&status);
            } else if (i + 1 == numProcessos) {
                // Cria o pipe
                if (pipe(pipes[i]) < 0) {
                    perror("pipe");
                    _exit(1);
                }
                if (fork() == 0) {
                    // CÓDIGO DO FILHO

                    dup2(pipes[i - 1][0], 0); // direcionar o input para o pipe anterior
                    close(pipes[i - 1][0]); // uma vez que não vai ser usado

                    // Criar o array com os argumentos
                    int tamanho = argc - comandoInicio;
                    char *argumentos[tamanho + 1];
                    for (int i = 0; i < tamanho; i++) {
                        argumentos[i] = argv[comandoInicio + i];
                    }
                    argumentos[tamanho] = NULL;
                    execvp(argv[comandoInicio], argumentos);
                    _exit(1);
                }
                close(pipes[i - 1][0]); // fechar o descritor de leitura do pipe anterior
                int status;
                wait(&status);
            }
            comandoInicio = comandoFim + 1;
        }

    }else{
        char erro[]="Argumentos inválidos\n";
    }
}