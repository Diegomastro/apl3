#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define BUFFER_LEN 1024

int yaGano(char* cadena);
int murio(int vidas);

int main(int argc, char const* argv[]) {
    int partidaTerminada = 0;
    int server = 0, valread, client;
    struct sockaddr_in serv_addr;
    int resultadoTurno;
    char buffer[BUFFER_LEN] = { 0 };
    int vidas = 6;
    int val;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 0;
    setsockopt(
      server,     // Socket descriptor
      SOL_SOCKET, // To manipulate options at the sockets API level
      SO_RCVTIMEO,// Specify the receiving or sending timeouts 
      (const void *)(&t), // option values
      sizeof(t) 
    );

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    bind(server, (const  struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if ((client
         = connect(server, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(!murio(vidas)) {
        system("clear");
        read(server, buffer, BUFFER_LEN);
        if (yaGano(buffer)) {
            break;
        }
        puts("Es tu turno! tu palabra es:");
        puts("ingrese un caracter");
        puts(buffer);
        fflush(stdout);
        char intento;
        scanf(" %c", &intento);
        getchar();
        val = send(server, &intento, sizeof(char), 0);
        printf("%d\n", val);

        read(server, &resultadoTurno, sizeof(int));
        if (resultadoTurno == 0) { //if fallo
            vidas--;
            puts("Error: la letra no se encuentra en la palabra");
            fflush(stdout);
        } else { // if acierto
            puts("correcto! tu letra figura en la palabra");
            fflush(stdout);
        }
    }

    read(server, buffer, BUFFER_LEN);
    puts(buffer);
    


    close(client);
    return 0;
}


int murio(int vidas) {
    return (vidas <= 0);
}


int yaGano(char* cadena) {
    while (*cadena) {
        if (*cadena == '*') {
            return 0;
        }
        ++cadena;
    }

    return 1;
}