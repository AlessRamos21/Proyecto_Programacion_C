#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Definir que significa bit 0/1
enum Estado { LIBRE = 0, OCUPADO = 1 };
enum Limpieza { LIMPIA = 0, SUCIA = 1 };
enum TipoCama { TWIN = 0, KING = 1 };

// Estructura de bits
struct hab_bits {
    unsigned short estado : 1;      // 1 bit para cada cosa
    unsigned short limpieza : 1;
    unsigned short tipo_de_cama : 1;
};

// La union con la estructura de bits junto con el id del huesped
union habitacion {
    struct hab_bits bits;
    unsigned short id_huesped;
};

// Función de checkout
void procesar_checkout(union habitacion *ptr) {
    ptr = NULL;
    if (ptr != NULL) {
        ptr->bits.estado = LIBRE;
        ptr->bits.limpieza = SUCIA;
    }
}

int main() {
    union habitacion h1; // creacion de la habitacion
    union habitacion *ptr_h1 = &h1; // apuntamos a la habitación

    // Se inicia con la habitacion OCUPADA Y LIMPIA (CAMA KING)
    h1.bits.estado = OCUPADO;
    h1.bits.limpieza = LIMPIA;
    h1.bits.tipo_de_cama = KING;

    printf("--- Antes del Checkout ---\n");
    printf("Estado: %d, Limpieza: %d", h1.bits.estado, h1.bits.limpieza);

    // Llamamos a la función para el check out
    procesar_checkout(ptr_h1);

    // Damos un estado de como figuran las habitaciones despues del C.O.
    printf("\n--- Despues del Checkout ---\n");
    printf("Estado: %d\n, %d\n", h1.bits.estado, h1.bits.limpieza);

    return 0;
}
