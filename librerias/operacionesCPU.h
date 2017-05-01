/*
 * operacionesCPU.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef OPERACIONESCPU_H_
#define OPERACIONESCPU_H_

// Primera aproximación de las funciones que realiza la CPU y sus estructuras de datos.
// Tranquilos, va a haber cambios XD



// Declaraciones


// Estructuras de datos (Cuidado si hay estructuras similares a otros procesos. ¡¡Avisen!!)

// Otra cosa: por ahora dice "int", pero lo mejor es tener cuidado con eso. El tamaño de un "int" depende de la palabra del procesador.
// Lo mejor es siempre usar un "int" con tamaño fijo.
// Lean bien esto: http://www.gnu.org/software/libc/manual/html_node/Integers.html


typedef struct{
	int pag; // Página donde está almacenado un dato en memoria.
	int offset; // Despleazamiento de un dato respecto a la posición inicial de memoria.
	int size; // Tamaño del dato alojado en memoria.
} posicionMemoria; // Modelo de una posición en memoria.


typedef struct{
	// listaArgumentos argumentos; // Posiciones de memoria donde se almacenan las copias de los argumentos de la función.
	// listaVariables variables; // Identificadores y posiciones de memoria donde se almacenan las variables locales de la función.
	int* retPos; // Posición del índice de código donde se debe retornar al finalizar la ejecución de la función.
	posicionMemoria retVar; // Posición de memoria donde se debe almacenar el resultado de la función provisto por la sentencia RETURN.
} stack; // Datos de una llamada a función.


//PCB
typedef struct {
	int pid; // Identificador de un proceso.
	int pc; // Program counter: indica el número de la siguiente instrucción a ejecutarse.
	int paginasUsadas; // Cantidad de páginas usadas por el programa (Desde 0).
	int indiceCodigo[][]; // Identifica línea útil de código a partir de su posición en el programa para convertirla en una posición de memoria para su consulta.
	// ¿diccionario? indiceEtiqueta; // Tiene el valor del PC que necesita una función para ser llamada.
	stack indiceStack; // Ordena valores almacenados en la pila de funciones con sus valores.
} pcb; // El Process Control Block que maneja el Kernel.



// Funciones de comunicación: las clasificamos así porque requieren de interacción entre el CPU con otros procesos.


// Con Kernel

void recibirPCB(pcb unPcb); // Recibe PCB del Kernel para ejecutar un programa.

void concluirOperacion(); // Notificar al Kernel que se terminó la ejecución de una operación para que la pueda seguir otra CPU de ser necesario.

void finEjecucion(); // Indicar finalización de un proceso.

void conectarNuevaCPU(); // Conectar otra CPU al sistema.

void desconectarCPU(int senial); // ¿Por qué no puedo poner 'ñ'?


// Con Memoria

void solicitarSentencia(posicionMemoria unaPos); // Solicitar sentencia en Memoria.

void obtenerDatos(posicionMemoria unaPos); // Obtiene información de un programa en ejecución.

void actualizarValores(/* Cosas, */ posicionMemoria unaPos); // Actualiza estrtucturas tras una operación.



// Funciones de CPU: las clasificamos así porque son las funciones que componen al CPU para que haga su trabajo.

void interpretarOperacion(); // Recibe una instrucción de un programa y la parsea.

void ejecutarOperacion(); // Ejecuta una instrucción parseada.

void llamarFuncion(/* Stack */); // Llama a una función o procedimiento.

void actualizarPC(int PC, int valor); // Incrementa el Program Counter con la próxima instrucción a ejecutar.

void arrojarExcepcion(/* Excepción */);



#endif /* OPERACIONESCPU_H_ */
