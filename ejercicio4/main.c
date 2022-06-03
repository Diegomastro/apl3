#include <stdio.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>

int main() {
    size_t len = 1024;
    int shmid = 0;
    char* addr = NULL;
    struct shmid_ds shmbuf; // para shmctl
    key_t key = ftok("/dev/shm/shm-test", 'x');

    shmid = shmget(key, len, IPC_CREAT);

    printf("shmid = %d\n", shmid);

    if (shmid < 0) {
        puts("Error con shmid");
        return 1;
    }

    addr = shmat(shmid, NULL, 0);

    if ((int)addr == -1) {
        puts("Addr es -1, error");
        return -2;
    }

    strcpy(addr, "string escrito en memoria compartida");
    printf("addr = %d\n", (int)addr);

    while(1);
    return 0;
}
