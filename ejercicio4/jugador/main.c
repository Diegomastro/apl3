#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 

int main() {
    size_t size = 1024;
    int shmid;
    char* puntero_shm = NULL;
    key_t key = ftok("/dev/shm/shm-mamo", 'x');

    printf("IPC_CREAT = %d\n", IPC_CREAT);
    shmid = shmget(key, size, 0);

    puntero_shm = shmat(shmid, NULL, 0);

    puts(puntero_shm);
    return 0;
}
