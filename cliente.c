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

int mystrcmp(const char *s1, const char *s2){
    int i = 0;
    while (s1[i] == s2[i]) { // percorre as strings até encontrar uma diferença
        if (s1[i] == '\0') { // as strings são iguais até o final
            return 0;
        }
        i++;
    }
    // as strings são diferentes a partir do índice i
    return s1[i] - s2[i];
}

int main(int argc, char const *argv[]){
    int escolhe_struct=0;

    if((argc>3 && !strcmp(argv[1],"execute")) && !strcmp(argv[2],"-u")){
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
            StructFim estruturaFim;//criar a struct

            escolhe_struct=1;
            gettimeofday(&estruturaFim.tempo, NULL);
            estruturaFim.pid=getppid();

            fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita

            sleep(10);

            write(fifo, &escolhe_struct, sizeof(int));
            write(fifo,&estruturaFim, sizeof(StructFim));
            close(fifo);

            _exit(0);
        }
    }else if(argc==2 && !strcmp(argv[1],"status")){
        //PARA TESTAR O HISTORICO
        int fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        escolhe_struct=2;
        write(1, &escolhe_struct, sizeof(int));
        close(fifo);
        
    }else{
        char erro[]="Argumentos inválidos\n";
        write(1, &erro, sizeof(erro));
    }
    return 0;
}