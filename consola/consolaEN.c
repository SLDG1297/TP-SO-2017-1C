
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define puerto 5000

int main(int argc, char* argv[])

{
        int sock, bytesRecibidos;
        char datosEnviar[1024],datosRecibir[1024];
        memset (datosEnviar,'\0',1024);
        memset (datosRecibir,'\0',1024);
        struct hostent *host;
        struct sockaddr_in kernel_dir;

        host = gethostbyname(argv[1]);

        //Se crea el socket para conectarse con el kernel
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("Socket");
            exit(1);
        }

        //Direccion del servidor(kernel)
        kernel_dir.sin_family = AF_INET;
        kernel_dir.sin_port = htons(puerto);
        kernel_dir.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(kernel_dir.sin_zero),8);

        //Conexion al kernel
        if (connect(sock, (struct sockaddr *)&kernel_dir,sizeof(struct sockaddr)) == -1)
        {
            perror("Connect");
            exit(1);
        }

        //El cliente espera un mensaje de parte del kernel (handshake)
        bytesRecibidos=recv(sock,datosRecibir,1024,0);
        datosRecibir[bytesRecibidos] = '\0';

        while(1)
        {
           printf("\nDatos a enviar: ");
           gets(datosEnviar);
           send(sock,datosEnviar,strlen(datosEnviar), 0);

               memset (datosEnviar,'\0',1024);
        }
        return 0;
}
