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

        // Localiza o índice do argumento que separa os comandos
        int separatorIndex=-1;
        for (int i=3; i<argc; i++) {
            if (strcmp(argv[i],"/") == 0) {
                separatorIndex = i;
                break;
            }
        }

        // Verifica se o separador foi encontrado
        if (separatorIndex==-1 || separatorIndex==argc-1) {
            write(1,"Argumentos inválidos.\n",sizeof(char)*24);
            return 1;
        }

        int pipefd[2];
        pid_t pid;

        // Cria o pipe
        if(pipe(pipefd)==-1){
            perror("pipe");
            return 1;
        }

        // Cria um processo filho para executar o segundo comando
        pid = fork();

        if(pid < 0) {
            perror("fork");
            return 1;
        }else if(pid == 0){
            // Código do processo filho (segundo comando)
            close(pipefd[1]);  // Fecha a extremidade de escrita do pipe

            // Redireciona a leitura do pipe para a entrada padrão (stdin)
            dup2(pipefd[0], STDIN_FILENO);

            // Fecha a extremidade de leitura do pipe
            close(pipefd[0]);

            // Constrói o vetor de argumentos para o segundo comando
            char *secondCommandArgs[argc - separatorIndex];
            for(int i = separatorIndex + 1; i < argc; i++) {
                secondCommandArgs[i - separatorIndex - 1] = argv[i];
            }
            secondCommandArgs[argc - separatorIndex - 1] = NULL;

            // Executa o segundo comando especificado pelo argumento
            execvp(secondCommandArgs[0], secondCommandArgs);

            perror("execvp");
            exit(1); // Termina o processo filho
        }else{
            // Código do processo pai (primeiro comando)
            close(pipefd[0]);  // Fecha a extremidade de leitura do pipe

            // Redireciona a saída padrão (stdout) para a escrita no pipe
            dup2(pipefd[1], STDOUT_FILENO);

            // Fecha a extremidade de escrita do pipe
            close(pipefd[1]);

            // Constrói o vetor de argumentos para o primeiro comando
            char *firstCommandArgs[separatorIndex - 2];
            for(int i = 3; i < separatorIndex; i++){
                firstCommandArgs[i - 3] = argv[i];
            }
            firstCommandArgs[separatorIndex - 3] = NULL;

            // Executa o primeiro comando especificado pelo argumento
            execvp(firstCommandArgs[0], firstCommandArgs);

            perror("execvp");
            exit(1); // Termina o processo pai
        }   

    }else{
        char erro[]="Argumentos inválidos\n";
    }
}