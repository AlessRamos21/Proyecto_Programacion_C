#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "archivos.h"

const char *texto_tipo(int tipo);
const char *texto_estado(unsigned int ocupada);
const char *texto_limpieza(unsigned int limpieza);

#define ARCHIVO_BASE_DATOS "base_datos_hotel.bin"

struct habitacion_bin {
    int numero;
    int tipo;
    int capacidad;
    float precio;
    unsigned int ocupada : 1;
    unsigned int limpieza : 2;
    struct {
        int tipo_documento;
        long int dni;
        char pasaporte[MAX_PASAPORTE];
        char nombre[128];
        char fecha_checkin[20];
    } huespedes[MAX_HUESPEDES_POR_HAB];
    int cantidad_huespedes;
};

static void copiar_texto(char *destino, int capacidad, const char *origen);
static char *duplicar_texto(const char *origen);

int contar_habitaciones_guardadas(void)
{
    FILE *archivo = NULL;
    long tamanio = 0;
    int cantidad = 0;

    archivo = fopen(ARCHIVO_BASE_DATOS, "rb");
    if (archivo == NULL) {
        return 0;
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        fclose(archivo);
        return 0;
    }

    tamanio = ftell(archivo);
    fclose(archivo);

    if (tamanio <= 0) {
        return 0;
    }

    cantidad = (int)(tamanio / (long)sizeof(struct habitacion_bin));
    return cantidad;
}

