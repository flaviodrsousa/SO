all: folders server client

server: monitor
client: target
folders:
	@mkdir -p obj PIDS-folder

monitor: obj/servidor.o
	@gcc -g obj/servidor.o -o monitor
obj/servidor.o: src/servidor.c
	@gcc -g -c src/servidor.c -o obj/servidor.o -I include

target: obj/cliente.o
	@gcc -g obj/cliente.o -o target
obj/cliente.o: src/cliente.c
	@gcc -g -c src/cliente.c -o obj/cliente.o -I include

clean:
	@rm -f obj/* PIDS-folder/* 
	@rm -rf obj PIDS-folder
	@rm monitor target fifo *.txt 
