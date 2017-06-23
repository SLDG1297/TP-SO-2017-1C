##QUE ÉS UN SCRIPT

Un script básicamente es un .txt que ejecuta las líneas de comandos que hay escritas dentro del mismo.

Es re útil porque a veces se torna re denso volver a escribir las mismas líneas de comandos para ejecutar un comando.


##ARMAR UN SCRIPT

1) Primero crean un .txt.

2) La primer línea tiene que ser: #!/bin/bash
(Esto es para que reconozca que todas las líneas que siguen a continuación son de comandos).

3) Después, en el mismo escriben las líneas de comando que quieren que ocurran.

Con eso ya tienen armado su script, lo tienen que habilitar para ejecutarlo como tal.


##HABILITAR SCRIPT

Desde la terminal, se ubican en el directorio de su script, y ejecutan la línea: chmod +x <nombreDelScript>
Con esa línea le dan permiso (chmod = change mode) de ejecución (+x) al script para que ejecute las líneas de comandos dentro de él.


##UTILIZAR UN SCRIPT

Lo único que tienen que hacer es ejecutarlo como lo harían normalmente: ./<nombreDelScript>