void guardar_base_datos(const struct habitacion *hotel, int cantidad)
{
    FILE *archivo = NULL;
    int i = 0;
    int j = 0;

    if (hotel == NULL || cantidad <= 0) {
        return;
    }

    //abrimos el archivo para escribir "wb"
    archivo = fopen(ARCHIVO_BASE_DATOS, "wb");
    if (archivo == NULL) {
        printf("[ERROR]: No se pudo guardar la base de datos binaria.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        struct habitacion_bin registro;

        memset(&registro, 0, sizeof(registro));
        registro.numero = hotel[i].numero;
        registro.tipo = hotel[i].tipo;
        registro.capacidad = hotel[i].capacidad;
        registro.precio = hotel[i].precio;
        registro.ocupada = hotel[i].estado.ocupada;
        registro.limpieza = hotel[i].estado.limpieza;
        registro.cantidad_huespedes = hotel[i].cantidad_huespedes;

        for (j = 0; j < MAX_HUESPEDES_POR_HAB; j++) {
            registro.huespedes[j].tipo_documento = hotel[i].huespedes[j].tipo_documento;

            if (hotel[i].huespedes[j].tipo_documento == DNI) {
                registro.huespedes[j].dni = hotel[i].huespedes[j].documento.dni;
                registro.huespedes[j].pasaporte[0] = '\0';
            } else {
                registro.huespedes[j].dni = 0L;
                copiar_texto(registro.huespedes[j].pasaporte,
                             MAX_PASAPORTE,
                             hotel[i].huespedes[j].documento.pasaporte);
            }

            if (hotel[i].huespedes[j].nombre != NULL && hotel[i].huespedes[j].nombre[0] != '\0') {
                copiar_texto(registro.huespedes[j].nombre, 128, hotel[i].huespedes[j].nombre);
            } else {
                copiar_texto(registro.huespedes[j].nombre, 128, "-");
            }

            if (hotel[i].huespedes[j].fecha_checkin[0] != '\0') {
                copiar_texto(registro.huespedes[j].fecha_checkin, 20, hotel[i].huespedes[j].fecha_checkin);
            } else {
                copiar_texto(registro.huespedes[j].fecha_checkin, 20, "-");
            }
        }

        if (fwrite(&registro, sizeof(registro), 1, archivo) != 1) {
            printf("[ERROR]: No se pudo escribir una habitacion en el archivo.\n");
            fclose(archivo);
            return;
        }
    }

    fclose(archivo);
    printf("[SISTEMA]: Base de datos guardada en '%s'.\n", ARCHIVO_BASE_DATOS);
}

void cargar_base_datos(struct habitacion *hotel, int cantidad)
{
    FILE *archivo = NULL;
    int i = 0;
    int j = 0;

    if (hotel == NULL || cantidad <= 0) {
        return;
    }

    //se abre el archivo para leer "rb"
    archivo = fopen(ARCHIVO_BASE_DATOS, "rb");
    if (archivo == NULL) {
        printf("[SISTEMA]: No hay base binaria previa. Inicio limpio.\n");
        return;
    }

    for (i = 0; i < cantidad; i++) {
        struct habitacion_bin registro;

        if (fread(&registro, sizeof(registro), 1, archivo) != 1) {
            break;
        }

        hotel[i].numero = registro.numero;
        hotel[i].tipo = registro.tipo;
        hotel[i].capacidad = registro.capacidad;
        hotel[i].precio = registro.precio;
        hotel[i].estado.ocupada = registro.ocupada;
        hotel[i].estado.limpieza = registro.limpieza;
        hotel[i].cantidad_huespedes = registro.cantidad_huespedes;

        if (hotel[i].cantidad_huespedes < 0) {
            hotel[i].cantidad_huespedes = 0;
        }

        if (hotel[i].cantidad_huespedes > hotel[i].capacidad) {
            hotel[i].cantidad_huespedes = hotel[i].capacidad;
        }

        if (hotel[i].cantidad_huespedes > MAX_HUESPEDES_POR_HAB) {
            hotel[i].cantidad_huespedes = MAX_HUESPEDES_POR_HAB;
        }

        for (j = 0; j < MAX_HUESPEDES_POR_HAB; j++) {
            hotel[i].huespedes[j].tipo_documento = registro.huespedes[j].tipo_documento;

            if (registro.huespedes[j].tipo_documento == DNI) {
                hotel[i].huespedes[j].documento.dni = registro.huespedes[j].dni;
                hotel[i].huespedes[j].documento.pasaporte[0] = '\0';
            } else {
                hotel[i].huespedes[j].documento.dni = 0L;
                copiar_texto(hotel[i].huespedes[j].documento.pasaporte,
                             MAX_PASAPORTE,
                             registro.huespedes[j].pasaporte);
            }

            free(hotel[i].huespedes[j].nombre);
            hotel[i].huespedes[j].nombre = NULL;

            if (j < hotel[i].cantidad_huespedes && strcmp(registro.huespedes[j].nombre, "-") != 0) {
                hotel[i].huespedes[j].nombre = duplicar_texto(registro.huespedes[j].nombre);
                if (hotel[i].huespedes[j].nombre == NULL) {
                    printf("[ERROR]: No se pudo reservar memoria para un nombre.\n");
                }
            }

            if (j < hotel[i].cantidad_huespedes && strcmp(registro.huespedes[j].fecha_checkin, "-") != 0) {
                copiar_texto(hotel[i].huespedes[j].fecha_checkin, 20, registro.huespedes[j].fecha_checkin);
            } else {
                hotel[i].huespedes[j].fecha_checkin[0] = '\0';
            }
        }

        hotel[i].estado.ocupada = hotel[i].cantidad_huespedes > 0 ? OCUPADA : LIBRE;
    }

    fclose(archivo);
    printf("[SISTEMA]: Base de datos binaria cargada.\n");
}

void generar_reporte(const struct habitacion *hotel, int cantidad, char *nombre_out, int tam_out)
{
    FILE *archivo = NULL;
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);
    char nombre_archivo[64];
    char fecha_texto[20];
    int ocupadas = 0;
    int libres = 0;
    int sucias = 0;
    int mantenimiento = 0;
    int i = 0;
    float porcentaje_ocupadas = 0.0f;
    float porcentaje_libres = 0.0f;

    if (hotel == NULL || cantidad <= 0 || fecha == NULL) {
        return;
    }

    strftime(nombre_archivo, sizeof(nombre_archivo), "reporte_%Y%m%d_%H%M%S.txt", fecha);
    strftime(fecha_texto, sizeof(fecha_texto), "%d/%m/%Y %H:%M:%S", fecha);

    for (i = 0; i < cantidad; i++) {
        if (hotel[i].estado.ocupada == OCUPADA) {
            ocupadas++;
        } else {
            libres++;
        }

        if (hotel[i].estado.limpieza == SUCIA) {
            sucias++;
        }

        if (hotel[i].estado.limpieza == MANTENIMIENTO) {
            mantenimiento++;
        }
    }

    porcentaje_ocupadas = ((float)ocupadas * 100.0f) / (float)cantidad;
    porcentaje_libres = ((float)libres * 100.0f) / (float)cantidad;

    //se crea el reporte txt
    archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL) {
        printf("[ERROR]: No se pudo generar el reporte.\n");
        return;
    }

    fprintf(archivo, "REPORTE GENERAL DEL HOTEL\n");
    fprintf(archivo, "Fecha: %s\n", fecha_texto);
    fprintf(archivo, "==========================================\n");
    fprintf(archivo, "Total de habitaciones: %d\n", cantidad);
    fprintf(archivo, "Habitaciones ocupadas: %d  (%.2f%%)\n", ocupadas, porcentaje_ocupadas);
    fprintf(archivo, "Habitaciones libres:   %d  (%.2f%%)\n", libres, porcentaje_libres);
    fprintf(archivo, "Habitaciones sucias:   %d\n", sucias);
    fprintf(archivo, "En mantenimiento:      %d\n", mantenimiento);
    fprintf(archivo, "------------------------------------------\n");
    fprintf(archivo, "DETALLE POR HABITACION:\n");

    for (i = 0; i < cantidad; i++) {
        const char *nombre_mostrar = "-";
        const char *fecha_mostrar = "-";

        if (hotel[i].cantidad_huespedes > 0 && hotel[i].huespedes[0].nombre != NULL) {
            nombre_mostrar = hotel[i].huespedes[0].nombre;
        }

        if (hotel[i].cantidad_huespedes > 0 && hotel[i].huespedes[0].fecha_checkin[0] != '\0') {
            fecha_mostrar = hotel[i].huespedes[0].fecha_checkin;
        }

        fprintf(archivo, "[%d] [%s] [%.2f] [%s] [%s] [%s] [%s]\n",
                hotel[i].numero,
                texto_tipo(hotel[i].tipo),
                hotel[i].precio,
                texto_estado(hotel[i].estado.ocupada),
                texto_limpieza(hotel[i].estado.limpieza),
                nombre_mostrar,
                fecha_mostrar);
    }

    fprintf(archivo, "==========================================\n");

    if (nombre_out != NULL && tam_out > 0) {
        strncpy(nombre_out, nombre_archivo, (size_t)tam_out - 1);
        nombre_out[tam_out - 1] = '\0';
    }

    fclose(archivo);
    printf("[SISTEMA]: Reporte generado en '%s'.\n", nombre_archivo);
}

