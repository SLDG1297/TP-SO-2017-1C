#include "../librerias/fileSystem/sadica.h"
#include <stdio.h>
#include <cspecs/cspec.h>

#define TAMANIO_BLOQUES			64
#define CANTIDAD_BLOQUES		5192
#define CANTIDAD_BLOQUES_BITS	CANTIDAD_BLOQUES / 8
#define TAMANIO_METADATA		2 * sizeof(u_int32_t) + strlen("SADICA") + 1



// Estructuras de datos

char* puntoMontaje	= "pruebaFS";


context (SADICA) {
	describe("Inicio de Metadata.bin") {
		iniciarDirectorios(puntoMontaje);

		registroMetadata prueba  = creadorMetadata(64, 5192, "SADICA");
		registroMetadata resultado;

    	before{
    		iniciarMetadata(puntoMontaje);
    		resultado = devolverMetadata();
    	} end

    	it("Se puede acceder al tamaño de bloques de Metadata.bin"){
    		should_int(resultado.tamanioBloques) be equal to (prueba.tamanioBloques);
    	} end

		it("Se puede acceder a la cantidad de bloques de Metadata.bin"){
    		should_int(resultado.cantidadBloques) be equal to (prueba.cantidadBloques);
    	} end

		it("Se puede acceder al número mágico de Metadata.bin"){
    		should_string(resultado.numeroMagico) be equal to (prueba.numeroMagico);
    	} end

		it("Se pueden cambiar los datos de Metadata.bin"){

    	} end

    } end

	describe("Inicio de Bitmap.bin") {
    	FILE* archivoBitmap;
    	t_bitarray* bitmap;

    	before{
    		archivoBitmap = iniciarBitmap(puntoMontaje);
    		bitmap = leerBitmap(archivoBitmap);
    	} end

		after{
    		cerrarBitmap(archivoBitmap, bitmap);
    	} end

		it("Se puede acceder al bitmap de Bitmap.bin con todos los bloques libres"){
    		bool verificador = false;
    		int i = 0;

    		while(i < CANTIDAD_BLOQUES && verificador == false)
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}

    		should_bool(verificador) be falsey;
    	} end

    } end
}

