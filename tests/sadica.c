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
		registroMetadata resultado = obtenerMetadata();

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
		iniciarBitmapFD(puntoMontaje);
		iniciarBitmap(puntoMontaje);
		abrirBitmap(puntoMontaje);
		leerBitmap();

		t_bitarray* bitmap = obtenerBitmap();

		it("Se puede acceder al bitmap de Bitmap.bin con todos los bloques libres"){
    		bool verificador = false;
    		off_t i = 0;

    		while(i < bloquesEnBytes() * 8 && verificador == false)
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}

    		should_bool(verificador) be falsey;
    	} end

		cerrarBitmap();

    } end
}

