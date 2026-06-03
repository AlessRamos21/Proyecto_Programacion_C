#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "archivos.h"

void inicializar_hotel(struct habitacion *hotel, int cantidad);
void liberar_hotel(struct habitacion *hotel, int cantidad);
void listar_habitaciones(const struct habitacion *hotel, int cantidad);
int pedir_cantidad_habitaciones(void);
int leer_entero(const char *mensaje, int *valor);
int leer_long(const char *mensaje, long int *valor);
int leer_entero_rango(const char *mensaje, int minimo, int maximo, int *valor);
char *leer_texto_dinamico(const char *mensaje);
int buscar_habitacion_por_numero(const struct habitacion *hotel, int cantidad, int numero);
int buscar_huesped_por_nombre(const struct habitacion *hotel, int cantidad, const char *nombre);
void hacer_checkin(struct habitacion *hotel, int cantidad);
void hacer_checkout(struct habitacion *hotel, int cantidad);
void buscar_huesped_en_menu(const struct habitacion *hotel, int cantidad);
void buscar_huesped_por_documento(const struct habitacion *hotel, int cantidad);
void modificar_huesped(struct habitacion *hotel, int cantidad);
void cambiar_limpieza_menu(struct habitacion *hotel, int cantidad);
int cargar_documento(struct huesped *huesped, const char *mensaje_dni, const char *mensaje_pasaporte);
void ordenar_por_precio(struct habitacion *hotel, int cantidad);
void intercambiar_habitaciones(struct habitacion *primera, struct habitacion *segunda);
int generar_ticket_checkout(const struct habitacion *habitacion);
void mostrar_documento(const struct huesped *huesped);
void mostrar_datos_huesped(const struct habitacion *habitacion);
void mostrar_menu(void);
void limpiar_pantalla(void);
void pausa(void);

int main(void)
{
    int cantidad_habitaciones = 0;
    struct habitacion *hotel = NULL;
    int opcion = -1;
    char ultimo_reporte[64] = "";

    printf("========================================\n");
    printf("     INICIALIZACION DEL SISTEMA\n");
    printf("========================================\n");

    cantidad_habitaciones = contar_habitaciones_guardadas();

    if (cantidad_habitaciones > 0) {
        printf("[SISTEMA]: Base binaria encontrada con %d habitaciones.\n", cantidad_habitaciones);
    } else {
        printf("[SISTEMA]: No hay base binaria previa. Inicio limpio.\n");
        cantidad_habitaciones = pedir_cantidad_habitaciones();
    }

    /* MEMORIA DINAMICA: reserva el vector de habitaciones */
    hotel = (struct habitacion *)calloc((size_t)cantidad_habitaciones, sizeof(struct habitacion));
    if (hotel == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return 1;
    }

    inicializar_hotel(hotel, cantidad_habitaciones);

    if (contar_habitaciones_guardadas() > 0) {
        cargar_base_datos(hotel, cantidad_habitaciones);
    }

    do {
        mostrar_menu();
        if (!leer_entero("Opcion: ", &opcion)) {
            opcion = -1;
        }

        limpiar_pantalla();

        switch (opcion) {
            case 1:
                listar_habitaciones(hotel, cantidad_habitaciones);
                break;

            case 2:
                hacer_checkin(hotel, cantidad_habitaciones);
                break;

            case 3:
                hacer_checkout(hotel, cantidad_habitaciones);
                break;

            case 4:
                buscar_huesped_en_menu(hotel, cantidad_habitaciones);
                break;

            case 5:
                ordenar_por_precio(hotel, cantidad_habitaciones);
                printf("Habitaciones ordenadas por precio.\n");
                break;

            case 6:
                cambiar_limpieza_menu(hotel, cantidad_habitaciones);
                break;

            case 7:
                generar_reporte(hotel, cantidad_habitaciones, ultimo_reporte, sizeof(ultimo_reporte));
                break;

            case 8:
                buscar_huesped_por_documento(hotel, cantidad_habitaciones);
                break;

            case 9:
                modificar_huesped(hotel, cantidad_habitaciones);
                break;

            case 10:
                ver_ultimo_reporte(ultimo_reporte);
                break;

            case 0:
                guardar_base_datos(hotel, cantidad_habitaciones);
                printf("Cerrando sistema...\n");
                break;

            default:
                printf("Opcion invalida.\n");
                break;
        }

        if (opcion != 0) {
            pausa();
        }
    } while (opcion != 0);

    liberar_hotel(hotel, cantidad_habitaciones);
    free(hotel);
    hotel = NULL;

    return 0;
}

