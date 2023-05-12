all: folders server client

server: monitor
client: target
folders:
	@mkdir -p src obj PIDS-folder

monitor: obj/servidor.o
	@gcc -g obj/servidor.o -o monitor
obj/servidor.o: servidor.c
	@gcc -g -c servidor.c -o obj/servidor.o -I include

target: obj/cliente.o
	@gcc -g obj/cliente.o -o target
obj/cliente.o: cliente.c
	@gcc -g -c cliente.c -o obj/cliente.o -I include

clean:
	@rm -f obj/* bin/* PIDS-folder/* 
	@rm -rf obj PIDS-folder
	@rm monitor target fifo *.txt 
