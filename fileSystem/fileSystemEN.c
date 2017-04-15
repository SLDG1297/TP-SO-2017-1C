#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define puertoFileSystem 5020

int main(int argc, char* argv[]) {

	int sockDeEspera; //Socket para esperar conexion con el kernel
	int longitudDatosEnviados;
	int sockAlKernel;//Socket para entablar conexion con el kernel
	int yes = 1;
	char buffer[1024];
	memset (buffer,'\0',1024);
	char* handshake = "La conexion al proceso fileSystem fue exitosa";
	struct sockaddr_in espera,datosDelKernel;


	sockDeEspera = socket(AF_INET,SOCK_STREAM,0);
	if (sockDeEspera == -1)
	{
		perror("Fallo en la creacion del socket de espera");
		exit(1);
	}

	if (setsockopt(sockDeEspera, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
	perror("Error en setsockopt en el socket de espera");
	exit(1);
	}

	printf("Setsockopt correcto\n");
	puts("Socket de espera creado\n");

	//Direccion para esperar la conexion con el kernel
	espera.sin_family = AF_INET;
	espera.sin_port = htons(puertoFileSystem);
	espera.sin_addr.s_addr = INADDR_ANY;
	bzero(&(espera.sin_zero), 8);

	if (bind(sockDeEspera, (struct sockaddr *) &espera,sizeof(struct sockaddr)) == -1)
	{
		perror("Error en Bind");
		exit(1);
	}

	printf("Bind correcto\n");
	puts("Esperando la conexion del kernel\n");

	if (listen(sockDeEspera, 1) == -1)
	{
		perror("Error en listen");
		exit(1);
	}
    int longitudEstructuraSocket= sizeof (datosDelKernel);
	if ((sockAlKernel = accept(sockDeEspera,(struct sockaddr *) &datosDelKernel,&longitudEstructuraSocket)) == -1)

	{
	 perror("Error en accept");
	}
	close(sockDeEspera);

	if((longitudDatosEnviados = send(sockAlKernel,handshake, strlen(handshake),0)) == -1)
	{
	 perror ("Fallo en el handshake");
	}
	puts ("La conexion al proceso kernel fue exitosa\n");
	puts ("Esperando mensajes\n");

	while(1)
	{
	int bytesRecibidos = recv(sockAlKernel,buffer,sizeof(buffer),0);
	if(bytesRecibidos == -1)
	{
	  perror ("Error en la recepcion");
	}
	if(bytesRecibidos == 0)
	{
	  perror("El kernel se ha desconectado");
	  exit(1);
	}
	printf ("Buffer: %s\n",buffer);
    memset (buffer,'\0',1024);
	}
	return 0;
}