void inicializar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;

    if (hotel == NULL) {
        return;
    }

    for (indice = 0; indice < cantidad; indice++) {
        hotel[indice].numero = 101 + indice;

        if (indice < 2) {
            hotel[indice].tipo = SIMPLE;
            hotel[indice].capacidad = 1;
        } else if (indice < 5) {
            hotel[indice].tipo = DOBLE;
            hotel[indice].capacidad = 2;
        } else {
            hotel[indice].tipo = SUITE;
            hotel[indice].capacidad = 4;
        }

        hotel[indice].precio = PRECIO_BASE + ((float)indice * 2500.0f);
        hotel[indice].estado.ocupada = LIBRE;
        hotel[indice].estado.limpieza = LIMPIA;
        hotel[indice].huesped.nombre = NULL;
        hotel[indice].huesped.tipo_documento = DNI;
        hotel[indice].huesped.documento.dni = 0L;
        hotel[indice].huesped.fecha_checkin[0] = '\0';
    }
}

void liberar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;

    if (hotel == NULL) {
        return;
    }

    for (indice = 0; indice < cantidad; indice++) {
        /* MEMORIA DINAMICA: libera la memoria del nombre de cada huesped */
        free(hotel[indice].huesped.nombre);
        hotel[indice].huesped.nombre = NULL;
    }
}

void listar_habitaciones(const struct habitacion *hotel, int cantidad)
{
    int indice = 0;

    if (hotel == NULL) {
        return;
    }

    printf("\n%-8s %-10s %-10s %-10s %-12s %-20s %s\n",
           "Numero", "Tipo", "Precio", "Estado", "Limpieza", "Check-in", "Huesped");

    for (indice = 0; indice < cantidad; indice++) {
        printf("%-8d %-10s %-10.2f %-10s %-12s %-20s %s\n",
               hotel[indice].numero,
               texto_tipo(hotel[indice].tipo),
               hotel[indice].precio,
               texto_estado(hotel[indice].estado.ocupada),
               texto_limpieza(hotel[indice].estado.limpieza),
               hotel[indice].huesped.fecha_checkin[0] != '\0' ? hotel[indice].huesped.fecha_checkin : "-",
               hotel[indice].huesped.nombre != NULL ? hotel[indice].huesped.nombre : "-");
    }
}

int pedir_cantidad_habitaciones(void)
{
    int cantidad = 0;

    while (cantidad <= 0) {
        if (!leer_entero("Ingrese la cantidad de habitaciones para el hotel: ", &cantidad) || cantidad <= 0) {
            printf("Error: debe ingresar un numero entero positivo.\n");
            cantidad = 0;
        }
    }

    return cantidad;
}

int leer_entero(const char *mensaje, int *valor)
{
    char linea[64];
    char extra = '\0';

    if (mensaje == NULL || valor == NULL) {
        return 0;
    }

    printf("%s", mensaje);

    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        return 0;
    }

    if (sscanf(linea, "%d %c", valor, &extra) != 1) {
        return 0;
    }

    return 1;
}

int leer_long(const char *mensaje, long int *valor)
{
    char linea[64];
    char extra = '\0';

    if (mensaje == NULL || valor == NULL) {
        return 0;
    }

    printf("%s", mensaje);

    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        return 0;
    }

    if (sscanf(linea, "%ld %c", valor, &extra) != 1) {
        return 0;
    }

    return 1;
}

