#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

int main() {
    size_t size = 1024;
    int shmid;
    char* puntero_shm = NULL;
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/test", 'x');
    printf("%d\n", key);
    printf("IPC_CREAT = %d\n", IPC_CREAT);
    shmid = shmget(key, size, 0644 | IPC_CREAT);
    printf("%d\n", shmid);
    puntero_shm = shmat(shmid, NULL, 0);
    printf("%s", puntero_shm);
    puts(puntero_shm);
    return 0;
}
