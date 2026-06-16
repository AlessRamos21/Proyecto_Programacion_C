#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "archivos.h"
#include "hotel.h"

//logica de las funciones


void inicializar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;
    int huesped_indice = 0;
    int limite_simple = 0;
    int limite_doble = 0;

    if (hotel == NULL) {
        return;
    }

    limite_simple = cantidad * 40 / 100;
    limite_doble = cantidad * 80 / 100;

    if (cantidad == 1) {
        limite_simple = 1;
        limite_doble = 1;
    } else if (cantidad == 2) {
        limite_simple = 1;
        limite_doble = 2;
    } else {
        if (limite_simple < 1) {
            limite_simple = 1;
        }

        if (limite_doble <= limite_simple) {
            limite_doble = limite_simple + 1;
        }

        if (limite_doble >= cantidad) {
            limite_doble = cantidad - 1;
        }
    }

    for (indice = 0; indice < cantidad; indice++) {
        hotel[indice].numero = 101 + indice;

        if (indice < limite_simple) {
            hotel[indice].tipo = SIMPLE;
            hotel[indice].capacidad = 1;
            hotel[indice].precio = PRECIO_BASE;
        } else if (indice < limite_doble) {
            hotel[indice].tipo = DOBLE;
            hotel[indice].capacidad = 2;
            hotel[indice].precio = PRECIO_BASE * 1.5f;
        } else {
            hotel[indice].tipo = SUITE;
            hotel[indice].capacidad = 4;
            hotel[indice].precio = PRECIO_BASE * 2.5f;
        }

        hotel[indice].estado.ocupada = LIBRE;
        hotel[indice].estado.limpieza = LIMPIA;
        hotel[indice].cantidad_huespedes = 0;

        for (huesped_indice = 0; huesped_indice < MAX_HUESPEDES_POR_HAB; huesped_indice++) {
            hotel[indice].huespedes[huesped_indice].nombre = NULL;
            hotel[indice].huespedes[huesped_indice].tipo_documento = DNI;
            hotel[indice].huespedes[huesped_indice].documento.dni = 0L;
            hotel[indice].huespedes[huesped_indice].fecha_checkin[0] = '\0';
        }
    }
}

void liberar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;
    int huesped_indice = 0;

    if (hotel == NULL) {
        return;
    }

    for (indice = 0; indice < cantidad; indice++) {
        for (huesped_indice = 0; huesped_indice < MAX_HUESPEDES_POR_HAB; huesped_indice++) {
            free(hotel[indice].huespedes[huesped_indice].nombre);
            hotel[indice].huespedes[huesped_indice].nombre = NULL;
        }
    }
}