int leer_entero_rango(const char *mensaje, int minimo, int maximo, int *valor)
{
    int leido = 0;

    if (valor == NULL) {
        return 0;
    }

    if (!leer_entero(mensaje, &leido)) {
        return 0;
    }

    if (leido < minimo || leido > maximo) {
        printf("El numero debe estar entre %d y %d.\n", minimo, maximo);
        return 0;
    }

    *valor = leido;
    return 1;
}

char *leer_texto_dinamico(const char *mensaje)
{
    char *texto = NULL;
    char *temporal = NULL;
    int caracter = 0;
    int largo = 0;

    if (mensaje != NULL) {
        printf("%s", mensaje);
    }

    while ((caracter = getchar()) != '\n' && caracter != EOF) {
        temporal = (char *)realloc(texto, (size_t)largo + 2U);
        if (temporal == NULL) {
            free(texto);
            return NULL;
        }

        texto = temporal;
        texto[largo] = (char)caracter;
        largo++;
        texto[largo] = '\0';
    }

    if (texto == NULL) {
        /* MEMORIA DINAMICA: reserva espacio para el nombre del huesped */
        texto = (char *)malloc(1U);
        if (texto == NULL) {
            return NULL;
        }
        texto[0] = '\0';
    }

    return texto;
}

int buscar_habitacion_por_numero(const struct habitacion *hotel, int cantidad, int numero)
{
    int indice = 0;

    if (hotel == NULL) {
        return -1;
    }

    for (indice = 0; indice < cantidad; indice++) {
        if (hotel[indice].numero == numero) {
            return indice;
        }
    }

    return -1;
}

int buscar_huesped_por_nombre(const struct habitacion *hotel, int cantidad, const char *nombre)
{
    int indice = 0;

    if (hotel == NULL || nombre == NULL) {
        return -1;
    }

    for (indice = 0; indice < cantidad; indice++) {
        if (hotel[indice].estado.ocupada == OCUPADA &&
            hotel[indice].huesped.nombre != NULL &&
            strcmp(hotel[indice].huesped.nombre, nombre) == 0) {
            return indice;
        }
    }

    return -1;
}

void hacer_checkin(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    char *nombre = NULL;
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CHECK-IN ---\n");
    printf("Puede ver el tipo de cada habitacion desde la opcion 1.\n");

    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1) {
        printf("No existe esa habitacion.\n");
        return;
    }

    printf("Tipo seleccionado: %s\n", texto_tipo(hotel[posicion].tipo));

    if (hotel[posicion].estado.ocupada == OCUPADA) {
        printf("La habitacion ya esta ocupada.\n");
        return;
    }

    if (hotel[posicion].estado.limpieza != LIMPIA) {
        printf("La habitacion no esta limpia. No se puede usar para check-in.\n");
        return;
    }

    nombre = leer_texto_dinamico("Nombre del huesped: ");
    if (nombre == NULL || nombre[0] == '\0') {
        free(nombre);
        printf("El nombre no puede quedar vacio.\n");
        return;
    }

    free(hotel[posicion].huesped.nombre);
    hotel[posicion].huesped.nombre = nombre;

    if (!cargar_documento(&hotel[posicion].huesped, "DNI: ", "Pasaporte: ")) {
        free(hotel[posicion].huesped.nombre);
        hotel[posicion].huesped.nombre = NULL;
        printf("No se pudo cargar el documento.\n");
        return;
    }

    if (fecha != NULL) {
        strftime(hotel[posicion].huesped.fecha_checkin,
                 sizeof(hotel[posicion].huesped.fecha_checkin),
                 "%d/%m/%Y %H:%M:%S",
                 fecha);
    } else {
        hotel[posicion].huesped.fecha_checkin[0] = '\0';
    }

    hotel[posicion].estado.ocupada = OCUPADA;
    printf("Check-in realizado en la habitacion %d.\n", hotel[posicion].numero);
}

