#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>
#include <stdlib.h>

char* crearMemoriaJugadores() {
    size_t len = sizeof(char);
    int shmid = 0;
    char* addr = NULL;
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/cant_jugadores", 'A');
    
    shmid = shmget(key, len, IPC_CREAT);
    addr = shmat(shmid, NULL, 0);
    printf("%d\n", shmid);
    printf("%p\n", addr);
    char x = 0;
    memcpy(addr, &x, sizeof(char)); 
    return addr;
}

int main() {
    char* addr = crearMemoriaJugadores(); 
    int numJugadores;
    printf("%d", *addr);
    printf("Ingrese el numero de jugadores\n");
    scanf("%d", &numJugadores);
    char estanTodos = 0;
    puts("esperando que se conecten todos los jugadores");
    while (numJugadores != *addr);
    printf("fin del programa");
    return 0;
}
