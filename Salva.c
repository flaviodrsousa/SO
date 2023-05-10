 }else if(argc>1 && !strcmp(argv[1],"stats-uniq")){
        //No servidor vamos usar a opcao 4
        escolhe_struct=4;
        int fifo = open("./fifo", O_WRONLY, 0666); //abre o fifo em modo de escrita
        write(fifo, &escolhe_struct, sizeof(int));

        //cria o fifo com nome e manda para o servidor o nome do mesmo
        char pipe[5]={};
        sprintf(pipe,"%i",getpid());//Cria o nome do pipe

        int fifo_novo;
        if((fifo_novo = mkfifo(pipe, 0666))<0){
            perror("Erro\n"); //Pode dar erro se o fifo ja existir
        } 

        //formatar o nome do pipe para os servidor
        char nome[7]={};
        sprintf(nome,"./%i",getpid());//Cria o nome do pipe

        //manda o nome do pipe para o servidor
        write(fifo, &nome, sizeof(char)*7);
        close(fifo);

        //manda para o pipe especifico os argumentos
        int fd_fifo_novo = open(nome, O_WRONLY, 0666);
        //Manda o numero de programas a ver se sao diferentes
        int numero_progs=argc-2;
        write(fd_fifo_novo, &numero_progs, sizeof(int)); 

        //Manda os varios programas a somar
        while(numero_progs>0){
            write(fd_fifo_novo, &argv[numero_progs+1], sizeof(char)*5);
            numero_progs--;
        }
        close(fd_fifo_novo);

        //Le do pipe o numero de vezes
        fd_fifo_novo = open(pipe, O_RDONLY, 0666);
        int progs_diferentes;
        read(fd_fifo_novo,&progs_diferentes,sizeof(int));

        //Escreve todos os programas diferentes que exiatem na lista fornecida
        char paragrafo='\n';
        for(int i=0;i<progs_diferentes;i++){
            char prog_dif[7]={};
            read(fd_fifo_novo,&prog_dif,sizeof(char)*7);
            write(1,&prog_dif,sizeof(char)*7);
            write(1,&paragrafo,sizeof(char));
        }
        close(fd_fifo_novo);

//........................................
}else if(escolhe_struct==4){
                 ProgramaTerminado programaAcabado; //Inicializar struct que guarda conteudo do programa terminado
                //Le o pipe para onde tem de mandar
                char pipe[7]={};
                read(fd_fifo, &pipe, sizeof(char)*7);

                //Le os varios argumentos do fifo
                int vezes_ocorre=0;
                char programa[5]={};

                int fd_fifo_novo = open(pipe, O_RDONLY, 0666);
                //Vai buscar o numero de argumentos
                int num_prog;
                read(fd_fifo_novo, &num_prog, sizeof(int));

                //Percorre os varios argumentos
                int programas_diferentes=0;
                char nome_diferentes[num_prog][7];

                //variavel que testa se o programa ja se encontra na lista
                int encontrado=0;

                while(num_prog>0){
                    read(fd_fifo_novo,&programa,sizeof(char)*5);
                    printf("%s\n",programa);
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
                    printf("%s\n",nome_diferentes[i]);
                    write(fd_fifo_novo,nome_diferentes[i],sizeof(char)*7);
                }
                close(fd_fifo_novo);
            }