void hacer_checkout(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CHECK-OUT ---\n");
    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1) {
        printf("No existe esa habitacion.\n");
        return;
    }

    if (hotel[posicion].estado.ocupada == LIBRE) {
        printf("La habitacion ya esta libre.\n");
        return;
    }

    printf("Fecha de check-in: %s\n",
           hotel[posicion].huesped.fecha_checkin[0] != '\0' ? hotel[posicion].huesped.fecha_checkin : "-");

    if (generar_ticket_checkout(&hotel[posicion])) {
        printf("Ticket generado correctamente.\n");
    } else {
        printf("No se pudo generar el ticket, pero se hara el check-out.\n");
    }

    free(hotel[posicion].huesped.nombre);
    hotel[posicion].huesped.nombre = NULL;
    hotel[posicion].huesped.tipo_documento = DNI;
    hotel[posicion].huesped.documento.dni = 0L;
    hotel[posicion].huesped.fecha_checkin[0] = '\0';
    hotel[posicion].estado.ocupada = LIBRE;
    hotel[posicion].estado.limpieza = SUCIA;

    printf("Check-out realizado. La habitacion queda sucia.\n");
}

void buscar_huesped_en_menu(const struct habitacion *hotel, int cantidad)
{
    char *nombre = NULL;
    int posicion = -1;

    printf("\n--- BUSCAR HUESPED POR NOMBRE ---\n");
    nombre = leer_texto_dinamico("Nombre a buscar: ");
    if (nombre == NULL || nombre[0] == '\0') {
        free(nombre);
        printf("Nombre invalido.\n");
        return;
    }

    posicion = buscar_huesped_por_nombre(hotel, cantidad, nombre);
    if (posicion == -1) {
        printf("No se encontro un huesped con ese nombre.\n");
    } else {
        mostrar_datos_huesped(&hotel[posicion]);
    }

    free(nombre);
}

void buscar_huesped_por_documento(const struct habitacion *hotel, int cantidad)
{
    int tipo_documento = 0;
    long int dni = 0L;
    char *pasaporte = NULL;
    int indice = 0;
    int encontrado = 0;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- BUSCAR HUESPED POR DOCUMENTO ---\n");
    printf("Tipo de documento: 1-DNI / 2-Pasaporte\n");

    if (!leer_entero_rango("Opcion: ", DNI, PASAPORTE, &tipo_documento)) {
        printf("Tipo invalido.\n");
        return;
    }

    if (tipo_documento == DNI) {
        if (!leer_long("DNI a buscar: ", &dni)) {
            printf("DNI invalido.\n");
            return;
        }

        for (indice = 0; indice < cantidad; indice++) {
            if (hotel[indice].estado.ocupada == OCUPADA &&
                hotel[indice].huesped.tipo_documento == DNI &&
                hotel[indice].huesped.documento.dni == dni) {
                mostrar_datos_huesped(&hotel[indice]);
                encontrado = 1;
            }
        }
    } else {
        pasaporte = leer_texto_dinamico("Pasaporte a buscar: ");
        if (pasaporte == NULL || pasaporte[0] == '\0') {
            free(pasaporte);
            printf("Pasaporte invalido.\n");
            return;
        }

        for (indice = 0; indice < cantidad; indice++) {
            if (hotel[indice].estado.ocupada == OCUPADA &&
                hotel[indice].huesped.tipo_documento == PASAPORTE &&
                strcmp(hotel[indice].huesped.documento.pasaporte, pasaporte) == 0) {
                mostrar_datos_huesped(&hotel[indice]);
                encontrado = 1;
            }
        }

        free(pasaporte);
    }

    if (!encontrado) {
        printf("No se encontro ningun huesped con ese documento.\n");
    }
}

