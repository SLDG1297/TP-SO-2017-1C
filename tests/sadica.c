#include "../librerias/fileSystem/sadica.h"
#include <stdio.h>
#include <cspecs/cspec.h>



// Estructuras de datos

char* puntoMontaje	= "pruebaFS";



context (SADICA) {
	describe("Inicio de Metadata.bin") {
		iniciarDirectorios(puntoMontaje);

		iniciarMetadata(puntoMontaje);

		registroMetadata prueba  = creadorMetadata(64, 5192, "SADICA");
		registroMetadata resultado = devolverMetadata();

    	it("Se puede acceder al tamaño de bloques de Metadata.bin"){
    		should_int(resultado.tamanioBloques) be equal to (prueba.tamanioBloques);
    	} end

		it("Se puede acceder a la cantidad de bloques de Metadata.bin"){
    		should_int(resultado.cantidadBloques) be equal to (prueba.cantidadBloques);
    	} end

		it("Se puede acceder al número mágico de Metadata.bin"){
    		should_string(resultado.numeroMagico) be equal to (prueba.numeroMagico);
    	} end

    } end

	describe("Inicio de Bitmap.bin") {
		it("Se puede acceder al bitmap de Bitmap.bin con todos los bloques libres (Deprecado)"){
	    	FILE* archivoBitmap = iniciarBitmap(puntoMontaje);
	    	t_bitarray* bitmap = leerBitmap(archivoBitmap);

    		bool verificador = false;
    		off_t i = 0;

    		while(i < bloquesEnBytes() && verificador == false)
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}

    		cerrarBitmap(archivoBitmap, bitmap);

    		should_bool(verificador) be falsey;
    	} end

		it("Se puede acceder al bitmap de Bitmap.bin con todos los bloques libres"){
    		//iniciarBitmapMap(puntoMontaje);
    	} end

    } end
}

