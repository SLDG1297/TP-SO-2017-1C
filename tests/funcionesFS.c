#include "../librerias/fileSystem/funcionesFileSystem.h"
#include <stdio.h>
#include <cspecs/cspec.h>

#define TAMANIO_METADATA		2 * sizeof(u_int32_t) + strlen("SADICA") + 1



// Estructuras de datos

char* puntoMontaje = "pruebaFS/";
char* pathMetadata = "pruebaFS/Metadata/Metadata.bin";



context (funcionesFileSystem) {

    describe("Validación de archivos") {
        it("El archivo 'crearPCBCodigo.txt' debería existir."){
        	should_bool(validar("crearPCBCodigo.txt")) be truthy;
        } end

		it("El archivo 'noExiste.txt' no debería existir."){
		    should_bool(validar("noExiste.txt")) be falsey;
		} end

    } end

	describe("Acceso a Metadata.bin") {
    	FILE* archivoMetadata;
    	registroMetadata metadata;

    	before{
    		iniciarMetadata(puntoMontaje);

    		archivoMetadata = fopen(pathMetadata, "r");

    		fread(&metadata, TAMANIO_METADATA, 1, archivoMetadata);
    	} end

    	it("Se puede acceder al tamaño de bloques de Metadata.bin"){
    		should_int(metadata.tamanioBloques) be equal to (64);
    	} end

		it("Se puede acceder a la cantidad de bloques de Metadata.bin"){
    		should_int(metadata.cantidadBloques) be equal to (5192);
    	} end

		it("Se puede acceder al número mágico de Metadata.bin"){
    		should_string(metadata.numeroMagico) be equal to ("SADICA");
    	} end

		after{
    		fclose(archivoMetadata);
    	} end

    } end

}