void modificar_huesped(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    int opcion = 0;
    char *nuevo_texto = NULL;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- MODIFICAR DATOS DE HUESPED ---\n");
    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1 || hotel[posicion].estado.ocupada == LIBRE) {
        printf("La habitacion no existe o no esta ocupada.\n");
        return;
    }

    mostrar_datos_huesped(&hotel[posicion]);
    printf("1. Nombre\n");
    printf("2. Documento\n");
    printf("3. Cancelar\n");

    if (!leer_entero_rango("Campo a modificar: ", 1, 3, &opcion)) {
        printf("Opcion invalida.\n");
        return;
    }

    if (opcion == 1) {
        nuevo_texto = leer_texto_dinamico("Nuevo nombre: ");
        if (nuevo_texto == NULL || nuevo_texto[0] == '\0') {
            free(nuevo_texto);
            printf("Nombre invalido.\n");
            return;
        }

        free(hotel[posicion].huesped.nombre);
        hotel[posicion].huesped.nombre = nuevo_texto;
        printf("Nombre actualizado.\n");
    } else if (opcion == 2) {
        if (!cargar_documento(&hotel[posicion].huesped, "Nuevo DNI: ", "Nuevo pasaporte: ")) {
            printf("Documento invalido.\n");
            return;
        }

        printf("Documento actualizado.\n");
    } else {
        printf("Modificacion cancelada.\n");
    }
}

int cargar_documento(struct huesped *huesped, const char *mensaje_dni, const char *mensaje_pasaporte)
{
    int tipo_documento = 0;
    long int dni = 0L;
    char *pasaporte = NULL;

    if (huesped == NULL) {
        return 0;
    }

    printf("Tipo de documento: 1-DNI / 2-Pasaporte\n");
    if (!leer_entero_rango("Opcion: ", DNI, PASAPORTE, &tipo_documento)) {
        return 0;
    }

    huesped->tipo_documento = tipo_documento;

    if (tipo_documento == DNI) {
        if (!leer_long(mensaje_dni, &dni)) {
            return 0;
        }

        huesped->documento.dni = dni;
        return 1;
    }

    pasaporte = leer_texto_dinamico(mensaje_pasaporte);
    if (pasaporte == NULL || pasaporte[0] == '\0') {
        free(pasaporte);
        return 0;
    }

    strncpy(huesped->documento.pasaporte, pasaporte, MAX_PASAPORTE - 1);
    huesped->documento.pasaporte[MAX_PASAPORTE - 1] = '\0';
    free(pasaporte);
    return 1;
}

void cambiar_limpieza_menu(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    int limpieza = 0;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CAMBIAR LIMPIEZA / MANTENIMIENTO ---\n");
    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1) {
        printf("No existe esa habitacion.\n");
        return;
    }

    if (hotel[posicion].estado.ocupada == OCUPADA) {
        printf("No se cambia limpieza/mantenimiento si la habitacion esta ocupada.\n");
        return;
    }

    printf("0. Limpia\n");
    printf("1. Sucia\n");
    printf("2. Mantenimiento\n");

    if (!leer_entero_rango("Nuevo estado: ", LIMPIA, MANTENIMIENTO, &limpieza)) {
        printf("Estado invalido.\n");
        return;
    }

    hotel[posicion].estado.limpieza = (unsigned int)limpieza;
    printf("Habitacion %d marcada como %s.\n",
           hotel[posicion].numero,
           texto_limpieza(hotel[posicion].estado.limpieza));
}

void ordenar_por_precio(struct habitacion *hotel, int cantidad)
{
    int posicion_actual = 0;
    int posicion_menor = 0;
    int comparador = 0;

    if (hotel == NULL) {
        return;
    }

    for (posicion_actual = 0; posicion_actual < cantidad - 1; posicion_actual++) {
        posicion_menor = posicion_actual;

        for (comparador = posicion_actual + 1; comparador < cantidad; comparador++) {
            if (hotel[comparador].precio < hotel[posicion_menor].precio) {
                posicion_menor = comparador;
            }
        }

        if (posicion_menor != posicion_actual) {
            intercambiar_habitaciones(&hotel[posicion_actual], &hotel[posicion_menor]);
        }
    }
}