void ver_ultimo_reporte(const char *nombre_archivo)
{
    FILE *archivo = NULL;
    char linea[256];

    //se lee el reporte y se muestra
    if (nombre_archivo == NULL || nombre_archivo[0] == '\0') {
        printf("[SISTEMA]: No hay reportes generados todavia.\n");
        printf("           Use la opcion 6 para generar uno.\n");
        return;
    }

    archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("[SISTEMA]: No se encontro el archivo '%s'.\n",
               nombre_archivo);
        printf("           Use la opcion 6 para generar un nuevo reporte.\n");
        return;
    }

    printf("\n========================================\n");
    printf("  CONTENIDO DEL REPORTE: %s\n", nombre_archivo);
    printf("========================================\n");

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        printf("%s", linea);
    }

    fclose(archivo);
}

static void copiar_texto(char *destino, int capacidad, const char *origen)
{
    if (destino == NULL || capacidad <= 0) {
        return;
    }

    if (origen == NULL) {
        origen = "";
    }

    strncpy(destino, origen, (size_t)capacidad - 1U);
    destino[capacidad - 1] = '\0';
}

static char *duplicar_texto(const char *origen)
{
    char *copia = NULL;
    size_t largo = 0U;

    if (origen == NULL) {
        return NULL;
    }

    largo = strlen(origen);
    copia = (char *)malloc(largo + 1U);
    if (copia == NULL) {
        return NULL;
    }

    strcpy(copia, origen);
    return copia;
}

