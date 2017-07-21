#include "../librerias/fileSystem/sadica.h"
#include <stdio.h>
#include <cspecs/cspec.h>



// Funciones auxiliares



// Declaraciones

void mostrarBitmap();



// Definiciones

void mostrarBitmap(){
	t_bitarray* bitmap = obtenerBitmap();

	off_t i = 0;

	while(i < bloquesEnBytes() * 8)
	{
		printf("%d", bitarray_test_bit(bitmap, i));
		i++;
	}
}



// Pruebas

context (SADICA) {
	iniciarDirectorioRaiz("pruebaFS");

	iniciarDirectorios();

	iniciarMetadata();

	iniciarBitmap();

	describe("Inicio de Metadata.bin") {
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
		abrirBitmap();

		t_bitarray* bitmap = obtenerBitmap();

		bool verificador;

		before{
			verificador = false;
		} end

		it("Se puede acceder al bitmap de Bitmap.bin con todos los bloques libres"){
    		off_t i = 0;

    		while(i < bloquesEnBytes() * 8 && verificador == false)
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}

    		should_bool(verificador) be falsey;
    	} end

		it("Se puede ocupar el bloque 512 en el bitarray"){
    		ocuparBloque(512);

    		verificador = bitarray_test_bit(bitmap, 512);

    		should_bool(verificador) be truthy;
    	} end

		it("Se puede descoupar el bloque 512 en el bitarray"){
    		desocuparBloque(512);

    		verificador = bitarray_test_bit(bitmap, 512);

    		should_bool(verificador) be falsey;
    	} end

		it("Se pueden ocupar los primeros 1000 bloques"){
    		off_t i = 0;

    		while(i < 1000)
    		{
    			ocuparBloque(i);
    			i++;
    		}

    		i = 0;

    		do
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}
    		while(i < 1000 && verificador == true);

    		should_int(i) be equal to (1000);

    		should_bool(verificador) be truthy;
    	} end

		it("Se pueden desocupar los bloques entre 20 y 50"){
    		off_t i = 19;

    		while(i < 50)
    		{
    			desocuparBloque(i);
    			i++;
    		}

    		i = 19;

    		do
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}
    		while(i < 50 && verificador == false);

    		should_int(i) be equal to (50);

    		should_bool(verificador) be falsey;
    	} end

		it("Se puede setear todos los bloques ocupados, cerrar el bitmap, abrirlo, y que sigan los bloques llenos"){
    		off_t i = 0;

    		while(i < bloquesEnBytes() * 8 && verificador == false)
    		{
    			ocuparBloque(i);
    			i++;
    		}

    		cerrarBitmap();

    		abrirBitmap();

    		bitmap = obtenerBitmap();

    		i = 0;

    		do
    		{
    			verificador = bitarray_test_bit(bitmap, i);
    			i++;
    		}
    		while(i < bloquesEnBytes() * 8 && verificador == true);

    		should_int(i) be equal to (bloquesEnBytes() * 8);

    		should_bool(verificador) be truthy;
    	} end

    } end


	cerrarBitmap();

	cerrarDirectorioRaiz();
}

