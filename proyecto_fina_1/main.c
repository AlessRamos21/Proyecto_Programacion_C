#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LIBRE 0
#define OCUPADA 1
#define LIMPIA 0
#define SUCIA 1
#define DNI 1
#define PASAPORTE 2
#define MAX_PASAPORTE 16
#define PRECIO_BASE 10000.0f

/* Campos de bits: guardamos dos estados usando solo 1 bit para cada dato. */
struct estado_habitacion {
    unsigned int ocupada : 1;
    unsigned int limpieza : 1;
};

/* Union: un huesped puede tener DNI o pasaporte, pero no necesitamos guardar ambos. */
union documento_identidad {
    long int dni;
    char pasaporte[MAX_PASAPORTE];
};

/* El nombre es char* porque lo reservamos con malloc/realloc segun lo que escriba el usuario. */
struct huesped {
    char *nombre;
    int tipo_documento;
    union documento_identidad documento;
};

/* Cada habitacion contiene sus datos fijos y, si esta ocupada, los datos del huesped. */
struct habitacion {
    int numero;
    int capacidad;
    float precio;
    struct estado_habitacion estado;
    struct huesped huesped;
};

int convertir_cantidad(const char *texto, int *cantidad);
void inicializar_hotel(struct habitacion *hotel, int cantidad);
void liberar_hotel(struct habitacion *hotel, int cantidad);
void listar_habitaciones(const struct habitacion *hotel, int cantidad);
int leer_entero(const char *mensaje, int *valor);
int leer_entero_rango(const char *mensaje, int minimo, int maximo, int *valor);
char *leer_texto_dinamico(const char *mensaje);
int buscar_habitacion_por_numero(const struct habitacion *hotel, int cantidad, int numero);
int buscar_huesped_por_nombre(const struct habitacion *hotel, int cantidad, const char *nombre);
void hacer_checkin(struct habitacion *hotel, int cantidad);
void hacer_checkout(struct habitacion *hotel, int cantidad);
void buscar_huesped_en_menu(const struct habitacion *hotel, int cantidad);
void limpiar_habitacion_menu(struct habitacion *hotel, int cantidad);
void ordenar_por_precio(struct habitacion *hotel, int cantidad);
void intercambiar_habitaciones(struct habitacion *primera, struct habitacion *segunda);
int generar_ticket_checkout(const char *nombre, int numero_habitacion, float precio);
void mostrar_documento(const struct huesped *huesped);
void mostrar_menu(void);
void pausa(void);

int main(int argc, char *argv[])
{
    int cantidad_habitaciones = 0;
    struct habitacion *hotel = NULL;
    int opcion = -1;

    if (argc < 2) {
        printf("Uso: %s <cantidad_habitaciones>\n", argv[0]);
        return 1;
    }

    if (!convertir_cantidad(argv[1], &cantidad_habitaciones)) {
        printf("Error: la cantidad de habitaciones debe ser un entero positivo.\n");
        return 1;
    }

    hotel = calloc((size_t)cantidad_habitaciones, sizeof(struct habitacion));
    if (hotel == NULL) {
        printf("Error: no se pudo reservar memoria.\n");
        return 1;
    }

    inicializar_hotel(hotel, cantidad_habitaciones);

    do {
        mostrar_menu();
        if (!leer_entero("Opcion: ", &opcion)) {
            opcion = -1;
        }

        switch (opcion) {
            case 1:
                listar_habitaciones(hotel, cantidad_habitaciones);
                pausa();
                break;

            case 2:
                hacer_checkin(hotel, cantidad_habitaciones);
                pausa();
                break;

            case 3:
                hacer_checkout(hotel, cantidad_habitaciones);
                pausa();
                break;

            case 4:
                buscar_huesped_en_menu(hotel, cantidad_habitaciones);
                pausa();
                break;

            case 5:
                ordenar_por_precio(hotel, cantidad_habitaciones);
                printf("Habitaciones ordenadas por precio.\n");
                pausa();
                break;

            case 6:
                limpiar_habitacion_menu(hotel, cantidad_habitaciones);
                pausa();
                break;

            case 0:
                printf("Cerrando sistema...\n");
                break;

            default:
                printf("Opcion invalida.\n");
                pausa();
                break;
        }
    } while (opcion != 0);

    liberar_hotel(hotel, cantidad_habitaciones);
    free(hotel);
    hotel = NULL;

    return 0;
}

int convertir_cantidad(const char *texto, int *cantidad)
{
    int valor = 0;
    char extra = '\0';

    if (texto == NULL || cantidad == NULL) {
        return 0;
    }

    if (sscanf(texto, "%d %c", &valor, &extra) != 1 || valor <= 0) {
        return 0;
    }

    *cantidad = valor;
    return 1;
}

void inicializar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;

    if (hotel == NULL) {
        return;
    }

    for (indice = 0; indice < cantidad; indice++) {
        hotel[indice].numero = 101 + indice;
        hotel[indice].capacidad = (indice < 3) ? 2 : 4;
        hotel[indice].precio = PRECIO_BASE + ((float)indice * 2500.0f);
        hotel[indice].estado.ocupada = LIBRE;
        hotel[indice].estado.limpieza = LIMPIA;
        hotel[indice].huesped.nombre = NULL;
        hotel[indice].huesped.tipo_documento = DNI;
        hotel[indice].huesped.documento.dni = 0L;
    }
}

