#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define puerto 5000
#define puertoMemoria 5010
#define puertoFileSystem 5020


int main(int argc, char* argv[]) {

	//Defincion de variables

	fd_set socketsRelevantes; //Lista completa de sockets
	fd_set socketsFiltrados; //Sockets filtrados clientaddrpor el select
	FD_ZERO(&socketsRelevantes);
	FD_ZERO(&socketsFiltrados);
	int fileDescMax; // Numero del descriptor de fichero mas grande
	int sockListener; //Socket para escuchar nuevas conexiones
	int sockMemoria;
	int sockFileSystem;
	int nuevoSocket; //Socket para aceptar
	char buffer[1024]; //Buffer de datos
	memset (buffer,'\0',1024);
	char* handshake= "Kernel conectado";
	int longitudBytesRecibidos;
	struct sockaddr_in kernel_dir, cliente_dir, filesystem_dir,memoria_dir;
	int yes = 1;
	int longitudEstructuraSocket;
	int i, j; //Contadores del for
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 500000;



	//Conexion al proceso Memoria

	sockMemoria = socket (AF_INET,SOCK_STREAM,0);
	if (sockMemoria == -1)
	{
		perror("Fallo en la creacion del socket a memoria");
		exit(1);
	}

	if (setsockopt(sockMemoria, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("Error en el setsockopt del socket memoria");
		exit(1);
	}

    memoria_dir.sin_family = AF_INET;
    memoria_dir.sin_port = htons(puertoMemoria);
    memoria_dir.sin_addr.s_addr = INADDR_ANY;
    bzero(&(memoria_dir.sin_zero), 8);

	puts("Enviando conexion a proceso memoria\n");

    if(connect(sockMemoria, (struct sockaddr *)&memoria_dir, sizeof(struct sockaddr))==-1)
	{
	  perror ("Error al conectarse al proceso memoria");
	  exit (1);
	}

	if (recv(sockMemoria,buffer,sizeof(buffer),0) == -1)
	{
	  perror ("Error en el handshake de memoria (recepcion)");
	  exit(1);
	}

    if (send(sockMemoria,handshake,strlen(handshake),0) == -1)
	{
      perror ("Error en el handshake de memoria (envio)");
	  exit(1);
	}

    puts(buffer);
    FD_SET(sockMemoria, &socketsRelevantes);
    memset (buffer,'\0',1024);


	//Conexion al proceso fileSystem

	sockFileSystem = socket (AF_INET,SOCK_STREAM,0);
	if (sockFileSystem == -1)
	{
		perror("Fallo en la creacion del socket fileSystem");
		exit(1);
	}

	if (setsockopt(sockFileSystem, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("Error en el setsockopt del socket fileSystem");
		exit(1);
	}

    filesystem_dir.sin_family = AF_INET;
    filesystem_dir.sin_port = htons(puertoFileSystem);
    filesystem_dir.sin_addr.s_addr = INADDR_ANY;
    bzero(&(filesystem_dir.sin_zero), 8);

    puts("Enviando conexion a proceso FileSystem\n");

    if(connect(sockFileSystem, (struct sockaddr *)&filesystem_dir, sizeof(struct sockaddr))==-1)
	{
	perror ("Error al conectarse al proceso fileSystem");
	exit (1);
	}

	if (recv (sockFileSystem,buffer,sizeof(buffer),0)==-1)
	{
	perror ("Error en el handshake de fileSystem (recepcion)");
	exit(1);
	}

	if (send(sockFileSystem,handshake,strlen(handshake),0)==-1)
	{
	perror ("Error en el handshake de fileSystem (envio)");
	exit(1);
	}

	puts(buffer);
	FD_SET(sockFileSystem, &socketsRelevantes);
	    memset (buffer,'\0',1024);



	//Creacion del socket escucha y verificacion de error

	if ((sockListener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	if (setsockopt(sockListener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("Error en setsockopt");
			exit(1);
		}

	printf("Setsockopt correcto\n");

	puts("Socket listener creado");
	kernel_dir.sin_family = AF_INET;
	kernel_dir.sin_port = htons(puerto);
	kernel_dir.sin_addr.s_addr = INADDR_ANY;
	bzero(&(kernel_dir.sin_zero), 8);

	//Se asocia el listener al puerto de escucha

	if (bind(sockListener, (struct sockaddr *) &kernel_dir,sizeof(struct sockaddr)) == -1)
	{
		perror("Error en Bind");
		exit(1);
	}

	printf("Bind correcto\n");

	if (listen(sockListener, 10) == -1)
	{
		perror("Error en listen");
		exit(1);
	}

	printf("Listen correcto\n");
	printf("\nEsperando en el puerto %i\n",puerto);

	//Se agregan el listener a los sockets relevantes y se lo asigna como maximo por ser el unico en la lista en este momento
	FD_SET(sockListener, &socketsRelevantes);
	fileDescMax = sockListener;

	while (1)
	{
	memset(buffer,'\0',1024);
	socketsFiltrados = socketsRelevantes;

	if (select(fileDescMax + 1, &socketsFiltrados, NULL, NULL, &tv) == -1)

	{
		perror("Error en select");
		exit(1);
	}



	for (i = 0; i <= fileDescMax; i++)

	{
		if(FD_ISSET(i, &socketsFiltrados))

		{

			if(i == sockListener)

			{
				longitudEstructuraSocket = sizeof(cliente_dir);
				if ((nuevoSocket = accept(sockListener,(struct sockaddr *) &cliente_dir,&longitudEstructuraSocket)) == -1)

				{
					perror("Error en accept");
				}

				else

				{
					printf("Accept correcto\n");
					FD_SET(nuevoSocket, &socketsRelevantes);
					send(nuevoSocket, "Conexion aceptada", sizeof("Conexion aceptada"), 0);

					//Actualizando el maximo descriptor de fichero

					if(nuevoSocket > fileDescMax)

					    {
					      fileDescMax = nuevoSocket;
					    }

					    printf("%s: Nueva conexion de %s en el socket %d\n", argv[0], inet_ntoa(cliente_dir.sin_addr), nuevoSocket);

					    }

					    }
			else

			    {

			    if((longitudBytesRecibidos = recv(i, buffer, sizeof(buffer), 0)) <= 0)
			    {

			    //Error o fin de conexion?

			    if(longitudBytesRecibidos == 0)

			    //Conexion cerrada

			    printf("%s: Socket %d colgado\n", argv[0], i);

                else
                {
			    perror("Error en el recv");
				}

                //Se cierra el socket
			    close(i);
			    FD_CLR(i, &socketsRelevantes);

                }
			    else
			    {

			      puts(buffer);
			     // send(nuevoSocket, buffer, longitudBytesRecibidos, 0);


			      for(j = 0; j <= fileDescMax; j++)
			      {
			    	if(FD_ISSET(j, &socketsRelevantes))
			    	{
			    	   //Reenviar el mensaje a todos menos al listener y al socket que recibio el mensaje

			    	     if(j != sockListener && j != i )

			    	      {
			    	       if(send(j, buffer, longitudBytesRecibidos, 0) == -1)
                           perror("Error en send");
			    	      }

			    	    }
			    	 }
			       }
			     }
			   }
			 }
	}

   return 0;
}
