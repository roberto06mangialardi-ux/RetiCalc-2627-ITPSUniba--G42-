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
#define PORT 8080

int main (void){
#ifdef _WIN32
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		printf("Errore nell'inizializzazione di Winsock\n");
		return 1;
	}
#endif
	SOCKET server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	int client_addr_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE];

	//creazine socker server
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET){
		printf("Errore nella creazione della socket del server\n");
#ifdef _WIN32
		WSACleanup();
#endif
		return 1;
	}
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr =INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR){
    	printf("Errore durante il bind\n");
    	closesocket(server_socket);
#ifdef _WIN32
    	WSACleanup();
#endif
    	return 1;
    }

    //messa in ascolto
    if(listen(server_socket, 3) == SOCKET_ERROR){
    	printf("Errore durante l'ascolto\n");
    	closesocket(server_socket);
#ifdef _WIN32
    	WSACleanup();
#endif
    	return 1;
    }
    printf("Server in ascolto sulla porta %d...\n", PORT);
    while (1){
    //accettazione di una connessione in arrivo dai client
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == INVALID_SOCKET){
    	printf("Errore nell'accettazione\n");
    	continue;
    }
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    printf("Connesso con il client il suo nome IP: %s\n",client_ip);
    char *welcome_msg = "connessione avvenuta";
    send(client_socket, welcome_msg, strlen(welcome_msg), 0);
    while (1){
    	int n[3];
    	int bytes_received = recv(client_socket, (char*)n, sizeof(n), 0);
    	if (bytes_received <= 0){
    		printf("Client disconnesso.\n");
    		break;
    	}
    	int n1 = n[0];
    	int n2 = n[1];
    	int n3 = n[2];

    	int res = n1 + n2 - n3;
    	printf("La somma di %d+%d-%d=%d\n", n1, n2,n3, res);

    	if(res == 100){
    		send(client_socket, "QUIT", 4,0);
    		printf("Risultato 100 raggiungo. Invio QUIT e chiusura connessione con il client.\n");
    		break;
    	}else{
    		char res_str[32];
    		sprintf(res_str, "%d", res);
    		send(client_socket, res_str, strlen(res_str), 0);
    	}
    }
    closesocket(client_socket);
    printf("Connessione chiusa. il server rimane in ascolto...\n");
    }

    //chiusura finale della socket
    closesocket(client_socket);
    closesocket(server_socket);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
