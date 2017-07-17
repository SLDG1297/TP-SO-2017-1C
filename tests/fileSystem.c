#include "../librerias/fileSystem/funcionesFileSystem.h"
#include <stdio.h>
#include <cspecs/cspec.h>

context (fileSystem) {

    describe("Validación de archivos") {
        it("El archivo 'crearPCBCodigo.txt' debería existir."){
        	should_bool(validar("crearPCBCodigo.txt")) be truthy;
        } end

		it("El archivo 'noExiste.txt' no debería existir."){
		    should_bool(validar("noExiste.txt")) be falsey;
		} end

    } end
}
