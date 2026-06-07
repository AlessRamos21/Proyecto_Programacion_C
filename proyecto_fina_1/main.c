#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "archivos.h"
#include "hotel.h" //prototipos de la logica

int main(int argc, char *argv[])
{
    int cantidad_habitaciones = 0;
    struct habitacion *hotel = NULL;
    int opcion = -1;
    char ultimo_reporte[64] = "";

    printf("\n  ==========================================\n");
    printf("       SISTEMA DE GESTION HOTELERA\n");
    printf("  ==========================================\n\n");


    cantidad_habitaciones = contar_habitaciones_guardadas();

    if (cantidad_habitaciones > 0) {
        printf("[SISTEMA]: Base de datos encontrada con %d habitaciones.\n", cantidad_habitaciones);
        // Reservamos memoria para el hotel existente
        hotel = (struct habitacion *)malloc(sizeof(struct habitacion) * cantidad_habitaciones);
        cargar_base_datos(hotel, cantidad_habitaciones);
    } else {
        printf("[SISTEMA]: No se encontraron datos previos.\n");
        cantidad_habitaciones = pedir_cantidad_habitaciones();

        //Reservar memoria segun las habitaciones que pongamos
        hotel = (struct habitacion *)malloc(sizeof(struct habitacion) * cantidad_habitaciones);

        inicializar_hotel(hotel, cantidad_habitaciones);
    }
    //menu
    do {
        mostrar_menu();
        if (!leer_entero("Opcion: ", &opcion)) {
            opcion = -1;
        }
        system("cls");

        switch (opcion) {
            case 1: listar_habitaciones(hotel, cantidad_habitaciones); break;
            case 2: hacer_checkin(hotel, cantidad_habitaciones); break;
            case 3: hacer_checkout(hotel, cantidad_habitaciones); break;
            case 4: buscar_huesped_en_menu(hotel, cantidad_habitaciones); break;
            case 5: cambiar_limpieza_menu(hotel, cantidad_habitaciones); break;
            case 6: generar_reporte(hotel, cantidad_habitaciones, ultimo_reporte, sizeof(ultimo_reporte)); break;
            case 7: buscar_huesped_por_documento(hotel, cantidad_habitaciones); break;
            case 8: modificar_huesped(hotel, cantidad_habitaciones); break;
            case 9: ver_ultimo_reporte(ultimo_reporte); break;
            case 0:
                guardar_base_datos(hotel, cantidad_habitaciones);
                printf("Guardando datos y cerrando el sistema...\n");
                break;
            default:
                printf("Opcion invalida. Intente de nuevo.\n");
                break;
        }

        if (opcion != 0) {
            pausa();
        }
    } while (opcion != 0);

    liberar_hotel(hotel, cantidad_habitaciones);
    free(hotel);
    hotel = NULL; // Anulamos el puntero por seguridad

    return 0;
}

