#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFFER_LEN 1024
#define CANT_PARAMS 3
#define INDICE_PUERTO 2
#define INDICE_IP 1

int yaGano(char* cadena);
int murio(int vidas);
void checkHelp(int argc, char const* argv[]);
void printHelpAndExit(char const* argv[]);
void checkErroresRead(int valread);

int main(int argc, char const* argv[]) {
    const char* LOCALHOST = "127.0.0.1"; 
    if (argc < CANT_PARAMS) {
        checkHelp(argc, argv); //
        printHelpAndExit(argv);
    }
    unsigned int puerto = atoi(argv[INDICE_PUERTO]);
    const char* ip = argv[INDICE_IP];
    const char* aux = ip;

    if (strcmp(ip, "localhost") == 0) {
        ip = LOCALHOST;
    }

    printf("IP: %s\n", ip);
    printf("Puerto : %d\n", puerto);

    int yagano = 0;
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
    serv_addr.sin_port = htons(puerto);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
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

        valread = read(server, &yagano, sizeof(int));
        checkErroresRead(valread);

        if (yagano) {
            break;
        }

        valread = read(server, buffer, BUFFER_LEN);
        checkErroresRead(valread);

        system("clear");

        puts("Es tu turno! tu palabra es:");
        puts("ingrese un caracter");
        puts(buffer);
        fflush(stdout);
        char intento;
        scanf(" %c", &intento);
        getchar();
        val = send(server, &intento, sizeof(char), 0);
        printf("%d\n", val);

        valread = read(server, &resultadoTurno, sizeof(int));
        checkErroresRead(valread);

        if (resultadoTurno == 0) { //if fallo
            vidas--;
            puts("Error: la letra no se encuentra en la palabra");
            fflush(stdout);
        } else { // if acierto
            puts("correcto! tu letra figura en la palabra");
            fflush(stdout);
        }
    }

    puts("esperando");
    valread = read(server, buffer, BUFFER_LEN);
    checkErroresRead(valread);

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

void printHelpAndExit(char const* argv[]) {
        printf("Cliente del Ahorcado!\n");
        printf("Sintaxis: %s IP PUERTO\n", argv[0]);
        printf("IP: ip del host (puede indicar el string localhost si esta jugando local)\n");
        printf("PUERTO: puerto por el cual se conectara al servidor\n");
        exit(1);
}

void checkHelp(int argc, char const* argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            printHelpAndExit(argv);
        }
    }

    return;
}

void checkErroresRead(int valread) {
    if (valread == -1) {
        puts("Error en la coneccion");
        exit(1);
    }
    if (valread == 0) {
        puts("El server cerro");
        exit(1);
    }
    return;
}