void listar_habitaciones(const struct habitacion *hotel, int cantidad)
{
    int indice = 0;
    char ocup_texto[20];
    char huesped_mostrar[160];

    if (hotel == NULL) {
        return;
    }

    printf("\n%-8s %-10s %-10s %-16s %-12s %-20s %s\n",
           "Numero", "Tipo", "Precio", "Estado", "Limpieza", "Check-in", "Huesped");
    printf("------------------------------------------------------------------------------------------\n");

    for (indice = 0; indice < cantidad; indice++) {
        if (hotel[indice].cantidad_huespedes == 0) {
            sprintf(ocup_texto, "Libre %d/%d",
                    hotel[indice].cantidad_huespedes,
                    hotel[indice].capacidad);
            snprintf(huesped_mostrar, sizeof(huesped_mostrar), "-");
        } else if (hotel[indice].cantidad_huespedes >= hotel[indice].capacidad) {
            sprintf(ocup_texto, "Llena %d/%d",
                    hotel[indice].cantidad_huespedes,
                    hotel[indice].capacidad);
            snprintf(huesped_mostrar,
                     sizeof(huesped_mostrar),
                     "%s%s",
                     hotel[indice].huespedes[0].nombre != NULL ? hotel[indice].huespedes[0].nombre : "-",
                     hotel[indice].cantidad_huespedes > 1 ? "..." : "");
        } else {
            sprintf(ocup_texto, "Parcial %d/%d",
                    hotel[indice].cantidad_huespedes,
                    hotel[indice].capacidad);
            snprintf(huesped_mostrar,
                     sizeof(huesped_mostrar),
                     "%s%s",
                     hotel[indice].huespedes[0].nombre != NULL ? hotel[indice].huespedes[0].nombre : "-",
                     hotel[indice].cantidad_huespedes > 1 ? "..." : "");
        }

        printf("%-8d %-10s %-10.2f %-16s %-12s %-20s %s\n",
               hotel[indice].numero,
               texto_tipo(hotel[indice].tipo),
               hotel[indice].precio,
               ocup_texto,
               texto_limpieza(hotel[indice].estado.limpieza),
               hotel[indice].cantidad_huespedes > 0 && hotel[indice].huespedes[0].fecha_checkin[0] != '\0'
                   ? hotel[indice].huespedes[0].fecha_checkin
                   : "-",
               huesped_mostrar);

        if (hotel[indice].cantidad_huespedes > 0) {
            printf("------------------------------------------------------------------------------------------\n");
        }
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

char *leer_texto_dinamico(const char *mensaje)
{
    char buffer[128];
    char *texto = NULL;
    size_t largo = 0;

    if (mensaje != NULL) {
        printf("%s", mensaje);
    }

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        texto = (char *)malloc(1U);
        if (texto != NULL) {
            texto[0] = '\0';
        }
        return texto;
    }

    largo = strlen(buffer);
    if (largo > 0 && buffer[largo - 1] == '\n') {
        buffer[largo - 1] = '\0';
        largo--;
    }

    texto = (char *)malloc(largo + 1U);
    if (texto != NULL) {
        strcpy(texto, buffer);
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
    int huesped_indice = 0;

    if (hotel == NULL || nombre == NULL) {
        return -1;
    }

    for (indice = 0; indice < cantidad; indice++) {
        for (huesped_indice = 0; huesped_indice < hotel[indice].cantidad_huespedes; huesped_indice++) {
            if (hotel[indice].huespedes[huesped_indice].nombre != NULL &&
                strcmp(hotel[indice].huespedes[huesped_indice].nombre, nombre) == 0) {
                return indice;
            }
        }
    }

    return -1;
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
    if (!leer_entero("Opcion: ", &tipo_documento)) {
        return 0;
    }
    if (tipo_documento < DNI || tipo_documento > PASAPORTE) {
        printf("El numero debe estar entre %d y %d.\n", DNI, PASAPORTE);
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

void hacer_checkin(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    int nuevo_indice = 0;
    char *nombre = NULL;
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);
    struct huesped *nuevo_huesped = NULL;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CHECK-IN ---\n");
    printf("----------------\n");
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

    if (hotel[posicion].cantidad_huespedes >= hotel[posicion].capacidad) {
        printf("Habitacion llena (capacidad: %d huespedes).\n", hotel[posicion].capacidad);
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

    nuevo_indice = hotel[posicion].cantidad_huespedes;
    nuevo_huesped = &hotel[posicion].huespedes[nuevo_indice];

    free(nuevo_huesped->nombre);
    nuevo_huesped->nombre = nombre;

    if (!cargar_documento(nuevo_huesped, "DNI: ", "Pasaporte: ")) {
        free(nuevo_huesped->nombre);
        nuevo_huesped->nombre = NULL;
        printf("No se pudo cargar el documento.\n");
        return;
    }

    if (fecha != NULL) {
        strftime(nuevo_huesped->fecha_checkin,
                 sizeof(nuevo_huesped->fecha_checkin),
                 "%d/%m/%Y %H:%M:%S",
                 fecha);
    } else {
        nuevo_huesped->fecha_checkin[0] = '\0';
    }

    hotel[posicion].cantidad_huespedes++;
    hotel[posicion].estado.ocupada = OCUPADA;

    printf("\n--- CHECKIN CONFIRMADO ---\n");
    printf("Habitacion: %d\n", hotel[posicion].numero);
    printf("Check-in exitoso. Lugares ocupados: %d/%d\n",
           hotel[posicion].cantidad_huespedes,
           hotel[posicion].capacidad);
    printf("--------------------------\n");
}

int generar_ticket_checkout(const struct habitacion *habitacion, const struct huesped *huesped)
{
    time_t ahora = time(NULL);
    struct tm *fecha = localtime(&ahora);
    char nombre_archivo[64];
    FILE *archivo = NULL;

    if (habitacion == NULL || huesped == NULL || huesped->nombre == NULL || fecha == NULL) {
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
    fprintf(archivo, "Huesped: %s\n", huesped->nombre);
    fprintf(archivo, "Habitacion: %d\n", habitacion->numero);
    fprintf(archivo, "Tipo: %s\n", texto_tipo(habitacion->tipo));
    fprintf(archivo, "Fecha de check-in: %s\n",
            huesped->fecha_checkin[0] != '\0' ? huesped->fecha_checkin : "-");
    fprintf(archivo, "Importe: %.2f\n", habitacion->precio);

    return fclose(archivo) == 0;
}

void hacer_checkout(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    int opcion_huesped = 1;
    int indice_huesped = 0;
    int indice_compactar = 0;
    struct huesped *huesped_checkout = NULL;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CHECK-OUT ---\n");
    printf("-----------------\n");
    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1) {
        printf("No existe esa habitacion.\n");
        return;
    }

    if (hotel[posicion].cantidad_huespedes == 0) {
        printf("La habitacion ya esta libre.\n");
        return;
    }

    if (hotel[posicion].cantidad_huespedes > 1) {
        printf("Huespedes alojados:\n");
        for (indice_huesped = 0; indice_huesped < hotel[posicion].cantidad_huespedes; indice_huesped++) {
            printf("%d. %s\n",
                   indice_huesped + 1,
                   hotel[posicion].huespedes[indice_huesped].nombre != NULL
                       ? hotel[posicion].huespedes[indice_huesped].nombre
                       : "-");
        }

        if (!leer_entero("Huesped que hace check-out: ", &opcion_huesped)) {
            printf("Opcion invalida.\n");
            return;
        }
        if (opcion_huesped < 1 || opcion_huesped > hotel[posicion].cantidad_huespedes) {
            printf("El numero debe estar entre 1 y %d.\n", hotel[posicion].cantidad_huespedes);
            printf("Opcion invalida.\n");
            return;
        }
    }

    indice_huesped = opcion_huesped - 1;
    huesped_checkout = &hotel[posicion].huespedes[indice_huesped];

    printf("Fecha de check-in: %s\n",
           huesped_checkout->fecha_checkin[0] != '\0' ? huesped_checkout->fecha_checkin : "-");

    if (generar_ticket_checkout(&hotel[posicion], huesped_checkout)) {
        printf("Ticket generado correctamente.\n");
    } else {
        printf("No se pudo generar el ticket, pero se hara el check-out.\n");
    }

    free(huesped_checkout->nombre);
    huesped_checkout->nombre = NULL;

    for (indice_compactar = indice_huesped; indice_compactar < hotel[posicion].cantidad_huespedes - 1; indice_compactar++) {
        hotel[posicion].huespedes[indice_compactar] = hotel[posicion].huespedes[indice_compactar + 1];
    }

    hotel[posicion].cantidad_huespedes--;
    hotel[posicion].huespedes[hotel[posicion].cantidad_huespedes].nombre = NULL;
    hotel[posicion].huespedes[hotel[posicion].cantidad_huespedes].tipo_documento = DNI;
    hotel[posicion].huespedes[hotel[posicion].cantidad_huespedes].documento.dni = 0L;
    hotel[posicion].huespedes[hotel[posicion].cantidad_huespedes].fecha_checkin[0] = '\0';

    if (hotel[posicion].cantidad_huespedes == 0) {
        hotel[posicion].estado.ocupada = LIBRE;
        hotel[posicion].estado.limpieza = SUCIA;
    } else {
        hotel[posicion].estado.ocupada = OCUPADA;
    }

    printf("Check-out realizado. Huespedes restantes: %d/%d\n",
           hotel[posicion].cantidad_huespedes,
           hotel[posicion].capacidad);
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
    int huesped_indice = 0;

    if (habitacion == NULL) {
        return;
    }

    printf("\n----------------------------------------\n");
    printf("Habitacion: %d\n", habitacion->numero);
    printf("Tipo: %s\n", texto_tipo(habitacion->tipo));
    printf("Precio: %.2f\n", habitacion->precio);
    printf("Huespedes alojados: %d/%d\n",
           habitacion->cantidad_huespedes,
           habitacion->capacidad);

    for (huesped_indice = 0; huesped_indice < habitacion->cantidad_huespedes; huesped_indice++) {
        printf("\nHuesped %d\n", huesped_indice + 1);
        printf("Nombre: %s\n",
               habitacion->huespedes[huesped_indice].nombre != NULL
                   ? habitacion->huespedes[huesped_indice].nombre
                   : "-");
        printf("Fecha check-in: %s\n",
               habitacion->huespedes[huesped_indice].fecha_checkin[0] != '\0'
                   ? habitacion->huespedes[huesped_indice].fecha_checkin
                   : "-");
        mostrar_documento(&habitacion->huespedes[huesped_indice]);
    }

    printf("----------------------------------------\n");
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
    int huesped_indice = 0;
    int encontrado = 0;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- BUSCAR HUESPED POR DOCUMENTO ---\n");
    printf("Tipo de documento: 1-DNI / 2-Pasaporte\n");

    if (!leer_entero("Opcion: ", &tipo_documento)) {
        printf("Tipo invalido.\n");
        return;
    }
    if (tipo_documento < DNI || tipo_documento > PASAPORTE) {
        printf("El numero debe estar entre %d y %d.\n", DNI, PASAPORTE);
        printf("Tipo invalido.\n");
        return;
    }

    if (tipo_documento == DNI) {
        if (!leer_long("DNI a buscar: ", &dni)) {
            printf("DNI invalido.\n");
            return;
        }

        for (indice = 0; indice < cantidad; indice++) {
            for (huesped_indice = 0; huesped_indice < hotel[indice].cantidad_huespedes; huesped_indice++) {
                if (hotel[indice].huespedes[huesped_indice].tipo_documento == DNI &&
                    hotel[indice].huespedes[huesped_indice].documento.dni == dni) {
                    mostrar_datos_huesped(&hotel[indice]);
                    encontrado = 1;
                }
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
            for (huesped_indice = 0; huesped_indice < hotel[indice].cantidad_huespedes; huesped_indice++) {
                if (hotel[indice].huespedes[huesped_indice].tipo_documento == PASAPORTE &&
                    strcmp(hotel[indice].huespedes[huesped_indice].documento.pasaporte, pasaporte) == 0) {
                    mostrar_datos_huesped(&hotel[indice]);
                    encontrado = 1;
                }
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
    int opcion_huesped = 1;
    int indice_huesped = 0;
    char *nuevo_texto = NULL;
    struct huesped *huesped_modificar = NULL;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- MODIFICAR DATOS DE HUESPED ---\n");
    if (!leer_entero("Numero de habitacion: ", &numero)) {
        printf("Numero invalido.\n");
        return;
    }

    posicion = buscar_habitacion_por_numero(hotel, cantidad, numero);
    if (posicion == -1 || hotel[posicion].cantidad_huespedes == 0) {
        printf("La habitacion no existe o no esta ocupada.\n");
        return;
    }

    mostrar_datos_huesped(&hotel[posicion]);

    if (hotel[posicion].cantidad_huespedes > 1) {
        if (!leer_entero("Huesped a modificar: ", &opcion_huesped)) {
            printf("Opcion invalida.\n");
            return;
        }
        if (opcion_huesped < 1 || opcion_huesped > hotel[posicion].cantidad_huespedes) {
            printf("El numero debe estar entre 1 y %d.\n", hotel[posicion].cantidad_huespedes);
            printf("Opcion invalida.\n");
            return;
        }
    }

    indice_huesped = opcion_huesped - 1;
    huesped_modificar = &hotel[posicion].huespedes[indice_huesped];

    printf("1. Nombre\n");
    printf("2. Documento\n");
    printf("3. Cancelar\n");

    if (!leer_entero("Campo a modificar: ", &opcion)) {
        printf("Opcion invalida.\n");
        return;
    }
    if (opcion < 1 || opcion > 3) {
        printf("El numero debe estar entre 1 y 3.\n");
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

        free(huesped_modificar->nombre);
        huesped_modificar->nombre = nuevo_texto;
        printf("Nombre actualizado.\n");
    } else if (opcion == 2) {
        if (!cargar_documento(huesped_modificar, "Nuevo DNI: ", "Nuevo pasaporte: ")) {
            printf("Documento invalido.\n");
            return;
        }

        printf("Documento actualizado.\n");
    } else {
        printf("Modificacion cancelada.\n");
    }
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

    if (!leer_entero("Nuevo estado: ", &limpieza)) {
        printf("Estado invalido.\n");
        return;
    }
    if (limpieza < LIMPIA || limpieza > MANTENIMIENTO) {
        printf("El numero debe estar entre %d y %d.\n", LIMPIA, MANTENIMIENTO);
        printf("Estado invalido.\n");
        return;
    }

    hotel[posicion].estado.limpieza = (unsigned int)limpieza;
    printf("Habitacion %d marcada como %s.\n",
           hotel[posicion].numero,
           texto_limpieza(hotel[posicion].estado.limpieza));
}

void mostrar_menu(void)
{
    printf("\n========================================\n");
    printf("        SISTEMA HOTELERO - EDA 1\n");
    printf("========================================\n");
    printf("----------------------------------------\n");
    printf("1. Listar habitaciones\n");
    printf("2. Check-in\n");
    printf("3. Check-out\n");
    printf("4. Buscar huesped por nombre\n");
    printf("5. Cambiar limpieza/mantenimiento\n");
    printf("6. Generar reporte general\n");
    printf("7. Buscar huesped por documento\n");
    printf("8. Modificar datos de huesped\n");
    printf("9. Ver ultimo reporte generado\n");
    printf("0. Salir y guardar\n");
    printf("----------------------------------------\n\n");
}

void pausa(void)
{
    int caracter = 0;

    printf("\nPresione ENTER para continuar...");
    while ((caracter = getchar()) != '\n' && caracter != EOF) {
    }
    system("cls");
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
