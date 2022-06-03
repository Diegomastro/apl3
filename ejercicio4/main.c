#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>

int main() {
    // execute as "sudo ./main if you are using /something"
    size_t len = 1024;
    int shmid = 0;
    char* addr = NULL;
    struct shmid_ds shmbuf; // para shmctl
    key_t key = ftok("/home/marco/sisop-apl/apl3/ejercicio4/test", 'x');
    printf("Key = %d\n", key);
    shmid = shmget(key, len, IPC_CREAT);

    printf("shmid = %d\n", shmid);

    if (shmid < 0) {
        puts("Error con shmid");
        return 1;
    }

    addr = shmat(shmid, NULL, 0);

    if ((int)addr == -1) {
        printf("%d\n", key);
        puts("Addr == -1");
        return -2;
    }

    printf("addr = %p\n", addr);
    strcpy(addr, "string escrito en memoria compartida");
    
    while(1);
    return 0;
}
