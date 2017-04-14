/*
============================================================================
 Name        : kernel.c
 Author      : Zero Gravity
 Version     : 0.01
 Description : Kernel recibe un mesaje
 ============================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MIPUERTO 5000
#define MAX 1024


int main()
{
        int sock, connected, bytes_recibidos , true = 1;
        char datos_enviados [MAX] , datos_recibidos[MAX];

        struct sockaddr_in server_addr,client_addr;
        int sin_size;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(MIPUERTO);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(server_addr.sin_zero),8);

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
                                                                       == -1) {
            perror("Error en la funcion BIND");
            exit(1);
        }

        if (listen(sock, 5) == -1) {
            perror("Er");
            exit(1);
        }

	printf("\nTCPServer esperando al cliente en el puerto %d", MIPUERTO);
        fflush(stdout);


        while(1)
        {

            sin_size = sizeof(struct sockaddr_in);

            connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

            printf("\n Tengo una conexion de (%s , %d)",
                   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

            while (1)
            {
              printf("\n SEND (q or Q to quit) : ");
              gets(datos_enviados);

              if (strcmp(datos_enviados , "q") == 0 || strcmp(datos_enviados , "Q") == 0)
              {
                send(connected, datos_enviados,strlen(datos_enviados), 0);
                close(connected);
                break;
              }

              else
                 send(connected, datos_enviados,strlen(datos_enviados), 0);

              bytes_recibidos = recv(connected,datos_recibidos,1024,0);

              datos_recibidos[bytes_recibidos] = '\0';

              if (strcmp(datos_recibidos , "q") == 0)
              {
                close(connected);
                break;
              }

              else
              printf("\n DATOS RECIBIDOS = %s " , datos_recibidos);
              fflush(stdout);
            }
        }

      close(sock);
      return 0;
}

