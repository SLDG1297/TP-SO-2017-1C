#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>

// Hay toda una teoría atrás de los términos que pongo. Algo de BDD, no sé que corno. Así que algunas cosas son medio raras XD

context (example) {	// Indica el comportamiento de los tests con un identificador.

    describe("Hello world") {	// Un posible escenario de los tests con su descripción.

    	// Cada "it" es un test. Los "should" vendrían a ser los asserts.
        it("true should be true") {
            should_bool(true) be equal to(true);
        } end

        it("true shouldn't be false") {
            should_bool(true) not be equal to(false);
        } end

        it("this test will fail because 10 is not equal to 11") {
            should_int(10) be equal to(11);
        } end

        skip("this test will fail because \"Hello\" is not \"Bye\"") {
            should_string("Hello") be equal to("Bye");
        } end

    } end

}
