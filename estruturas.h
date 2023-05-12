#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <unistd.h>
#include <sys/time.h>

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

#endif