#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum Estado { LIBRE = 0, OCUPADO = 1 };
enum Limpieza { LIMPIA = 0, SUCIA = 1 };

struct hab_bits {
    unsigned short estado : 1;
    unsigned short limpieza : 1;
    unsigned short tipo_de_cama : 1;
};

struct habitacion {
    int numero_hab;
    char categoria[20];
    int capacidad;
    struct hab_bits bits;
    unsigned short id_huesped;
};

void procesar_checkout(struct habitacion *ptr) {
    if (ptr != NULL) {
        ptr->bits.estado = LIBRE;
        ptr->bits.limpieza = SUCIA;
        ptr->id_huesped = 0;
    }
}

int main() {
    struct habitacion hotel[5];
    int opcion = 0, i, num_aux;
    char filtro[20];

    // Inicializacion (Usamos strcpy porque es necesario para strings)
    for(i = 0; i < 5; i++) {
        hotel[i].numero_hab = 101 + i;
        hotel[i].bits.estado = LIBRE;
        hotel[i].bits.limpieza = LIMPIA;
        hotel[i].id_huesped = 0;

        if(i < 3) {
            strcpy(hotel[i].categoria, "Estandar");
            hotel[i].capacidad = 2;
        } else {
            strcpy(hotel[i].categoria, "Suite");
            hotel[i].capacidad = 4;
        }
    }

    do {
        system("cls");
        printf("====================================================\n");
        printf("          SISTEMA HOTELERO - FRONT DESK\n");
        printf("====================================================\n\n");

        printf("%-5s %-12s %-12s %-12s %-5s\n", "NUM", "CAT.", "ESTADO", "LIMPIEZA", "CAP.");
        for(i = 0; i < 5; i++) {
            printf("%-5d %-12s %-12s %-12s %-5d\n",
                hotel[i].numero_hab,
                hotel[i].categoria,
                (hotel[i].bits.estado == OCUPADO ? "OCUPADA" : "LIBRE"),
                (hotel[i].bits.limpieza == LIMPIA ? "LIMPIA" : "SUCIA"),
                hotel[i].capacidad);
        }

        printf("\n1. Procesar Check-Out (Punteros)");
        printf("\n2. Buscar por Categoria (strcmp)");
        printf("\n3. Asignacion Automatica (Logica de Negocio)");
        printf("\n4. Salir");
        printf("\n\nSeleccione opcion: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                printf("\nIngrese numero de habitacion: ");
                scanf("%d", &num_aux);
                for(i = 0; i < 5; i++) {
                    if(hotel[i].numero_hab == num_aux) {
                        procesar_checkout(&hotel[i]); // Pasaje por referencia
                        printf("\n[OK] Check-out realizado con exito.\n");
                    }
                }
                printf("\nPresione ENTER para continuar...");
                fflush(stdin);
                getchar();
                getchar();
                break;

            case 2:
                printf("\nCategoria a buscar (Estandar/Suite): ");
                scanf("%s", filtro);
                printf("\n--- Resultados ---\n");
                for(i = 0; i < 5; i++) {
                    // strcmp es necesario para comparar contenido de strings
                    if(strcmp(hotel[i].categoria, filtro) == 0) {
                        printf("Habitacion %d - Capacidad: %d\n", hotel[i].numero_hab, hotel[i].capacidad);
                    }
                }
                printf("\nPresione ENTER para continuar...");
                fflush(stdin);
                getchar();
                getchar();
                break;

            case 3:
                {
                    int personas;
                    printf("\n¿Cuantos huespedes llegan?: ");
                    scanf("%d", &personas);
                    printf("\n--- Sugerencia Automatizada ---\n");

                    int falta_ubicar = personas;
                    for(i = 0; i < 5 && falta_ubicar > 0; i++) {
                        if(hotel[i].bits.estado == LIBRE && hotel[i].bits.limpieza == LIMPIA) {
                            printf("-> Asignar a Hab %d (%d lugares)\n", hotel[i].numero_hab, hotel[i].capacidad);
                            falta_ubicar -= hotel[i].capacidad;
                            hotel[i].bits.estado = OCUPADO; // La marcamos para el ejemplo
                        }
                    }
                    if(falta_ubicar > 0) printf("Aviso: Faltan %d personas por ubicar.\n", falta_ubicar);
                }
                printf("\nPresione ENTER para continuar...");
                fflush(stdin); getchar(); getchar();
                break;

            case 4:
                printf("\nCerrando sesion...\n");
                break;

            default:
                printf("\nOpcion no valida.\n");
                fflush(stdin); getchar(); getchar();
        }

    } while(opcion != 4);

    return 0;
}
