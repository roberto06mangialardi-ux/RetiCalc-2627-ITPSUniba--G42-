#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#define BUFFER_SIZE 1024

int main(void){
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    char server_ip[50];
    int port;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    //lettura dell'ip e porta
    printf("Inserisci l'indirizzo IP del server: ");
    scanf("%s", server_ip);
    printf("Inserisci il numero di porta: ");
    scanf("%d", &port);

    //creazione socket
    client_socket= socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET){
    	printf("Errore nella creazione della socket\n");
    	return 1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // richiesta di connessione
    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==SOCKET_ERROR){
    	printf("Connession fallita\n");
    	closesocket(client_socket);
        #ifdef _WIN32
    	    WSACleanup();
        #endif
    	return 1;
    }
    //connessione avvenuta
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    printf("Server: %s\n", buffer);

    //Ciclo di invio interi e ricezione
    int n[3];
    while (1){
    	printf("Inserisi tre numeri interi (n1 n2 n3): ");
    	scanf("%d %d %d", &n[0], &n[1], &n[2]);

    	//invio interi al server
    	send(client_socket, (char*)n, sizeof(n), 0);
    	//ricezione
    	memset(buffer, 0,BUFFER_SIZE);
    	recv(client_socket, buffer, BUFFER_SIZE -1, 0);

    	//stringa QUIT chiude
    	if (strcmp(buffer, "QUIT") == 0){
    		printf("Ricevuto QUIT. Chiusura in corso...\n");
    		break;
    	} else{
    		printf("Risultato ricevuto: %s\n", buffer);
    	}
    }
    closesocket(client_socket);
    #ifdef _WIN32
        WSACleanup();
    #endif
    return 0;
}


