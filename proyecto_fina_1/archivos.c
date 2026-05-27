#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PASAPORTE 16
#define DNI 1

/* --- ESTRUCTURAS COMPARTIDAS CON MAIN.C --- */
struct estado_habitacion {
    unsigned int ocupada : 1;
    unsigned int limpieza : 1;
};

union documento_identidad {
    long int dni;
    char pasaporte[MAX_PASAPORTE];
};

struct huesped {
    char *nombre;
    int tipo_documento;
    union documento_identidad documento;
};

struct habitacion {
    int numero;
    int capacidad;
    float precio;
    struct estado_habitacion estado;
    struct huesped huesped;
};

/* Prototipo de la funcion de busqueda (Su implementacion real vive en main.c) */
int buscar_habitacion_por_numero(const struct habitacion *hotel, int cantidad, int numero);


/* --- IMPLEMENTACION DE LAS FUNCIONES DE PERSISTENCIA --- */

int contar_habitaciones_guardadas(void)
{
    FILE *archivo = fopen("base_datos_hotel.txt", "r");
    char linea[256];
    int contador = 0;

    if (archivo == NULL) {
        // Si el archivo no existe, retornamos 0 para indicar un inicio limpio
        return 0;
    }

    // Contamos cuantas lineas validas (habitaciones) tiene el archivo guardado
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (linea[0] != '\n' && linea[0] != '\0') {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

void guardar_base_datos(const struct habitacion *hotel, int cantidad)
{
    FILE *archivo = fopen("base_datos_hotel.txt", "w");
    int i = 0;

    if (archivo == NULL) {
        printf("[ERROR]: No se pudo escribir la base de datos fisica.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        fprintf(archivo, "%d;%d;%.2f;%u;%u;%d;%ld;%s;%s\n",
                hotel[i].numero,
                hotel[i].capacidad,
                hotel[i].precio,
                hotel[i].estado.ocupada,
                hotel[i].estado.limpieza,
                hotel[i].huesped.tipo_documento,
                hotel[i].huesped.documento.dni,
                hotel[i].huesped.documento.pasaporte[0] != '\0' ? hotel[i].huesped.documento.pasaporte : "-",
                hotel[i].huesped.nombre != NULL ? hotel[i].huesped.nombre : "-");
    }

    fclose(archivo);
    printf("[SISTEMA]: Base de datos guardada correctamente en 'base_datos_hotel.txt'.\n");
}

void cargar_base_datos(struct habitacion *hotel, int cantidad)
{
    FILE *archivo = fopen("base_datos_hotel.txt", "r");
    char linea[256];

    if (archivo == NULL) {
        printf("[SISTEMA]: No se detecto archivo previo. Iniciando base de datos limpia.\n");
        return;
    }

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        int num = 0, cap = 0, ocup = 0, limp = 0, tipo_doc = 0;
        float prec = 0.0f;
        long int temp_dni = 0L;
        char temp_pas[64] = "";
        char temp_nom[128] = "";

        int leidos = sscanf(linea, "%d;%d;%f;%d;%d;%d;%ld;%[^;];%[^\n]",
                            &num, &cap, &prec, &ocup, &limp, &tipo_doc, &temp_dni, temp_pas, temp_nom);

        if (leidos == 9) {
            int pos = buscar_habitacion_por_numero(hotel, cantidad, num);
            if (pos != -1) {
                hotel[pos].capacidad = cap;
                hotel[pos].precio = prec;
                hotel[pos].estado.ocupada = (unsigned int)ocup;
                hotel[pos].estado.limpieza = (unsigned int)limp;
                hotel[pos].huesped.tipo_documento = tipo_doc;

                if (tipo_doc == DNI) {
                    hotel[pos].huesped.documento.dni = temp_dni;
                    hotel[pos].huesped.documento.pasaporte[0] = '\0';
                } else {
                    if (strcmp(temp_pas, "-") != 0) {
                        strncpy(hotel[pos].huesped.documento.pasaporte, temp_pas, MAX_PASAPORTE - 1);
                        hotel[pos].huesped.documento.pasaporte[MAX_PASAPORTE - 1] = '\0';
                    } else {
                        hotel[pos].huesped.documento.pasaporte[0] = '\0';
                    }
                }

                free(hotel[pos].huesped.nombre);
                hotel[pos].huesped.nombre = NULL;

                if (strcmp(temp_nom, "-") != 0) {
                    hotel[pos].huesped.nombre = (char *)malloc(strlen(temp_nom) + 1);
                    if (hotel[pos].huesped.nombre != NULL) {
                        strcpy(hotel[pos].huesped.nombre, temp_nom);
                    }
                }
            }
        }
    }

    fclose(archivo);
    printf("[SISTEMA]: Historial y estados de habitaciones sincronizados con exito.\n");
}