void intercambiar_habitaciones(struct habitacion *primera, struct habitacion *segunda)
{
    struct habitacion temporal;

    if (primera == NULL || segunda == NULL) {
        return;
    }

    temporal = *primera;
    *primera = *segunda;
    *segunda = temporal;
}

int generar_ticket_checkout(const struct habitacion *habitacion)
{
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);
    char nombre_archivo[64];
    FILE *archivo = NULL;

    if (habitacion == NULL || habitacion->huesped.nombre == NULL || fecha == NULL) {
        return 0;
    }

    if (strftime(nombre_archivo, sizeof(nombre_archivo), "ticket_%Y%m%d_%H%M%S.txt", fecha) == 0) {
        return 0;
    }

    archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL) {
        return 0;
    }

    fprintf(archivo, "Ticket de check-out\n");
    fprintf(archivo, "Huesped: %s\n", habitacion->huesped.nombre);
    fprintf(archivo, "Habitacion: %d\n", habitacion->numero);
    fprintf(archivo, "Tipo: %s\n", texto_tipo(habitacion->tipo));
    fprintf(archivo, "Fecha de check-in: %s\n",
            habitacion->huesped.fecha_checkin[0] != '\0' ? habitacion->huesped.fecha_checkin : "-");
    fprintf(archivo, "Importe: %.2f\n", habitacion->precio);

    return fclose(archivo) == 0;
}

void mostrar_documento(const struct huesped *huesped)
{
    if (huesped == NULL) {
        return;
    }

    if (huesped->tipo_documento == DNI) {
        printf("DNI: %ld\n", huesped->documento.dni);
    } else {
        printf("Pasaporte: %s\n", huesped->documento.pasaporte);
    }
}

void mostrar_datos_huesped(const struct habitacion *habitacion)
{
    if (habitacion == NULL) {
        return;
    }

    printf("Habitacion: %d\n", habitacion->numero);
    printf("Tipo: %s\n", texto_tipo(habitacion->tipo));
    printf("Precio: %.2f\n", habitacion->precio);
    printf("Huesped: %s\n", habitacion->huesped.nombre != NULL ? habitacion->huesped.nombre : "-");
    printf("Fecha check-in: %s\n",
           habitacion->huesped.fecha_checkin[0] != '\0' ? habitacion->huesped.fecha_checkin : "-");
    mostrar_documento(&habitacion->huesped);
}

const char *texto_tipo(int tipo)
{
    if (tipo == SIMPLE) {
        return "Simple";
    }

    if (tipo == DOBLE) {
        return "Doble";
    }

    if (tipo == SUITE) {
        return "Suite";
    }

    return "Sin tipo";
}

const char *texto_estado(unsigned int ocupada)
{
    return ocupada == OCUPADA ? "Ocupada" : "Libre";
}

const char *texto_limpieza(unsigned int limpieza)
{
    if (limpieza == LIMPIA) {
        return "Limpia";
    }

    if (limpieza == SUCIA) {
        return "Sucia";
    }

    if (limpieza == MANTENIMIENTO) {
        return "Mantenimiento";
    }

    return "Desconocida";
}

void mostrar_menu(void)
{
    printf("\n========================================\n");
    printf("        SISTEMA HOTELERO - EDA 1\n");
    printf("========================================\n");
    printf("1. Listar habitaciones\n");
    printf("2. Check-in\n");
    printf("3. Check-out\n");
    printf("4. Buscar huesped por nombre\n");
    printf("5. Ordenar por precio\n");
    printf("6. Cambiar limpieza/mantenimiento\n");
    printf("7. Generar reporte general\n");
    printf("8. Buscar huesped por documento\n");
    printf("9. Modificar datos de huesped\n");
    printf("10. Ver ultimo reporte generado\n");
    printf("0. Salir\n\n");
}

void limpiar_pantalla(void)
{
    system("cls");
}

void pausa(void)
{
    int caracter = 0;

    printf("\nPresione ENTER para continuar...");
    while ((caracter = getchar()) != '\n' && caracter != EOF) {
    }
    limpiar_pantalla();
}
