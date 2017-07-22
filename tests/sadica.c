#include "../librerias/fileSystem/sadica.h"
#include <stdio.h>
#include <cspecs/cspec.h>



// Funciones auxiliares



// Declaraciones

void mostrarBitmap();

bool todosLibres(off_t desde, off_t hasta);

bool todosOcupados(off_t desde, off_t hasta);

void ocuparDesdeHasta(off_t desde, off_t hasta);

void desocuparDesdeHasta(off_t desde, off_t hasta);

void ocuparTodo();

void desocuparTodo();

bool plantillaTesteo(off_t desde, off_t hasta, bool condicion);

void plantillaOcupacion(off_t desde, off_t hasta, void(*asignador)(off_t));


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

bool todosLibres(off_t desde, off_t hasta){
	plantillaTesteo(desde, hasta, false);
}

bool todosOcupados(off_t desde, off_t hasta){
	plantillaTesteo(desde, hasta, true);
}

void ocuparDesdeHasta(off_t desde, off_t hasta){
	plantillaOcupacion(desde, hasta, &ocuparBloque);
}

void desocuparDesdeHasta(off_t desde, off_t hasta){
	plantillaOcupacion(desde, hasta, &desocuparBloque);
}

void ocuparTodo(){
	ocuparDesdeHasta(0, bloquesEnBytes() * 8);
}

void desocuparTodo(){
	desocuparDesdeHasta(0, bloquesEnBytes() * 8);
}

bool plantillaTesteo(off_t desde, off_t hasta, bool condicion){
	bool verificador = condicion;
	t_bitarray* bitmap = obtenerBitmap();

	off_t i = desde;

	while(i < hasta && verificador == condicion)
	{
		verificador = bitarray_test_bit(bitmap, i);
		i++;
	}

	return verificador;
}

void plantillaOcupacion(off_t desde, off_t hasta, void(*asignador)(off_t)){
	t_bitarray* bitmap = obtenerBitmap();

	off_t i = desde;

	while(i < hasta)
	{
		asignador(i);
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
			verificador = todosLibres(0, bloquesEnBytes() * 8);

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
    		ocuparDesdeHasta(0, 1000);

    		verificador = todosOcupados(0, 1000);

    		should_bool(verificador) be truthy;
    	} end

		it("Se pueden desocupar los bloques entre 20 y 50"){
    		desocuparDesdeHasta(19, 50);

    		verificador = todosLibres(19, 50);

    		should_bool(verificador) be falsey;
    	} end

		it("Se puede setear todos los bloques ocupados, cerrar el bitmap, abrirlo, y que sigan los bloques llenos"){
    		ocuparTodo();

    		cerrarBitmap();

    		abrirBitmap();

    		verificador = todosOcupados(0, bloquesEnBytes() * 8);

    		should_bool(verificador) be truthy;
    	} end

    } end

	describe("Inicio del directorio de Archivos."){
    		desocuparTodo();
    } end

	cerrarBitmap();

	cerrarDirectorioRaiz();
}

