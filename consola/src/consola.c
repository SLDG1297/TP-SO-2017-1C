/*
 ============================================================================
 Name        : consola.c
 Author      : Zero Gravity
 Version     : 0.01
 Description : Consola envia mensaje y se conecta con el kernel
 ============================================================================
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


#define MAX 1024
#define DIRECCION_IP "127.0.0.1"
int main()

{

        int sock, bytes_recibidos;
        char datos_enviados[MAX],datos_recibidos[MAX];
        struct hostent *host;
        struct sockaddr_in server_addr;


        host = gethostbyname(DIRECCION_IP);

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(5000);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8);

        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1)
        {
            perror("Conectado");
            exit(1);
        }

        while(1)
        {

          bytes_recibidos=recv(sock,datos_recibidos,1024,0);
          datos_recibidos[bytes_recibidos] = '\0';

          if (strcmp(datos_recibidos , "q") == 0 || strcmp(datos_recibidos , "Q") == 0)
          {
           close(sock);
           break;
          }

          else
           printf("\nDatos Recibidos = %s " , datos_recibidos);

           printf("\nENVIAR (q to Quit) : ");
           gets(datos_enviados);

          if (strcmp(datos_enviados , "q") != 0)
           send(sock,datos_enviados,strlen(datos_enviados), 0);

          else
          {
           send(sock,datos_enviados,strlen(datos_enviados), 0);
           close(sock);
           break;
          }

        }
return 0;
}
