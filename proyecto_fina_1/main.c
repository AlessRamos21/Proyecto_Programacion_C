#include <stdlib.h>
#include <stdio.h>

// Definición de enumeraciones para mejorar la legibilidad
enum Estado { LIBRE = 0, OCUPADO = 1 };
enum Limpieza { LIMPIA = 0, SUCIA = 1 };
enum TipoCama { TWIN = 0, KING = 1 };

// Estructura de bits para optimizar memoria
struct hab_bits {
    unsigned short estado : 1;      // 1 bit para estado
    unsigned short limpieza : 1;    // 1 bit para limpieza
    unsigned short tipo_de_cama : 1; // 1 bit para tipo de cama
};

// Unión que solapa los bits con el id_huesped
union habitacion {
    struct hab_bits bits;
    unsigned short id_huesped;
};

// Función de checkout usando punteros (como pide la cátedra)
void procesar_checkout(union habitacion *ptr) {
    if (ptr != NULL) {
        ptr->bits.estado = LIBRE;
        ptr->bits.limpieza = SUCIA;
        // El tipo de cama suele mantenerse igual según la habitación
    }
}

int main() {
    union habitacion h1;
    union habitacion *ptr_h1 = &h1; // Puntero a la habitación

    // Inicializamos la habitación como ocupada y limpia para la prueba
    h1.bits.estado = OCUPADO;
    h1.bits.limpieza = LIMPIA;
    h1.bits.tipo_de_cama = KING;

    printf("--- Antes del Checkout ---\n");
    printf("Estado: %d, Limpieza: %d", h1.bits.estado, h1.bits.limpieza);

    // Llamamos a la función
    procesar_checkout(ptr_h1);

    // Las líneas que agregaste al final para verificar los cambios
    printf("--- Después del Checkout ---\n");
    printf("Estado: %d\n, %d\n", h1.bits.estado, h1.bits.limpieza);

    return 0;
}