void liberar_hotel(struct habitacion *hotel, int cantidad)
{
    int indice = 0;

    if (hotel == NULL) {
        return;
    }

    for (indice = 0; indice < cantidad; indice++) {
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

    printf("\n%-8s %-10s %-10s %-10s %-12s %s\n",
           "Numero", "Capacidad", "Precio", "Estado", "Limpieza", "Huesped");

    for (indice = 0; indice < cantidad; indice++) {
        printf("%-8d %-10d %-10.2f %-10s %-12s %s\n",
               hotel[indice].numero,
               hotel[indice].capacidad,
               hotel[indice].precio,
               hotel[indice].estado.ocupada == OCUPADA ? "Ocupada" : "Libre",
               hotel[indice].estado.limpieza == LIMPIA ? "Limpia" : "Sucia",
               hotel[indice].huesped.nombre != NULL ? hotel[indice].huesped.nombre : "-");
    }
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

    /* realloc agranda el texto de a un caracter. Es simple para practicar memoria dinamica. */
    while ((caracter = getchar()) != '\n' && caracter != EOF) {
        temporal = realloc(texto, (size_t)largo + 2U);
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
        texto = malloc(1U);
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
        if (hotel[indice].huesped.nombre != NULL && strcmp(hotel[indice].huesped.nombre, nombre) == 0) {
            return indice;
        }
    }

    return -1;
}

void hacer_checkin(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;
    int tipo_documento = 0;
    int dni = 0;
    char *nombre = NULL;
    char *pasaporte = NULL;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- CHECK-IN ---\n");
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
        printf("La habitacion ya esta ocupada.\n");
        return;
    }

    if (hotel[posicion].estado.limpieza == SUCIA) {
        printf("La habitacion esta sucia. Primero hay que limpiarla.\n");
        return;
    }

    nombre = leer_texto_dinamico("Nombre del huesped: ");
    if (nombre == NULL || nombre[0] == '\0') {
        free(nombre);
        printf("El nombre no puede quedar vacio.\n");
        return;
    }

    printf("Tipo de documento: 1-DNI / 2-Pasaporte\n");
    if (!leer_entero_rango("Opcion: ", DNI, PASAPORTE, &tipo_documento)) {
        free(nombre);
        printf("Tipo de documento invalido.\n");
        return;
    }

    /* Si hubiera un nombre anterior, lo liberamos antes de guardar el nuevo. */
    free(hotel[posicion].huesped.nombre);
    hotel[posicion].huesped.nombre = nombre;
    hotel[posicion].huesped.tipo_documento = tipo_documento;

    if (tipo_documento == DNI) {
        if (!leer_entero("DNI: ", &dni)) {
            printf("DNI invalido. Se guardara como 0.\n");
            dni = 0;
        }
        hotel[posicion].huesped.documento.dni = dni;
    } else {
        pasaporte = leer_texto_dinamico("Pasaporte: ");
        if (pasaporte == NULL) {
            printf("No se pudo cargar el pasaporte.\n");
            hotel[posicion].huesped.documento.pasaporte[0] = '\0';
        } else {
            strncpy(hotel[posicion].huesped.documento.pasaporte, pasaporte, MAX_PASAPORTE - 1);
            hotel[posicion].huesped.documento.pasaporte[MAX_PASAPORTE - 1] = '\0';
            free(pasaporte);
        }
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

    if (generar_ticket_checkout(hotel[posicion].huesped.nombre,
                                hotel[posicion].numero,
                                hotel[posicion].precio)) {
        printf("Ticket generado correctamente.\n");
    } else {
        printf("No se pudo generar el ticket, pero se hara el check-out.\n");
    }

    /* Al liberar la habitacion tambien liberamos la memoria del nombre. */
    free(hotel[posicion].huesped.nombre);
    hotel[posicion].huesped.nombre = NULL;
    hotel[posicion].huesped.tipo_documento = DNI;
    hotel[posicion].huesped.documento.dni = 0L;
    hotel[posicion].estado.ocupada = LIBRE;
    hotel[posicion].estado.limpieza = SUCIA;

    printf("Check-out realizado. La habitacion queda sucia.\n");
}

void buscar_huesped_en_menu(const struct habitacion *hotel, int cantidad)
{
    char *nombre = NULL;
    int posicion = -1;

    printf("\n--- BUSCAR HUESPED ---\n");
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
        printf("Huesped encontrado en habitacion %d.\n", hotel[posicion].numero);
        printf("Precio de la habitacion: %.2f\n", hotel[posicion].precio);
        mostrar_documento(&hotel[posicion].huesped);
    }

    free(nombre);
}

void limpiar_habitacion_menu(struct habitacion *hotel, int cantidad)
{
    int numero = 0;
    int posicion = -1;

    if (hotel == NULL) {
        return;
    }

    printf("\n--- LIMPIAR HABITACION ---\n");
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
        printf("No se puede limpiar desde el sistema si esta ocupada.\n");
        return;
    }

    hotel[posicion].estado.limpieza = LIMPIA;
    printf("Habitacion %d marcada como limpia.\n", hotel[posicion].numero);
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

int generar_ticket_checkout(const char *nombre, int numero_habitacion, float precio)
{
    time_t ahora;
    struct tm *fecha = NULL;
    char nombre_archivo[64];
    FILE *archivo = NULL;

    if (nombre == NULL) {
        return 0;
    }

    ahora = time(NULL);
    if (ahora == (time_t)-1) {
        return 0;
    }

    fecha = localtime(&ahora);
    if (fecha == NULL) {
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
    fprintf(archivo, "Huesped: %s\n", nombre);
    fprintf(archivo, "Habitacion: %d\n", numero_habitacion);
    fprintf(archivo, "Importe: %.2f\n", precio);

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
    printf("6. Limpiar habitacion\n");
    printf("0. Salir\n\n");
}

void pausa(void)
{
    int caracter = 0;

    printf("\nPresione ENTER para continuar...");
    while ((caracter = getchar()) != '\n' && caracter != EOF) {
    }
}
