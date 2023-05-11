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

int mystrcmp(char *s1, char *s2){
    int i = 0;
    while (s1[i] == s2[i]) { // percorre as strings ate encontrar uma diferenca
        if (s1[i] == '\0') { // as strings sao iguais ate o final
            return 0;
        }
        i++;
    }
    // as strings sao diferentes a partir do indice i
    return s1[i] - s2[i];
}

int main(int argc, char *argv[]){
    int escolhe_opcao=0;

    if((argc>3 && !strcmp(argv[1],"execute")) && !strcmp(argv[2],"-u")){ //So faz no caso de ser execute -u ls l por exemplo
        StructInicio estruturaInicio;//criar a struct

        gettimeofday(&estruturaInicio.tempo, NULL);
        estruturaInicio.pid=getpid();
        strcpy(estruturaInicio.programa,argv[3]);
        
        int fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fifo, &escolhe_opcao, sizeof(escolhe_opcao));
        write(fifo, &estruturaInicio, sizeof(StructInicio));//TIREI A CENA DO IF
        close(fifo);

        int status;
        if (fork()==0){
            execlp(argv[3],argv[3],NULL);

            _exit(0);
        }
        wait(&status);
                    
        StructFim estruturaFim;//criar a struct

        escolhe_opcao=1;
        gettimeofday(&estruturaFim.tempo, NULL);
        estruturaFim.pid=getpid();

        fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fifo, &escolhe_opcao, sizeof(escolhe_opcao));
        write(fifo,&estruturaFim, sizeof(StructFim));
        close(fifo);


    }else if(argc==2 && !strcmp(argv[1],"status")){
        //PARA TESTAR O HISTORICO
        StructInicio estruturaInicio;//criar a struct
        int fd_historico = open("./historico.txt",O_RDONLY,0666);
   
        while((read(fd_historico,&estruturaInicio,sizeof(StructInicio)))>0){
            char output1[20]={};
            sprintf(output1,"pid:%d ",estruturaInicio.pid);//Formata a string num buffer
            write(1,output1,sizeof(output1));

            struct timeval fim; 
            char output2[40]={};
            gettimeofday(&fim, NULL);
            double secs = (double)(fim.tv_usec - estruturaInicio.tempo.tv_usec) / 1000000 + (double)(fim.tv_sec - estruturaInicio.tempo.tv_sec);
            float miliseg = secs*1000;
            sprintf(output2,"Tempo em execução:%.0f ", miliseg);
            write(1,output2,sizeof(output2));

            char output3[20]={};
            sprintf(output3,"programa:%s\n",estruturaInicio.programa);//Formata a string num buffer
            write(1,output3,sizeof(output3));
        }
        close(fd_historico);
    }else if(argc>1 && !strcmp(argv[1],"stats-time")){
        //No servidor vamos usar a opcao 2
        escolhe_opcao=2;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_opcao, sizeof(escolhe_opcao));

        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe

        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro, o fifo já existe\n"); //Pode dar erro se o fifo ja existir
        } 

        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(pipe));
        close(fd_fifo);

        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe

        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a somar
        int numero_progs=argc-2;
        write(fd_fifo_novo, &numero_progs, sizeof(numero_progs)); 
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+1]); //se tivermos 5 programas (o ultimo estara no argv[6] (5+1))
            write(fd_fifo_novo, nome, sizeof(nome)); 
            numero_progs--;
        }
        close(fd_fifo_novo);

        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        float tempo;
        read(fd_fifo_novo,&tempo,sizeof(tempo));
        close(fd_fifo_novo);

        //Formata a string que da o tempo
        char string_tempo[20]={};
        sprintf(string_tempo,"Tempo:%f\n",tempo);
        write(1, &string_tempo, sizeof(string_tempo)); 

    }else if(argc>1 && !strcmp(argv[1],"stats-command")){
        //No servidor vamos usar a opcao 3
        escolhe_opcao=3;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_opcao, sizeof(escolhe_opcao));

        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe

        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro, o fifo já existe\n"); //Pode dar erro se o fifo ja existir
        } 

        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(pipe));
        close(fd_fifo);

        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe

        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a ver se sao do tipo
        int numero_progs=argc-3;
        write(fd_fifo_novo, &numero_progs, sizeof(numero_progs)); 
        //Manda o programa a comparar
        char comando[8];
        strcpy(comando,argv[2]);
        write(fd_fifo_novo, comando, sizeof(comando));
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+2]); 
            write(fd_fifo_novo, nome, sizeof(nome)); 
            numero_progs--;
        }
        close(fd_fifo_novo);

        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        int vezes;
        read(fd_fifo_novo,&vezes,sizeof(vezes));
        close(fd_fifo_novo);

        //Formata a string que da o tempo
        char string_tempo[20]={};
        sprintf(string_tempo,"Vezes:%d\n",vezes);
        write(1, &string_tempo, sizeof(string_tempo)); 
    
    }else if(argc>1 && !strcmp(argv[1],"stats-uniq")){
        //No servidor vamos usar a opcao 4
        escolhe_opcao=4;
        int fd_fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fd_fifo, &escolhe_opcao, sizeof(escolhe_opcao));

        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe

        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro, o fifo ja existe\n"); //Pode dar erro se o fifo ja existir
        } 

        //manda o nome do pipe para o servidor
        write(fd_fifo, &pipe, sizeof(pipe));
        close(fd_fifo);

        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe

        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a ver se sao diferentes
        int numero_progs=argc-2;
        write(fd_fifo_novo, &numero_progs, sizeof(numero_progs)); 
        //Manda os varios programas a somar
        while(numero_progs>0){
            char nome[8];
            strcpy(nome,argv[numero_progs+1]);
            write(fd_fifo_novo, nome, sizeof(nome)); 
            numero_progs--;
        }
        close(fd_fifo_novo);

        //Le do pipe o tempo 
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        int num_progs_difs;
        read(fd_fifo_novo,&num_progs_difs,sizeof(num_progs_difs));

        for(int i=0;i<num_progs_difs;i++){
            char programa_dif[8]={};
            read(fd_fifo_novo,&programa_dif,sizeof(programa_dif));
            char string_formatada[19];
            sprintf(string_formatada,"Programa: %s\n",programa_dif);
            write(1, &string_formatada, sizeof(char)*14); 
        }
        close(fd_fifo_novo);

    }else{
        char erro[23]="Argumentos inválidos\n";
        write(1, &erro, sizeof(erro));
    }
    return 0;
}