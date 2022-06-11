#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>
#include <stdlib.h>

char* crearMemoriaJugadores() {
    size_t len = 1;
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/cant_jugadores", 'A');
    
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    printf("%d\n", shmid);
    printf("%p\n", addr);
    if ((int)addr != -1) {
        *addr = 0;
        return NULL;
    }

    return addr;
}

int main() {
    char* addr = crearMemoriaJugadores(); 
    // char* addr = (char*)malloc(sizeof(char));
    int numJugadores;
    printf("Ingrese el numero de jugadores\n");
    scanf("%d", &numJugadores);
    printf("esperando a que se conecten todos los jugadores");
    int estanTodos = 0;

    while (estanTodos != numJugadores) {
        char nuevosJugadores = *addr;
        if (numJugadores != nuevosJugadores) {
            ++numJugadores;
        }
    }
    printf("fin del programa");
    return 0;
}
